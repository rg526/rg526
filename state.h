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
	char* note_file;
	char* music_file;
	char* name;
} Theme;

typedef struct {
	float speed;
	char* name;
} Difficulty;

extern const Theme theme_options[];
extern const size_t theme_count;
extern const Difficulty difficulty_options[];
extern const size_t difficulty_count;

typedef struct {
	Music music;
	Input input;
	GPIO gpio;
	Text textregular;
	Text textbold;
	Image image;

	size_t difficulty_opt;
	size_t theme_opt;
} Device;

enum StateRetType {
	STATE_CONT,
	STATE_SWITCH_NOSAVE,
	STATE_SWITCH_SAVE,
	STATE_SWITCH_RESTORE,
	STATE_SHUTDOWN,
	STATE_SWITCH_DISCARD,
};

typedef struct {
	enum StateRetType ret;
	struct State* next;
} StateChg;

typedef struct State {
	int (*init)(ESContext*, struct State*, Device*);
	void (*destroy)(ESContext*, struct State*);
	void (*resume)(ESContext*, struct State*);
	StateChg (*update)(ESContext*, struct State*);
	void (*draw)(ESContext*, struct State*);
	void* data;
} State;

#endif
