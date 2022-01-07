#include "note.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

int cmp (const void* L, const void* R){
    const NoteBlock* B1 = L;
    const NoteBlock* B2 = R;
    if (B1->start > B2->start) {return 1;}
    else if (B1->start == B2->start) {return 0;}
    else {return -1;}
}

int note_init(NoteArray* note, const char* filename)
{
    FILE *f = fopen(filename, "r");
    if(f == NULL){return -1;}
    fscanf(f, "%zu", &note->length);
    NoteBlock *note_arr = malloc(note->length * sizeof(NoteBlock));
    if (note_arr == NULL) {
        fclose(f);
        return -1;
    }
    for(size_t i = 0; i < note->length; i++){
        int type = 0;
        fscanf(f, "%d", &type);
        if (type == 0) {
            note_arr[i].notetype = NOTE_SHORT;
        } 
        else {
            note_arr[i].notetype = NOTE_LONG;
        } 
        fscanf(f, "%zu", &note_arr[i].pos);
        fscanf(f, "%f", &note_arr[i].start);
        fscanf(f, "%f", &note_arr[i].end);
    }
    fscanf(f, "%f", &note->endtime);
    note->arr = note_arr;
    //qsort(note->arr, sizeof(NoteBlock), note->length, cmp);
    fclose(f);
    return 0;
}
void note_destroy(NoteArray* note){
    free(note->arr);
}
