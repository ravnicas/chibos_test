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
static uint8_t debouncing = DEBOUNCE;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static matrix_row_t read_cols(void);
static void init_cols(void);
static void unselect_rows(void);
static void select_row(uint8_t row);


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

#define LED_ON()    do { palSetPad(TEENSY_PIN13_IOPORT, TEENSY_PIN13) ;} while (0)
#define LED_OFF()   do { palClearPad(TEENSY_PIN13_IOPORT, TEENSY_PIN13); } while (0)
#define LED_TGL()   do { palTogglePad(TEENSY_PIN13_IOPORT, TEENSY_PIN13); } while (0)

static const I2CConfig i2ccfg = {
    700000,
};

void init_mcp23017(void){
    uint8_t tx[3];

    tx[0] = 0x00;
    tx[1] = 0x00;
    tx[2] = 0x3F;
    i2cMasterTransmitTimeout(&I2CD1, 0x20, tx, 3, NULL, 0, 100);

    tx[0] = 0x0C;
    tx[1] = 0x00;
    tx[2] = 0x3F;
    i2cMasterTransmitTimeout(&I2CD1, 0x20, tx, 3, NULL, 0, 100);
}

void matrix_init(void)
{
    palSetPadMode(GPIOB, 0, PAL_MODE_ALTERNATIVE_2); // PTB0/I2C0/SCL
    palSetPadMode(GPIOB, 1, PAL_MODE_ALTERNATIVE_2); // PTB1/I2C0/SDA

    i2cStart(&I2CD1, &i2ccfg);

    init_mcp23017();

    // initialize row and col
    unselect_rows();
    init_cols();

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
}

uint8_t matrix_scan(void)
{
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

/* Column pin configuration
 */
static void  init_cols(void)
{
    
}

/* Returns status of switches(1:on, 0:off) */
static matrix_row_t read_cols(void)
{
    uint8_t data = 0;
            
    uint8_t tx[1];
    uint8_t rx[1];

    tx[0] = 0x13;
    i2cMasterTransmitTimeout(&I2CD1, 0x20, tx, 1, rx, 1, 100);

    data = ~((uint8_t)rx[0]);
    return data;
}

/* Row pin configuration
 */
static void unselect_rows(void)
{
    
}

static void select_row(uint8_t row)
{
    if (row < 7) 
    {
        uint8_t tx[2];

        tx[0] = 0x12;
        tx[1] = (0xFF & ~(1<<row));
        i2cMasterTransmitTimeout(&I2CD1, 0x20, tx, 2, NULL, 0, 100);
        }
}
