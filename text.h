#include <ft2build.h>
#include "image.h"
#include FT_FREETYPE_H

#ifndef __TEXT_H__
#define __TEXT_H__

typedef struct{
    FT_Library ft;
    FT_Face face;
    Image* image;
} Text;


int text_init(Text*, const char*, Image*);

void text_draw(Text*, char, int x_cord, int y_cord, float); 

void text_destroy(Text*);

#endif


