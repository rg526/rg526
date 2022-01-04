#include "esUtil.h"
#include "state.h"
#include "playmode.h"

typedef struct{
    Device* dev;
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
    return 0;
}

void homemode_destroy(ESContext* esContext, State* state){
    free(state->data);
}

StateChg homemode_update(ESContext* esContext, State* state)
{
    HomemodeData* data = state->data;
    //Test resume button
    InputLine pause_line = input_query_clear(&data->dev->input, 0);
	if (pause_line.active) {
		input_clearall(&data->dev->input);

		StateChg change;
		change.ret = STATE_SWITCH_NOSAVE;
		change.next = &playmode_state;
		return change;
	}

    //continue pause mode
    StateChg change;
	change.ret = STATE_CONT;
	return change;
}
void homemode_draw(ESContext* esContext, State* state)
{
    HomemodeData* data = state->data;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	Vec color;
	color.v[0] = 1.0;
	color.v[1] = 0.5;
	color.v[2] = 0.0;
	text_draw(&data->dev->text, "Press 0 to Start the Game" , 0.3, 0.55, 0.5, &color);
}

State homemode_state = {
	.init = homemode_init,
	.destroy = homemode_destroy,
	.resume = NULL,
	.update = homemode_update,
	.draw = homemode_draw,
	.data = NULL
};
