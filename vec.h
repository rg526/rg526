#include <GLES3/gl3.h>
#include <stdio.h>

#ifndef __VEC_H__
#define __VEC_H__

typedef GLfloat vec_v;

typedef struct {
	vec_v v[3];
} Vec;

vec_v* vec_ptr(Vec*);
void vec_copy(Vec*, Vec*);

void vec_input(Vec*, FILE*);
void vec_cross(Vec*, Vec*, Vec*);
void vec_add(Vec*, Vec*, Vec*);
void vec_subtract(Vec*, Vec*, Vec*);
void vec_normalize(Vec*, Vec*);

#endif
