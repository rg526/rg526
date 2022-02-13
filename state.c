#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esUtil.h"
#include "state.h"
#include "gpio.h"
#include "input.h"
#include "playmode.h"
#include "image.h"
#include "text.h"
#include "homemode.h"

const Theme theme_options[] = {
	{
		.note_file = "note/note_1.dat",
		.music_file = "music/music1.mp3",
		.name = "music1",
	},
	{
		.note_file = "note/note_2.dat",
		.music_file = "music/music2.mp3",
		.name = "music2",
	},
	{
		.note_file = "note/note_3.dat",
		.music_file = "music/music3.mp3",
		.name = "music3",
	}
};
const size_t theme_count = 3;

const Difficulty difficulty_options[] = {
	{
		.speed = 5.0 / 3.0,
		.name = "Normal",
	},
	{
		.speed = 5.0 / 2.0,
		.name = "Fast",
	},
	{
		.speed = 5.0 / 4.0,
		.name = "Slow",
	}
};

const size_t difficulty_count = 3;

int device_init(Device* dev, ESContext *esContext) {
	if (gpio_init(&dev->gpio) != 0) {
		return 1;
	}
	if (input_init(&dev->input, esContext, &dev->gpio) != 0) {
		return 1;
	}
	if (music_init(&dev->music) != 0) {
		return 1;
	}
	if (image_init(&dev->image, esContext) != 0) {
		return 1;
	}
	if (text_init(&dev->textregular, "Poppins/Poppins-Regular.ttf", &dev->image, esContext) != 0) {
		return 1;
	}
	if (text_init(&dev->textbold, "Poppins/Poppins-ExtraBold.ttf", &dev->image, esContext) != 0) {
		return 1;
	}

	//Init speed and filename
	dev->difficulty_opt = 0;
	dev->theme_opt = 0;

	return 0;
}

void device_destroy(Device* dev) {
	input_destroy(&dev->input);
	gpio_destroy(&dev->gpio);
	music_destroy(&dev->music);
	text_destroy(&dev->textregular);
	text_destroy(&dev->textbold);
	image_destroy(&dev->image);
}

int main () {
	ESContext esContext;
	memset(&esContext, 0, sizeof(esContext));
	esCreateWindow(&esContext, "Haptic Rhythm Music Game", 960, 540, ES_WINDOW_RGB);

	//Specify gl options
	glViewport(0, 0, esContext.width, esContext.height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	glEnable(GL_CULL_FACE);

	//Config alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Device* device = malloc(sizeof(Device));
	if (device == NULL) {
		fprintf(stderr, "Device malloc failed\n");
		return 1;
	}
	if (device_init(device, &esContext) != 0) {
		fprintf(stderr, "Init device failed\n");
		return 1;
	}

	State *state = &homemode_state, *saved = NULL;
	if (state->init(&esContext, state, device) != 0) {
		fprintf(stderr, "Init state failed\n");
		return 1;
	}

	while (esUserInterrupt(&esContext) == GL_FALSE) {
		if (state->update != NULL) {
			StateChg change = state->update(&esContext, state);
			if (change.ret == STATE_SWITCH_NOSAVE) {
				state->destroy(&esContext, state);
				state = change.next;
				if (state->init(&esContext, state, device) != 0) {
					fprintf(stderr, "state init failed\n");
					return 1;
				}
				continue;
			} else if (change.ret == STATE_SWITCH_SAVE) {
				saved = state;
				state = change.next;
				if (state->init(&esContext, state, device) != 0) {
					fprintf(stderr, "state init failed\n");
					return 1;
				}
				continue;
			} else if (change.ret == STATE_SWITCH_RESTORE) {
				state->destroy(&esContext, state);
				state = saved;
				if (state->resume != NULL) {
					state->resume(&esContext, state);
				}
				continue;
			} else if (change.ret == STATE_SHUTDOWN) {
				break;
			} else if (change.ret == STATE_SWITCH_DISCARD) {
				state->destroy(&esContext, state);
				saved->destroy(&esContext, saved);
				state = change.next;
				if (state->init(&esContext, state, device) != 0) {
					fprintf(stderr, "state init failed\n");
					return 1;
				}
				continue;
			}
		}
		if (state->draw != NULL) state->draw(&esContext, state);
		eglSwapBuffers(esContext.eglDisplay, esContext.eglSurface);
	}

	state->destroy(&esContext, state);
	device_destroy(device);
	free(device);

	return 0;
}
