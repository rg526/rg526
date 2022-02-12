#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#ifdef RASPI
#include <gpiod.h>
#endif

#ifndef __GPIO_H__
#define __GPIO_H__

#define GPIO_IN_CNT 5
#define GPIO_OUT_CNT 4

typedef struct {
	struct timeval tv;
	double req;
	int active;
} GPIODelay;

typedef struct {
#ifdef RASPI
	struct gpiod_chip* chip;
	struct gpiod_line *in[GPIO_IN_CNT], *out[GPIO_OUT_CNT];

	GPIODelay delay[GPIO_OUT_CNT];
	pthread_mutex_t lock[GPIO_OUT_CNT];
	pthread_t delay_scan_thread;
#endif
} GPIO;

extern const char* gpio_chipname;
extern const int gpio_in_port[];
extern const int gpio_out_port[];

int gpio_init(GPIO*);
void gpio_destroy(GPIO*);

int gpio_input(GPIO*, size_t);
void gpio_output(GPIO*, size_t, int);
void gpio_output_delay(GPIO*, size_t, double);

#endif
