#include <ft2build.h>
#include "image.h"
#include FT_FREETYPE_H
#include "vec.h"
#include "image.h"

#ifndef __TEXT_H__
#define __TEXT_H__

typedef struct{
    FT_Library ft;
    FT_Face face;
    Image* image;
	GLuint texture[128];
	float width[128], rows[128], left[128], top[128], advancex[128];
} Text;


int text_init(Text*, const char*, Image*);

void text_draw(Text*, char*, float x_cord, float y_cord, float, Vec* color); 

void text_destroy(Text*);

#endif


