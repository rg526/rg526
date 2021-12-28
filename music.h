#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>

#ifndef __MUSIC_H__
#define __MUSIC_H__

enum MusicState {
	MUSIC_STOP,
	MUSIC_PAUSE,
	MUSIC_PLAYING
};

typedef struct {
	float time_elapsed, time_left;
	enum MusicState state;
} MusicStatus;

typedef struct {
	pid_t pid;
	pthread_t wait_thread;
	FILE *in, *out;

	MusicStatus status;
	pthread_mutex_t lock;
} Music;

MusicStatus music_status(Music*);

void music_play(Music*, const char*);
void music_pause(Music*);
void music_volume(Music*, float);

int music_init(Music*);
void music_destroy(Music*);

#endif
