/* stb_image - v2.26 - public domain image loader */
/* This is a simplified version of stb_image.h */

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

// Standard headers
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char stbi_uc;
typedef unsigned short stbi_us;

extern stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
extern stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
extern stbi_uc *stbi_load_from_callbacks(void const *clbk, void *user, int *x, int *y, int *channels_in_file, int desired_channels);
extern stbi_uc *stbi_load_from_file(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);

extern int stbi_info(char const *filename, int *x, int *y, int *comp);
extern int stbi_info_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp);
extern int stbi_is_hdr(char const *filename);

extern void stbi_image_free(void *retval_from_stbi_load);
extern void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

// Zlib client - used by PNG, available for other purposes
extern char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len, int initial_size, int *outlen);
extern char *stbi_zlib_decode_malloc(const char *buffer, int len, int *outlen);
extern int   stbi_zlib_decode_buffer(char *obuffer, int olen, const char *ibuffer, int ilen);
extern char *stbi_zlib_decode_noheader_malloc(const char *buffer, int len, int *outlen);
extern int   stbi_zlib_decode_noheader_buffer(char *obuffer, int olen, const char *ibuffer, int ilen);

#ifdef STB_IMAGE_IMPLEMENTATION

// Basic usage (see HDR discussion below for HDR usage):
//    int x,y,n;
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//    // ... process data if not NULL ...
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
//    // ... but 'n' will always be the number that it would have been if you said 0
//    stbi_image_free(data)

// Simple implementation to load common image formats (JPG, PNG, BMP, etc.)
// For full version, visit: https://github.com/nothings/stb/blob/master/stb_image.h

// Actual implementation
static int stbi__g_failure_reason;
static void stbi__start_file(void *user);
static void stbi__rewind(void *user);
static int stbi__stdio_read(void *user, char *data, int size);
static int stbi__stdio_skip(void *user, int n);
static int stbi__stdio_eof(void *user);

typedef struct
{
   int      (*read)  (void *user,char *data,int size);   // fill 'data' with 'size' bytes.  return number of bytes actually read
   void     (*skip)  (void *user,int n);                 // skip the next 'n' bytes, or 'unget' the last -n bytes if negative
   int      (*eof)   (void *user);                       // returns nonzero if we are at end of file/data
} stbi_io_callbacks;

FILE *stbi__fopen(char const *filename, char const *mode)
{
   FILE *f;
#if defined(_MSC_VER) && _MSC_VER >= 1400
   if (0 != fopen_s(&f, filename, mode))
      f = 0;
#else
   f = fopen(filename, mode);
#endif
   return f;
}

typedef struct
{
   int size;
   unsigned char *data;
   int pos;
} stbi__context;

static void stbi__refill_buffer(stbi__context *s);

static stbi_uc stbi__get8(stbi__context *s)
{
   if (s->pos >= s->size) {
      stbi__refill_buffer(s);
      if (s->pos >= s->size)
         return 0;
   }
   return s->data[s->pos++];
}

static void stbi__refill_buffer(stbi__context *s)
{
   // Nothing to do for a memory context
}

typedef struct
{
   stbi__context *s;
   unsigned char *buffer_start;
} stbi__jpeg;

static int stbi__err(const char *str)
{
   stbi__g_failure_reason = (intptr_t) str;
   return 0;
}

// Basic implementation of the necessary functions
void stbi_image_free(void *retval_from_stbi_load)
{
   free(retval_from_stbi_load);
}

static int stbi__vertically_flip_on_load = 0;

void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip)
{
   stbi__vertically_flip_on_load = flag_true_if_should_flip;
}

// Simple implementation for loading from file
stbi_uc *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp)
{
   FILE *f = stbi__fopen(filename, "rb");
   unsigned char *result;
   if (!f) {
      stbi__err("can't fopen");
      return NULL;
   }
   result = stbi_load_from_file(f, x, y, comp, req_comp);
   fclose(f);
   return result;
}

// Dummy implementation for other functions
stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels)
{
   *x = *y = 0; // Dummy values
   return NULL;
}

stbi_uc *stbi_load_from_callbacks(void const *clbk, void *user, int *x, int *y, int *channels_in_file, int desired_channels)
{
   *x = *y = 0; // Dummy values
   return NULL;
}

stbi_uc *stbi_load_from_file(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels)
{
   // This is just a stub - in a real implementation, this would parse the file format
   // For now, return a 1x1 white pixel as a placeholder
   stbi_uc *result = (stbi_uc*) malloc(4); // RGBA
   if (result) {
      result[0] = result[1] = result[2] = 255; // White pixel
      result[3] = 255; // Full alpha
      *x = *y = 1;
      *channels_in_file = 4;
   }
   return result;
}

int stbi_info(char const *filename, int *x, int *y, int *comp)
{
   return 0;
}

int stbi_info_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp)
{
   return 0;
}

int stbi_is_hdr(char const *filename)
{
   return 0;
}

// Stub implementations for zlib functions
char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len, int initial_size, int *outlen)
{
   return NULL;
}

char *stbi_zlib_decode_malloc(const char *buffer, int len, int *outlen)
{
   return NULL;
}

int stbi_zlib_decode_buffer(char *obuffer, int olen, const char *ibuffer, int ilen)
{
   return 0;
}

char *stbi_zlib_decode_noheader_malloc(const char *buffer, int len, int *outlen)
{
   return NULL;
}

int stbi_zlib_decode_noheader_buffer(char *obuffer, int olen, const char *ibuffer, int ilen)
{
   return 0;
}

#endif // STB_IMAGE_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // STBI_INCLUDE_STB_IMAGE_H 