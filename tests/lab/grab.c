/* Grab: the front screen, or a part of it, as a PNG file, for showing a
 * lab's screens in a README (Cradle's screenshot tool returns the picture
 * to the caller only). Reads the screen's pixels through
 * cybergraphics.library (RTG screens); libpng and zlib from the stove.
 * A picture of 256 colours or fewer is written with a palette (a game's
 * frame is a few kilobytes then, small enough to fetch from a lab).
 *   Grab FILE [X Y W H]   (default: the whole front screen)
 * MIT, Copyright (c) 2026 AmiCradle Team. */
#include <exec/types.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>
#include <cybergraphx/cybergraphics.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>

struct Library *CyberGfxBase;

/* the picture's colours, up to 256, as a palette; 0 when there are more */
static int palette_of(const unsigned char *rgb, long n, png_color *pal, unsigned char *idx)
{
    int count = 0;
    long i;
    for (i = 0; i < n; i++) {
        const unsigned char *p = rgb + i * 3;
        int k;
        for (k = 0; k < count; k++)
            if (pal[k].red == p[0] && pal[k].green == p[1] && pal[k].blue == p[2])
                break;
        if (k == count) {
            if (count == 256)
                return 0;
            pal[count].red = p[0];
            pal[count].green = p[1];
            pal[count].blue = p[2];
            count++;
        }
        idx[i] = (unsigned char)k;
    }
    return count;
}

int main(int argc, char **argv)
{
    struct Screen *s;
    ULONG lock;
    long x = 0, y = 0, w, h;
    unsigned char *buf;
    FILE *out;
    png_structp png;
    png_infop info;
    long row;
    png_color pal[256];
    unsigned char *idx;
    int colours;
    if (argc != 2 && argc != 6) {
        fprintf(stderr, "usage: Grab FILE [X Y W H]\n");
        return 20;
    }
    if (!(CyberGfxBase = OpenLibrary((STRPTR)"cybergraphics.library", 40))) {
        fprintf(stderr, "Grab: no cybergraphics.library\n");
        return 20;
    }
    lock = LockIBase(0);
    s = IntuitionBase->FirstScreen;
    UnlockIBase(lock);
    w = s->Width;
    h = s->Height;
    if (argc == 6) {
        x = atol(argv[2]);
        y = atol(argv[3]);
        w = atol(argv[4]);
        h = atol(argv[5]);
    }
    if (x < 0 || y < 0 || w < 1 || h < 1 || x + w > s->Width || y + h > s->Height) {
        fprintf(stderr, "Grab: %ld,%ld %ldx%ld is not inside the %dx%d screen\n", x, y, w, h, s->Width, s->Height);
        CloseLibrary(CyberGfxBase);
        return 20;
    }
    if (!(buf = malloc((size_t)w * 3 * (size_t)h))) {
        CloseLibrary(CyberGfxBase);
        return 20;
    }
    ReadPixelArray(buf, 0, 0, (UWORD)(w * 3), &s->RastPort, (UWORD)x, (UWORD)y, (UWORD)w, (UWORD)h, RECTFMT_RGB);
    if (!(out = fopen(argv[1], "wb"))) {
        fprintf(stderr, "Grab: can't write %s\n", argv[1]);
        free(buf);
        CloseLibrary(CyberGfxBase);
        return 20;
    }
    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info = png ? png_create_info_struct(png) : NULL;
    if (!info || setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "Grab: libpng failed\n");
        fclose(out);
        free(buf);
        CloseLibrary(CyberGfxBase);
        return 20;
    }
    png_init_io(png, out);
    idx = malloc((size_t)w * (size_t)h);
    colours = idx ? palette_of(buf, w * h, pal, idx) : 0;
    png_set_IHDR(png, info, (png_uint_32)w, (png_uint_32)h, 8,
                 colours ? PNG_COLOR_TYPE_PALETTE : PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    if (colours)
        png_set_PLTE(png, info, pal, colours);
    png_set_compression_level(png, 9);
    png_write_info(png, info);
    for (row = 0; row < h; row++)
        png_write_row(png, colours ? idx + (size_t)row * (size_t)w : buf + (size_t)row * (size_t)w * 3);
    free(idx);
    png_write_end(png, info);
    png_destroy_write_struct(&png, &info);
    fclose(out);
    printf("%s: %ld,%ld %ldx%ld of the %dx%d screen \"%s\", %s\n", argv[1], x, y, w, h, s->Width, s->Height,
           s->Title ? (char *)s->Title : "", colours ? "with a palette" : "RGB");
    free(buf);
    CloseLibrary(CyberGfxBase);
    return 0;
}
