/*
 * Color emoji rendering via DirectWrite + Direct2D.
 * Pure C header — the implementation lives in emoji_render.cpp.
 */

#ifndef EMOJI_RENDER_H
#define EMOJI_RENDER_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Call once at startup (idempotent).  Returns false if Direct2D /
 * DirectWrite are unavailable — caller should just skip color emoji. */
bool emoji_renderer_init(void);

/* Call at shutdown. */
void emoji_renderer_cleanup(void);

/* Supply an ordered list of font family names for the emoji renderer to
 * try.  Must be called after emoji_renderer_init().  The pointer array
 * and the strings it points to must remain valid until cleanup. */
void emoji_renderer_set_fonts(const WCHAR *const *names, int count);

/* Should this code point be rendered as colour emoji?  Binary-searches
 * the sorted interval table in unicode/force_color_emoji_chars.h (the
 * broad emoji blocks plus a few default-text code points forced to
 * colour).  A trailing VS15 (U+FE0E) overrides this and selects text
 * presentation. */
bool emoji_should_render_color(unsigned int uc);

/* Render |text[0..len-1]| (UTF-16) as a color emoji onto |hdc|.
 * (x,y) = top-left of the render target; w×h = render target size
 * (may be larger than the cell to allow overflow).
 * cell_w = original cell width; background is painted only here.
 * emoji_size_px = glyph size in pixels.  Because emoji glyphs are
 *   roughly square, the caller passes the desired *width* here —
 *   DirectWrite scales the glyph to this value in both dimensions.
 * Returns true on success. */
bool emoji_render_color(HDC hdc, int x, int y, int w, int h,
                        int cell_w,
                        const wchar_t *text, int len,
                        int emoji_size_px, COLORREF fg, COLORREF bg);

#ifdef __cplusplus
}
#endif

#endif /* EMOJI_RENDER_H */
