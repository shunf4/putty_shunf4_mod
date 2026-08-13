#!/bin/bash
# Character-classification & rendering test for pterm's width/emoji
# handling.
#
# Run this inside the terminal under test:
#   bash test/test_chars.sh
#
# Uses bash printf with \u (BMP) and \U (full) escapes.  Force UTF-8 so
# the escapes expand to UTF-8 bytes rather than the locale's charset.
export LC_ALL=C.UTF-8 2>/dev/null || export LC_ALL=en_US.UTF-8

# ─────────────────────────────────────────────────────────────────
# Conventions used in the "expect" notes below:
#
#   "occupy N cell(s)"  — buffer width / cursor advance.
#   "draw N cell(s)"    — visible glyph width.
#
#   "overflow" — glyph spills into a blank right cell (draw 2 cells).
#   "squeeze"  — glyph compressed into its 1 cell (right neighbour not
#                blank, or no cell to borrow at the right edge).
#   "colour" / "mono" — DirectWrite colour vs monochrome glyph.
#
#   Wide (East Asian Wide) characters occupy 2 cells and NEVER overflow;
#   the VS selector only decides colour vs monochrome, never width.
#   Half-width characters occupy 1 cell; whether they overflow is decided
#   by (a) membership in overflow_glyph_chars.h, or (b) a VS16 combining
#   mark.  VS15 never grants overflow but does NOT suppress it either.
#
# Every "expect" line states the NUMBER of glyphs so a vanished emoji
# is easy to spot.
# ─────────────────────────────────────────────────────────────────


# ==================================================================
# 1. Half-width NON-colour overflow symbols (arrows, Roman numerals,
#    enclosed digits, black circle).  No variation selector.  These are
#    in overflow_glyph_chars.h but NOT in the colour table, so they
#    render MONOCHROME and overflow when the right cell is blank.
# ==================================================================
echo "=== 1. Half-width non-colour overflow symbols (mono) ==="
echo "    occupy 1 cell; draw 2 cells (overflow) when right cell blank,"
echo "    else squeeze into 1 cell.  MONOCHROME (not in colour table)."
echo
printf "  Arrows +space:     \U00002190 \U00002192 \U00002191 \U00002193\n"
echo "    expect: 4 arrows, each draw 2 cells (overflow into the space)"
echo
printf "  Arrows tight:      \U00002190\U00002192\U00002191\U00002193X\n"
echo "    expect: 4 arrows each squeezed into 1 cell, then 'X'; total 5 cells"
echo
printf "  Roman +space:      \U00002160 \U00002161 \U00002162\n"
echo "    expect: 3 glyphs (I II III), each draw 2 cells"
echo
printf "  Circled +space:    \U00002460 \U00002461 \U00002462\n"
echo "    expect: 3 circled digits (1 2 3), each draw 2 cells"
echo
printf "  Circled tight:     \U00002460\U00002461\U00002462X\n"
echo "    expect: 3 circled digits squeezed into 1 cell each, then 'X'"
echo
printf "  Black circle +sp:  \U000025CF \U000025CF\n"
echo "    expect: 2 MONOCHROME black circles, each draw 2 cells (overflow,"
echo "            NOT colour — U+25CF is not in the colour table)"
echo

# ==================================================================
# 2. Half-width COLOUR symbols (smiley U+263A, frown U+2639,
#    heart U+2764, warning U+26A0).  These are in BOTH the colour table
#    AND overflow_glyph_chars.h, so they overflow with OR without VS16.
#    The full VS x right-neighbour matrix is exercised below.
# ==================================================================
echo "=== 2. Half-width colour symbols (colour + overflow) ==="
echo "    Bases: U+263A U+2764 U+2639 U+26A0 (all half-width, colour,"
echo "          and now overflow glyphs)."
echo

echo "  -- (no VS, right = SPACE): colour + overflow --"
printf "  bare+sp:  \U0000263A \U00002764 \U00002639 \U000026A0\n"
echo "    expect: 4 COLOUR glyphs (smiley, heart, frown, warning), each draw"
echo "            2 cells (overflow into the separating spaces); NOT mono"
echo
echo "  -- (no VS, right = SOLID): colour + squeeze --"
printf "  bare+sd:  \U0000263AX \U00002764X \U00002639X\n"
echo "    expect: 3 COLOUR glyphs each squeezed into 1 cell, 'X' right after;"
echo "            NOT mono; 3 glyphs + 3 'X' visible"
echo
echo "  -- (VS16, right = SPACE): colour + overflow --"
printf "  vs16+sp:  \U0000263A\U0000FE0F \U00002764\U0000FE0F \U00002639\U0000FE0F\n"
echo "    expect: 3 colour emoji, each draw 2 cells (overflow into the space);"
echo "            the separating spaces are borrowed, so the emoji look adjacent"
echo
echo "  -- (VS16, right = SOLID): colour + squeeze --"
printf "  vs16+sd:  \U0000263A\U0000FE0FX \U00002764\U0000FE0FX \U00002639\U0000FE0FX\n"
echo "    expect: 3 colour emoji each squeezed into 1 cell, 'X' right after"
echo
echo "  -- (VS15, right = SPACE): MONO + overflow (still an overflow glyph) --"
printf "  vs15+sp:  \U0000263A\U0000FE0E \U000026A0\U0000FE0E\n"
echo "    expect: 2 MONOCHROME glyphs (smiley, warning), each draw 2 cells."
echo "            VS15 forces text/monochrome but does NOT suppress overflow"
echo "            because the base is in overflow_glyph_chars.h"
echo
echo "  -- (VS15, right = SOLID): MONO + squeeze --"
printf "  vs15+sd:  \U0000263A\U0000FE0EX \U000026A0\U0000FE0EX\n"
echo "    expect: 2 monochrome glyphs squeezed into 1 cell each, 'X' after"
echo
echo "  -- at END OF LINE (no cell to borrow) --"
printf "  eol-bare: \U0000263A\n"
printf "  eol-vs16: \U0000263A\U0000FE0F\n"
printf "  eol-vs15: \U000026A0\U0000FE0E\n"
echo "    expect: each line's trailing glyph is squeezed/clipped at the right"
echo "            edge, NOT blank/black; colour where no VS15, mono for VS15"
echo

# ==================================================================
# 3. Wide (East Asian Wide) colour emoji — occupy 2 cells, draw 2 cells,
#    NEVER overflow.  U+2615 HOT BEVERAGE, U+26A1 HIGH VOLTAGE,
#    U+2705 WHITE HEAVY CHECK, and SMP U+1F600 are all wcwidth 2.
#    The VS selector only picks colour vs mono; width stays 2.
# ==================================================================
echo "=== 3. Wide colour emoji (occupy 2, draw 2, never overflow) ==="
echo "    U+1F600 U+2615 U+26A1 U+2705 are East Asian Wide (wcwidth 2)."
echo
printf "  Wide:        \U0001F600 \U00002615 \U000026A1 \U00002705\n"
echo "    expect: 4 colour emoji (grin, coffee, bolt, check), each 2 cells"
echo
printf "  Wide + VS16: \U0001F600\U0000FE0F \U00002615\U0000FE0F\n"
echo "    expect: 2 colour emoji, STILL 2 cells each (VS16 must NOT shrink"
echo "            a Wide emoji to 1 cell); 2 glyphs, none missing"
echo
printf "  Wide + VS15: \U00002615\U0000FE0E \U000026A1\U0000FE0E\n"
echo "    expect: 2 MONOCHROME emoji, STILL 2 cells each (VS15 forces text/"
echo "            monochrome but width stays 2); NOT colour"
echo
printf "  Wide+solid:  \U0001F600X \U0001F642X\n"
echo "    expect: 2 colour emoji (2 cells each) then 'X'; 2 glyphs + 2 'X'"
echo

# ==================================================================
# 4. Forced-colour emoji (U+3299 CIRCLED SECRET — outside the broad
#    colour blocks; forced via force_color_emoji_chars.h).  U+3299 is
#    East Asian Wide, so it occupies 2 cells and does not overflow.
# ==================================================================
echo "=== 4. Forced-colour emoji (U+3299, wide) ==="
echo
printf "  forced:  \U00003299 \U00003299 \U00003299\n"
echo "    expect: 3 colour 'SECRET' glyphs (forced colour), each 2 cells;"
echo "            wide so NO overflow"
echo

# ==================================================================
# 5. Right-edge behaviour.  A long run of SYMBOL/emoji (NOT ASCII) is
#    printed so it wraps and the trailing test glyphs land on the
#    terminal's right edge.  This exercises the three combinations at
#    the edge:
#       (a) occupy 1, draw 1  — bare half-width symbol packed tight
#       (b) occupy 1, draw 2  — half-width emoji, wants overflow but at
#                               the edge there is no cell to borrow
#       (c) occupy 2, draw 2  — Wide emoji straddling the edge
#    The prefix is EDGE_PREFIX copies of U+2460 circled-one (occupy 1,
#    draw 1 when packed).  EDGE_PREFIX defaults to 230 (≈3× a 76-col
#    line) so the run wraps and the trailing glyph hits the right edge
#    on common widths (80-200 cols).  Override via the environment, or
#    set it to (your_width - 3) to land the test glyph on the last
#    column of a single line.
# ==================================================================
EDGE_PREFIX=${EDGE_PREFIX:-230}
# Build EDGE_PREFIX copies of U+2460 via a bash arithmetic loop (no
# external seq, no word-splitting).
circled_prefix_array=()
_i=0
while [ "$_i" -lt "$EDGE_PREFIX" ]; do
    circled_prefix_array+=(x)
    _i=$((_i + 1))
done
prefix=$(printf '\U00002460%.0s' "${circled_prefix_array[@]}")
echo "=== 5. Right-edge behaviour (symbol/emoji at the terminal edge) ==="
echo "    Prefix = $EDGE_PREFIX x circled-one (occupy 1, draw 1), 2-col indent."
echo "    Long enough to wrap on common widths so the trailing glyph hits the"
echo "    right edge.  Set EDGE_PREFIX=<cols-3> to land it on the last column."
echo
echo "  (a) occupy 1 / draw 1 — bare symbol packed at the edge (no VS):"
printf '  %s\n' "$prefix"
echo "    expect: $EDGE_PREFIX circled-ones (occupy+draw 1 each); where the run"
echo "            meets the right edge the glyphs squeeze/clip cleanly, NOT black;"
echo "            the line wraps to the next row"
echo
echo "  (b) occupy 1 / draw 2 — half-width emoji at the edge:"
printf ' %s\U0000263A\U0000FE0F\n' "$prefix"
echo "    expect: $EDGE_PREFIX circled-ones then 1 smiley.  Where the smiley"
echo "            lands at the right edge it wants overflow but has no cell to"
echo "            borrow, so it squeezes or clips — NOT blank/black;"
echo "            $((EDGE_PREFIX+1)) glyphs, 1 emoji; line wraps"
echo
echo "  (c) occupy 2 / draw 2 — Wide emoji straddling the edge:"
printf ' %s\U0001F600\n' "$prefix"
echo "    expect: $EDGE_PREFIX circled-ones then 1 Wide grin emoji (2 cells)."
echo "            Where the grin meets the right edge it wraps to the next row or"
echo "            clips — NOT split into two halves / not blank; the last circled-ones is drawn full."
echo "            $((EDGE_PREFIX+1)) glyphs, 1 emoji"
echo
echo

# ==================================================================
# 6. Mixed runs — emoji interleaved with ASCII.  This is the case that
#    originally exposed emoji being erased by following characters.
#    All emoji must remain visible.
# ==================================================================
echo "=== 6. Mixed runs (regression: emoji must not vanish) ==="
echo
printf "  \U0000263A\U0000FE0F a \U00002764\U0000FE0F b \U00002639\U0000FE0F c\n"
echo "    expect: smiley, 'a', heart, 'b', frown, 'c' — 3 emoji + 3 letters."
echo "            Each emoji's right neighbour is a space, so each overflows and"
echo "            borrows that space; the letter follows in the next cell."
echo "            ALL 3 emoji visible (none erased by the following ASCII)"
echo
printf "  \U0000263A\U0000FE0F\U00002764\U0000FE0F\U00002639\U0000FE0F\n"
echo "    expect: 3 emoji back-to-back, NO separating space.  The FIRST two"
echo "            (smiley, heart) have another emoji as right neighbour -> cannot"
echo "            overflow -> each occupies 1 / draws 1 cell (squeezed, small)."
echo "            The LAST (frown) has the line's trailing blank as right"
echo "            neighbour -> it overflows to draw 2 cells.  So: 2 squeezed +"
echo "            1 full-width.  All 3 glyphs must remain present (none erased)"
echo

# ==================================================================
# 7. Composed emoji.
#    ZWJ sequences and skin-tone modifiers are NOT expected to work
#    (PuTTY's wcwidth does not model them — see utf8.txt).  Regional
#    flags ARE expected to work.
# ==================================================================
echo "=== 7. Composed emoji ==="
echo
printf "  ZWJ:       \U0001F469\U0000200D\U0001F4BB\n"
echo "    expect (KNOWN FAILURE): NOT rendered as a single woman-technologist"
echo "            ligature; wcwidth does not understand ZWJ, so this shows as"
echo "            separate woman + laptop glyphs (2 + 2 cells) or similar"
echo
printf "  Skin tone: \U0001F469\U0001F3FB \U0001F469\U0001F3FF\n"
echo "    expect (KNOWN FAILURE): skin-tone modifiers are NOT applied;"
echo "            wcwidth does not recognise them, so this shows as woman +"
echo "            detached modifier squares (2 cells each), not toned faces"
echo
printf "  Flags:     \U0001F1EC\U0001F1E7 \U0001F1FA\U0001F1E6 \U0001F1EA\U0001F1FA\n"
echo "    expect (SHOULD PASS): 3 regional flag glyphs (GB US EU), each 2 cells"
echo
printf "  Flag tags:\U0001F3F4\U000E0067\U000E0062\U000E0065\U000E006E\U000E0067\U000E007F\n"
echo "    expect (KNOWN LIMITATION): tag characters are treated as combining"
echo "            marks, so a tagged flag (England) does NOT ligate into one"
echo "            flag glyph; shows as a base flag + trailing tag marks"
