#include "model.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>


int model_init(Model* mod, const char* filename) {
	FILE* f = fopen(filename, "r");
	if (f == NULL) {
		return -1;
	}
	vec_input(&mod->color, f);

	Vec* v_arr = NULL;
	size_t* s_arr = NULL;
	size_t** i_arr = NULL;
	Vec* vertex = NULL;
	Vec* normal = NULL;

	fscanf(f, "%zu%zu", &mod->n, &mod->m);
	v_arr = malloc(mod->n * sizeof(Vec));
	s_arr = malloc(mod->m * sizeof(size_t));
	i_arr = malloc(mod->m * sizeof(size_t*));
	if (v_arr == NULL || s_arr == NULL || i_arr == NULL) goto fail;

	for (size_t i = 0;i < mod->n;i++) vec_input(&v_arr[i], f);

	mod->length = 0;
	for (size_t i = 0;i < mod->m;i++) {
		fscanf(f, "%zu", &s_arr[i]);
		i_arr[i] = malloc(s_arr[i] * sizeof(size_t));
		if (i_arr[i] == NULL) goto fail;
		for (size_t j = 0;j < s_arr[i];j++) fscanf(f, "%zu", &i_arr[i][j]);
		mod->length += 3 * (s_arr[i] - 2);
	}

	vertex = malloc(mod->length * sizeof(Vec));
	normal = malloc(mod->length * sizeof(Vec));
	if (vertex == NULL || normal == NULL) goto fail;
	mod->vertex = vertex;
	mod->normal = normal;

	for (size_t i = 0;i < mod->m;i++) {
		for (size_t j = 1;j < s_arr[i] - 1;j++) {
			vec_copy(&vertex[0], &v_arr[i_arr[i][0]]);
			vec_copy(&vertex[1], &v_arr[i_arr[i][j]]);
			vec_copy(&vertex[2], &v_arr[i_arr[i][j + 1]]);
			Vec d1, d2, norm;
			vec_subtract(&d1, &vertex[1], &vertex[0]);
			vec_subtract(&d2, &vertex[2], &vertex[0]);
			vec_cross(&norm, &d1, &d2);
			vec_normalize(&norm, &norm);
			vec_copy(&normal[0], &norm);
			vec_copy(&normal[1], &norm);
			vec_copy(&normal[2], &norm);
			vertex += 3;
			normal += 3;
		}
	}

	free(v_arr);
	free(s_arr);
	for (size_t i = 0;i < mod->m;i++) {
		free(i_arr[i]);
	}
	free(i_arr);
	return 0;

fail:
	if (v_arr != NULL) free(v_arr);
	if (s_arr != NULL) free(s_arr);
	if (i_arr != NULL) {
		for (size_t i = 0;i < mod->m;i++) {
			if (i_arr[i] != NULL) free(i_arr[i]);
		}
		free(i_arr);
	}
	if (vertex != NULL) free(vertex);
	if (normal != NULL) free(normal);
	return -1;
}

void model_destroy(Model* mod) {
	free(mod->vertex);
	free(mod->normal);
}
