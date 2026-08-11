/*
 * Extra code points (beyond the broad emoji blocks already handled
 * inside emoji_should_render_color() in windows/emoji_render.cpp)
 * that should be rendered as colour emoji even if they would otherwise
 * appear in a default text (monochrome) form.  E.g. U+3299 CIRCLED
 * IDEOGRAPH SECRET, used as an emoji in Japanese text.
 *
 * emoji_render.cpp inlines the same intervals so that the .cpp TU
 * needs no extra include path; keep this list and that inline table
 * in sync.
 *
 * "Colour" is independent of width: whether a character occupies one
 * or two cells is decided elsewhere (wcwidth / overflow), and this
 * table only decides that the DirectWrite colour renderer is used
 * rather than a monochrome font glyph.  A trailing VS15 (U+FE0E)
 * still overrides this and selects the text presentation instead.
 *
 * Manually maintained — not auto-generated from UCD data.
 */

/* CIRCLED IDEOGRAPH SECRET */
{0x3299, 0x3299},
