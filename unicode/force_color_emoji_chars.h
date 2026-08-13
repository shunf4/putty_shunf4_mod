/*
 * Code points rendered as colour emoji by emoji_should_render_color()
 * in windows/emoji_render.cpp.  The table is sorted ascending so that
 * emoji_render.cpp can binary-search it, in the same style as
 * unicode/overflow_glyph_chars.h.
 *
 * It holds the broad emoji blocks (U+2600-U+26FF Misc Symbols,
 * U+2700-U+27BF Dingbats, and the SMP emoji U+1F000-U+1FAFF) plus a few
 * default-text code points forced to colour (e.g. U+3299 CIRCLED
 * IDEOGRAPH SECRET, used as an emoji in Japanese text).
 *
 * "Colour" is independent of width: whether a character occupies one
 * or two cells is decided elsewhere (wcwidth / overflow), and this
 * table only decides that the DirectWrite colour renderer is used
 * rather than a monochrome font glyph.  A trailing VS15 (U+FE0E)
 * still overrides this and selects the text presentation instead.
 *
 * The half-width (BMP) ranges below are mirrored in
 * unicode/overflow_glyph_chars.h so that a code point drawn in colour
 * also gets its full-width glyph overflowed into a blank right-hand
 * cell.  Wide colour emoji (U+3299 and the SMP blocks) already occupy
 * two cells and need no overflow.
 *
 * Manually maintained — not auto-generated from UCD data.
 */

/* Misc Symbols */
{0x2600, 0x26ff},
/* Dingbats */
{0x2700, 0x27bf},
/* CIRCLED IDEOGRAPH SECRET */
{0x3299, 0x3299},
/* Mahjong Tiles / Playing Cards */
{0x1f000, 0x1f0ff},
/* Regional Indicator Symbols */
{0x1f1e6, 0x1f1ff},
/* Miscellaneous Symbols and Pictographs */
{0x1f300, 0x1f5ff},
/* Emoticons */
{0x1f600, 0x1f64f},
/* Transport and Map Symbols */
{0x1f680, 0x1f6ff},
/* Supplemental Symbols and Pictographs */
{0x1f900, 0x1f9ff},
/* Symbols and Pictographs Extended-A */
{0x1fa00, 0x1faff},
