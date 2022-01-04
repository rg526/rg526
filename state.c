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
	if (text_init(&dev->text, "Poppins-ExtraLight.ttf", &dev->image) != 0) {
		return 1;
	}
	return 0;
}

void device_destroy(Device* dev) {
	input_destroy(&dev->input);
	music_destroy(&dev->music);
	gpio_destroy(&dev->gpio);
	text_destroy(&dev->text);
	image_destroy(&dev->image);
}

int main () {
	ESContext esContext;
	memset(&esContext, 0, sizeof(esContext));
	esCreateWindow(&esContext, "Title", 1600, 900, ES_WINDOW_RGB);

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
