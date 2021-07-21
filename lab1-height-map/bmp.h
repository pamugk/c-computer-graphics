// Based on official Microsoft documentation for wingdi.h: https://docs.microsoft.com/en-us/windows/win32/api/wingdi/
#ifndef BMP_
#define BMP_

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const short BM = 0x424D; //Windows 3.1x, 95, NT, ... etc.
static const short BA = 0x4241; //OS/2 struct bitmap array
static const short CI = 0x4349; //OS/2 struct color icon
static const short CP = 0x4350; //OS/2 const color pointer
static const short IC = 0x4943; //OS/2 struct icon
static const short PT = 0x5054; //OS/2 pointer

struct bmp_file_header {
   __int16_t header;
   __uint32_t size;
   __int32_t reserved;
   __uint32_t offset;
};

struct bitmap_core_header {
    __uint32_t size;

    __uint16_t width;
    __uint16_t height;

    __uint16_t colorPlanesCount; // must be 1
    __uint16_t imageColorDepth; // Typical values are 1, 4, 8, 16, 24 and 32
};

enum BMP_COMPRESSION_METHOD {
    BI_RGB,
    BI_RLE8,
    BI_RLE4,
    BI_BITFIELDS,
    BI_JPEG,
    BI_PNG,
    BI_ALPHABITFIELDS,
    BI_CMYK = 11,
    BI_CMYKRLE8,
    BI_CMYKRLE4
};

struct os2_2x_bitmap_header16 {
    __uint32_t size;

    __int32_t imageWidth;
    __int32_t imageHeight;

    __uint16_t colorPlanesCount; // must be 1
    __uint16_t imageColorDepth; // Typical values are 1, 4, 8, 16, 24 and 32
};

struct bitmap_info_header {
    struct os2_2x_bitmap_header16 parentHeader;
    
    enum BMP_COMPRESSION_METHOD compression;
    __uint32_t imageSize;

    __int32_t imageHorizontalResolution;
    __int32_t imageVerticalResolution;

    __uint32_t usedColorCount; // if 0, number of colors defaults to 2^n
    __uint32_t importantColorCount; // 0 when every color is important
};

struct os2_2x_bitmap_header {
    struct bitmap_info_header parentHeader;

    __int16_t units; // The only defined value is 0, meaning pixels per metre 
    __int16_t padding; // Ignored and should be zero 
    __int16_t direction; // The only defined value is 0, meaning the origin is the lower-left corner
    __uint16_t halftoningAlgorithm; // 0 - none, 1 - error diffusion, 2 - PANDA, 3 - supert-circle
    __int32_t halftoningParameters[2];
    __int32_t colorEncoding; // The only defined value is 0, indicating RGB
    __int32_t applicationDefinedIdentifier; // Not used for image rendering 
};

struct bitmap_v2_info_header {
    struct bitmap_info_header parentHeader;

    __uint32_t redMask;
    __uint32_t greenMask;
    __uint32_t blueMask;
};

struct bitmap_v3_info_header {
    struct bitmap_v2_info_header parentHeader;

    __uint32_t alphaMask;
};

struct CIEXYZ {
    __int32_t x;
    __int32_t y;
    __int32_t z;
};

struct CIEXYZTRIPLE {
    struct CIEXYZ red;
    struct CIEXYZ green;
    struct CIEXYZ blue;
};

struct bitmap_v4_header {
    struct bitmap_v3_info_header parentHeader;

    __uint32_t colorSpaceType;
    struct CIEXYZTRIPLE endpoints;
    __uint32_t gammaRed;
    __uint32_t gammaGreen;
    __uint32_t gammaBlue;
};

struct bitmap_v5_header {
    struct bitmap_v4_header parentHeader;

    __uint32_t intent;
    __uint32_t profileData;
    __uint32_t profileSize;
    __uint32_t reserved;
};

union dib_header {
    struct bitmap_core_header coreHeader;
    struct os2_2x_bitmap_header os2Header;
    struct os2_2x_bitmap_header16 os2Header16;
    struct bitmap_info_header infoHeader;
    struct bitmap_v2_info_header infoHeaderV2;
    struct bitmap_v3_info_header infoHeaderV3;
    struct bitmap_v4_header infoHeaderV4;
    struct bitmap_v5_header infoHeaderV5;
};

struct bmp_image {
    struct bmp_file_header fileHeader;

    unsigned char *rawFileContents;
    union dib_header *dibHeader;
    unsigned char *pixelArray;
};

bool startReadingBmp(struct bmp_image *image, FILE *bmpFile);
bool finishReadingBmp(struct bmp_image *image, FILE *bmpFile);
bool extractImage(struct bmp_image *image, unsigned int *out_width, unsigned int *out_height, unsigned char **out_image);
void freeBmp(struct bmp_image *image);

#endif // BMP