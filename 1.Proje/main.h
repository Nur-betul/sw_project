#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stdbool.h>

// main.c'deki fonksiyon prototipleri burada tanımlıdır.
extern uint8_t reg_config_port_1;
extern uint8_t reg_config_port_2;
extern uint8_t reg_config_uart;
// extern uint8_t reg_uart_rx;
// extern uint16_t reg_uart_chn;

  void clk_init(void);
  bool uart_init(uint8_t baud_rate, uint8_t parity_bit);
  bool pin_init(uint8_t pin_num, uint8_t pin_mode);
  bool clk_ticked(void);
  int8_t pin_read(uint8_t pin_num);
  bool pin_write(uint8_t pin_num, uint8_t pin_state);
  uint8_t port_read(uint8_t port_num);
  bool port_write(uint8_t port_num, uint8_t port_state);
  bool uart_read(void);
  bool uart_write(uint8_t data_size);
  


#endif // MAIN_H
