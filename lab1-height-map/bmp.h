// Based on official Microsoft documentation for wingdi.h: https://docs.microsoft.com/en-us/windows/win32/api/wingdi/
#ifndef BMP_
#define BMP_
struct bitmap_core_header {
    unsigned long size;

    unsigned short width;
    unsigned short height;

    unsigned short colorPlanesCount; // must be 1
    unsigned short imageColorDepth; // Typical values are 1, 4, 8, 16, 24 and 32
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
    unsigned long size;

    long imageWidth;
    long imageHeight;

    unsigned short colorPlanesCount; // must be 1
    unsigned short imageColorDepth; // Typical values are 1, 4, 8, 16, 24 and 32
};

struct bitmap_info_header {
    struct os2_2x_bitmap_header16 parentHeader;
    
    enum BMP_COMPRESSION_METHOD compression;
    unsigned long imageSize;

    long imageHorizontalResolution;
    long imageVerticalResolution;

    unsigned long usedColorCount; // if 0, number of colors defaults to 2^n
    unsigned long importantColorCount; // 0 when every color is important
};

struct os2_2x_bitmap_header {
    struct bitmap_info_header parentHeader;

    short units; // The only defined value is 0, meaning pixels per metre 
    short padding; // Ignored and should be zero 
    short direction; // The only defined value is 0, meaning the origin is the lower-left corner
    unsigned short halftoningAlgorithm; // 0 - none, 1 - error diffusion, 2 - PANDA, 3 - supert-circle
    long halftoningParameters[2];
    long colorEncoding; // The only defined value is 0, indicating RGB
    long applicationDefinedIdentifier; // Not used for image rendering 
};

struct bitmap_v2_info_header {
    struct bitmap_info_header parentHeader;

    unsigned long redMask;
    unsigned long greenMask;
    unsigned long blueMask;
};

struct bitmap_v3_info_header {
    struct bitmap_v2_info_header parentHeader;

    unsigned long alphaMask;
};

struct CIEXYZ {
    long x;
    long y;
    long z;
};

struct CIEXYZTRIPLE {
    struct CIEXYZ red;
    struct CIEXYZ green;
    struct CIEXYZ blue;
};

struct bitmap_v4_header {
    struct bitmap_v3_info_header parentHeader;

    unsigned long colorSpaceType;
    struct CIEXYZTRIPLE endpoints;
    unsigned long gammaRed;
    unsigned long gammaGreen;
    unsigned long gammaBlue;
};

struct bitmap_v5_header {
    struct bitmap_v4_header parentHeader;

    unsigned long intent;
    unsigned long profileData;
    unsigned long profileSize;
    unsigned long reserved;
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

//BMP
struct bmp_header {
   // Bitmap file header
   char header[2];
   unsigned long size;
   long reserved;
   unsigned long offset;

   union dib_header dibHeader;
};
#endif // BMP