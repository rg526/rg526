#include "esUtil.h"
#include "state.h"
#include "playmode.h"
#include "homemode.h"

typedef struct{
    Device* dev;
	GLuint blank;
	GLuint button, active_button;
	size_t count, current;
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
	unsigned char active_button_buffer[3] = {128, 0, 0};
	data->blank = image_load(&dev->image, 1, 1, blank_buffer, 3);
	data->button = image_load(&dev->image, 1, 1, button_buffer, 3);
	data->active_button = image_load(&dev->image, 1, 1, active_button_buffer, 3);

	data->count = 2;
	data->current = 0;

    return 0;
}

void pausemode_destroy(ESContext* esContext, State* state){
    PausemodeData* data = state->data;
	image_unload(&data->dev->image, data->blank);
	image_unload(&data->dev->image, data->button);
	image_unload(&data->dev->image, data->active_button);
    free(state->data);
}

StateChg pausemode_update(ESContext* esContext, State* state)
{
    PausemodeData* data = state->data;
    //Test enter line
    InputLine enter_line = input_query_clear(&data->dev->input, 0);
	if (enter_line.active) {
		input_clearall(&data->dev->input);

		StateChg change;
		switch (data->current) {
		case 0:
			change.ret = STATE_SWITCH_RESTORE;
			change.next = &playmode_state;
			return change;
		case 1:
			change.ret = STATE_SWITCH_NOSAVE;
			change.next = &homemode_state;
			return change;
		}
	}

	//Test up/down line
    InputLine up_line = input_query_clear(&data->dev->input, 2);
	if (up_line.active) {
		input_clearall(&data->dev->input);

		if (data->current > 0) {
			data->current--;
		}
	}

    InputLine down_line = input_query_clear(&data->dev->input, 3);
	if (down_line.active) {
		input_clearall(&data->dev->input);

		if (data->current < data->count - 1) {
			data->current++;
		}
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

	image_render(&data->dev->image, 0.35, 0.55, 0.3, 0.1, data->current == 0 ? data->active_button : data->button, NULL);
	image_render(&data->dev->image, 0.35, 0.35, 0.3, 0.1, data->current == 1 ? data->active_button : data->button, NULL);

	Vec color;
	color.v[0] = 0.0;
	color.v[1] = 0.0;
	color.v[2] = 0.0;
	text_draw(data->current == 0 ? &data->dev->textbold : &data->dev->textregular, "Resume" , 0.4285, 0.58, 0.6, &color);
	text_draw(data->current == 1 ? &data->dev->textbold : &data->dev->textregular, "Exit" , 0.4715, 0.376, 0.6, &color);
}

State pausemode_state = {
	.init = pausemode_init,
	.destroy = pausemode_destroy,
	.resume = NULL,
	.update = pausemode_update,
	.draw = pausemode_draw,
	.data = NULL
};
