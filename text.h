#include <ft2build.h>
#include FT_FREETYPE_H
#include "esUtil.h"
#include "image.h"
#include "vec.h"
#include "image.h"

#ifndef __TEXT_H__
#define __TEXT_H__

typedef struct{
    FT_Library ft;
    FT_Face face;
    Image* image;
	ESContext* esContext;
	GLuint texture[128];
	float width[128], rows[128], left[128], top[128], advancex[128];
} Text;


int text_init(Text*, const char*, Image*, ESContext*);

void text_draw(Text*, char*, float x_cord, float y_cord, float, Vec* color); 

void text_destroy(Text*);

#endif


