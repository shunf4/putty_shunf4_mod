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
#   Half-width occupy + full-width draw (GNOME Terminal / xterm /
#   wezterm / Alacritty style; NOT Windows Terminal's occupy-2-cells
#   style): the character occupies 1 cell, and its glyph overflows into
#   a blank right cell to draw 2 cells wide.  Overflow is drawing only;
#   it never changes buffer width or cursor column.
#
#   "overflow" — glyph spills into a blank right cell (draw 2 cells).
#   "squeeze"  — glyph compressed into its 1 cell (right neighbour not
#                blank, or no cell to borrow at the right edge).
#   "colour" / "mono" — DirectWrite colour vs monochrome glyph.
#
# Every "expect" line states the NUMBER of glyphs so a vanished emoji
# is easy to spot.
# ─────────────────────────────────────────────────────────────────


# ==================================================================
# 1. Half-width symbols whose glyph is wider than one cell
#    (arrows, Roman numerals, enclosed digits).  No variation selector.
#    Occupy 1 cell; draw 2 cells when the right cell is blank.
# ==================================================================
echo "=== 1. Half-width symbols (no variation selector) ==="
echo "    occupy 1 cell; draw 2 cells (overflow) when right cell blank,"
echo "    else squeeze into 1 cell."
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

# ==================================================================
# 2. VS16 (U+FE0F) — emoji presentation on a HALF-width base.
#    Occupy 1 cell; draw 2 cells (overflow) when the right neighbour
#    is blank, else squeeze.  THIS IS THE CORE RULE.  Bases here are
#    genuinely half-width (East Asian Width Neutral), unlike the Wide
#    emoji in section 4.
# ==================================================================
echo "=== 2. VS16 on half-width base (occupy 1, draw 2 when possible) ==="
echo "    Base characters: U+263A U+2764 U+2639 (all half-width)."
echo
echo "  -- right neighbour is SPACE (overflow expected, colour) --"
printf "  +space:   \U0000263A\U0000FE0F \U00002764\U0000FE0F \U00002639\U0000FE0F\n"
echo "    expect: 3 colour emoji (smiley, heart, frown), each draw 2 cells;"
echo "            the separating spaces are borrowed for overflow, so the 3"
echo "            emoji appear adjacent; 3 glyphs visible, none missing"
echo
echo "  -- right neighbour is SOLID char (squeeze expected, colour) --"
printf "  +solid:   \U0000263A\U0000FE0FX \U00002764\U0000FE0FX \U00002639\U0000FE0FX\n"
echo "    expect: 3 colour emoji each squeezed into 1 cell (small, occupy 1 /"
echo "            draw 1), 'X' right after; 3 emoji + 3 'X' visible, none missing"
echo
echo "  -- at END OF LINE (no cell to borrow) --"
printf "  eol:      \U0000263A\U0000FE0F\n"
echo "    expect: 1 colour emoji at line end; squeezed or clipped at the"
echo "            right edge, NOT blank/black; 1 glyph visible"
echo

# ==================================================================
# 3. VS15 (U+FE0E) — text (monochrome) presentation.
#    Forces a monochrome glyph; VS15 must NOT trigger colour rendering.
#    NOTE on overflow: VS15 does NOT itself grant overflow (only VS16
#    does, or membership in overflow_glyph_chars.h).  So a VS15 base
#    overflows ONLY if the bare base is already an overflow glyph
#    (U+26A0 WARNING is).  U+2639 frown is NOT, so VS15 frown never
#    overflows even with a blank right cell.
# ==================================================================
echo "=== 3. VS15 text presentation (monochrome) ==="
echo "    Bases: U+26A0 (overflow glyph) and U+2639 (not)."
echo
echo "  -- U+26A0 + VS15, right neighbour SPACE (overflow, MONO) --"
printf "  warn+sp:  \U000026A0\U0000FE0E \U000026A0\U0000FE0E\n"
echo "    expect: 2 MONOCHROME warning glyphs, each draw 2 cells (overflow,"
echo "            because U+26A0 is an overflow glyph); NOT colour; 2 glyphs"
echo
echo "  -- U+2639 + VS15, right neighbour SPACE (NO overflow, MONO) --"
printf "  frown+sp: \U00002639\U0000FE0E \U00002639\U0000FE0E\n"
echo "    expect: 2 MONOCHROME frown glyphs, each occupy+draw 1 cell (NO"
echo "            overflow — VS15 does not grant it and U+2639 is not an"
echo "            overflow glyph); NOT colour; 2 glyphs, space remains"
echo
echo "  -- U+26A0 + VS15, right neighbour SOLID (squeeze, MONO) --"
printf "  warn+sd:  \U000026A0\U0000FE0EX \U000026A0\U0000FE0EX\n"
echo "    expect: 2 monochrome warning glyphs squeezed into 1 cell each,"
echo "            'X' after; 2 glyphs + 2 'X'"
echo
echo "  -- at END OF LINE --"
printf "  eol:      \U000026A0\U0000FE0E\n"
echo "    expect: 1 monochrome warning glyph at line end, NOT colour/blank/black"
echo

# ==================================================================
# 4. Bare WARNING sign U+26A0 (the single case retained in
#    overflow_glyph_chars.h).  Half-width; no variation selector.
# ==================================================================
echo "=== 4. Bare WARNING sign (no variation selector) ==="
echo
printf "  +space:   \U000026A0 \U000026A0 \U000026A0\n"
echo "    expect: 3 warning glyphs, each draw 2 cells"
echo
printf "  +solid:   \U000026A0X \U000026A0X\n"
echo "    expect: 2 warning glyphs squeezed into 1 cell each, 'X' after"
echo

# ==================================================================
# 5. Wide (SMP) emoji — East Asian Wide, occupy 2 cells, draw 2 cells.
#    These are DIFFERENT from sections 2-3: wcwidth already returns 2,
#    so there is no "occupy 1 draw 2" possibility.  VS16 on a Wide
#    emoji must NOT collapse it to half-width — it keeps 2 cells.
#    U+2615 HOT BEVERAGE belongs here (it is Wide), NOT in section 2/3.
# ==================================================================
echo "=== 5. Wide (SMP) emoji: occupy 2 cells, draw 2 cells ==="
echo "    U+1F600 U+1F642 U+2615 are all East Asian Wide (wcwidth 2)."
echo
printf "  Wide:        \U0001F600 \U0001F642 \U00002615\n"
echo "    expect: 3 colour emoji (grin, smile, coffee), each occupy+draw 2 cells"
echo
printf "  Wide + VS16: \U0001F600\U0000FE0F \U0001F642\U0000FE0F \U00002615\U0000FE0F\n"
echo "    expect: 3 colour emoji, STILL 2 cells each (VS16 must NOT shrink"
echo "            a Wide emoji to 1 cell); 3 glyphs visible, none missing"
echo
printf "  Wide+solid:  \U0001F600X \U0001F642X\n"
echo "    expect: 2 colour emoji (2 cells each) then 'X'; 2 glyphs + 2 'X'"
echo

# ==================================================================
# 6. Forced-colour emoji (U+3299 CIRCLED SECRET — outside the broad
#    colour blocks; forced via force_color_emoji_chars.h).
# ==================================================================
echo "=== 6. Forced-colour emoji (U+3299) ==="
echo
printf "  forced:  \U00003299 \U00003299 \U00003299\n"
echo "    expect: 3 colour 'SECRET' glyphs (forced colour), each 2 cells"
echo

# ==================================================================
# 7. Right-edge behaviour.  A long run of SYMBOL/emoji (NOT ASCII) is
#    printed so it wraps and the trailing test glyphs land on the
#    terminal's right edge.  This exercises the three combinations at
#    the edge:
#       (a) occupy 1, draw 1  — bare half-width symbol packed tight
#       (b) occupy 1, draw 2  — VS16 half-width emoji, wants overflow
#                               but at the edge there is no cell to borrow
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
echo "=== 7. Right-edge behaviour (symbol/emoji at the terminal edge) ==="
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
echo "  (b) occupy 1 / draw 2 — VS16 half-width emoji at the edge:"
printf ' %s\U0000263A\U0000FE0F\n' "$prefix"
echo "    expect: $EDGE_PREFIX circled-ones then 1 VS16 smiley.  Where the smiley"
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
# 8. Mixed runs — emoji interleaved with ASCII.  This is the case that
#    originally exposed emoji being erased by following characters
#    (when VS16 promoted them to 2-cell wide and the overwrite logic
#    clobbered them).  All three emoji must remain visible.
# ==================================================================
echo "=== 8. Mixed runs (regression: emoji must not vanish) ==="
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
# 9. Composed emoji.
#    ZWJ sequences and skin-tone modifiers are NOT expected to work
#    (PuTTY's wcwidth does not model them — see utf8.txt).  Regional
#    flags ARE expected to work.
# ==================================================================
echo "=== 9. Composed emoji ==="
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
