/*
 * Implementation of FontSpec for Windows.
 */

#include "putty.h"

FontSpec *fontspec_new(const char *name, bool bold, int height, int charset)
{
    FontSpec *f = snew(FontSpec);
    f->name = dupstr(name);
    f->isbold = bold;
    f->height = height;
    f->charset = charset;
    return f;
}

FontSpec *fontspec_new_default(void)
{
    return fontspec_new("", false, 0, 0);
}

FontSpec *fontspec_new_from_override(const char *value)
{
    /*
     * Parse a font override string in the format:
     *   FontName[,Height[,Bold[,Charset]]]
     * Omitted fields use defaults. The font name may contain spaces.
     */
    const char *comma = strchr(value, ',');
    char *name;
    int height = 10, bold = 0, charset = ANSI_CHARSET;

    if (!comma) {
        /* Just a font name, no extras */
        return fontspec_new(value, false, height, charset);
    }

    name = dupprintf("%.*s", (int)(comma - value), value);
    const char *p = comma + 1;

    /* Height */
    height = atoi(p);
    comma = strchr(p, ',');
    if (!comma) {
        FontSpec *f = fontspec_new(name, bold, height, charset);
        sfree(name);
        return f;
    }
    p = comma + 1;

    /* Bold */
    bold = atoi(p);
    comma = strchr(p, ',');
    if (!comma) {
        FontSpec *f = fontspec_new(name, bold, height, charset);
        sfree(name);
        return f;
    }
    p = comma + 1;

    /* Charset */
    charset = atoi(p);

    FontSpec *f = fontspec_new(name, bold, height, charset);
    sfree(name);
    return f;
}

FontSpec *fontspec_copy(const FontSpec *f)
{
    return fontspec_new(f->name, f->isbold, f->height, f->charset);
}

void fontspec_free(FontSpec *f)
{
    sfree(f->name);
    sfree(f);
}

void fontspec_serialise(BinarySink *bs, FontSpec *f)
{
    put_asciz(bs, f->name);
    put_uint32(bs, f->isbold);
    put_uint32(bs, f->height);
    put_uint32(bs, f->charset);
}

FontSpec *fontspec_deserialise(BinarySource *src)
{
    const char *name = get_asciz(src);
    unsigned isbold = get_uint32(src);
    unsigned height = get_uint32(src);
    unsigned charset = get_uint32(src);
    return fontspec_new(name, isbold, height, charset);
}
