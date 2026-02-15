#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "zephyr/logging/log.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static const struct gpio_dt_spec button =
    GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

int main() {
    if (!gpio_is_ready_dt(&led) || !gpio_is_ready_dt(&button)) {
        return 0;
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&button, GPIO_INPUT);

    while (true) {
        int value = gpio_pin_get_dt(&button);
        if (value == true) {
            is_led_enabled = !is_led_enabled;

            gpio_pin_set_dt(&led, is_led_enabled);
            LOG_INF("LED state changed: %d", is_led_enabled);

            k_msleep(250);
        }

        k_msleep(10); // Small debounce/delay
    }
    return 0;
}