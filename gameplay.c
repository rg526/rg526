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
#include <sys/time.h>

const char* vertex_shader =
	"#version 300 es\n"
	"layout(location = 0) in vec4 a_position;"
	"layout(location = 1) in vec4 a_normal;"
	"layout(location = 2) in vec4 a_color;"
	"uniform mat4 mv_mat;"
	"uniform mat4 p_mat;"
	"out vec4 v_color;"
	"out vec4 v_pos;"
	"out vec4 v_normal;"
	"void main() {"
	"    v_color = a_color;"
	"    v_pos = mv_mat * a_position;"
	"    v_normal = -mv_mat * a_normal;"
	"    gl_Position = p_mat * mv_mat * a_position;"
	"}";


const char* frag_shader =
	"#version 300 es\n"
	"precision mediump float;"
	"in vec4 v_color;"
	"in vec4 v_pos;"
	"in vec4 v_normal;"
	"uniform vec4 l_pos;"
	"uniform vec4 l_color;"
	"out vec4 o_fragColor;"
	"void main() {"
	"    float diff = 0.5 * max(dot(normalize(v_normal), normalize(l_pos - v_pos)), 0.0);"
	"    vec4 diffuse_color = diff * l_color;"
	"    float ambient = 0.1;"
	"    vec4 ambient_color = ambient * l_color;"
	"    vec4 h_vec = normalize(-v_pos + l_pos - v_pos);"
	"    float spec = 0.6 * pow(max(dot(normalize(v_normal), h_vec), 0.0), 64.0);"
	"    vec4 specular_color = spec * l_color;"
	"    o_fragColor = v_color * (diffuse_color + ambient_color + specular_color);"
	"}";


typedef struct {
	GLuint prog;
	GLuint vbuffer;
	Model block, railway;
	Device* dev;
	float speed;
	NoteArray note;
	struct timeval abstime; 
	double timeelapsed;
	int score;
	int* judge;
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

	int ret1 = model_init(&data->block, "block.dat");
	int ret2 = model_init(&data->railway, "railway.dat");
	if (ret1 != 0 || ret2 != 0) {
		fprintf(stderr, "gameplay model init failed\n");
		free(&data->judge);
		note_destroy(&data->note);
		free(data);
		return -1;
	}

    data->prog = esLoadProgram(vertex_shader, frag_shader);
	if (data->prog == 0) {
		fprintf(stderr, "gameplay shader load failed\n");
		free(&data->judge);
		note_destroy(&data->note);
		model_destroy(&data->block);
		model_destroy(&data->railway);
		free(data);
		return -1;
	}
    
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glLineWidth(3.0);
	glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	glEnable(GL_CULL_FACE);
	glViewport(0, 0, esContext->width, esContext->height);

	glGenBuffers(1, &data->vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, data->vbuffer);
	return 0;
}

StateChg gameplay_update(ESContext *esContext, State* state) {
	GameplayData* data = state->data;
	struct timeval currenttime;
	gettimeofday(&currenttime, NULL); 
	data->timeelapsed = (double)(currenttime.tv_sec - data->abstime.tv_sec) + 1e-6 * ((double)(currenttime.tv_usec - data->abstime.tv_usec));
	for(size_t i = 0; i < data->note.length; i++)
		if (data->note.arr[i].notetype == NOTE_SHORT){
			float touchtime = data->note.arr[i].start;
			if(abs(data->timeelapsed - touchtime) < 0.1 && data->judge[i] ==0){
				InputLine line = input_query_clear(&data->dev->input, data->note.arr[i].pos);
				if (!line.active) continue;
				double deltatime = (double)(line.tv.tv_sec - data->abstime.tv_sec) + 1e-6 * ((double)(line.tv.tv_usec - data->abstime.tv_usec));
				if(abs(deltatime - touchtime) < 0.1 ){
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
		
	//printf("%d\n", data->score);

	StateChg change;
	change.ret = STATE_CONT;
	return change;
}

void set_obj_data(Model* model) {
	glBufferData(GL_ARRAY_BUFFER, 2 * model->length * sizeof(Vec), model->data,  GL_DYNAMIC_DRAW);
}

void draw_obj(Model* model, Mat* mv_mat, Mat* p_mat, Mat* front_mat, Vec* override_color, GLuint prog) {
	Mat t_mat;
	mat_multiply(&t_mat, mv_mat, front_mat);

	//Set uniform
	GLint mv_loc = glGetUniformLocation(prog, "mv_mat");
	glUniformMatrix4fv(mv_loc, 1, GL_TRUE, mat_ptr(&t_mat));
	GLint p_loc = glGetUniformLocation(prog, "p_mat");
	glUniformMatrix4fv(p_loc, 1, GL_TRUE, mat_ptr(p_mat));

	GLint l_pos = glGetUniformLocation(prog, "l_pos");
	glUniform4f(l_pos, 0.0, -30.0, 0.0, 0.0);
	GLint l_color = glGetUniformLocation(prog, "l_color");
	glUniform4f(l_color, 1.0, 1.0, 1.0, 1.0);

	//Set attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec), (void*)(model->length * sizeof(Vec)));
	glEnableVertexAttribArray(1);

	if (override_color == NULL) {
		glVertexAttrib3fv(2, vec_ptr(&model->color));
	} else {
		glVertexAttrib3fv(2, vec_ptr(override_color));
	}
	glDisableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES, 0, model->length);
}

void gameplay_draw(ESContext *esContext, State* state) {
	GameplayData *data = state->data;

	//Matrix transformation
	Mat scl, swp, trl, rot, persp;
	mat_scale(&scl, 56.0, 24.0, 24.0);
	mat_swapyz(&swp);
	mat_translate(&trl, 0.0, -80.0, 36.0);
	mat_rotate(&rot, -45, 0, 0);
	float aspect = (float)(esContext->height) / (float)(esContext->width);
	mat_projection(&persp, -14.0, 14.0, -14.0 * aspect, 14.0 * aspect, 20.0, 180.0);

	Mat mv_mat, p_mat, mvp;
	mat_multiply(&mv_mat, &swp, &scl);
	mat_multiply(&mv_mat, &trl, &mv_mat);
	mat_multiply(&mv_mat, &rot, &mv_mat);
	mat_copy(&p_mat, &persp);
	mat_multiply(&mvp, &p_mat, &mv_mat);

	//Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(data->prog);

	//Draw railway model
	set_obj_data(&data->railway);
	Mat front_mat;
	for (int i = -2; i < 3; i+=1){
		mat_translate(&front_mat, i*0.5, 2.5, 0.0);
		draw_obj(&data->railway, &mv_mat, &p_mat, &front_mat, NULL, data->prog);
	}
	
	//Draw block model
	set_obj_data(&data->block);
	for(int i=0; i < data->note.length; i++){		
		if(data->note.arr[i].notetype == NOTE_LONG){
			double start_pos = 1+(data->note.arr[i].start - data->timeelapsed)*(data->speed);
			double end_pos = 1+(data->note.arr[i].end - data->timeelapsed)*(data->speed);
			if(start_pos > 5 || end_pos < 0) continue;
			if(start_pos < 0) start_pos = 0;
			if(end_pos > 5) end_pos = 5;

			if (!data->judge[i]) {
				Mat scaleup, translate;
				mat_scale(&scaleup, 1.0, (end_pos - start_pos)/0.1, 1.0);
				mat_translate(&translate, (float)(data->note.arr[i].pos)*(0.5) - 1.25, (end_pos + start_pos)/2 , 0.0);

				mat_multiply(&front_mat, &translate, &scaleup);
				draw_obj(&data->block, &mv_mat, &p_mat, &front_mat, NULL, data->prog);
			} else {
				Vec active_color;
				active_color.v[0] = 0.953;
				active_color.v[1] = 0.588;
				active_color.v[2] = 0.290;

				Mat scaleup, translate;
				mat_scale(&scaleup, 1.0, (end_pos - start_pos)/0.1, 2.0);
				mat_translate(&translate, (float)(data->note.arr[i].pos)*(0.5) - 1.25, (end_pos + start_pos)/2 , 0.0);

				mat_multiply(&front_mat, &translate, &scaleup);
				draw_obj(&data->block, &mv_mat, &p_mat, &front_mat, &active_color, data->prog);
			}
		}
		else { 
			double y_pos = 1+(data->note.arr[i].start - data->timeelapsed)*(data->speed);
			if((y_pos<0) || (y_pos>5)){
				continue;
			}

			if (!data->judge[i]) {
				mat_translate(&front_mat, (float)(data->note.arr[i].pos)*(0.5) - 1.25, y_pos , 0.0);
				draw_obj(&data->block, &mv_mat, &p_mat, &front_mat, NULL, data->prog);
			} else {
				Vec active_color;
				active_color.v[0] = 0.953;
				active_color.v[1] = 0.588;
				active_color.v[2] = 0.290;

				Mat scaleup, translate;
				mat_scale(&scaleup, 1.0, 1.0, 2.0);
				mat_translate(&translate, (float)(data->note.arr[i].pos)*(0.5) - 1.25, y_pos , 0.0);
				mat_multiply(&front_mat, &translate, &scaleup);
				draw_obj(&data->block, &mv_mat, &p_mat, &front_mat, &active_color, data->prog);
			}
		}
	}
}

void gameplay_destroy(ESContext *esContext, State* state) {
	GameplayData *data = state->data;

	//Clear data components
	free(data->judge);
	note_destroy(&data->note);
	model_destroy(&data->block);
	model_destroy(&data->railway);

	//Delete program and vertex buffer object
	glDeleteProgram(data->prog);
	glDeleteBuffers(1, &data->vbuffer);

	//Free data struct
	free(data);
}

State gameplay_state = {
	.init = gameplay_init,
	.destroy = gameplay_destroy,
	.update = gameplay_update,
	.draw = gameplay_draw,
	.data = NULL
};

