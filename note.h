#include<stddef.h>
#ifndef __NOTE_H__
#define __NOTE_H__

enum Note {NOTE_SHORT, NOTE_LONG};

typedef struct{
    enum Note notetype;
    size_t pos;
    float start;
    float end;
} NoteBlock;

typedef struct {
    size_t length;
    float endtime;
    NoteBlock *arr;
} NoteArray;

int note_init(NoteArray*, const char*);

void note_destroy(NoteArray*);

#endif 

