/***************************************************************************************************
* File Name: test_mcu.c
* Module: test
* Abstract: Test cases for "mcu" module
* Author: Naim ALMASRI
* Date: 06.07.2024
* Revision: 1.0
***************************************************************************************************/

/***************************************************************************************************
* Header files.
***************************************************************************************************/

#include "main.h"
#include "unity.h"

/***************************************************************************************************
* Macro definitions.
***************************************************************************************************/

#define PIN_NUM  (2U)
#define PINS_IN  ((uint8_t[PIN_NUM]){0x08U, 0x0BU})
#define PINS_OUT ((uint8_t[PIN_NUM]){0x00U, 0x05U})

#define PIN_MODE_IN  (0U)
#define PIN_MODE_OUT (1U)

#define PIN_STATE_LOW  (0U)
#define PIN_STATE_HIGH (1U)

#define PORT_1 (0U)
#define PORT_2 (1U)

/***************************************************************************************************
* Local type definitions.
***************************************************************************************************/

/***************************************************************************************************
 * Local data definitions.
***************************************************************************************************/

static bool s_retval_pin_init = true;
static bool s_retval_uart_init = false;
static bool s_retval_clk_init = false;

static uint8_t pins[2][PIN_NUM] = {PINS_IN, PINS_OUT};

/***************************************************************************************************
* Local function definitions.
***************************************************************************************************/

/***************************************************************************************************
* External data definitions.
***************************************************************************************************/

/***************************************************************************************************
* External function definitions.
***************************************************************************************************/

void setUp()
{
    uint8_t i = 0U;
    uint8_t j = 0U;

    /* Initialize the clock with 10Hz frequency */
    s_retval_clk_init = clk_init(void);

    /* Traverse through pin modes */
    for (i = 0; i < 2U; i++)
    {
        /* Traverse through pins */
        for (j = 0; j < PIN_NUM; j++)
        {
            s_retval_pin_init &= pin_init(pins[i][j], i);
        }
    }

    /* Initialize the UART with 10 baud rate and no parity bit */
    s_retval_uart_init = uart_init(10U, 0);
}

void tearDown()
{

}

int main(void)
{
    UNITY_BEGIN();

    /* Test the initialization of the MCU */
    TEST_ASSERT_EQUAL_UINT8(1U, s_retval_clk_init);
    TEST_ASSERT_EQUAL_UINT8(1U, s_retval_pin_init);
    TEST_ASSERT_EQUAL_UINT8(1U, s_retval_uart_init);

    /* Test the initialization of the configuration registers */
    TEST_ASSERT_EQUAL_UINT8(33U, reg_config_port_1);
    TEST_ASSERT_EQUAL_UINT8(0U, reg_config_port_2);
    TEST_ASSERT_EQUAL_UINT8(10U, reg_config_uart);

    /* Test write funcionality of the pins */
    TEST_ASSERT_EQUAL_UINT8(1U, pin_write(pins[PIN_MODE_OUT][0], PIN_STATE_HIGH));
    TEST_ASSERT_EQUAL_UINT8(1U, pin_write(pins[PIN_MODE_OUT][1], PIN_STATE_LOW));
    TEST_ASSERT_EQUAL_UINT8(1U, pin_write(pins[PIN_MODE_OUT][0], PIN_STATE_LOW));
    TEST_ASSERT_EQUAL_UINT8(1U, pin_write(pins[PIN_MODE_OUT][1], PIN_STATE_HIGH));
    TEST_ASSERT_EQUAL_UINT8(0U, pin_write(pins[PIN_MODE_IN][0], PIN_STATE_LOW));
    TEST_ASSERT_EQUAL_UINT8(0U, port_write(PORT_2, PIN_STATE_HIGH));

    /* Test read funcionality of the pins */
    TEST_ASSERT_EQUAL_UINT8(32U, port_read(PORT_1));
    TEST_ASSERT_EQUAL_INT8(0U, pin_read(pins[PIN_MODE_OUT][0]));
    TEST_ASSERT_EQUAL_INT8(-1, pin_read(0xAA));

    UNITY_END();

    return 0;
}