#include "modedisplay.h"

const char* modedisplay_vertex_shader =
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


const char* modedisplay_frag_shader =
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
	"    o_fragColor = vec4((v_color * (diffuse_color + ambient_color + specular_color)).rgb, 1.0);"
	"}";

int modedisplay_init(ModeDisplay* D, ESContext* esContext, Device* dev, float speed, NoteArray* note, int* judge)
{
    int ret1 = model_init(&D->block, "block.dat");
	int ret2 = model_init(&D->railway, "railway.dat");
	if (ret1 != 0 || ret2 != 0) {
		fprintf(stderr, "gameplay model init failed\n");
		return -1;
	}

    D->dev = dev;
    D->speed = speed;
    D->note = note;
    D->esContext = esContext;
	D->judge = judge; 

    D->prog = esLoadProgram(modedisplay_vertex_shader, modedisplay_frag_shader);
	if (D->prog == 0) {
		fprintf(stderr, "gameplay shader load failed\n");
		model_destroy(&D->block);
		model_destroy(&D->railway);
		return -1;
	}
        
	glGenBuffers(1, &D->vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, D->vbuffer);
    return 0;
}

void draw_line(Mat* mv_mat, Mat* p_mat, Vec* override_color, GLfloat line_width, GLuint prog) {
	//Set vertex and normal data
	GLfloat line_data[12] = {-1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0};
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(Vec), line_data,  GL_DYNAMIC_DRAW);

	//Set uniform
	GLint mv_loc = glGetUniformLocation(prog, "mv_mat");
	glUniformMatrix4fv(mv_loc, 1, GL_TRUE, mat_ptr(mv_mat));
	GLint p_loc = glGetUniformLocation(prog, "p_mat");
	glUniformMatrix4fv(p_loc, 1, GL_TRUE, mat_ptr(p_mat));

	GLint l_pos = glGetUniformLocation(prog, "l_pos");
	glUniform4f(l_pos, 0.0, -30.0, 0.0, 0.0);
	GLint l_color = glGetUniformLocation(prog, "l_color");
	glUniform4f(l_color, 1.0, 1.0, 1.0, 1.0);

	//Set attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(GLfloat)));

	glVertexAttrib3fv(2, vec_ptr(override_color));
	glDisableVertexAttribArray(2);

	glLineWidth(line_width);
	glDrawArrays(GL_LINES, 0, 2);
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

void modedisplay_draw(ModeDisplay* D, double time)
{
    //Matrix transformation
	Mat scl, swp, trl, rot, persp;
	mat_scale(&scl, 56.0, 24.0, 24.0);
	mat_swapyz(&swp);
	mat_translate(&trl, 0.0, -80.0, 36.0);
	mat_rotate(&rot, -45, 0, 0);
	float aspect = (float)(D->esContext->height) / (float)(D->esContext->width);
	mat_projection(&persp, -14.0, 14.0, -14.0 * aspect, 14.0 * aspect, 20.0, 180.0);

	Mat mv_mat, p_mat, mvp;
	mat_multiply(&mv_mat, &swp, &scl);
	mat_multiply(&mv_mat, &trl, &mv_mat);
	mat_multiply(&mv_mat, &rot, &mv_mat);
	mat_copy(&p_mat, &persp);
	mat_multiply(&mvp, &p_mat, &mv_mat);

	//Use program
	glUseProgram(D->prog);
	glBindBuffer(GL_ARRAY_BUFFER, D->vbuffer);

	//Draw judge line
	Vec line_color;
	line_color.v[0] = 1.0;
	line_color.v[1] = 0.0;
	line_color.v[2] = 1.0;
	draw_line(&mv_mat, &p_mat, &line_color, 10.0, D->prog);

	//Draw railway model
	set_obj_data(&D->railway);
	Mat front_mat;
	for (int i = -2; i < 3; i+=1){
		mat_translate(&front_mat, i*0.5, 2.5, 0.0);
		draw_obj(&D->railway, &mv_mat, &p_mat, &front_mat, NULL, D->prog);
	}
	
	//Draw block model
	set_obj_data(&D->block);
	for(size_t i=0; i < D->note->length; i++){
		if(D->note->arr[i].notetype == NOTE_LONG){
			double start_pos = 1+(D->note->arr[i].start - time)*(D->speed);
			double end_pos = 1+(D->note->arr[i].end - time)*(D->speed);
			if(start_pos > 5 || end_pos < 0) continue;
			if(start_pos < 0) start_pos = 0;
			if(end_pos > 5) end_pos = 5;

			if (!D->judge[i]) {
				Mat scaleup, translate;
				mat_scale(&scaleup, 1.0, (end_pos - start_pos)/0.1, 1.0);
				mat_translate(&translate, (float)(D->note->arr[i].pos)*(0.5) - 1.25, (end_pos + start_pos)/2 , 0.0);

				mat_multiply(&front_mat, &translate, &scaleup);
				draw_obj(&D->block, &mv_mat, &p_mat, &front_mat, NULL, D->prog);
			} else {
				Vec active_color;
				active_color.v[0] = 0.953;
				active_color.v[1] = 0.588;
				active_color.v[2] = 0.290;

				Mat scaleup, translate;
				mat_scale(&scaleup, 1.0, (end_pos - start_pos)/0.1, 2.0);
				mat_translate(&translate, (float)(D->note->arr[i].pos)*(0.5) - 1.25, (end_pos + start_pos)/2 , 0.0);

				mat_multiply(&front_mat, &translate, &scaleup);
				draw_obj(&D->block, &mv_mat, &p_mat, &front_mat, &active_color, D->prog);
			}
		}
		else { 
			double y_pos = 1+(D->note->arr[i].start - time)*(D->speed);
			if((y_pos<0) || (y_pos>5)){
				continue;
			}

			if (!D->judge[i]) {
				mat_translate(&front_mat, (float)(D->note->arr[i].pos)*(0.5) - 1.25, y_pos , 0.0);
				draw_obj(&D->block, &mv_mat, &p_mat, &front_mat, NULL, D->prog);
			} else {
				Vec active_color;
				active_color.v[0] = 0.953;
				active_color.v[1] = 0.588;
				active_color.v[2] = 0.290;

				Mat scaleup, translate;
				mat_scale(&scaleup, 1.0, 1.0, 2.0);
				mat_translate(&translate, (float)(D->note->arr[i].pos)*(0.5) - 1.25, y_pos , 0.0);
				mat_multiply(&front_mat, &translate, &scaleup);
				draw_obj(&D->block, &mv_mat, &p_mat, &front_mat, &active_color, D->prog);
			}
		}
	}
	
}

void modedisplay_destroy(ModeDisplay* D)
{
    model_destroy(&D->block);
	model_destroy(&D->railway);

	//Delete program and vertex buffer object
	glDeleteProgram(D->prog);
	glDeleteBuffers(1, &D->vbuffer);
}
