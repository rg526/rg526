#include "esUtil.h"
#include "state.h"
#include "playmode.h"

typedef struct{
    Device* dev;
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
    return 0;
}

void pausemode_destroy(ESContext* esContext, State* state){
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

    //continue pause mode
    StateChg change;
	change.ret = STATE_CONT;
	return change;
}
void pausemode_draw(ESContext* esContext, State* state)
{

}

State pausemode_state = {
	.init = pausemode_init,
	.destroy = pausemode_destroy,
	.resume = NULL,
	.update = pausemode_update,
	.draw = pausemode_draw,
	.data = NULL
};