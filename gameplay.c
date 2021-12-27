#include "esUtil.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "mat.h"
#include "gameplay.h"
#include "model.h"

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
	Model block, railway;
} gameplay_data;

int gameplay_init(ESContext *esContext, State* state) {
	state->data = malloc(sizeof(gameplay_data));
	if (state->data == NULL) {
		fprintf(stderr, "gameplay data malloc failed\n");
		return -1;
	}
	gameplay_data *data = state->data;

	data->prog = esLoadProgram(vertex_shader, frag_shader);
	if (data->prog == 0) {
		fprintf(stderr, "gameplay shader load failed\n");
		return -1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glLineWidth(3.0);
	glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	glEnable(GL_CULL_FACE);
	glViewport(0, 0, esContext->width, esContext->height);

	int ret1 = model_init(&data->block, "block.dat");
	int ret2 = model_init(&data->railway, "railway.dat");
	if (ret1 != 0 || ret2 != 0) {
		fprintf(stderr, "gameplay model init failed\n");
		return -1;
	}
	return 0;
}

StateChg gameplay_update(ESContext *esContext, State* state) {
	StateChg change;
	change.ret = STATE_CONT;
	return change;
}

void draw_obj(Model* model, Mat* mv_mat, Mat* p_mat, Mat* front_mat, GLuint prog) {
	Mat t_mat;
	mat_multiply(&t_mat, mv_mat, front_mat);

	GLuint mv_loc = glGetUniformLocation(prog, "mv_mat");
	glUniformMatrix4fv(mv_loc, 1, GL_TRUE, mat_ptr(&t_mat));
	GLuint p_loc = glGetUniformLocation(prog, "p_mat");
	glUniformMatrix4fv(p_loc, 1, GL_TRUE, mat_ptr(p_mat));

	GLuint l_pos = glGetUniformLocation(prog, "l_pos");
	glUniform4f(l_pos, 0.0, -30.0, 0.0, 0.0);
	GLuint l_color = glGetUniformLocation(prog, "l_color");
	glUniform4f(l_color, 1.0, 1.0, 1.0, 1.0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec), model->vertex);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec), model->normal);
	glEnableVertexAttribArray(1);
	glVertexAttrib3fv(2, vec_ptr(&model->color));
	glDisableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES, 0, model->length);
}

void gameplay_draw(ESContext *esContext, State* state) {
	gameplay_data *data = state->data;

	GLfloat color[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
	GLfloat vertexPos[4 * 3] = {
		-1, 0, 0,
		1, 0, 0,
		1, 5, 0,
		-1, 5, 0
	};

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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(data->prog);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertexPos);
	glEnableVertexAttribArray(0);
	glVertexAttrib3f(1, 0.0, 0.0, 1.0);
	glDisableVertexAttribArray(1);
	glVertexAttrib4fv(2, color);
	glDisableVertexAttribArray(2);

	GLuint mv_loc = glGetUniformLocation(data->prog, "mv_mat");
	glUniformMatrix4fv(mv_loc, 1, GL_TRUE, mat_ptr(&mv_mat));
	GLuint p_loc = glGetUniformLocation(data->prog, "p_mat");
	glUniformMatrix4fv(p_loc, 1, GL_TRUE, mat_ptr(&p_mat));

	GLuint l_pos = glGetUniformLocation(data->prog, "l_pos");
	glUniform4f(l_pos, 0.0, -30.0, 0.0, 0.0);
	GLuint l_color = glGetUniformLocation(data->prog, "l_color");
	glUniform4f(l_color, 1.0, 1.0, 1.0, 1.0);

	glDrawArrays(GL_LINE_LOOP, 0, 4);

	Mat front_mat;
	mat_translate(&front_mat, 0.0, 2.5, 0.0);
	draw_obj(&data->railway, &mv_mat, &p_mat, &front_mat, data->prog);
	mat_translate(&front_mat, -0.5, 2.5, 0.0);
	draw_obj(&data->railway, &mv_mat, &p_mat, &front_mat, data->prog);
	mat_translate(&front_mat, -1.0, 2.5, 0.0);
	draw_obj(&data->railway, &mv_mat, &p_mat, &front_mat, data->prog);
	mat_translate(&front_mat, 0.5, 2.5, 0.0);
	draw_obj(&data->railway, &mv_mat, &p_mat, &front_mat, data->prog);
	mat_translate(&front_mat, 1.0, 2.5, 0.0);
	draw_obj(&data->railway, &mv_mat, &p_mat, &front_mat, data->prog);
	mat_translate(&front_mat, -0.75, 2.0, 0.0);
	draw_obj(&data->block, &mv_mat, &p_mat, &front_mat, data->prog);
	mat_translate(&front_mat, -0.25, 1.0, 0.0);
	draw_obj(&data->block, &mv_mat, &p_mat, &front_mat, data->prog);
	mat_translate(&front_mat, 0.25, 3.0, 0.0);
	draw_obj(&data->block, &mv_mat, &p_mat, &front_mat, data->prog);
	mat_translate(&front_mat, 0.75, 4.0, 0.0);
	draw_obj(&data->block, &mv_mat, &p_mat, &front_mat, data->prog);
	mat_translate(&front_mat, 0.75, 0.5, 0.0);
	draw_obj(&data->block, &mv_mat, &p_mat, &front_mat, data->prog);
}

void gameplay_destroy(ESContext *esContext, State* state) {
	gameplay_data *data = state->data;

	glDeleteProgram(data->prog);
	model_destroy(&data->block);

	free(data);
}

State gameplay_state = {
	.init = gameplay_init,
	.destroy = gameplay_destroy,
	.update = gameplay_update,
	.draw = gameplay_draw,
	.data = NULL
};

