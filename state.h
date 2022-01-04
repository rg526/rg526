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
} UserSelect;
extern const UserSelect select_options[];
extern const size_t select_count;
extern const float speed_options[];
extern const size_t speed_count;

typedef struct {
	Music music;
	Input input;
	GPIO gpio;
	Text textregular;
	Text textbold;
	Image image;

	int speed_opt;
	int select_opt;
} Device;

enum StateRetType {
	STATE_CONT,
	STATE_SWITCH_NOSAVE,
	STATE_SWITCH_SAVE,
	STATE_SWITCH_RESTORE,
	STATE_SHUTDOWN,
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
