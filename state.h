#include "esUtil.h"
#include "input.h"
#include "music.h"
#include "gpio.h"
#include "model.h"
#include "image.h"
#include "text.h"

#ifndef __STATE_H__
#define __STATE_H__

typedef struct {
	Music music;
	Input input;
	GPIO gpio;
	Text text;
	Image image;
} Device;

enum StateRetType {
	STATE_CONT,
	STATE_SWITCH_NOSAVE,
	STATE_SWITCH_SAVE,
	STATE_SWITCH_RESTORE
};

typedef struct {
	enum StateRetType ret;
	struct State* next;
} StateChg;

typedef struct State {
	int (*init)(ESContext*, struct State*, Device*);
	void (*destroy)(ESContext*, struct State*);
	StateChg (*update)(ESContext*, struct State*);
	void (*draw)(ESContext*, struct State*);
	void* data;
} State;

#endif
