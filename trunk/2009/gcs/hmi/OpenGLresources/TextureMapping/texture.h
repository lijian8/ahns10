#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#define False 0
#define True 1

/*****************************************/
/*  Load Bitmaps And Convert To Textures */

typedef unsigned int GLuint;

typedef struct {
    int width;
    int height;
    unsigned char *data;
} textureImage;

/* simple loader for 24bit bitmaps (data is in rgb-format) */
int loadBMP(const char *filename, textureImage *texture);

GLuint LoadGLTexture(const char* name);

#endif
