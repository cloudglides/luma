#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"

#define ROWS 2
#define COLS 2

uint8_t row_pins[ROWS] = {2, 3};
uint8_t col_pins[COLS] = {4, 5};

uint8_t keymap[ROWS][COLS] = {
    {HID_KEY_A, HID_KEY_B},
    {HID_KEY_C, HID_KEY_D}
};

void matrix_init() {
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], 1);
    }
    for (int i = 0; i < COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_up(col_pins[i]);
    }
}

void matrix_scan(uint8_t *keys, int *count) {
    *count = 0;
    for (int r = 0; r < ROWS; r++) {
        gpio_put(row_pins[r], 0);
        sleep_us(5);
        for (int c = 0; c < COLS; c++) {
            if (!gpio_get(col_pins[c])) {
                keys[(*count)++] = keymap[r][c];
            }
        }
        gpio_put(row_pins[r], 1);
    }
}

int main() {
    board_init();
    tusb_init();
    matrix_init();

    uint8_t keys[6];
    int count = 0;

    while (1) {
        tud_task();

        if (tud_hid_ready()) {
            matrix_scan(keys, &count);
            tud_hid_keyboard_report(0, 0, keys);
        }

        sleep_ms(10);
    }
}
