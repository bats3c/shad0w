









// use winapi for this








#include "../lib/zlib/zlib.h"

#include <stdio.h>
#include <string.h>

int gzip_inflate(char *compr, int comprLen, char *uncompr, int uncomprLen)
{
    int err;
    z_stream d_stream;

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = (unsigned char *)compr;
    d_stream.avail_in = comprLen;

    d_stream.next_out = (unsigned char *)uncompr;
    d_stream.avail_out = uncomprLen;

    err = inflateInit2(&d_stream, 16+MAX_WBITS);
    if (err != Z_OK) return err;

    while (err != Z_STREAM_END) err = inflate(&d_stream, Z_NO_FLUSH);

    err = inflateEnd(&d_stream);
    return err;
}