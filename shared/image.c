#include "image.h"

#include "file.h"

#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"
#include <jpeglib.h>
#include <png.h>

// PNG
bool isPng(const char *fileExtension) {
    return strcmp(fileExtension, "png") == 0;
}

struct image readPng(const char *filePath, unsigned int requiredFormat) {
    struct image result = { 0, 0, NULL };
    png_image loadedImage;
    loadedImage.version = PNG_IMAGE_VERSION;
    loadedImage.opaque = NULL;

    if (!png_image_begin_read_from_file(&loadedImage, filePath)) {
        printf("Error occured while reading a PNG image from %s: %s\n", filePath, loadedImage.message);
        return result;
    }
    
    result.width = loadedImage.width;
    result.height = loadedImage.height;

    loadedImage.format = requiredFormat;
    unsigned long imageSize = PNG_IMAGE_SIZE(loadedImage);
    result.contents = malloc(imageSize);

    if (result.contents != NULL) {
        if (!png_image_finish_read(&loadedImage, NULL, result.contents, 0, NULL)) {
            printf("Error occured while reading PNG from %s: %s\n", filePath, loadedImage.message);
            free(result.contents);
            result.contents = NULL;
        }
    } else {
        printf("Not enough memory for a PNG image from %s: %lu bytes required\n", filePath, imageSize);
        png_image_free(&loadedImage);
    }
   
   return result;
}

// JPEG
bool isJpeg(const char *fileExtension) {
    return strcmp(fileExtension, "jpeg") == 0
            || strcmp(fileExtension, "jpg") == 0
            || strcmp(fileExtension, "jpe") == 0
            || strcmp(fileExtension, "jif") == 0
            || strcmp(fileExtension, "jfi") == 0
            || strcmp(fileExtension, "jfif");
}

struct custom_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmpBuffer;
};

typedef struct custom_error_mgr custom_error_mgr_struct;

void handledErrorExit(j_common_ptr cinfo)
{
    custom_error_mgr_struct *errorManager = (custom_error_mgr_struct *)cinfo->err;
    cinfo->err->output_message(cinfo);
    longjmp(errorManager->setjmpBuffer, 1);
}

struct image readJpeg(const char *filePath, J_COLOR_SPACE requiredFormat) {
    struct image result = { 0, 0, NULL };
    FILE *imageFile = fopen(filePath, "rb");

    if (imageFile == NULL) {
        printf("JPEG image file can not be opened\n");
        return result;
    }
    
    struct jpeg_decompress_struct decompressor;

    custom_error_mgr_struct jpegErrorHandler;
    decompressor.err = jpeg_std_error(&jpegErrorHandler.pub);
    jpegErrorHandler.pub.error_exit = handledErrorExit;
    if (setjmp(jpegErrorHandler.setjmpBuffer)) {
        if (result.contents != NULL) {
            free(result.contents);
            result.contents = NULL;
        }

        jpeg_destroy_decompress(&decompressor);
        fclose(imageFile);
         
        return result;
    }

    jpeg_create_decompress(&decompressor);
    jpeg_stdio_src(&decompressor, imageFile);
    jpeg_read_header(&decompressor, true);

    decompressor.out_color_space = requiredFormat;
    jpeg_calc_output_dimensions(&decompressor);
    result.width = decompressor.output_width;
    result.height = decompressor.output_height;
    
    unsigned int samplesPerRow = result.width * decompressor.output_components;
    result.contents = malloc(result.height * samplesPerRow);
    JSAMPARRAY buffer = decompressor.mem->alloc_sarray((j_common_ptr)&decompressor, JPOOL_IMAGE, samplesPerRow, 1);

    jpeg_start_decompress(&decompressor);
    unsigned long shift = 0;
    while (decompressor.output_scanline < result.height) {
        jpeg_read_scanlines(&decompressor, buffer, 1);
        memcpy(result.contents + shift, buffer[0], samplesPerRow);
        shift += samplesPerRow;
    }

    jpeg_finish_decompress(&decompressor);
    jpeg_destroy_decompress(&decompressor);

    fclose(imageFile);

    return result;
}

//BMP
bool isBmp(const char *fileExtension) {
    return strcmp(fileExtension, "bmp") == 0;
}

struct image readBmp(const char *filePath) {
    struct image result = { 0, 0, NULL };
    FILE *imageFile = fopen(filePath, "rb");

    if (imageFile == NULL) {
        printf("BMP image file can not be opened\n");
        return result;
    }

    struct bmp_image image;
    if (startReadingBmp(&image, imageFile)) {
        image.rawFileContents = NULL;
        if (finishReadingBmp(&image, imageFile)) {
        } else {
            printf("Error occured while reading BMP image");
        }
    } else {
        printf("Error occured while reading BMP file header\n");
    }

    freeBmp(&image);
    fclose(imageFile);

    return result;
}

struct image readHeightmap(const char *filePath) {
    const char *fileExtension = defineFileExtension(fileNameFromPath(filePath));
    if (isPng(fileExtension)) {
        return readPng(filePath, PNG_FORMAT_GRAY);
    } else if (isJpeg(fileExtension)){
        return readJpeg(filePath, JCS_GRAYSCALE);
    } else if (isBmp(fileExtension)) {
        return readBmp(filePath);
    } else {
        printf("Unknown file extension: %s\n", fileExtension);
    }

    struct image stubResult = { 0, 0, NULL };
    return stubResult;
}

struct image readTexture(const char *filePath) {
    const char *fileExtension = defineFileExtension(fileNameFromPath(filePath));
    if (isPng(fileExtension)) {
        return readPng(filePath, PNG_FORMAT_RGBA);
    } else if (isJpeg(fileExtension)){
        return readJpeg(filePath, JCS_EXT_RGBA);
    } else if (isBmp(fileExtension)) {
        return readBmp(filePath);
    } else {
        printf("Unknown file extension: %s\n", fileExtension);
    }

    return (struct image){ 0, 0, NULL };
}

void freeImage(struct image *image) {
    if (image->contents != NULL) {
      free(image->contents);
      image->contents = NULL;
   }
   image->width = 0;
   image->height = 0;
}
