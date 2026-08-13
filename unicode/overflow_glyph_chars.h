/*
 * Characters that occupy ONE character cell (half-width) in the
 * terminal buffer, but whose glyph is designed wide and may be
 * rendered overflowing into a neighbouring blank cell.
 *
 * These are mostly East Asian Ambiguous (width class A) symbols
 * whose typographic glyph is full-width, plus half-width colour-emoji
 * code points.  Keeping them half-width in the buffer avoids breaking
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
/* Black circle / circle: default-text overflow glyphs (not colour). */
{0x25cf, 0x25cf},
{0x25ef, 0x25ef},
/*
 * Half-width colour-emoji ranges, mirrored from the BMP entries of
 * unicode/force_color_emoji_chars.h: a code point drawn in colour
 * generally wants its full-width glyph overflowed into a blank
 * right-hand cell.  Characters inside these ranges that are already
 * East Asian Wide (e.g. U+26A1 HIGH VOLTAGE, U+2705 WHITE HEAVY CHECK
 * MARK) occupy two cells already; terminal.c's (tattr & ATTR_WIDE)==0
 * guard keeps them from being treated as overflow candidates.  Wide
 * colour emoji outside the BMP (U+3299 and the SMP blocks) likewise
 * need no overflow and are not listed.
 */
/* Misc Symbols */
{0x2600, 0x26ff},
/* Dingbats */
{0x2700, 0x27bf},
