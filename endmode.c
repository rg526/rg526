#include "esUtil.h"
#include "state.h"
#include "playmode.h"
#include "homemode.h"

typedef struct{
    Device* dev;
	GLuint blank;
	GLuint button, active_button;
	size_t count, current;
} EndmodeData;


int endmode_init(ESContext* esContext, State* state, Device* dev)
{
    state->data = malloc(sizeof(EndmodeData));
    if (state->data == NULL){
        fprintf(stderr, "state data alloc failed\n");
        return -1;
    }
    EndmodeData* data = state->data;
    data->dev = dev; 
    
    int button_width, button_height, active_button_width, active_button_height;
	char* active_button_buf = esLoadTGA(NULL, "image/active-button.tga", &active_button_width, &active_button_height);
    char* button_buf = esLoadTGA(NULL, "image/button.tga", &button_width, &button_height);
	data->button = image_load(&dev->image, button_width, button_height, button_buf, 4);
    data->active_button = image_load(&dev->image, active_button_width, active_button_height, active_button_buf, 4);
	free(button_buf);
    free(active_button_buf);
	unsigned char blank_buffer[3] = {76, 131, 34};
	data->blank = image_load(&dev->image, 1, 1, blank_buffer, 3);

	data->count = 2;
	data->current = 0;

    return 0;
}

void endmode_destroy(ESContext* esContext, State* state){
    EndmodeData* data = state->data;
	image_unload(&data->dev->image, data->blank);
	image_unload(&data->dev->image, data->button);
	image_unload(&data->dev->image, data->active_button);
    free(state->data);
}

StateChg endmode_update(ESContext* esContext, State* state)
{
    EndmodeData* data = state->data;
    //Test enter line
    InputLine enter_line = input_query_clear(&data->dev->input, 0);
	if (enter_line.active) {
		input_clearall(&data->dev->input);

		StateChg change;
		switch (data->current) {
		case 0:
			change.ret = STATE_SWITCH_NOSAVE;
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

    //continue end mode
    StateChg change;
	change.ret = STATE_CONT;
	return change;
}
void endmode_draw(ESContext* esContext, State* state)
{
    EndmodeData* data = state->data;
	image_render(&data->dev->image, 0.3, 0.24, 0.4, 0.6, data->blank, NULL);
	//image_render(&data->dev->image, 0.35, 0.635, 0.3, 0.1, data->current == 0 ? data->active_button : data->button, NULL);
	image_render(&data->dev->image, 0.35, 0.485, 0.3, 0.1, data->current == 0 ? data->active_button : data->button, NULL);
	image_render(&data->dev->image, 0.35, 0.335, 0.3, 0.1, data->current == 1 ? data->active_button : data->button, NULL);

	Vec color;
	color.v[0] = 0.0;
	color.v[1] = 0.0;
	color.v[2] = 0.0;
	text_draw(data->current == 0 ? &data->dev->textbold : &data->dev->textregular, "Game Over" , 0.3295, 0.66, 1.00, &color);
	text_draw(data->current == 0 ? &data->dev->textbold : &data->dev->textregular, "Restart" , 0.4385, 0.512, 0.6, &color);
	text_draw(data->current == 1 ? &data->dev->textbold : &data->dev->textregular, "Exit" , 0.4685, 0.356, 0.6, &color);
}

State endmode_state = {
	.init = endmode_init,
	.destroy = endmode_destroy,
	.resume = NULL,
	.update = endmode_update,
	.draw = endmode_draw,
	.data = NULL
};
