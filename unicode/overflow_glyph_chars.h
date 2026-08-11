/*
 * Characters that occupy ONE character cell (half-width) in the
 * terminal buffer, but whose glyph is designed wide and may be
 * rendered overflowing into a neighbouring blank cell.
 *
 * These are mostly East Asian Ambiguous (width class A) symbols
 * whose typographic glyph is full-width, plus default-text-style
 * emoji.  Keeping them half-width in the buffer avoids breaking
 * TUI applications (e.g. Claude Code) that assume width 1; the
 * renderer is then allowed to draw the full-width glyph spilling
 * into a blank cell on the right when one is available.
 *
 * This table controls ONLY the "is this an overflow candidate"
 * question.  Whether overflow actually happens for a given cell is
 * decided at render time by looking ahead to the right neighbour
 * (blank cell or end-of-line margin -> overflow; otherwise the
 * glyph is squeezed into the single cell).
 *
 * Manually maintained — not auto-generated from UCD data.
 */

/* Roman Numerals: Ⅰ Ⅱ Ⅲ Ⅳ Ⅴ Ⅵ Ⅶ Ⅷ Ⅸ Ⅹ Ⅺ Ⅻ */
{0x2160, 0x216b},
/* Roman Numerals (lowercase): ⅰ ⅱ ⅲ ⅳ ⅴ ⅵ ⅶ ⅷ ⅸ */
{0x2170, 0x2179},
/* Roman Numeral Reversed One Hundred */
{0x2189, 0x2189},
/* Arrows: ← → ↑ ↓ ↔ ↕ ↖ ↗ ↘ ↙ */
{0x2190, 0x2199},
/* Enclosed Alphanumerics: ① ② ... ⑳, ⓫ ⓬ ... ⓿ */
{0x2460, 0x24ff},
/*
 * Default-text-presentation emoji that are half-width (East Asian
 * Width Neutral, so width 1) yet whose monochrome glyph is wider than
 * a single cell.  WARNING SIGN (U+26A0) is the canonical example and
 * kept here as a bare-glyph overflow test case: in a non-colour-emoji
 * build it is drawn with the main font and benefits from overflowing
 * into a blank right-hand cell.  Emoji that are already East Asian
 * Wide (e.g. U+26A1, U+2705) are NOT listed here because they occupy
 * two cells already and need no overflow.
 *
 * Emoji that gain full-width presentation via VS16 (U+FE0F) are NOT
 * listed here either: their overflow is granted by the "this cell
 * carries a VS16 combining mark" rule in terminal.c's do_paint, not
 * by this table.  In the colour-emoji branch all such glyphs are drawn
 * by the DirectWrite colour path, which has its own overflow handling.
 */
{0x26a0, 0x26a0},   /* ⚠ WARNING SIGN */
