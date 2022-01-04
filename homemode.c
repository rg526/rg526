#include "esUtil.h"
#include "state.h"
#include "playmode.h"

typedef struct{
    Device* dev;
    GLuint button;
	GLuint active_button;
	GLuint background;
	size_t count;
	size_t current;
} HomemodeData;


int homemode_init(ESContext* esContext, State* state, Device* dev)
{
    state->data = malloc(sizeof(HomemodeData));
    if (state->data == NULL){
        fprintf(stderr, "state data alloc failed\n");
        return -1;
    }
    HomemodeData* data = state->data;
    data->dev = dev; 
	unsigned char button_buffer[3] = {68, 193, 247}; 
	data->button = image_load(&dev->image, 1, 1, button_buffer, 3);
	unsigned char active_button_buffer[3] = {69, 193, 247}; 
	data->active_button = image_load(&dev->image, 1, 1, active_button_buffer, 3);

	int bg_width, bg_height;
	char* bg_buf = esLoadTGA(NULL, "bg.tga", &bg_width, &bg_height);
	data->background = image_load(&dev->image, bg_width, bg_height, bg_buf, 3);
	free(bg_buf);

	data->count = 2;
	data->current = 0;
    return 0;
}

void homemode_destroy(ESContext* esContext, State* state){
	HomemodeData* data = state->data;
	image_unload(&data->dev->image, data->button);
	image_unload(&data->dev->image, data->active_button);
	image_unload(&data->dev->image, data->background);
    free(state->data);
}

StateChg homemode_update(ESContext* esContext, State* state)
{
    HomemodeData* data = state->data;
    //Test enter button
    InputLine enter_line = input_query_clear(&data->dev->input, 0);
	if (enter_line.active) {
		input_clearall(&data->dev->input);

		StateChg change;
		if (data->current == 0){
			change.ret = STATE_SWITCH_NOSAVE;
			change.next = &playmode_state;
			return change;
		}
		if (data->current == 1){
			change.ret = STATE_SHUTDOWN;
			return change;
		}	
	}
    //Test up button
	InputLine up_line = input_query_clear(&data->dev->input, 2);
	if (up_line.active) {
		input_clearall(&data->dev->input);
		if(data->current > 0){
		    data->current --;
		}
	}

	InputLine down_line = input_query_clear(&data->dev->input, 3);
	if (down_line.active) {
		input_clearall(&data->dev->input);
		if(data->current < data->count-1){
			data->current ++;
			}
	}


    //continue pause mode
    StateChg change;
	change.ret = STATE_CONT;
	return change;
}
void homemode_draw(ESContext* esContext, State* state)
{   
    HomemodeData* data = state->data;
    glClear(GL_COLOR_BUFFER_BIT);

	image_render(&data->dev->image, 0.0, 0.0, 1.0, 1.0, data->background, NULL);
    
    image_render(&data->dev->image, 0.35, 0.5, 0.3, 0.1, data->current == 0 ? data->active_button : data->button, NULL);
	image_render(&data->dev->image, 0.35, 0.3, 0.3, 0.1, data->current == 1 ? data->active_button : data->button, NULL);

	Vec color;
	color.v[0] = 0.0;
	color.v[1] = 0.0;
	color.v[2] = 0.0;
	text_draw(data->current == 0 ? &data->dev->textbold : &data->dev->textregular, "Start" , 0.46, 0.53, 0.6, &color);
	text_draw(data->current == 1 ? &data->dev->textbold : &data->dev->textregular, "Exit" , 0.4715, 0.326, 0.6, &color);
}

State homemode_state = {
	.init = homemode_init,
	.destroy = homemode_destroy,
	.resume = NULL,
	.update = homemode_update,
	.draw = homemode_draw,
	.data = NULL
};
