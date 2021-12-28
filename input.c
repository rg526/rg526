#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "esUtil.h"
#include "input.h"

void __input_keyboard_cb(ESContext* esContext, unsigned char ch, int a, int b) {
	Input *input = esContext->rg526_input;
	if (ch >= '0' && ch <= '4') {
		//Get line number
		size_t line = ch - '0';
		struct timeval current_time;
		gettimeofday(&current_time, NULL);

		//Activate line
		pthread_mutex_lock(&input->lock[line]);
		input->v[line].active = 1;
		input->v[line].tv = current_time;
		pthread_mutex_unlock(&input->lock[line]);
	}
}

InputLine input_query(Input* input, size_t line) {
	InputLine ret;
	pthread_mutex_lock(&input->lock[line]);
	ret = input->v[line];
	pthread_mutex_unlock(&input->lock[line]);
	return ret;
}

InputLine input_query_clear(Input* input, size_t line) {
	InputLine ret;
	pthread_mutex_lock(&input->lock[line]);
	ret = input->v[line];
	input->v[line].active = 0;
	memset(&input->v[line].tv, 0, sizeof(input->v[line].tv));
	pthread_mutex_unlock(&input->lock[line]);
	return ret;
}

void input_clearall(Input* input) {
	for (size_t i = 0;i < INPUT_COUNT;i++) {
		pthread_mutex_lock(&input->lock[i]);
		input->v[i].active = 0;
		memset(&input->v[i].tv, 0, sizeof(input->v[i].tv));
		pthread_mutex_unlock(&input->lock[i]);
	}
}

int input_init(Input* input, ESContext* esContext) {
	//Init input structure
	for (size_t i = 0;i < INPUT_COUNT;i++) {
		if (pthread_mutex_init(&input->lock[i], NULL) != 0) {
			for (size_t j = 0;j < i;j++) pthread_mutex_destroy(&input->lock[j]);
			perror("Input mutex init failed");
			return 1;
		}
		input->v[i].active = 0;
		memset(&input->v[i].tv, 0, sizeof(input->v[i].tv));
	}

	//Keyboard callback registration
	esContext->rg526_input = input;
	esContext->keyFunc = __input_keyboard_cb;
	input->esContext = esContext;
	return 0;
}

void input_destroy(Input* input) {
	//Destroy locks
	for (size_t i = 0;i < INPUT_COUNT;i++) {
		pthread_mutex_destroy(&input->lock[i]);
	}

	//Clear esContext input content
	input->esContext->rg526_input = NULL;
	input->esContext->keyFunc = NULL;
}

