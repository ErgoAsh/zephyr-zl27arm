#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/tracing/tracing.h>

#include <SEGGER_SYSVIEW.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static const struct gpio_dt_spec button =
    GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

#define BUTTON_STATE_MARKER_ID  0
#define LED_STATE_MARKER_ID  1
#define LED_TOGGLE_CALCULATION_MARKER_ID  1

int is_led_enabled = false;

static SEGGER_SYSVIEW_DATA_REGISTER button_plot;
static SEGGER_SYSVIEW_DATA_REGISTER led_plot;

static void setup_plots() {
    button_plot.ID = BUTTON_STATE_MARKER_ID;
    button_plot.sName = "Button State";
    button_plot.DataType = SEGGER_SYSVIEW_TYPE_U32;
    button_plot.RangeMin = 0;
    button_plot.RangeMax = 4;
    button_plot.ScalingFactor = 1;
    button_plot.Offset = 0;
    SEGGER_SYSVIEW_RegisterData(&button_plot);

    led_plot.ID = LED_STATE_MARKER_ID;
    led_plot.sName = "LED State";
    led_plot.DataType = SEGGER_SYSVIEW_TYPE_U32;
    led_plot.RangeMin = 0;
    led_plot.RangeMax = 4;
    led_plot.ScalingFactor = 1;
    led_plot.Offset = 2;
    SEGGER_SYSVIEW_RegisterData(&led_plot);
}

int gpio_init(const struct gpio_dt_spec *gpio, const gpio_flags_t extra_flags) {
    // Check if GPIO exist
    if (!gpio_is_ready_dt(gpio)) {
        LOG_ERR("Error: GPIO device %s is not ready", gpio->port->name);
        return 1; // Indicate error
    } else {
        LOG_INF("GPIO device %s is ready", gpio->port->name);
    }

    // Init GPIO with extra flags
    const int ret = gpio_pin_configure_dt(gpio, extra_flags);
    if (ret < 0) {
        LOG_ERR("Error %d: failed to configure pin %d", ret, gpio->pin);
        return 1; // Indicate error
    } else {
        LOG_INF("Successfully configured pin %d", gpio->pin);
    }

    return ret;
}

int main(void)
{
    k_msleep(100);

    LOG_INF("System Starting via RTT...");
    SEGGER_SYSVIEW_Conf();

    SEGGER_SYSVIEW_NameMarker(LED_TOGGLE_CALCULATION_MARKER_ID, "LED toggle calculation");

    setup_plots();

    gpio_init(&led, GPIO_OUTPUT_ACTIVE);
    gpio_init(&button, GPIO_INPUT);

    int current_btn_val = -1;
    int last_btn_val = -1;

    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        current_btn_val = gpio_pin_get_dt(&button);

        if (current_btn_val != last_btn_val) {
            SEGGER_SYSVIEW_DATA_SAMPLE button_sample;
            button_sample.ID = BUTTON_STATE_MARKER_ID;
            button_sample.pValue.pU32 = (U32*) &current_btn_val;
            SEGGER_SYSVIEW_SampleData(&button_sample);

            last_btn_val = current_btn_val;
            SEGGER_SYSVIEW_PrintfHost("Button Processed: %d", current_btn_val);
        }

        if (current_btn_val == true) {
            SEGGER_SYSVIEW_OnUserStart(LED_TOGGLE_CALCULATION_MARKER_ID);
            is_led_enabled = !is_led_enabled;

            SEGGER_SYSVIEW_DATA_SAMPLE led_state_sample;
            led_state_sample.ID = LED_STATE_MARKER_ID;
            led_state_sample.pValue.pU32 = (U32*) &is_led_enabled;
            SEGGER_SYSVIEW_SampleData(&led_state_sample);

            gpio_pin_set_dt(&led, is_led_enabled);
            SEGGER_SYSVIEW_OnUserStop(LED_TOGGLE_CALCULATION_MARKER_ID);
            //LOG_INF("LED state changed: %d", is_led_enabled);

            k_msleep(250);
        }

        k_msleep(50);
    }
}