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

	for (size_t ch = 0;ch < 128;ch++) {
		FT_Load_Char(T->face, (char)(ch), FT_LOAD_RENDER);
		T->texture[ch] = image_load(T->image,  T->face->glyph->bitmap.width, T->face->glyph->bitmap.rows, T->face->glyph->bitmap.buffer, 1);
		T->rows[ch] = T->face->glyph->bitmap.rows;
		T->width[ch] = T->face->glyph->bitmap.width;
		T->left[ch] = T->face->glyph->bitmap_left;
		T->top[ch] = T->face->glyph->bitmap_top;
		T->advancex[ch] = T->face->glyph->advance.x / 64.0;
	}

    FT_Done_Face(T->face);
    FT_Done_FreeType(T->ft);

    return 0;
}

void text_draw(Text* T, char* c, float x_cord, float y_cord, float scale, Vec* color)
{
	while (*c) {
		size_t ch = *(c++);
		float x_pos = x_cord + T->left[ch] * scale;
		float y_pos = y_cord - (T->rows[ch] - T->top[ch]) * scale; 
		float width = T->width[ch] * scale;
		float height = T->rows[ch] * scale;

		image_render(T->image, x_pos, y_pos, width, height, T->texture[ch], color);
		x_cord += scale * T->advancex[ch];
	}
}

void text_destroy(Text* T)
{
	for (size_t ch = 0;ch < 128;ch++) {
		image_unload(T->image, T->texture[ch]);
	}
}
