/*
 * emoji_render.cpp — Color emoji rendering via DirectWrite + Direct2D.
 *
 * This is the only C++ file in the Windows pterm build.  It exposes a
 * pure C API (see emoji_render.h) and is linked against dwrite.dll and
 * d2d1.dll which are available on Windows 7+ (with platform update).
 *
 * Colour emoji (COLR / CBDT) require Windows 8.1+; on older systems
 * the flag is silently ignored and emoji render monochrome — which is
 * the same behaviour as without this module.
 */

#include "emoji_render.h"

#include <stdio.h>
#include <dwrite.h>
#include <d2d1.h>

/* ---------- helpers for the colour-font flag (8.1+) ---------- */

#ifndef D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
#define D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT \
    (D2D1_DRAW_TEXT_OPTIONS)0x00000004
#endif

/* ---------- dynamic-loaded entry points ---------- */

typedef HRESULT (WINAPI *pfn_DWriteCreateFactory)(
    DWRITE_FACTORY_TYPE, REFIID, IUnknown **);
typedef HRESULT (WINAPI *pfn_D2D1CreateFactory)(
    int /* D2D1_FACTORY_TYPE */, REFIID,
    const void *, void **);

static pfn_DWriteCreateFactory  pDWriteCreateFactory  = nullptr;
static pfn_D2D1CreateFactory    pD2D1CreateFactory    = nullptr;
static HMODULE                  g_hDWrite              = nullptr;
static HMODULE                  g_hD2D1                = nullptr;

/* ---------- singletons ---------- */

static IDWriteFactory         *g_dw   = nullptr;
static ID2D1Factory           *g_d2d  = nullptr;
static ID2D1DCRenderTarget    *g_rt   = nullptr;

/* One-entry text-format cache (size may change on font reconfig). */
static IDWriteTextFormat *g_fmt     = nullptr;
static float              g_fmtDip = 0.0f;

/* Font list supplied by the caller (fallback_names from window.c). */
static const WCHAR *const *g_font_names = nullptr;
static int                 g_font_count = 0;

extern "C" void emoji_renderer_set_fonts(const WCHAR *const *names, int count)
{
    g_font_names = names;
    g_font_count = count;
    if (g_fmt) { g_fmt->Release(); g_fmt = nullptr; }
    g_fmtDip = 0.0f;
}

static IDWriteTextFormat *get_fmt(float dip)
{
    if (g_fmt && g_fmtDip == dip)
        return g_fmt;

    if (g_fmt) { g_fmt->Release(); g_fmt = nullptr; }

    /* Try each caller-supplied font in order; pick the first that
     * CreateTextFormat succeeds with (i.e. that exists on the system). */
    if (g_font_names && g_font_count > 0) {
        for (int fi = 0; fi < g_font_count; fi++) {
            if (SUCCEEDED(g_dw->CreateTextFormat(
                    g_font_names[fi], nullptr,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL,
                    dip, L"", &g_fmt))) {
                g_fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                g_fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                g_fmtDip = dip;
                return g_fmt;
            }
        }
    }

    return nullptr;
}

/* ================================================================ */
/*  C API                                                          */
/* ================================================================ */

extern "C" bool emoji_renderer_init(void)
{
    if (g_rt) return true;                 /* already initialised */

    g_hDWrite = LoadLibraryW(L"dwrite.dll");
    g_hD2D1   = LoadLibraryW(L"d2d1.dll");
    if (!g_hDWrite || !g_hD2D1) return false;

    pDWriteCreateFactory = (pfn_DWriteCreateFactory)
        GetProcAddress(g_hDWrite, "DWriteCreateFactory");
    pD2D1CreateFactory = (pfn_D2D1CreateFactory)
        GetProcAddress(g_hD2D1, "D2D1CreateFactory");
    if (!pDWriteCreateFactory || !pD2D1CreateFactory) return false;

    if (FAILED(pD2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory), nullptr, (void **)&g_d2d)))
        return false;

    if (FAILED(pDWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory), (IUnknown **)&g_dw)))
        return false;

    D2D1_RENDER_TARGET_PROPERTIES rp = {};
    rp.type            = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    rp.pixelFormat.format     = DXGI_FORMAT_B8G8R8A8_UNORM;
    rp.pixelFormat.alphaMode  = D2D1_ALPHA_MODE_PREMULTIPLIED;

    if (FAILED(g_d2d->CreateDCRenderTarget(&rp, &g_rt)))
        return false;

    return true;
}

extern "C" void emoji_renderer_cleanup(void)
{
    if (g_fmt)    { g_fmt->Release();    g_fmt   = nullptr; }
    if (g_rt)     { g_rt->Release();     g_rt    = nullptr; }
    if (g_dw)     { g_dw->Release();     g_dw    = nullptr; }
    if (g_d2d)    { g_d2d->Release();    g_d2d   = nullptr; }
    if (g_hDWrite){ FreeLibrary(g_hDWrite); g_hDWrite = nullptr; }
    if (g_hD2D1)  { FreeLibrary(g_hD2D1);   g_hD2D1   = nullptr; }
    pDWriteCreateFactory = nullptr;
    pD2D1CreateFactory   = nullptr;
    g_fmtDip = 0.0f;
}

extern "C" bool emoji_is_color_candidate(unsigned int uc)
{
    return
        (uc >= 0x1F600 && uc <= 0x1F64F) ||   /* Emoticons           */
        (uc >= 0x1F300 && uc <= 0x1F5FF) ||   /* Misc Symbols & Picto */
        (uc >= 0x1F680 && uc <= 0x1F6FF) ||   /* Transport & Map      */
        (uc >= 0x1F900 && uc <= 0x1F9FF) ||   /* Supplemental Symbols */
        (uc >= 0x1FA00 && uc <= 0x1FAFF) ||   /* Extended-A           */
        (uc >= 0x1F000 && uc <= 0x1F0FF) ||   /* Mahjong / Cards      */
        (uc >= 0x1F1E6 && uc <= 0x1F1FF) ||   /* Regional Indicators  */
        (uc >= 0x2600  && uc <= 0x26FF)  ||   /* Misc Symbols         */
        (uc >= 0x2700  && uc <= 0x27BF);      /* Dingbats             */
}

extern "C" bool emoji_render_color(
    HDC hdc, int x, int y, int w, int h,
    const wchar_t *text, int len,
    int font_height_px, COLORREF fg, COLORREF bg)
{
    static wchar_t for_misc_symbols_add_var_sel_fe0f[2] = { '\x0000', '\x0000' };
    if (!g_rt || !g_dw) return false;

    // {
    //         char abc[300];sprintf(abc, "eee char %ld, %ld, len=%d, w=%d", (*text), (len<2?0:text[1]), len, w);
    //         OutputDebugStringA(abc);
    //     } 
    if (len == 2 && (text[0] >= 0x2600  && text[0] <= 0x26FF || text[0] >= 0x2700  && text[0] <= 0x27BF) && text[1] == 0xFE0E) {
        // Misc Symbols + VarSel Text
        return false;
    }

    /* Convert pixel height to DIPs for DirectWrite */
    int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    if (dpi <= 0) dpi = 96;
    float dip = (float)font_height_px * 96.0f / (float)dpi;

    IDWriteTextFormat *fmt = get_fmt(dip);
    if (!fmt) return false;

    RECT rc = { x, y, x + w, y + h };
    if (FAILED(g_rt->BindDC(hdc, &rc)))
        return false;

    g_rt->BeginDraw();

    /* Fill with background colour (opaque) */
    g_rt->Clear(D2D1::ColorF(
        GetRValue(bg) / 255.0f,
        GetGValue(bg) / 255.0f,
        GetBValue(bg) / 255.0f, 1.0f));

    ID2D1SolidColorBrush *brush = nullptr;
    g_rt->CreateSolidColorBrush(
        D2D1::ColorF(
            GetRValue(fg) / 255.0f,
            GetGValue(fg) / 255.0f,
            GetBValue(fg) / 255.0f, 1.0f), &brush);

    if (brush) {
        
        if (len == 1 && (text[0] >= 0x2600  && text[0] <= 0x26FF || text[0] >= 0x2700  && text[0] <= 0x27BF)) {
            // Misc Symbols
            // Almost wont hit? Misc Symbols most of the time comes with a VarSel
            for_misc_symbols_add_var_sel_fe0f[0] = text[0];
            for_misc_symbols_add_var_sel_fe0f[1] = L'\xDE00';
            text = for_misc_symbols_add_var_sel_fe0f;
            len = 2;
        } else if (len == 2 && (text[0] >= 0x2600  && text[0] <= 0x26FF || text[0] >= 0x2700  && text[0] <= 0x27BF) && text[1] == 0xFE0F) {
            // Misc Symbols + VarSel Emoji
            // go on
        }
        g_rt->DrawText(
            text, len, fmt,
            D2D1::RectF(0.0f, 0.0f, (float)w, (float)h),
            brush,
            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT,
            DWRITE_MEASURING_MODE_NATURAL);
        brush->Release();
        return SUCCEEDED(g_rt->EndDraw());
    }

    return false;
}
