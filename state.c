#include <stdio.h>
#include <string.h>
#include "esUtil.h"
#include "state.h"
#include "gameplay.h"

int main () {
	ESContext esContext;
	memset(&esContext, 0, sizeof(esContext));

	esCreateWindow(&esContext, "Title", 1600, 900, ES_WINDOW_RGB);

	State *state = &gameplay_state, *saved = NULL;
	if (state->init(&esContext, state) != 0) {
		return 1;
	}

	while (esUserInterrupt(&esContext) == GL_FALSE) {
		if (state->update != NULL) {
			StateChg change = state->update(&esContext, state);
			if (change.ret == STATE_SWITCH_NOSAVE) {
				state->destroy(&esContext, state);
				state = change.next;
				if (state->init(&esContext, state) != 0) {
					fprintf(stderr, "state init failed\n");
					return 1;
				}
				continue;
			} else if (change.ret == STATE_SWITCH_SAVE) {
				saved = state;
				state = change.next;
				if (state->init(&esContext, state) != 0) {
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

	return 0;
}
