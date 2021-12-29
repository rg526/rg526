#include <stdio.h>
#include <string.h>
#include "esUtil.h"
#include "state.h"
#include "input.h"
#include "gameplay.h"

int device_init(Device* dev, ESContext *esContext) {
	if (input_init(&dev->input, esContext) != 0) {
		return 1;
	}
	if (music_init(&dev->music) != 0) {
		return 1;
	}
	return 0;
}

void device_destroy(Device* dev) {
	input_destroy(&dev->input);
	music_destroy(&dev->music);
}

int main () {
	ESContext esContext;
	memset(&esContext, 0, sizeof(esContext));
	esCreateWindow(&esContext, "Title", 1600, 900, ES_WINDOW_RGB);

	Device device;
	if (device_init(&device, &esContext) != 0) {
		fprintf(stderr, "Init device failed\n");
		return 1;
	}

	State *state = &gameplay_state, *saved = NULL;
	if (state->init(&esContext, state, &device) != 0) {
		fprintf(stderr, "Init state failed\n");
		return 1;
	}

	while (esUserInterrupt(&esContext) == GL_FALSE) {
		if (state->update != NULL) {
			StateChg change = state->update(&esContext, state);
			if (change.ret == STATE_SWITCH_NOSAVE) {
				state->destroy(&esContext, state);
				state = change.next;
				if (state->init(&esContext, state, &device) != 0) {
					fprintf(stderr, "state init failed\n");
					return 1;
				}
				continue;
			} else if (change.ret == STATE_SWITCH_SAVE) {
				saved = state;
				state = change.next;
				if (state->init(&esContext, state, &device) != 0) {
					fprintf(stderr, "state init failed\n");
					return 1;
				}
				continue;
			} else if (change.ret == STATE_SWITCH_RESTORE) {
				state->destroy(&esContext, state);
				state = saved;
				continue;
			}
		}
		if (state->draw != NULL) state->draw(&esContext, state);

		eglSwapBuffers(esContext.eglDisplay, esContext.eglSurface);
	}

	state->destroy(&esContext, state);
	device_destroy(&device);

	return 0;
}
