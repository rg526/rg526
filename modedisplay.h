#include "esUtil.h"
#include "model.h"
#include "note.h"
#include "state.h"

#ifndef __MODEDISPLAY_H__
#define __MODEDISPLAY_H__

typedef struct{
    GLuint prog;
	GLuint vbuffer;
	Model block, railway;
	Device* dev;
	float speed;
	NoteArray* note;
    ESContext* esContext;
	int* judge;
} ModeDisplay;

int modedisplay_init(ModeDisplay*, ESContext*, Device*, float, NoteArray*, int*);

void modedisplay_draw(ModeDisplay*, double);

void modedisplay_destroy(ModeDisplay*);

#endif