#include "esUtil.h"
#include "mat.h"
#include "vec.h"

#ifndef __IMAGE_H__
#define __IMAGE_H__

typedef struct {
	GLuint color_prog, mono_prog;
	GLuint vbuffer;
	Mat projection;
} Image;

int image_init(Image*, ESContext*);
void image_destroy(Image*);

GLuint image_load(Image* image, int image_w, int image_h, void* image_data, int channel);
void image_render(Image* image, float x, float y, float w, float h, GLuint texture, Vec* color);
void image_unload(Image* image, GLuint texture);

#endif
