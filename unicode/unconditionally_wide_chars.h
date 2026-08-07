/*
 * Unicode characters that are East Asian Ambiguous (width class A) but
 * should unconditionally occupy two character cells in a terminal,
 * regardless of the CJK ambiguous-wide setting.
 *
 * This is a subset of ambiguous_wide_chars.h, excluding characters
 * that are critical for TUI applications (box-drawing, block elements,
 * mathematical operators, etc.).
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
