#include "text.h"
#include "image.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

int text_init(Text* T, const char* Filename, Image* I)
{
    T->image = I;
    if (FT_Init_FreeType(&T->ft)){
        printf("Fail to Init FreeType Library");
        return -1;
    }
    if (FT_New_Face(T->ft, Filename, 0, &T->face)){
        printf("Fail to Load Font");
        return -1;
    }
    FT_Set_Pixel_Sizes(T->face, 0, 96);
    return 0;
}

void text_draw(Text* T, char c, int x_cord, int y_cord, float scale)
{
    FT_Load_Char(T->face, c, FT_LOAD_RENDER);
    float x_pos = x_cord + T->face->glyph->bitmap_left * scale;
    float y_pos = y_cord - (T->face->glyph->bitmap.rows - T->face->glyph->bitmap_top) * scale; 
    float width = (float)(T->face->glyph->bitmap.width) * scale;
    float height = (float)(T->face->glyph->bitmap.rows )* scale;
    image_draw(T->image, x_pos, y_pos, width, height, T->face->glyph->bitmap.width, T->face->glyph->bitmap.rows, T->face->glyph->bitmap.buffer);
}

void text_destroy(Text* T)
{
    FT_Done_Face(T->face);
    FT_Done_FreeType(T->ft);
}