#include "image.h"

static const char JPG_FILE_EXTENSION[] = "jpg";
static const char JPEG_FILE_EXTENSION[] = "jpeg";

static const char PNG_FILE_EXTENSION[] = "png";

image readPng(const char *filePath, unsigned int requiredFormat) {
   png_image loadedImage;
   loadedImage.version = PNG_IMAGE_VERSION;
   loadedImage.opaque = NULL;
   
   unsigned int width = 0, height = 0;
   unsigned char *imageContents = NULL;

   if (png_image_begin_read_from_file(&loadedImage, filePath)) {
      width = loadedImage.width, height = loadedImage.height;

      loadedImage.format = requiredFormat;
      unsigned long imageSize = PNG_IMAGE_SIZE(loadedImage);
      imageContents = malloc(imageSize);

      if (imageContents != NULL) {
         if (!png_image_finish_read(&loadedImage, NULL, imageContents, 0, NULL)) {
            printf("Error occured while reading PNG from %s: %s\n", filePath, loadedImage.message);
            free(imageContents);
            imageContents = NULL;
         }
      } else {
         printf("Not enough memory for a PNG image from %s: %lu bytes required\n", filePath, imageSize);
         png_image_free(&loadedImage);
      }
   } else {
      printf("Error occured while reading a PNG image from %s: %s\n", filePath, loadedImage.message);
   }

   image result = { width, height, imageContents };
   return result;
}

image readHeightmap(const char *filePath) {
   const char *fileExtension = defineFileExtension(fileNameFromPath(filePath));
   if (strcmp(fileExtension, PNG_FILE_EXTENSION) == 0) {
      return readPng(filePath, PNG_FORMAT_GRAY);
   } else {
      printf("Unknown file extension: %s\n", fileExtension);
   }

   image stubResult = { 0, 0, NULL };
   return stubResult;
}

void freeImage(image *image) {
   if (image->contents != NULL) {
      free(image->contents);
      image->contents = NULL;
   }
   image->width = 0;
   image->height = 0;
}