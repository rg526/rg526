#include "esUtil.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "mat.h"
#include "playmode.h"
#include "state.h"
#include "music.h"
#include "model.h"
#include "note.h"
#include "modedisplay.h"
#include <sys/time.h>
#include "pausemode.h"
#include "endmode.h"

typedef struct {
	Device* dev;
	float speed;
	NoteArray note;
	struct timeval abstime; 
	double timeelapsed;
	int score;
	int* judge;
	ModeDisplay modedisplay;
	GLuint background;
	GLuint progress_bg;
	GLuint progress_bar;
} PlaymodeData;

int playmode_init(ESContext *esContext, State* state, Device* dev) {
	state->data = malloc(sizeof(PlaymodeData));
	if (state->data == NULL) {
		fprintf(stderr, "playmode data malloc failed\n");
		return -1;
	}
	PlaymodeData *data = state->data;

	if (gettimeofday(&data->abstime, NULL) != 0){
		free(data);
		return -1;
	}

	data->speed = difficulty_options[dev->difficulty_opt].speed;
	data->dev = dev;
	data->timeelapsed = 0;
	data->score = 0;

	if (note_init(&data->note, theme_options[dev->theme_opt].note_file) != 0){
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

	int bg_width, bg_height;
	char* bg_buf = esLoadTGA(NULL, "bg.tga", &bg_width, &bg_height);
	data->background = image_load(&dev->image, bg_width, bg_height, bg_buf, 3);
	free(bg_buf);
    
	unsigned char prbg_buf[3] = {255, 255, 255};  
	data->progress_bg = image_load(&dev->image, 1, 1, prbg_buf, 3);
	unsigned char prbr_buf[3] = {74, 150, 243};  
	data->progress_bar = image_load(&dev->image, 1, 1, prbr_buf, 3);

	return 0;
}

void playmode_resume(ESContext *esContext, State* state) {
	PlaymodeData* data = state->data;

	//Reset time
	gettimeofday(&data->abstime, NULL);

	//TODO: resume music playback on resume
}

StateChg playmode_update(ESContext *esContext, State* state) {
	PlaymodeData* data = state->data;

	//Test pause button
	InputLine pause_line = input_query_clear(&data->dev->input, 0);
	if (pause_line.active) {
		input_clearall(&data->dev->input);

		StateChg change;
		change.ret = STATE_SWITCH_SAVE;
		change.next = &pausemode_state;
		return change;
	}

	//Get current time
	struct timeval currenttime;
	gettimeofday(&currenttime, NULL); 

	//Update time elapsed
	data->timeelapsed += ((double)(currenttime.tv_sec - data->abstime.tv_sec) + 1e-6 * ((double)(currenttime.tv_usec - data->abstime.tv_usec)));
	data->abstime = currenttime;
    
	//judge end time
    if (data->timeelapsed >= data->note.endtime){
		StateChg change;
		change.ret = STATE_SWITCH_NOSAVE;
		change.next = &endmode_state;
		return change;	
	}
	//Judge notes
	for(size_t i = 0; i < data->note.length; i++)
		if (data->note.arr[i].notetype == NOTE_SHORT){
			if(fabs(data->timeelapsed - data->note.arr[i].start) < 0.5 && data->judge[i] ==0){
				InputLine line = input_query_clear(&data->dev->input, data->note.arr[i].pos);
				if (!line.active) continue;

				double deltatime = data->timeelapsed - ((double)(currenttime.tv_sec - line.tv.tv_sec) + 1e-6 * ((double)(currenttime.tv_usec - line.tv.tv_usec)));
				if(fabs(deltatime - data->note.arr[i].start) < 0.5 ){
					data->score ++;
					data->judge[i] = 1;
				}
			}
		}
		else{
			if( (data->timeelapsed > data->note.arr[i].start - 0.5) && ((data->timeelapsed < data->note.arr[i].end + 0.5)) && data->judge[i] != -1){
				InputLine line = input_query(&data->dev->input, data->note.arr[i].pos);
				if (!line.active) continue;

				double deltatime = data->timeelapsed - ((double)(currenttime.tv_sec - line.tv.tv_sec) + 1e-6 * ((double)(currenttime.tv_usec - line.tv.tv_usec)));
				if(data->judge[i] == 0){
					if(fabs(deltatime - data->note.arr[i].start) < 0.5){
						data->score ++;
						data->judge[i] = 1;
					}
				}
				else if(data->judge[i] > 0){
					if(data->timeelapsed - deltatime > 0.5 && (data->timeelapsed > data->note.arr[i].start) && ((data->timeelapsed < data->note.arr[i].end ))){
						data->judge[i] = -1;
					}
					else{
						if((deltatime - data->note.arr[i].start) / (data->note.arr[i].end - data->note.arr[i].start) > 0.5 && data->judge[i] == 1){
							data->judge[i] = 2;
							data->score ++;
						}
						else if((deltatime - data->note.arr[i].start) /  (data->note.arr[i].end - data->note.arr[i].start) > 0.85 && data->judge[i] == 2){
							data->judge[i] = 3;
							data->score ++;
						}
					}
				}
			}
		}
		
	//Continue playmode
	StateChg change;
	change.ret = STATE_CONT;
	return change;
}


void playmode_draw(ESContext *esContext, State* state) {
	PlaymodeData *data = state->data;

	//Clear screen
	glClear(GL_COLOR_BUFFER_BIT);
    
	//draw background and progress bar
	image_render(&data->dev->image, 0.0, 0.0, 1.0, 1.0, data->background, NULL);
	image_render(&data->dev->image, 0.1, 0.95, 0.8, 0.025, data->progress_bg, NULL);
	image_render(&data->dev->image, 0.1, 0.95, (data->timeelapsed / data->note.endtime) * 0.8, 0.025, data->progress_bar, NULL);

	//Draw disp
    modedisplay_draw(&data->modedisplay, data->timeelapsed);

	char st[21];
	snprintf(st, 20, "%d", data->score);

	Vec color;
	color.v[0] = 1.0;
	color.v[1] = 0.5;
	color.v[2] = 0.0;
	text_draw(&data->dev->textregular, st, 0.8, 0.8, 1.0, &color);
}

void playmode_destroy(ESContext *esContext, State* state) {
	PlaymodeData *data = state->data;

	//Clear data components
    modedisplay_destroy(&data->modedisplay);
	free(data->judge);
	note_destroy(&data->note);
	image_unload(&data->dev->image, data->background);
	image_unload(&data->dev->image, data->progress_bg);
	image_unload(&data->dev->image, data->progress_bar);
	//Free data struct
	free(data);
}

State playmode_state = {
	.init = playmode_init,
	.destroy = playmode_destroy,
	.resume = playmode_resume,
	.update = playmode_update,
	.draw = playmode_draw,
	.data = NULL
};

