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

/* Heuristic: is this Unicode codepoint likely to be a color-emoji glyph? */
bool emoji_is_color_candidate(unsigned int uc);

/* Render |text[0..len-1]| (UTF-16) as a color emoji onto |hdc|.
 * (x,y) = top-left of the destination cell, w×h = cell size.
 * font_height_px = terminal font height in pixels (used to size the emoji).
 * bg = background colour to fill behind the glyph.
 * Returns true on success. */
bool emoji_render_color(HDC hdc, int x, int y, int w, int h,
                        const wchar_t *text, int len,
                        int font_height_px, COLORREF fg, COLORREF bg);

#ifdef __cplusplus
}
#endif

#endif /* EMOJI_RENDER_H */
