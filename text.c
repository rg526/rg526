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
    return 0;
}

void text_destroy(Text* T)
{
    FT_Done_Face(T->face);
    FT_Done_FreeType(T->ft);
}