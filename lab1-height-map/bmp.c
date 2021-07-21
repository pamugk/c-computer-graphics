#include "bmp.h"

static const size_t BUFFER_SIZE = 4096U;

bool startReadingBmp(struct bmp_image *image, FILE *bmpFile) {
    if (bmpFile == NULL) {
        printf("No file was provided\n");
        return false;
    }
    
    if (fread(&image->fileHeader, sizeof(struct bmp_file_header), 1, bmpFile) < sizeof(struct bmp_file_header)) {
        printf("BMP file header is corrupted\n");
        return false;
    }

    if (image->fileHeader.header != BM && image->fileHeader.header != BA 
        && image->fileHeader.header != CI && image->fileHeader.header != CP
        && image->fileHeader.header != IC && image->fileHeader.header != PT) {
        printf("Unrecognized BM header\n");
        return false;
    }

    if (image->fileHeader.size < sizeof(struct bitmap_core_header) + 1U) {
        printf("BMP file seems to be corrupted\n");
        return false;
    }

    if (image->fileHeader.offset < sizeof(struct bitmap_core_header)) {
        printf("DIB header seems to be corrupted\n");
        return false;
    }
}

bool finishReadingBmp(struct bmp_image *image, FILE *bmpFile) {
    if (image->rawFileContents != NULL) {
        printf("BMP image structure already holds some data\n");
        return false;
    }

    image->rawFileContents = malloc(image->fileHeader.size);

    if (image->rawFileContents == NULL) {
        printf("Not enough memory available to hold BMP image\n");
        return false;
    }

    image->dibHeader = (union dib_header *) image->rawFileContents;
    image->pixelArray = image->rawFileContents + image->fileHeader.offset;

    fread(image->rawFileContents, sizeof(__uint32_t), 1, bmpFile);

    if (image->dibHeader->coreHeader.size != sizeof(struct bitmap_core_header)
        && image->dibHeader->coreHeader.size != sizeof(struct os2_2x_bitmap_header16)
        && image->dibHeader->coreHeader.size != sizeof(struct bitmap_info_header)
        && image->dibHeader->coreHeader.size != sizeof(struct os2_2x_bitmap_header)
        && image->dibHeader->coreHeader.size != sizeof(struct bitmap_v2_info_header)
        && image->dibHeader->coreHeader.size != sizeof(struct bitmap_v3_info_header)
        && image->dibHeader->coreHeader.size != sizeof(struct bitmap_v4_header)
        && image->dibHeader->coreHeader.size != sizeof(struct bitmap_v5_header)) {
        printf("DIB header not recognized\n");
        freeBmp(image);
        return false;
    }

    unsigned char *shiftedPointer = image->rawFileContents + sizeof(__uint32_t);
    while (fread(shiftedPointer, BUFFER_SIZE, 1U, bmpFile) == BUFFER_SIZE) {
        shiftedPointer += BUFFER_SIZE;
    }
}

bool extractImage(struct bmp_image *image, unsigned int *out_width, unsigned int *out_height, unsigned char **out_image) {
    __uint16_t colorDepth;
    bool topDown;
    if (image->dibHeader->coreHeader.size == sizeof(struct bitmap_core_header)) {
        *out_width = image->dibHeader->coreHeader.width;
        *out_height = image->dibHeader->coreHeader.height;
        colorDepth = image->dibHeader->coreHeader.imageColorDepth;
        topDown = false;
    } else {
        *out_width = abs(image->dibHeader->coreHeader.width);
        *out_height = abs(image->dibHeader->coreHeader.height);
        colorDepth = image->dibHeader->coreHeader.imageColorDepth;
        topDown = image->dibHeader->os2Header16.imageHeight < 0;
    }

    unsigned int rowSize = colorDepth * *out_width / 32 * 4;
    *out_image = malloc(rowSize * *out_height);
    return true;
}

void freeBmp(struct bmp_image *image) {
    if (image->rawFileContents != NULL) {
        free(image->rawFileContents);
        image->rawFileContents = NULL;
        image->dibHeader = NULL;
        image->pixelArray = NULL;
    }
}