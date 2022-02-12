#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "gpio.h"

const char* gpio_chipname = "gpiochip0";
const int gpio_in_port[GPIO_IN_CNT] = {17, 24, 23, 9, 22};
const int gpio_out_port[GPIO_OUT_CNT] = {25, 11, 10, 27};

void* __gpio_scan_delay(void* ptr) {
#ifdef RASPI
	GPIO* gpio = ptr;
	while (1) {
		pthread_testcancel();

		struct timeval current_time;
		gettimeofday(&current_time, NULL);

		for (size_t i = 0;i < GPIO_OUT_CNT;i++) {
			pthread_mutex_lock(&gpio->lock[i]);

			if (gpio->delay[i].active) {
				double delta_time = (double)(current_time.tv_sec - gpio->delay[i].tv.tv_sec) + 1e-6 * (double)(current_time.tv_usec - gpio->delay[i].tv.tv_usec);
				if (delta_time > gpio->delay[i].req) {
					gpio->delay[i].active = 0;
					gpio_output(gpio, i, 0);
				}
			}

			pthread_mutex_unlock(&gpio->lock[i]);
		}
		usleep(1000);
	}
#endif
	return NULL;
}

int gpio_init(GPIO* gpio) {
#ifdef RASPI
	gpio->chip = gpiod_chip_open_by_name(gpio_chipname);
	if (gpio->chip == NULL) {
		fprintf(stderr, "gpio chip open failed\n");
		return 1;
	}
	for (size_t i = 0;i < GPIO_IN_CNT;i++) {
		gpio->in[i] = gpiod_chip_get_line(gpio->chip, gpio_in_port[i]);
		if (gpio->in[i] == NULL) {
			gpiod_chip_close(gpio->chip);
			return 1;
		}
		if (gpiod_line_request_input(gpio->in[i], "rg526") != 0) {
			gpiod_chip_close(gpio->chip);
			return 1;
		}
	}
	for (size_t i = 0;i < GPIO_OUT_CNT;i++) {
		gpio->out[i] = gpiod_chip_get_line(gpio->chip, gpio_out_port[i]);
		if (gpio->out[i] == NULL) {
			gpiod_chip_close(gpio->chip);
			return 1;
		}
		if (gpiod_line_request_output(gpio->out[i], "rg526", 0) != 0) {
			gpiod_chip_close(gpio->chip);
			return 1;
		}
	}

	for (size_t i = 0;i < GPIO_OUT_CNT;i++) {
		if (pthread_mutex_init(&gpio->lock[i], NULL) != 0) {
			gpiod_chip_close(gpio->chip);
			return 1;
		}
		gpio->delay[i].active = 0;
	}

	pthread_create(&gpio->delay_scan_thread, NULL, __gpio_scan_delay, gpio);
#endif
	return 0;
}

int gpio_input(GPIO* gpio, size_t line) {
#ifdef RASPI
	return gpiod_line_get_value(gpio->in[line]);
#else
	return 0;
#endif
}

void gpio_output(GPIO* gpio, size_t line, int value) {
#ifdef RASPI
	gpiod_line_set_value(gpio->out[line], value);
#endif
}

void gpio_output_delay(GPIO* gpio, size_t line, double req) {
#ifdef RASPI
	pthread_mutex_lock(&gpio->lock[line]);

	gpio->delay[line].active = 1;
	gpio->delay[line].req = req;
	gettimeofday(&gpio->delay[line].tv, NULL);

	gpio_output(gpio, line, 1);

	pthread_mutex_unlock(&gpio->lock[line]);
#endif
}

void gpio_destroy(GPIO* gpio) {
#ifdef RASPI
	pthread_cancel(gpio->delay_scan_thread);
	pthread_join(gpio->delay_scan_thread, NULL);

	for (size_t i = 0;i < GPIO_IN_CNT;i++) {
		gpiod_line_release(gpio->in[i]);
	}
	for (size_t i = 0;i < GPIO_OUT_CNT;i++) {
		gpiod_line_release(gpio->out[i]);
	}
	gpiod_chip_close(gpio->chip);
#endif
}
