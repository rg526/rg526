#include <stdio.h>
#include "gpio.h"

const char* gpio_chipname = "gpiochip0";
const int gpio_in_port[GPIO_IN_CNT] = {17, 27, 22, 10, 9};
const int gpio_out_port[GPIO_OUT_CNT] = {25, 8, 7, 1};

int gpio_init(GPIO* gpio) {
#ifdef RASPI
	gpio->chip = gpiod_chip_open_by_name(gpio_chipname);
	if (gpio->chip == NULL) {
		fprintf(stderr, "gpio chip open failed\n");
		return -1;
	}
	for (size_t i = 0;i < GPIO_IN_CNT;i++) {
		gpio->in[i] = gpiod_chip_get_line(gpio->chip, gpio_in_port[i]);
		if (gpio->in[i] == NULL) {
			gpiod_chip_close(gpio->chip);
			return -1;
		}
		if (gpiod_line_request_input(gpio->in[i], "rg526") != 0) {
			gpiod_chip_close(gpio->chip);
			return -1;
		}
	}
	for (size_t i = 0;i < GPIO_OUT_CNT;i++) {
		gpio->out[i] = gpiod_chip_get_line(gpio->chip, gpio_out_port[i]);
		if (gpio->out[i] == NULL) {
			gpiod_chip_close(gpio->chip);
			return -1;
		}
		if (gpiod_line_request_output(gpio->out[i], "rg526", 0) != 0) {
			gpiod_chip_close(gpio->chip);
			return -1;
		}
	}
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

void gpio_destroy(GPIO* gpio) {
#ifdef RASPI
	gpiod_chip_close(gpio->chip);
#endif
}
