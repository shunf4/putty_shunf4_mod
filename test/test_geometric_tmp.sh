#!/bin/bash
# Temporary visual test for the Geometric Shapes overflow set
# (unicode/overflow_glyph_chars.h).  Run inside the terminal under test:
#   bash test/test_geometric_tmp.sh
#
# Row "A" prints each glyph followed by a SPACE -> overflow expected
# (full-width glyph spilling into the blank right-hand cell).
# Row "B" prints the glyphs back-to-back followed by X -> squeeze expected.
# Row "C" prints a glyph at the last column -> squeeze (no right margin).

# Print one code point as raw UTF-8 bytes (no reliance on printf \u,
# which breaks in the C locale).  Works for U+0800..U+FFFF.
cp() { # cp <hex-without-0x>
    local n=$((16#$1))
    printf "\\$(printf '%03o' $((0xE0 | (n >> 12))))\\$(printf '%03o' $((0x80 | ((n >> 6) & 0x3F))))\\$(printf '%03o' $((0x80 | (n & 0x3F))))"
}

# Every code point listed in overflow_glyph_chars.h, in table order
# (23CF/23ED-23EF/23F8-23FA media control, then Geometric Shapes).
# U+25FD/U+25FE are deliberately absent: they are East Asian Wide, so
# they occupy two cells and never take the overflow path.
codes=(
    23cf 23ed 23ee 23ef 23f8 23f9 23fa
    25a0 25a1 25a2 25a3 25a4 25a5 25a6 25a7 25a8 25a9
    25b2
    25b6 25b7 25b8 25b9 25ba 25bb
    25bc 25bd 25be 25bf
    25c0 25c1 25c2 25c3 25c4 25c5 25c6 25c7
    25c8 25c9
    25ca
    25cb 25cc 25cd
    25ce
    25cf
    25d0 25d1 25d2 25d3 25d4 25d5 25d6 25d7
    25d8 25d9 25da 25db
    25dc 25dd 25de 25df 25e0 25e1 25e2 25e3 25e4 25e5 25e6
    25e7 25e8 25e9 25ea 25eb 25ec 25ed 25ee 25ef 25f0 25f1 25f2 25f3
    25f4 25f5 25f6 25f7 25f8 25f9 25fa 25fb 25fc
    25ff
)

echo "A) glyph + space (expect: full-width, overflowing into the space):"
for c in "${codes[@]}"; do
    printf '%s ' "$c"
    cp "$c"
    printf ' |\n'
done

echo
echo "B) glyphs then X row (expect: squeezed to one cell, X aligned under each):"
for c in "${codes[@]}"; do
    cp "$c"
done
echo
for c in "${codes[@]}"; do
    printf 'X'
done
echo

echo
echo "C) glyph at last column (expect: squeezed, no right-margin spill):"
# Fill to width-1 then print one glyph; adjust 78 if your terminal is wider
printf '%.0sA' {1..78}
cp 25cf
echo

echo
echo "D) scroll test: repeat rows and scroll -- no ghosts at the spaces:"
for i in 1 2 3; do
    for c in 25a0 25b2 25bc 25cf 25ef; do
        cp "$c"
        printf ' '
    done
    echo "row $i"
done
