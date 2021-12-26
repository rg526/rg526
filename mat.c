#include "mat.h"
#include <math.h>

mat_v* mat_ptr(Mat* mat) {
	return mat->v;
}

void mat_multiply(Mat* mat, Mat* a, Mat* b) {
	mat_v v[16];
	for (int i = 0;i < 16;i++) v[i] = 0;
	for (int i = 0;i < 4;i++) 
		for (int j = 0;j < 4;j++) 
			for (int k = 0;k < 4;k++) 
				v[i * 4 + k] += a->v[i * 4 + j] * b->v[j * 4 + k];
	for (int i = 0;i < 16;i++) mat->v[i] = v[i];
}

void mat_copy(Mat* mat, Mat* a) {
	for (int i = 0;i < 16;i++) mat->v[i] = a->v[i];
}

void mat_zero(Mat* mat) {
	for (int i = 0;i < 16;i++) mat->v[i] = 0;
}

void mat_identity(Mat* mat) {
	mat_zero(mat);
	mat->v[0 * 4 + 0] = mat->v[1 * 4 + 1] = mat->v[2 * 4 + 2] = mat->v[3 * 4 + 3] = 1;
}

void mat_projection(Mat* mat, mat_v l, mat_v r, mat_v b, mat_v t, mat_v n, mat_v f) {
	mat->v[0 * 4 + 0] = 2 * n / (r - l);
	mat->v[0 * 4 + 1] = 0;
	mat->v[0 * 4 + 2] = -(r + l) / (r - l);
	mat->v[0 * 4 + 3] = 0;
	mat->v[1 * 4 + 0] = 0;
	mat->v[1 * 4 + 1] = 2 * n / (t - b);
	mat->v[1 * 4 + 2] = -(t + b) / (t - b);
	mat->v[1 * 4 + 3] = 0;
	mat->v[2 * 4 + 0] = 0;
	mat->v[2 * 4 + 1] = 0;
	mat->v[2 * 4 + 2] = (f + n) / (f - n);
	mat->v[2 * 4 + 3] = -2 * f * n / (f - n);
	mat->v[3 * 4 + 0] = 0;
	mat->v[3 * 4 + 1] = 0;
	mat->v[3 * 4 + 2] = 1;
	mat->v[3 * 4 + 3] = 0;
}

void mat_translate(Mat* mat, mat_v dx, mat_v dy, mat_v dz) {
	mat_zero(mat);
	mat->v[0 * 4 + 0] = 1;
	mat->v[1 * 4 + 1] = 1;
	mat->v[2 * 4 + 2] = 1;
	mat->v[3 * 4 + 3] = 1;
	mat->v[0 * 4 + 3] = dx;
	mat->v[1 * 4 + 3] = dy;
	mat->v[2 * 4 + 3] = dz;
}

void mat_scale(Mat* mat, mat_v sx, mat_v sy, mat_v sz) {
	mat_zero(mat);
	mat->v[0 * 4 + 0] = sx;
	mat->v[1 * 4 + 1] = sy;
	mat->v[2 * 4 + 2] = sz;
	mat->v[3 * 4 + 3] = 1;
}

void mat_rotate(Mat* mat, mat_v x, mat_v y, mat_v z) {
	mat_zero(mat);
	mat_v p = x / 180.0 * M_PI,
			h = y / 180.0 * M_PI,
			r = z / 180.0 * M_PI;
	mat->v[0 * 4 + 0] = cos(r) * cos(h) - sin(r) * sin(p) * sin(h);
	mat->v[0 * 4 + 1] = -sin(r) * cos(p);
	mat->v[0 * 4 + 2] = cos(r) * sin(h) + sin(r) * sin(p) * cos(h);
	mat->v[1 * 4 + 0] = sin(r) * cos(h) + cos(r) * sin(p) * sin(h);
	mat->v[1 * 4 + 1] = cos(r) * cos(p);
	mat->v[1 * 4 + 2] = sin(r) * sin(h) - cos(r) * sin(p) * cos(h);
	mat->v[2 * 4 + 0] = -cos(p) * sin(h);
	mat->v[2 * 4 + 1] = sin(p);
	mat->v[2 * 4 + 2] = cos(p) * cos(h);
	mat->v[3 * 4 + 3] = 1;
}

void mat_swapyz(Mat* mat) {
	mat_zero(mat);
	mat->v[0 * 4 + 0] = 1;
	mat->v[1 * 4 + 2] = 1;
	mat->v[2 * 4 + 1] = 1;
	mat->v[3 * 4 + 3] = 1;
}
