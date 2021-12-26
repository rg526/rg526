#include "vec.h"
#include <math.h>

vec_v* vec_ptr(Vec* vec) {
	return vec->v;
}

void vec_copy(Vec* vec, Vec* a) {
	for (int i = 0;i < 3;i++) vec->v[i] = a->v[i];
}

void vec_input(Vec* vec, FILE* f) {
	fscanf(f, "%f%f%f", &vec->v[0], &vec->v[1], &vec->v[2]);
}

void vec_cross(Vec* vec, Vec* a, Vec* b) {
	vec_v v[3];
	v[0] = a->v[1] * b->v[2] - a->v[2] * b->v[1];
	v[1] = a->v[2] * b->v[0] - a->v[0] * b->v[2];
	v[2] = a->v[0] * b->v[1] - a->v[1] * b->v[0];
	for (int i = 0;i < 3;i++) vec->v[i] = v[i];
}

void vec_add(Vec* vec, Vec* a, Vec* b) {
	for (int i = 0;i < 3;i++) vec->v[i] = a->v[i] + b->v[i];
}

void vec_subtract(Vec* vec, Vec* a, Vec* b) {
	for (int i = 0;i < 3;i++) vec->v[i] = a->v[i] - b->v[i];
}

void vec_normalize(Vec* vec, Vec* a) {
	vec_v len = 0;
	for (int i = 0;i < 3;i++) len += a->v[i] * a->v[i];
	len = sqrt(len);
	for (int i = 0;i < 3;i++) vec->v[i] = a->v[i] / len;
}
