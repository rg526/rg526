#include <stdio.h>
#include "esUtil.h"
#include "vec.h"
#include "image.h"

const char* image_vertex_shader = 
	"#version 300 es\n"
	"layout (location = 0) in vec2 a_vertcord;"
	"layout (location = 1) in vec2 a_textcord;"
	"uniform mat4 projection;"
	"out vec2 v_textcord;"
	"void main() {"
	"    gl_Position = projection * vec4(a_vertcord, 0.0, 1.0);"
	"    v_textcord = a_textcord;"
	"}";

const char* image_color_frag_shader = 
	"#version 300 es\n"
	"precision mediump float;"
	"uniform sampler2D s_text;"
	"in vec2 v_textcord;"
	"out vec4 o_fragcolor;"
	"void main() {"
	"    o_fragcolor = texture(s_text, v_textcord).bgra;"
	"}";

const char* image_mono_frag_shader = 
	"#version 300 es\n"
	"precision mediump float;"
	"uniform sampler2D s_text;"
	"uniform vec4 f_color;"
	"in vec2 v_textcord;"
	"out vec4 o_fragcolor;"
	"void main() {"
	"    o_fragcolor = vec4(f_color.rgb, texture(s_text, v_textcord).r);"
	"}";


int image_init(Image* image, ESContext *esContext) {
	image->color_prog = esLoadProgram(image_vertex_shader, image_color_frag_shader);
	image->mono_prog = esLoadProgram(image_vertex_shader, image_mono_frag_shader);
	if (image->color_prog == 0 || image->mono_prog == 0) {
		fprintf(stderr, "image shader load failed\n");
		return -1;
	}

	glGenBuffers(1, &image->vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, image->vbuffer);

	Mat trl, scl;
	mat_translate(&trl, -0.5, -0.5, 0.0);
	mat_scale(&scl, 2.0, 2.0, 0.0);
	mat_multiply(&image->projection, &scl, &trl);

	return 0;
}

/*
void image_draw(Image* image, float x, float y, float w, float h, int image_w, int image_h, void* image_data, Vec* color) {
	GLuint texture = image_load(image, image_w, image_h, image_data, color != NULL ? 1 : 3);
	image_render(image, x, y, w, h, texture, color);
	image_unload(image, texture);
}
*/

void image_render(Image* image, float x, float y, float w, float h, GLuint texture, Vec* color) {
	//Use program
	GLuint prog;
	if (color == NULL) {
		prog = image->color_prog;
	} else {
		prog = image->mono_prog;
		GLint f_color = glGetUniformLocation(prog, "f_color");
		glUniform4f(f_color, color->v[0], color->v[1], color->v[2], 1.0);
	}
	glUseProgram(prog);

	//Activate texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Prepare rectangle
	GLfloat rect_vertices[24] = {
		x, y + h, x, y, x + w, y, x, y + h, x + w, y, x + w, y + h,
		0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0
	};
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), rect_vertices, GL_DYNAMIC_DRAW);

	//Pass attributes and uniforms
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * 2 * sizeof(GLfloat)));
	
	GLint proj_loc = glGetUniformLocation(prog, "projection");
	glUniformMatrix4fv(proj_loc, 1, GL_TRUE, mat_ptr(&image->projection));

	//Draw image
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

GLuint image_load(Image* image, int image_w, int image_h, void* image_data, int channel) {
	//Create texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (channel == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	} else if (channel == 1) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_w, image_h, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);
	} else if (channel == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_w, image_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	} else {
		glDeleteTextures(1, &texture);
		return 0;
	}

	//Set texture parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;
}

void image_unload(Image* image, GLuint texture) {
	//Delete texture
	glDeleteTextures(1, &texture);
}

void image_destroy(Image* image) {
	glDeleteBuffers(1, &image->vbuffer);
	glDeleteProgram(image->color_prog);
	glDeleteProgram(image->mono_prog);
}
