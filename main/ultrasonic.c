#include "gpio.h"
#include "ultrasonic.h"
#include "esp_rom_sys.h"
#include "gptimer_types.h"
#include "gptimer.h"
#include "esp_err.h"
#include "esp_log.h"

#define ECHOPIN GPIO_NUM_1
#define TRIGPIN GPIO_NUM_2

long distance = 0;

static const char *tag = "TAG";

gpio_config_t echo_config = {
    .pin_bit_mask = 1ULL << ECHOPIN,
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
};

gpio_config_t trig_config = {
    .pin_bit_mask = 1ULL << TRIGPIN,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
};

void trig_pulse(void) {
    long distance = 0;
    gpio_set_level(TRIGPIN, 1);
    esp_rom_delay_us(20);
    gpio_set_level(TRIGPIN, 0);
    distance = get_distance();
}

gptimer_handle_t gptimer = NULL;
gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT, // Select the default clock source
    .direction = GPTIMER_COUNT_UP,      // Counting direction is up
    .resolution_hz = 1 * 1000 * 1000,   // Resolution is 1 MHz, i.e., 1 tick equals 1 microsecond
};

void start_timer(void) {
    // Create a timer instance
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    // Enable the timer
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    // Start the timer
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

double get_timer_count(void) {
    // Check the timer's resolution
    uint32_t resolution_hz;
    ESP_ERROR_CHECK(gptimer_get_resolution(gptimer, &resolution_hz));
    // Read the current count value
    uint64_t count;
    ESP_ERROR_CHECK(gptimer_get_raw_count(gptimer, &count));
    // (Optional) Convert the count value to time units (seconds)
    double time = (double)count / resolution_hz;

    return time;
}

long get_distance(void) {
    double time = get_timer_count();

    long distance = (334*time)/2;

    return distance;

    ESP_LOGI(tag, "distance: %f", distance);
}

