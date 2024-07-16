#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdint.h>

typedef enum
  {
        /* Komut yok*/
        CMD_NOP,
        /* Pin okuma komutu */
        CMD_PIN_R,
        /* Pin yazma komutu */
        CMD_PIN_W,
        /* Port okuma komutu */
        CMD_PORT_R,
        /* Port yazma komutu */
        CMD_PORT_W,
        /* UART mesajı okuma komutu */
        CMD_UART_R,
        /* UART mesajı gönderme komutu */
        CMD_UART_W,
  }app_cmd_t;

typedef struct
  {
        /* işlenecek komut */
        app_cmd_t cmd;
        /* fonksiyondan döndürülen değer */
        int8_t ret_val;
        /* fonksiyon 1. parametresi */
        uint8_t param1;
        /* fonksiyon 2. parametresi */
        uint8_t param2; 
  }app_instructs_t;

// Register ve diğer değişkenlerin extern ile bildirimi
extern uint8_t reg_config_port_1;
extern uint8_t reg_config_port_2;
extern uint8_t reg_config_uart;
extern uint8_t reg_state_port_1;
extern uint8_t reg_state_port_2;
extern uint8_t reg_uart_rx;
extern uint16_t reg_uart_chn;
extern app_instructs_t app_sequence_instructs[10];

#endif
