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
 *
 * Membership rule: only add a code point if its East Asian Width is
 * NOT W/F (so mk_wcwidth() reports 1; W/F characters are wide in the
 * buffer already and need no overflow) and its conventional glyph is
 * a full-cell pictorial symbol, not a letter/digit/latin-alike whose
 * narrow silhouette would look odd when expanded.  Keep the intervals
 * in strictly ascending, non-overlapping order: utils/wcwidth.c
 * binary-searches them and assumes table[0] is the lowest interval.
 */

/* INFORMATION SOURCE ℹ (default text presentation, colour via VS16) */
{0x2139, 0x2139},
/* Roman Numerals: Ⅰ Ⅱ Ⅲ Ⅳ Ⅴ Ⅵ Ⅶ Ⅷ Ⅸ Ⅹ Ⅺ Ⅻ */
{0x2160, 0x216b},
/* Roman Numerals (lowercase): ⅰ ⅱ ⅲ ⅳ ⅴ ⅵ ⅶ ⅷ ⅸ */
{0x2170, 0x2179},
/* Roman Numeral Reversed One Hundred */
{0x2189, 0x2189},
/* Arrows: ← → ↑ ↓ ↔ ↕ ↖ ↗ ↘ ↙, ↳ */
{0x2190, 0x2199},
{0x21b3, 0x21b3},
/*
 * Miscellaneous Technical media-control pictograms: ⏏ ⏭ ⏮ ⏯ ⏸ ⏹ ⏺.
 * These are Emoji=Yes but Emoji_Presentation=No and East Asian Width
 * Neutral, so mk_wcwidth() gives them one cell; their conventional
 * glyph, however, is a full-cell eject/skip/pause/stop/record disc or
 * triangle, which reads best overflowing into a blank right-hand
 * cell.  The Wide members of the same set (⏩ ⏪ ⏫ ⏬ U+23E9-23EC,
 * ⏰ U+23F0, ⏳ U+23F3) are in unicode/wide_chars.h and occupy two
 * cells already, so they are deliberately not listed here.
 *
 * This is the block that used to be missing PAUSE U+23F8; the whole
 * media transport family is included so playback indicators get the
 * same treatment.
 */
{0x23cf, 0x23cf},
{0x23ed, 0x23ef},
{0x23f8, 0x23fa},
/* Enclosed Alphanumerics: ① ② ... ⑳, ⓫ ⓬ ... ⓿ */
{0x2460, 0x24ff},
/*
 * Geometric Shapes.  Members below are East Asian Ambiguous (width
 * 1) and conventionally drawn as a square, circle, triangle, diamond
 * or similar full-cell pictogram, which looks natural overflowing
 * into a blank right-hand cell.
 *
 * The previous table omitted several of these; the gaps that are now
 * filled in (all width 1) are:
 *   U+25B8-25BB  ▸ ▹ ► ▻   right-pointing triangles
 *   U+25BD-25BF  ▽ ▾ ▿      down-pointing triangles
 *   U+25C2-25C5  ◂ ◃ ◄ ◅    left-pointing triangles
 *   U+25C6-25C7  ◆ ◇        diamonds
 *   U+25CA       ◊          lozenge
 *   U+25CE       ◎          bullseye
 *   U+25D8-25DB  ◘ ◙ ◚ ◛    inverse bullets, half circles
 *   U+25DC-25E6  ◜ ◝ ◞ ◟ ◠ ◡ ◢ ◣ ◤ ◥ ◦   arc quadrants, white circle
 *
 * NOT included, on purpose:
 *   U+25AA-25AB  ▪ ▫  - "small" square/bullet marks whose whole point
 *                is to be smaller than a cell; stretching is wrong.
 *   U+25AC       ▬    - horizontal bar; a 2x stretch distorts it.
 *   U+25AD       ▭    - white rectangle; same.
 *   U+25AE-25B1  ▮ ▯ ▰ ▱ - vertical/quadrant-ratio bars.  ▮/▯ are
 *                commonly used as full-height progress bars, and a
 *                horizontal stretch distorts their ratio.
 *   U+25B3-25B5  △ ▴ ▵ - the outline and small variants of ▲ U+25B2,
 *                which IS listed.  Kept out for now (borderline);
 *                add them if a real use case appears.
 *   U+25FD-25FE  ◽ ◾ - inside the merged U+25E7-25FF interval below,
 *                but they are East Asian Wide (unicode/wide_chars.h),
 *                so they occupy two cells and terminal.c's ATTR_WIDE
 *                guard never treats them as overflow candidates.
 *                (For the same reason U+25E7-25EF is not split out;
 *                only those two code points in the merged range are
 *                Wide and they are harmless.)
 *
 * Intervals must stay ascending for the binary search in
 * utils/wcwidth.c; the old {0x25e7,0x25fc} + {0x25ef,0x25ff} split
 * overlapped, so it is merged into one interval here.
 */
{0x25a0, 0x25a9},
{0x25b2, 0x25b2},
{0x25b6, 0x25b7},
{0x25b8, 0x25bb},
{0x25bc, 0x25bc},
{0x25bd, 0x25bf},
{0x25c0, 0x25c1},
{0x25c2, 0x25c5},
{0x25c6, 0x25c7},
{0x25c8, 0x25c9},
{0x25ca, 0x25ca},
{0x25cb, 0x25cd},
{0x25ce, 0x25ce},
{0x25cf, 0x25cf},
{0x25d0, 0x25d7},
{0x25d8, 0x25db},
{0x25dc, 0x25e6},
{0x25e7, 0x25ff},
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
