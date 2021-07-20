#include "image.h"

// JPEG image format
static const char JPG[] = "jpg";
static const char JPEG[] = "jpeg";
static const char JPE[] = "jpe";
static const char JIF[] = "jif";
static const char JFIF[] = "jfif";
static const char JFI[] = "jfi";

static const char PNG[] = "png";

image_struct readPng(const char *filePath, unsigned int requiredFormat) {
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

   image_struct result = { width, height, imageContents };
   return result;
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

image_struct readJpeg(const char *filePath, J_COLOR_SPACE requiredFormat) {
   unsigned int width = 0, height = 0;
   unsigned char *imageContents = NULL;

   FILE *imageFile = fopen(filePath, "rb");

   if (imageFile != NULL) {
      struct jpeg_decompress_struct decompressor;

      custom_error_mgr_struct jpegErrorHandler;
      decompressor.err = jpeg_std_error(&jpegErrorHandler.pub);
      jpegErrorHandler.pub.error_exit = handledErrorExit;
      if (setjmp(jpegErrorHandler.setjmpBuffer)) {
         if (imageContents != NULL) {
            free(imageContents);
            imageContents = NULL;
         }

         jpeg_destroy_decompress(&decompressor);
         fclose(imageFile);
         
         goto Complete;
      }

      jpeg_create_decompress(&decompressor);
      jpeg_stdio_src(&decompressor, imageFile);
      jpeg_read_header(&decompressor, true);

      decompressor.out_color_space = requiredFormat;
      jpeg_calc_output_dimensions(&decompressor);
      width = decompressor.output_width, height = decompressor.output_height;
      unsigned int samplesPerRow = width * decompressor.output_components;
      imageContents = malloc(height * samplesPerRow);
      JSAMPARRAY buffer = decompressor.mem->alloc_sarray((j_common_ptr)&decompressor, JPOOL_IMAGE, samplesPerRow, 1);

      jpeg_start_decompress(&decompressor);
      unsigned long shift = 0;
      while (decompressor.output_scanline < height) {
         jpeg_read_scanlines(&decompressor, buffer, 1);
         memcpy(imageContents + shift, buffer[0], samplesPerRow);
         shift += samplesPerRow;
      }

      jpeg_finish_decompress(&decompressor);
      jpeg_destroy_decompress(&decompressor);

      fclose(imageFile);
   } else {
      printf("JPEG image file can not be opened\n");
   }

   Complete:
   image_struct result = { width, height, imageContents };
   return result;
}

image_struct readHeightmap(const char *filePath) {
   const char *fileExtension = defineFileExtension(fileNameFromPath(filePath));
   if (strcmp(fileExtension, PNG) == 0) {
      return readPng(filePath, PNG_FORMAT_GRAY);
   } else if (strcmp(fileExtension, JPEG) == 0
      || strcmp(fileExtension, JPG) == 0
      || strcmp(fileExtension, JPE) == 0
      || strcmp(fileExtension, JIF) == 0
      || strcmp(fileExtension, JFI) == 0
      || strcmp(fileExtension, JFIF)){
      return readJpeg(filePath, JCS_GRAYSCALE);
   } else {
      printf("Unknown file extension: %s\n", fileExtension);
   }

   image_struct stubResult = { 0, 0, NULL };
   return stubResult;
}

void freeImage(image_struct *image) {
   if (image->contents != NULL) {
      free(image->contents);
      image->contents = NULL;
   }
   image->width = 0;
   image->height = 0;
}