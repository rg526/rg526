#include "esUtil.h"
#include "state.h"
#include "playmode.h"
#include "homemode.h"

typedef struct{
    Device* dev;
	GLuint blank;
	GLuint button;
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
	unsigned char button_buffer[3] = {24, 84, 170};
	data->blank = image_load(&dev->image, 1, 1, blank_buffer, 3);
	data->button = image_load(&dev->image, 1, 1, button_buffer, 3);

    return 0;
}

void pausemode_destroy(ESContext* esContext, State* state){
    PausemodeData* data = state->data;
	image_unload(&data->dev->image, data->blank);
	image_unload(&data->dev->image, data->button);
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
    
	image_render(&data->dev->image, 0.3, 0.3, 0.4, 0.4, data->blank, NULL);

	image_render(&data->dev->image, 0.35, 0.55, 0.3, 0.1, data->button, NULL);
	image_render(&data->dev->image, 0.35, 0.35, 0.3, 0.1, data->button, NULL);

	Vec color;
	color.v[0] = 0.0;
	color.v[1] = 0.0;
	color.v[2] = 0.0;
	text_draw(&data->dev->textregular, "Resume" , 0.4285, 0.58, 0.6, &color);
	text_draw(&data->dev->textregular, "Exit" , 0.4715, 0.376, 0.6, &color);
}

State pausemode_state = {
	.init = pausemode_init,
	.destroy = pausemode_destroy,
	.resume = NULL,
	.update = pausemode_update,
	.draw = pausemode_draw,
	.data = NULL
};
