#include "esUtil.h"
#include "mat.h"
#include "vec.h"

#ifndef __IMAGE_H__
#define __IMAGE_H__

typedef struct {
	GLuint prog;
	GLuint vbuffer;
	Mat projection;
} Image;

int image_init(Image*, ESContext*);
void image_destroy(Image*);

void image_draw(Image* image, float x, float y, float w, float h, float image_w, float image_h, void* image_data, Vec* color);

#endif
