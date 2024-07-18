#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "variables.h"
#include <stdlib.h>
#include <time.h>
#include "main.h"

// Mevcut main.c içeriğiniz buraya gelecek
uint8_t reg_config_port_1 = 33;
uint8_t reg_config_port_2 = 0;
uint8_t reg_config_uart = 10;
static clock_t start_time;
app_instructs_t instr;
uint8_t data;

  /*
   * @brief sistem saatini ayarlama fonksiyonu 
   */
  void clk_init(void);

  /*
   * @brief UART haberlesme ayarlama fonksiyonu
   * @param baud_rate veri gonderme hizi (1bps, 10bps, 100bps)
   * @return true islem basarili ise
   *         false parametreler hatali ise
   */
  bool uart_init(uint8_t baud_rate, uint8_t parity_bit);

  /*
   * @brief pin ayarlama fonksiyonu
   * @param pin_num pin numarasi
   *        pin_mode 0x0 pin giris, 0x01 pin cikis
   * @return true islem basarili ise
   *         false parametreler hatali ise
   */
  bool pin_init(uint8_t pin_num, uint8_t pin_mode);

  /*
   * @brief sistem saatinde ayarlanan surenin gecip gecmedigini sorgulama fonksiyonu
   * @return true sure gecti
   *         false sure henuz gecmedi
   */
  bool clk_ticked(void);

  /*
   * @brief pin okuma fonksiyonu
   * @param pin_num pin numarasi
   * @return -1 pin numarasi yanlis ise
   *          0 pin low
   *          1 pin high
   */
  int8_t pin_read(uint8_t pin_num);
  
  /*
   * @brief pin yazma fonksiyonu
   * @param pin_num pin numarasi
   *        pin_state pin durumu 0 Low 1 High
   * @return true islem basarili ise
   *         false parametreler hatali ise
   */
  bool pin_write(uint8_t pin_num, uint8_t pin_state);
  
  /*
   * @brief porttaki butun pinleri okuma fonksiyonu 
   * @note hem giris hem cikis pinlerini okur
   * @param port_num port numarasi
   * @return 0x00 - 0xFF pin durumlarina gore her bit bir pini temsil eder
   */
  uint8_t port_read(uint8_t port_num);
  
  /*
   * @brief porttaki butun pinleri yazma fonksiyonu
   * @note sadece cikis pinlerini etkilemeli
   * @param port_num port numarasi
   *        port_state 0x00 0xFF pin durumlarina gore her bit bir pini temsil eder
   * @return true islem basarili ise
   *         false parametreler hatali ise
   */
  bool port_write(uint8_t port_num, uint8_t port_state);

   /*
   * @brief UART portundan gelen veriyi okuma fonksiyonu
   * @return true arabellekteki veri başarılı bir şekilde okunmuş ise
   *         false arabellekte veri yoksa veya veri doğrulanamadı ise
   */
  bool uart_read(void);
  
  /*
   * @brief UART portundan veri gonderme fonksiyonu
   * @param data_size okunacak verinin boyutu
   * @return true islem basarili ise
   *         false parametreler hatali veya gonderme kanalı dolu ise
   */
  bool uart_write(uint8_t data_size);


int main(void) {
    // Sistem saati ve UART başlatılır
    clk_init();
    uart_init(10, 0);

    // Pin ve port ayarları
    pin_init(0, 1);  // Pin 0, output olarak ayarlanıyor
    pin_init(1, 0);  // Pin 1, input olarak ayarlanıyor

    // Uygulama komutları arrayleri
    app_sequence_instructs[0] = (app_instructs_t){CMD_PIN_W, 0, 0, 1};  // Pin 0, HIGH ayarla
    app_sequence_instructs[1] = (app_instructs_t){CMD_PIN_R, 0, 1, 0};  // Pin 1, oku
    app_sequence_instructs[2] = (app_instructs_t){CMD_PORT_W, 0, 1, 0};  // Port 1, tüm pinleri HIGH yap
    app_sequence_instructs[3] = (app_instructs_t){CMD_PORT_R, 0, 1, 0};  // Port 1 durumunu oku
    app_sequence_instructs[4] = (app_instructs_t){CMD_UART_W, 0, 1, 0};  // UART üzerinden veri gönder
    app_sequence_instructs[5] = (app_instructs_t){CMD_UART_R, 0, 0, 0};  // UART üzerinden veri al
    app_sequence_instructs[6] = (app_instructs_t){CMD_PIN_W, 2, 1, 1};  // Pin 2, HIGH yap
    app_sequence_instructs[7] = (app_instructs_t){CMD_PIN_R, 2, 1, 0};  // Pin 2 durumunu oku
    app_sequence_instructs[8] = (app_instructs_t){CMD_PORT_W, 1, 1, 0};  // Port 2, düşük nibble'ı HIGH yap
    app_sequence_instructs[9] = (app_instructs_t){CMD_PORT_R, 1, 0, 0};  // Port 2 durumunu oku

    // Ana döngü
    while (1) {
        if (clk_ticked()) {
            for (int i = 0; i < 10; i++) {
                instr = app_sequence_instructs[i];
                switch (instr.cmd) {
                    case CMD_PIN_R:
                        {
                            instr.ret_val = pin_read(instr.param1);    
                        }    
                        break;
                    case CMD_PIN_W:
                        {
                            pin_write(instr.param1, instr.param2);
                        }    
                        break;
                    case CMD_PORT_R:
                        {   
                            port_read(instr.param1); 
                        }
                        break;
                    case CMD_PORT_W:
                        {
                            port_write(instr.param1, instr.param2);
                            
                        }
                        break;
                    case CMD_UART_R:
                        {
                            uart_read();
                        }
                        break;
                    case CMD_UART_W:
                        {
                            uart_write(instr.param1);
                        }
                        break;
                    case CMD_NOP:
                    default:
                        break;
                }
            }
        }
    }

    return 0;
}

void clk_init(void) { 
    // Programın başlangıç zamanını kaydeder
    start_time = clock();
    printf("[clk_init] fonksiyonu Sistem saati ayarlandı. Başlangıç zamanı: %ld clock ticks\n", (long)start_time);
    
}

bool uart_init(uint8_t baud_rate, uint8_t parity_bit) {
    if ((baud_rate != 1 && baud_rate != 10 && baud_rate != 100) || (parity_bit != 0 && parity_bit != 1)) {
        printf("[uart_init] fonksiyonu Hatalı parametreler\n");
        return false;
    }
    reg_config_uart = (baud_rate << 1) | parity_bit;
    printf("[uart_init] fonksiyonu UART ayarlandı - Baud rate: %d, Parity bit: %s\n", baud_rate, parity_bit ? "EVEN" : "OFF");
    return true;
}

bool pin_init(uint8_t pin_num, uint8_t pin_mode) {
    if (pin_num >= 16 || (pin_mode != 0 && pin_mode != 1)) {
        printf("[pin_init] fonksiyonu Hatalı parametreler pin_num: %d pin_mode: %d \n",pin_num,pin_mode);
        return false;
    }
    if (pin_num < 8) {
        if (pin_mode == 1) {
            reg_config_port_1 = reg_config_port_1 | (1 << pin_num);
        } else {
            reg_config_port_1 = reg_config_port_1 & ~(1 << pin_num);
        }
    } else {
        pin_num = pin_num - 8;
        if (pin_mode == 1) {
            reg_config_port_2 = reg_config_port_2 | (1 << pin_num);
        } else {
            reg_config_port_2 = reg_config_port_2 & ~(1 << pin_num);
        }
    }
    printf("[pin_init] fonksiyonu Pin %d ayarlandı - Mod: %s\n", pin_num, pin_mode ? "Çıkış" : "Giriş");
    return true;
}

bool clk_ticked(void) {
    static clock_t last_time = 0;
    clock_t current_time = clock();
    // Her saate tıklandığında last_time bir önceki current time değeri ile yenilenir.
    if ((current_time - last_time) * 10 / CLOCKS_PER_SEC >= 1) {
        last_time = current_time;
        return true;
    }
    return false;
}

int8_t pin_read(uint8_t pin_num) {  
    // parametre olarak pin numarası girilir.
    // reg_config_port_1 register'larında pin numarasının input değeri mi yoksa output değeri mi olduğu kontrol edilir.
    // pin_num 1 ile reg_state_port_1 değeri ile bitwise and işlemi yapılır. Eğer sonuç 0 değilse pin_read fonksiyonu true döndürülür
    if (pin_num >= 16) {
        printf("[pin_read] fonksiyonu Hatalı pin numarası\n");
        return -1;
    }
    if (pin_num < 8) { 
        printf("[pin_read] fonksiyonu register port 1 değeri: 0x%02X (\n", reg_config_port_1);
        printf("[pin_read] fonksiyonu register port 1 statusu: 0x%02X (\n", reg_state_port_1);
        if (reg_config_port_1 & (1 << pin_num)) { 
            return (reg_state_port_1 & (1 << pin_num)) ? 1 : 0;
        } else {
            return rand() % 2;  // Rastgele HIGH veya LOW döndür
        }
    } else {
        printf("[pin_read] fonksiyonu register port 2 değeri: 0x%02X (\n", reg_config_port_2);
        printf("[pin_read] fonksiyonu register port 2 statusu: 0x%02X (\n", reg_state_port_2);
        pin_num -= 8;
        if (reg_config_port_2 & (1 << pin_num)) {
            return (reg_state_port_2 & (1 << pin_num)) ? 1 : 0;
        } else {
            return rand() % 2;  // Rastgele HIGH veya LOW döndür
        }
    }

    
    
}

bool pin_write(uint8_t pin_num, uint8_t pin_state) {
    // parametre olarak pin numarası girilir.
    // reg_config_port_1 register'larında pin numarasının input değeri mi yoksa output değeri mi olduğu kontrol edilir.
    // pin_num biti 1 ise reg_state_port_1 değeri ile bitwise and işlemi yapılır. 0 ise else scope'una geçilir.
    // Eğer reg_config_port_1 değeri de 0 değilse pin_state değeri kontrol edilir. 
    // Pin durumu HIGH ise register'ın state'i 1 olur, değilse 0 olur.
    if (pin_num < 8) {
        if (reg_config_port_1 & (1 << pin_num)) {
            if (pin_state) {
                reg_state_port_1 |= (1 << pin_num);
            } else {
                reg_state_port_1 &= ~(1 << pin_num);
            }
            printf("[pin_write] fonksiyonu register port 1 değeri: 0x%02X (\n", reg_config_port_1);
            printf("[pin_write] fonksiyonu register port 1 statusu: 0x%02X (\n", reg_state_port_1);
        } else {
            printf("[pin_write] Pin %d giriş olarak ayarlı\n", pin_num);
            return false;
        }
    } else {
        pin_num -= 8;
        if (reg_config_port_2 & (1 << pin_num)) {
            if (pin_state) {
                reg_state_port_2 |= (1 << pin_num);
            } else {
                reg_state_port_2 &= ~(1 << pin_num);
            }
            printf("[pin_write] fonksiyonu register port 2 değeri: 0x%02X (\n", reg_config_port_2);
            printf("[pin_write] fonksiyonu register port 2 statusu: 0x%02X (\n", reg_state_port_2);
        } else {
            printf("[pin_write] Pin %d giriş olarak ayarlı\n", pin_num);
            return false;
        }
    }
    return true;
}

uint8_t port_read(uint8_t port_num) {
    uint8_t port_state = (port_num == 1) ? reg_config_port_1 : reg_config_port_2;
    char message[50];
    snprintf(message, sizeof(message), "Port %d durumu: 0x%02X", instr.param1, port_state);
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
    printf("[%s] main fonksiyonu\t%s\n", time_str, message);

    printf("Port Read Register: 0x%04X ", port_state);
    for (int i = 15; i >= 0; i--) {
        printf("%d", (port_state >> i) & 1);
        if (i % 4 == 0) printf(" ");
    }
    printf("\n");
    if (port_num == 1) {
        return reg_state_port_1;
    } else if (port_num == 2) {
        return reg_state_port_2;
    } else {
        printf("[port_read] Hatalı port numarası port_num: %d\n",port_num);
        return 0;
    }
    
}

bool port_write(uint8_t port_num, uint8_t port_state) {
    if (port_num == 1) {
        for (int i = 0; i < 8; i++) {
            if (!(reg_config_port_1 & (1 << i))) {
                printf("[port_write] Port 1, Pin %d giriş olarak ayarlı\n", i);
                return false;
            }
        }
        reg_state_port_1 = port_state;
        printf("[port_write] fonksiyonu register port 1 değeri: 0x%02X (\n", reg_config_port_1);
        printf("[port_write] fonksiyonu register port 1 durumu: 0x%02X (\n", reg_state_port_1);
    } else if (port_num == 2) {
        for (int i = 0; i < 8; i++) {
            if (!(reg_config_port_2 & (1 << i))) {
                printf("[port_write] Port 2, Pin %d giriş olarak ayarlı\n", i);
                return false;
            }
        }
        reg_state_port_2 = port_state;
        printf("[port_write] fonksiyonu register port 2 değeri: 0x%02X (\n", reg_config_port_2);
        printf("[port_write] fonksiyonu register port 2 durumu: 0x%02X (\n", reg_state_port_2);
    } else {
        printf("[port_write] Hatalı port numarası\n");
        return false;
    }
    printf("[port_write] fonksiyonu Port %d durumu 0x%02X olarak ayarlandı\n", port_num, port_state);
    return true;
}

bool uart_read(void) {
    // UART arabellekte veri var mı kontrolü
    if ((reg_uart_chn & 0xFF) == 0) {
        // Arabellekte veri yok
        printf("[uart_read] Arabellekte veri yok.\n");
        return false;
    }

    // UART RX verisini oku
    data = (uint8_t)(reg_uart_chn & 0xFF);
    reg_uart_rx = data;
    printf("[uart_read] fonksiyonu UART RX verisi: 0x%02X\n", data);

    // Okunan veriyi sıfırla
    reg_uart_chn = 0;

    // Veri başarılı bir şekilde okundu
    return true;
}

bool uart_write(uint8_t data_frame) {
    if (reg_uart_chn != 0) {
        printf("[uart_write] fonksiyonu UART kanalı dolu\n");
        return false;
    }
    reg_uart_chn = data_frame;
    printf("[uart_write] fonksiyonu UART TX verisi: 0x%02X\n", reg_uart_chn);
    return true;
}

