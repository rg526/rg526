#include <ft2build.h>
#include "image.h"
#include FT_FREETYPE_H
#include "vec.h"

#ifndef __TEXT_H__
#define __TEXT_H__

typedef struct{
    FT_Library ft;
    FT_Face face;
    Image* image;
} Text;


int text_init(Text*, const char*, Image*);

void text_draw(Text*, char*, float x_cord, float y_cord, float, Vec* color); 

void text_destroy(Text*);

#endif

