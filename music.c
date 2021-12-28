#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdatomic.h>
#include <sys/wait.h>
#include "music.h"

void* music_wait(void* ptr) {
	Music* music = ptr;
	char buffer[32], op;
	while (true) {
		pthread_testcancel();
		fgets(buffer, 32, music->in);
		sscanf(buffer, "@%c", &op);
		if (op == 'P') {
			int state;
			sscanf(buffer, "@P %d", &state);

			//Modify music->status
			pthread_mutex_lock(&music->lock);
			switch (state) {
				case 0: music->status.state = MUSIC_STOP;break;
				case 1: music->status.state = MUSIC_PAUSE;break;
				case 2: music->status.state = MUSIC_PLAYING;break;
			}
			pthread_mutex_unlock(&music->lock);
		} else if (op == 'F') {
			int fc, fl;
			float sec, secl;
			sscanf(buffer, "@F %d %d %f %f", &fc, &fl, &sec, &secl);

			//Modify music->status
			pthread_mutex_lock(&music->lock);
			music->status.time_elapsed = sec;
			music->status.time_left = secl;
			pthread_mutex_unlock(&music->lock);
		}
	}
}

MusicStatus music_status(Music* music) {
	MusicStatus status;
	pthread_mutex_lock(&music->lock);
	status = music->status;
	pthread_mutex_unlock(&music->lock);
	return status;
}

void music_play(Music* music, const char* filename) {
	//Stop current player
	fprintf(music->out, "stop\n");
	fflush(music->out);

	//Play song file
	fprintf(music->out, "load %s\n", filename);
	fflush(music->out);
}

void music_pause(Music* music) {
	fprintf(music->out, "pause\n");
	fflush(music->out);
}

void music_volume(Music* music, float value) {
	fprintf(music->out, "volume %d\n", (int)(value * 100.0));
	fflush(music->out);
}

int music_init(Music* music) {
	//Init variables
	music->status.state = MUSIC_STOP;
	music->status.time_elapsed = music->status.time_left = 0;

	//Init lock
	if (pthread_mutex_init(&music->lock, NULL) != 0) {
		perror("Mutex error");
		return 1;
	}

	//Create pipes
	int in_fd[2], out_fd[2];
	if (pipe(in_fd) == -1 || pipe(out_fd) == -1) {
		perror("Pipe error");
		return 1;
	}

	//Launch mpg123
	music->pid = fork();
	if (music->pid == 0) {
		close(out_fd[1]);
		close(in_fd[0]);
		dup2(out_fd[0], STDIN_FILENO);
		dup2(in_fd[1], STDOUT_FILENO);
		execl("/usr/bin/mpg123", "mpg123", "-R", NULL);
	}
	close(out_fd[0]);
	close(in_fd[1]);
	music->out = fdopen(out_fd[1], "w");
	music->in = fdopen(in_fd[0], "r");

	//Launch thread
	pthread_create(&music->wait_thread, NULL, music_wait, music);
	return 0;
}

void music_destroy(Music* music) {
	//Terminate thread
	pthread_cancel(music->wait_thread);
	pthread_join(music->wait_thread, NULL);

	//Terminate child process
	fprintf(music->out, "stop\nquit\n");
	fflush(music->out);
	waitpid(music->pid, NULL, 0);

	//Destroy lock
	pthread_mutex_destroy(&music->lock);
}
