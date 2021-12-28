#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>
#include "esUtil.h"

#ifndef __INPUT_H__
#define __INPUT_H__

#define INPUT_COUNT 5

typedef struct {
	struct timeval tv;
	int active;
} InputLine;

typedef struct {
	ESContext* esContext;
	InputLine v[5];
	pthread_mutex_t lock[5];
} Input;

int input_init(Input*, ESContext*);
void input_destroy(Input*);
InputLine input_query(Input*, size_t line);
InputLine input_query_clear(Input*, size_t line);
void input_clearall(Input*);

#endif
