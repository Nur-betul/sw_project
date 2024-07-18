#include <stdint.h>
#include "variables.h"

/* 1. Port pin ayarlari */
  uint8_t reg_config_port_1;
  /* 2. Port pin ayarlari */
  uint8_t reg_config_port_2;
  /* UART birim ayarlari */
  uint8_t reg_config_uart;

  /* 1. Port pin durumalari */
  uint8_t reg_state_port_1;
  /* 1. Port pin durumalari */
  uint8_t reg_state_port_2;
  /* UART gelen veri registeri */
  uint8_t reg_uart_rx;
  /* UART veri tasiyan kanal registeri */
  uint16_t reg_uart_chn;

  /* Uygualama komutlar dizisi */
  app_instructs_t app_sequence_instructs[10] = 
  {
   {
   .cmd = CMD_NOP,
   .ret_val = 0x0,
   .param1 = 0x0,
   .param2 = 0x0,
   }
  };