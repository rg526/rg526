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

const char* image_frag_shader = 
	"#version 300 es\n"
	"precision mediump float;"
	"uniform sampler2D s_text;"
	"uniform vec4 f_color;"
	"in vec2 v_textcord;"
	"out vec4 o_fragcolor;"
	"void main() {"
	"    o_fragcolor = f_color * texture(s_text, v_textcord).bgra;"
	"}";

int image_init(Image* image, ESContext *esContext) {
	image->prog = esLoadProgram(image_vertex_shader, image_frag_shader);
	if (image->prog == 0) {
		fprintf(stderr, "image shader load failed\n");
		return -1;
	}

	glGenBuffers(1, &image->vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, image->vbuffer);

	Mat trl, scl;
	mat_translate(&trl, -(float)(esContext->width) / 2.0, -(float)(esContext->height) / 2.0, 0.0);
	mat_scale(&scl, 2.0 / (float)(esContext->width), 2.0 / (float)(esContext->height), 0.0);
	mat_multiply(&image->projection, &scl, &trl);

	return 0;
}

void image_draw(Image* image, float x, float y, float w, float h, float image_w, float image_h, void* image_data, Vec* color) {
	//Use program
	glUseProgram(image->prog);

	//Create texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (color == NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, image_w, image_h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, image_data);
	}
	glActiveTexture(GL_TEXTURE0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
	
	GLint f_color = glGetUniformLocation(image->prog, "f_color");
	if (color == NULL) {
		glUniform4f(f_color, 1.0, 1.0, 1.0, 1.0);
	} else {
		glUniform4f(f_color, color->v[0], color->v[1], color->v[2], 1.0);
	}
	GLint proj_loc = glGetUniformLocation(image->prog, "projection");
	glUniformMatrix4fv(proj_loc, 1, GL_TRUE, mat_ptr(&image->projection));

	//Draw image
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void image_destroy(Image* image) {
	glDeleteBuffers(1, &image->vbuffer);
	glDeleteProgram(image->prog);
}
