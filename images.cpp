#include "images.hpp"
#include <cstdio>

#ifdef __BIG_ENDIAN__
///////////////////////////////////////////////////////////
// This function says, "this pointer is a little endian value"
// If the value must be changed it is... otherwise, this
// function is defined away below (on Intel systems for example)
inline void LITTLE_ENDIAN_WORD(void *pWord)
{
  unsigned char *pBytes = (unsigned char *)pWord;
  unsigned char temp;
  
  temp = pBytes[0];
  pBytes[0] = pBytes[1];
  pBytes[1] = temp;
}

///////////////////////////////////////////////////////////
// This function says, "this pointer is a little endian value"
// If the value must be changed it is... otherwise, this
// function is defined away below (on Intel systems for example)
inline void LITTLE_ENDIAN_DWORD(void *pWord)
{
  unsigned char *pBytes = (unsigned char *)pWord;
  unsigned char temp;
  
  // Swap outer bytes
  temp = pBytes[3];
  pBytes[3] = pBytes[0];
  pBytes[0] = temp;
  
  // Swap inner bytes
  temp = pBytes[1];
  pBytes[1] = pBytes[2];
  pBytes[2] = temp;
}
#else

// Define them away on little endian systems
#define LITTLE_ENDIAN_WORD
#define LITTLE_ENDIAN_DWORD
#endif

#pragma pack(1)
typedef struct tga_header
{
  GLbyte	identsize;              // Size of ID field that follows header (0)
  GLbyte	colorMapType;           // 0 = None, 1 = paletted
  GLbyte	imageType;              // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
  unsigned short	colorMapStart;          // First colour map entry
  unsigned short	colorMapLength;         // Number of colors
  unsigned char 	colorMapBits;   // bits per palette entry
  unsigned short	xstart;                 // image x origin
  unsigned short	ystart;                 // image y origin
  unsigned short	width;                  // width in pixels
  unsigned short	height;                 // height in pixels
  GLbyte	bits;                   // bits per pixel (8 16, 24, 32)
  GLbyte	descriptor;             // image descriptor
} tga_header_t;
#pragma pack(8)

GLbyte *Images::readTGA(const char *fileName, int *width, int *height, int *components, GLenum *format)
{
  FILE *pFile;			// File pointer
  tga_header_t tgaHeader;		// TGA file header
  unsigned long lImageSize;		// Size in bytes of image
  short sDepth;			// Pixel depth;
  GLbyte	*pBits = NULL;          // Pointer to bits
  
  // Default/Failed values
  *width = 0;
  *height = 0;
  *format = GL_RGB;
  *components = GL_RGB;
  
  // Attempt to open the file
  pFile = fopen(fileName, "rb");
  if(pFile == NULL)
    return NULL;
	
  // Read in header (binary)
  fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1, pFile);
  
  // Do byte swap for big vs little endian
#ifdef __APPLE__
  LITTLE_ENDIAN_WORD(&tgaHeader.colorMapStart);
  LITTLE_ENDIAN_WORD(&tgaHeader.colorMapLength);
  LITTLE_ENDIAN_WORD(&tgaHeader.xstart);
  LITTLE_ENDIAN_WORD(&tgaHeader.ystart);
  LITTLE_ENDIAN_WORD(&tgaHeader.width);
  LITTLE_ENDIAN_WORD(&tgaHeader.height);
#endif
	
	
  // Get width, height, and depth of texture
  *width = tgaHeader.width;
  *height = tgaHeader.height;
  sDepth = tgaHeader.bits / 8;
  
  // Put some validity checks here. Very simply, I only understand
  // or care about 8, 24, or 32 bit targa's.
  if(tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32)
    return NULL;
	
  // Calculate size of image buffer
  lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
  
  // Allocate memory and check for success
  pBits = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
  
  // Read in the bits
  // Check for read error. This should catch RLE or other
  // weird formats that I don't want to recognize
  if(fread(pBits, lImageSize, 1, pFile) != 1)
  {
    if(pBits != NULL)
      free(pBits);
    return NULL;
  }
  
  // Set OpenGL format expected
  switch(sDepth)
  {
#ifndef OPENGL_ES
    case 3:     // Most likely case
      *format = GL_BGR;
      *components = GL_RGB;
      break;
#endif
    case 4:
      *format = GL_BGRA;
      *components = GL_RGBA;
      break;
    case 1:
      *format = GL_LUMINANCE;
      *components = GL_LUMINANCE;
      break;
    default:        // RGB
                    // If on the iPhone, TGA's are BGR, and the iPhone does not
                    // support BGR without alpha, but it does support RGB,
                    // so a simple swizzle of the red and blue bytes will suffice.
                    // For faster iPhone loads however, save your TGA's with an Alpha!
#ifdef OPENGL_ES
      for(int i = 0; i < lImageSize; i+=3)
      {
        GLbyte temp = pBits[i];
        pBits[i] = pBits[i+2];
        pBits[i+2] = temp;
      }
#endif
      break;
  }
	
  // Done with File
  fclose(pFile);
	
  // Return pointer to image data
  return pBits;
}