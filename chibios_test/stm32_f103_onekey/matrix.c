/*
Copyright 2012 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"

/*
 * scan matrix
 */
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "wait.h"

#ifndef DEBOUNCE
#   define DEBOUNCE 5
#endif
//static uint8_t debouncing = DEBOUNCE;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

/*
static matrix_row_t read_cols(void);
static void init_cols(void);
static void unselect_rows(void);
static void select_row(uint8_t row);
*/

static const SPIConfig ls_spicfg2 = {
    NULL,
    GPIOB,
    12,
    SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0
};

inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

/* generic STM32F103C8T6 board */
#ifdef BOARD_GENERIC_STM32_F103
#define LED_ON()    do { palClearPad(GPIOC, GPIOC_LED) ;} while (0)
#define LED_OFF()   do { palSetPad(GPIOC, GPIOC_LED); } while (0)
#define LED_TGL()   do { palTogglePad(GPIOC, GPIOC_LED); } while (0)
#endif

/* Maple Mini */
#ifdef BOARD_MAPLEMINI_STM32_F103
#define LED_ON()    do { palSetPad(GPIOB, 1) ;} while (0)
#define LED_OFF()   do { palClearPad(GPIOB, 1); } while (0)
#define LED_TGL()   do { palTogglePad(GPIOB, 1); } while (0)
#endif

void matrix_init(void)
{
    // initialize row and col
    //unselect_rows();
    //init_cols();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
    }

    //debug
    debug_matrix = true;
    LED_ON();
    wait_ms(500);
    LED_OFF();

    wait_ms(100);
    LED_ON();


    //SPI2
    palSetPadMode(GPIOB, 13, PAL_MODE_STM32_ALTERNATE_PUSHPULL);     /* New SCK.     */
    palSetPadMode(GPIOB, 14, PAL_MODE_INPUT_PULLUP);     /* New MISO.    */
    palSetPadMode(GPIOB, 15, PAL_MODE_STM32_ALTERNATE_PUSHPULL);     /* New MOSI.    */
    palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL);              /* New CS.      */

    palSetPadMode(GPIOB, 11, PAL_MODE_OUTPUT_PUSHPULL);              /* New CS.      */
    palSetPadMode(GPIOB, 10, PAL_MODE_OUTPUT_PUSHPULL);              /* New CS.      */



    
    spiStart(&SPID2, &ls_spicfg2);


    //uint8_t tx[6];
    //uint8_t rx[6];

    //tx[0] = 0x23;

    wait_ms(100);

    spiSelect(&SPID2);
    //spiExchange(&SPID2, 6, tx, rx);
    //spiUnselect(&SPID2);

    palSetPad(GPIOB, 11);
    palSetPad(GPIOB, 10);


    LED_OFF();
}

uint8_t matrix_scan(void)
{
    /*
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        select_row(i);
        //wait_us(30);  // without this wait read unstable value.
        matrix_row_t cols = read_cols();
        if (matrix_debouncing[i] != cols) {
            matrix_debouncing[i] = cols;
            if (debouncing) {
                debug("bounce!: "); debug_hex(debouncing); debug("\n");
            }
            debouncing = DEBOUNCE;
        }
        unselect_rows();
    }

    if (debouncing) {
        if (--debouncing) {
            wait_ms(1);
        } else {
            for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
                matrix[i] = matrix_debouncing[i];
            }
        }
    }
    */
    uint8_t tx[6] = {0};
    uint8_t rx_r[6] = {0};
    uint8_t rx_l[6] = {0};
    tx[0] = 0x23;

    //spiSelect(&SPID2);
    palClearPad(GPIOB, 11);
    spiExchange(&SPID2, 6, tx, rx_l);
    palSetPad(GPIOB, 11);
    //spiUnselect(&SPID2);

    //wait_ms(1);

    //spiSelect(&SPID2);
    palClearPad(GPIOB, 10);
    spiExchange(&SPID2, 6, tx, rx_r);
    palSetPad(GPIOB, 10);
    //spiUnselect(&SPID2);

    for(uint8_t i = 0; i < MATRIX_ROWS; i++)
    {
        //printf("%d %d \n\r", rx_l[i+1], rx_r[i+1]);
        matrix[i] = ((uint16_t)(rx_l[i+1] & 0x7F) | ((uint16_t)(rx_r[i+1] & 0xFE))<<6);
    }
    //printf("---------------------\n\r");
    //wait_ms(1);

    return 1;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

/* 

static void  init_cols(void)
{
#ifdef BOARD_MAPLEMINI_STM32_F103
    // don't need pullup/down, since it's pulled down in hardware
    palSetPadMode(GPIOB, 8, PAL_MODE_INPUT);
#else
    palSetPadMode(GPIOB, 8, PAL_MODE_INPUT_PULLDOWN);
#endif
}


static matrix_row_t read_cols(void)
{
    return ((palReadPad(GPIOB, 8)==PAL_LOW) ? 0 : (1<<0));
    // | ((palReadPad(...)==PAL_HIGH) ? 0 : (1<<1))
}


static void unselect_rows(void)
{
    // palSetPadMode(GPIOA, GPIOA_PIN10, PAL_MODE_INPUT); // hi-Z
}


static void select_row(uint8_t row)
{
    (void)row;
    // Output low to select
    // switch (row) {
    //     case 0:
    //         palSetPadMode(GPIOA, GPIOA_PIN10, PAL_MODE_OUTPUT_PUSHPULL);
    //         palSetPad(GPIOA, GPIOA_PIN10, PAL_LOW);
    //         break;
    // }
}
*/