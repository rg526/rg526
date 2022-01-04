#include "esUtil.h"
#include "state.h"
#include "playmode.h"
#include "homemode.h"

typedef struct{
    Device* dev;
	GLuint blank;
} PausemodeData;


int pausemode_init(ESContext* esContext, State* state, Device* dev)
{
    state->data = malloc(sizeof(PausemodeData));
    if (state->data == NULL){
        fprintf(stderr, "state data alloc failed\n");
        return -1;
    }
    PausemodeData* data = state->data;
    data->dev = dev; 

	unsigned char blank_buffer[3] = {128, 128, 128};
	data->blank = image_load(&dev->image, 1, 1, blank_buffer, 3);

    return 0;
}

void pausemode_destroy(ESContext* esContext, State* state){
    PausemodeData* data = state->data;
	image_unload(&data->dev->image, data->blank);
    free(state->data);
}

StateChg pausemode_update(ESContext* esContext, State* state)
{
    PausemodeData* data = state->data;
    //Test resume button
    InputLine pause_line = input_query_clear(&data->dev->input, 0);
	if (pause_line.active) {
		input_clearall(&data->dev->input);

		StateChg change;
		change.ret = STATE_SWITCH_RESTORE;
		change.next = &playmode_state;
		return change;
	}

    InputLine exit_line = input_query_clear(&data->dev->input, 1);
	if (exit_line.active) {
		input_clearall(&data->dev->input);

		StateChg change;
		change.ret = STATE_SWITCH_NOSAVE;
		change.next = &homemode_state;
		return change;
	}

    //continue pause mode
    StateChg change;
	change.ret = STATE_CONT;
	return change;
}
void pausemode_draw(ESContext* esContext, State* state)
{
    PausemodeData* data = state->data;

	image_render(&data->dev->image, 0.4, 0.4, 0.3, 0.3, data->blank, NULL);

	Vec color;
	color.v[0] = 1.0;
	color.v[1] = 0.5;
	color.v[2] = 0.0;
	text_draw(&data->dev->textregular, "Press 0 to resume" , 0.42, 0.6, 0.5, &color);
	text_draw(&data->dev->textregular, "Press 1 to exit" , 0.42, 0.5, 0.5, &color);
}

State pausemode_state = {
	.init = pausemode_init,
	.destroy = pausemode_destroy,
	.resume = NULL,
	.update = pausemode_update,
	.draw = pausemode_draw,
	.data = NULL
};
