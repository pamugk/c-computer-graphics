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

unsigned int openGlImageFormatToPngFormat(GLenum glFormat) {
    switch (glFormat) {
        case GL_LUMINANCE: {
            return PNG_FORMAT_GRAY;
        }
        case GL_LUMINANCE_ALPHA: {
            return PNG_FORMAT_GA;
        }
        case GL_BGR: {
            return PNG_FORMAT_BGR;
        }
        case GL_RGB: {
            return PNG_FORMAT_RGB;
        }
        case GL_BGRA: {
            return PNG_FORMAT_BGRA;
        }
        case GL_RGBA: {
            return PNG_FORMAT_RGBA;
        }
    }
    return 0U;
}

void readPng(const char *filePath, struct image *out_image) {
    png_image loadedImage;
    loadedImage.version = PNG_IMAGE_VERSION;
    loadedImage.opaque = NULL;

    if (!png_image_begin_read_from_file(&loadedImage, filePath)) {
        printf("Error occured while reading a PNG image from %s: %s\n", filePath, loadedImage.message);
        return;
    }
    
    out_image->width = loadedImage.width;
    out_image->height = loadedImage.height;

    loadedImage.format = openGlImageFormatToPngFormat(out_image->format);
    unsigned long imageSize = PNG_IMAGE_SIZE(loadedImage);
    out_image->contents = malloc(imageSize);

    if (out_image->contents != NULL) {
        if (!png_image_finish_read(&loadedImage, NULL, out_image->contents, 0, NULL)) {
            printf("Error occured while reading PNG from %s: %s\n", filePath, loadedImage.message);
            free(out_image->contents);
            out_image->contents = NULL;
        }
    } else {
        printf("Not enough memory for a PNG image from %s: %lu bytes required\n", filePath, imageSize);
        png_image_free(&loadedImage);
    }
   
   return;
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

unsigned int openGlImageFormatToJpegFormat(GLenum glFormat) {
    switch (glFormat) {
        case GL_LUMINANCE: {
            return JCS_GRAYSCALE;
        }
        case GL_BGR: {
            return JCS_EXT_BGR;
        }
        case GL_RGB: {
            return JCS_EXT_RGB;
        }
        case GL_BGRA: {
            return JCS_EXT_BGRA;
        }
        case GL_RGBA: {
            return JCS_EXT_RGBA;
        }
    }
    return JCS_UNKNOWN;
}

void readJpeg(const char *filePath, struct image *out_image) {
    FILE *imageFile = fopen(filePath, "rb");

    if (imageFile == NULL) {
        printf("JPEG image file can not be opened\n");
        return;
    }
    
    struct jpeg_decompress_struct decompressor;

    custom_error_mgr_struct jpegErrorHandler;
    decompressor.err = jpeg_std_error(&jpegErrorHandler.pub);
    jpegErrorHandler.pub.error_exit = handledErrorExit;
    if (setjmp(jpegErrorHandler.setjmpBuffer)) {
        if (out_image->contents != NULL) {
            free(out_image->contents);
            out_image->contents = NULL;
        }

        jpeg_destroy_decompress(&decompressor);
        fclose(imageFile);
         
        return;
    }

    jpeg_create_decompress(&decompressor);
    jpeg_stdio_src(&decompressor, imageFile);
    jpeg_read_header(&decompressor, true);

    decompressor.out_color_space = openGlImageFormatToJpegFormat(out_image->format);
    jpeg_calc_output_dimensions(&decompressor);
    out_image->width = decompressor.output_width;
    out_image->height = decompressor.output_height;
    
    unsigned int samplesPerRow = out_image->width * decompressor.output_components;
    out_image->contents = malloc(out_image->height * samplesPerRow);
    JSAMPARRAY buffer = decompressor.mem->alloc_sarray((j_common_ptr)&decompressor, JPOOL_IMAGE, samplesPerRow, 1);

    jpeg_start_decompress(&decompressor);
    unsigned long shift = 0;
    while (decompressor.output_scanline < out_image->height) {
        jpeg_read_scanlines(&decompressor, buffer, 1);
        memcpy(out_image->contents + shift, buffer[0], samplesPerRow);
        shift += samplesPerRow;
    }

    jpeg_finish_decompress(&decompressor);
    jpeg_destroy_decompress(&decompressor);

    fclose(imageFile);

    return;
}

//BMP
bool isBmp(const char *fileExtension) {
    return strcmp(fileExtension, "bmp") == 0;
}

void readBmp(const char *filePath, struct image* out_image) {
    FILE *imageFile = fopen(filePath, "rb");

    if (imageFile == NULL) {
        printf("BMP image file can not be opened\n");
        return;
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

    return;
}

struct image readImage(const char *filePath, GLenum format) {
    struct image result = { 0, 0, format, NULL, NULL };
    const char *fileExtension = defineFileExtension(fileNameFromPath(filePath));
    if (isPng(fileExtension)) {
        readPng(filePath, &result);
    } else if (isJpeg(fileExtension)){
        readJpeg(filePath, &result);
    } else if (isBmp(fileExtension)) {
        readBmp(filePath, &result);
    } else {
        printf("Unknown file extension: %s\n", fileExtension);
    }

    return result;
}

void freeImage(struct image *image) {
    if (image->contents != NULL) {
      free(image->contents);
      image->contents = NULL;
   }
   image->width = 0;
   image->height = 0;
}
