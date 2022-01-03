#include "esUtil.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "mat.h"
#include "gameplay.h"
#include "state.h"
#include "music.h"
#include "model.h"
#include "note.h"
#include "modedisplay.h"
#include <sys/time.h>

typedef struct {
	Device* dev;
	float speed;
	NoteArray note;
	struct timeval abstime; 
	double timeelapsed;
	int score;
	int* judge;
	ModeDisplay modedisplay;
} GameplayData;

int gameplay_init(ESContext *esContext, State* state, Device* dev) {
	state->data = malloc(sizeof(GameplayData));
	if (state->data == NULL) {
		fprintf(stderr, "gameplay data malloc failed\n");
		return -1;
	}
	GameplayData *data = state->data;

	if (gettimeofday(&data->abstime, NULL) != 0){
		free(data);
		return -1;
	}

	data->speed = 5.0 / 4.0; 
	data->dev = dev;
	data->timeelapsed = 0;
	data->score = 0;

	if (note_init(&data->note, "note.dat") != 0){
		free(data);
		return -1;
	}

	data->judge = malloc(data->note.length * sizeof(int));
	if(data->judge == NULL){
		note_destroy(&data->note);
		free(data);
		return -1;
	}
	memset(data->judge, 0, data->note.length * sizeof(int) );
    
	if(modedisplay_init(&data->modedisplay, esContext, data->dev, data->speed, &data->note, data->judge)){
		note_destroy(&data->note);
		free(data->judge);
		free(data);
		return -1;
	}
	return 0;
}

void gameplay_resume(ESContext *esContext, State* state) {
	//TODO: resume music playback on resume
}

StateChg gameplay_update(ESContext *esContext, State* state) {
	//Judge notes
	GameplayData* data = state->data;

	struct timeval currenttime;
	gettimeofday(&currenttime, NULL); 
	data->timeelapsed = (double)(currenttime.tv_sec - data->abstime.tv_sec) + 1e-6 * ((double)(currenttime.tv_usec - data->abstime.tv_usec));

	for(size_t i = 0; i < data->note.length; i++)
		if (data->note.arr[i].notetype == NOTE_SHORT){
			float touchtime = data->note.arr[i].start;
			if(fabs(data->timeelapsed - touchtime) < 0.1 && data->judge[i] ==0){
				InputLine line = input_query_clear(&data->dev->input, data->note.arr[i].pos);
				if (!line.active) continue;
				double deltatime = (double)(line.tv.tv_sec - data->abstime.tv_sec) + 1e-6 * ((double)(line.tv.tv_usec - data->abstime.tv_usec));
				if(fabs(deltatime - touchtime) < 0.1 ){
					data->score ++;
					data->judge[i] = 1;
				}
			}
		}
		else{
			if( (data->timeelapsed > data->note.arr[i].start - 0.1) && ((data->timeelapsed < data->note.arr[i].end + 0.1)) && data->judge[i] ==0){
				InputLine line = input_query_clear(&data->dev->input, data->note.arr[i].pos);
				if (!line.active) continue;
				double deltatime = (double)(line.tv.tv_sec - data->abstime.tv_sec) + 1e-6 * ((double)(line.tv.tv_usec - data->abstime.tv_usec));
				if((deltatime > data->note.arr[i].start - 0.1) && ((deltatime < data->note.arr[i].end + 0.1))){
					data->score ++;
					data->judge[i] = 1;
				}
			}
		}
		
	//Continue gameplay
	StateChg change;
	change.ret = STATE_CONT;
	return change;
}


void gameplay_draw(ESContext *esContext, State* state) {
	GameplayData *data = state->data;
    modedisplay_draw(&data->modedisplay, data->timeelapsed);

	char st[20];
	snprintf(st, 20, "%d", data->score);

	Vec color;
	color.v[0] = 1.0;
	color.v[1] = 0.5;
	color.v[2] = 0.0;
	text_draw(&data->dev->text, st, 0.8 * esContext->width, 0.8 * esContext->height, esContext->width / 1600.0, &color);
}

void gameplay_destroy(ESContext *esContext, State* state) {
	GameplayData *data = state->data;

	//Clear data components
    modedisplay_destroy(&data->modedisplay);
	free(data->judge);
	note_destroy(&data->note);
	
	//Free data struct
	free(data);
}

State gameplay_state = {
	.init = gameplay_init,
	.destroy = gameplay_destroy,
	.resume = gameplay_resume,
	.update = gameplay_update,
	.draw = gameplay_draw,
	.data = NULL
};

