#include <GLES3/gl3.h>

#ifndef __MAT_H__
#define __MAT_H__

typedef GLfloat mat_v;

typedef struct {
	mat_v v[16];
} Mat;

mat_v* mat_ptr(Mat*);

void mat_multiply(Mat*, Mat*, Mat*);
void mat_copy(Mat*, Mat*);

void mat_zero(Mat*);
void mat_identity(Mat*);
void mat_projection(Mat*, mat_v, mat_v, mat_v, mat_v, mat_v, mat_v);
void mat_translate(Mat*, mat_v, mat_v, mat_v);
void mat_scale(Mat*, mat_v, mat_v, mat_v);
void mat_rotate(Mat*, mat_v, mat_v, mat_v);
void mat_swapyz(Mat*);

#endif
