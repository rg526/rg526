#include "mat.h"
#include "vec.h"
#include <stddef.h>

#ifndef __MODEL_H__
#define __MODEL_H__

typedef struct Model {
	size_t n, m, length;
	Vec color;
	Vec* vertex;
	Vec* normal;
} Model;

int model_init(Model*, const char*);
void model_destroy(Model*);

#endif
