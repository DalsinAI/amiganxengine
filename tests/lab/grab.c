/* Grab: the front screen, or a part of it, as a binary PPM file, for
 * showing a lab's Workbench in a README (Cradle's screenshot tool returns
 * the picture to the caller only). Reads the screen's pixels through
 * cybergraphics.library (RTG screens).
 *   Grab FILE [X Y W H]   (default: the whole front screen)
 * Fetch the file with Chop's pieces; convert the PPM to PNG on the PC.
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

struct Library *CyberGfxBase;

int main(int argc, char **argv)
{
    struct Screen *s;
    ULONG lock;
    long x = 0, y = 0, w, h;
    unsigned char *buf;
    FILE *out;
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
    fprintf(out, "P6\n%ld %ld\n255\n", w, h);
    fwrite(buf, 3, (size_t)w * (size_t)h, out);
    fclose(out);
    printf("%s: %ld,%ld %ldx%ld of the %dx%d screen \"%s\"\n", argv[1], x, y, w, h, s->Width, s->Height,
           s->Title ? (char *)s->Title : "");
    free(buf);
    CloseLibrary(CyberGfxBase);
    return 0;
}
