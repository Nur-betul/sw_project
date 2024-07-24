#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "variables.h"
#include <stdlib.h>
#include <time.h>
#include "main.h"

static clock_t start_time;
app_instructs_t instr;
uint8_t data;

  /** 
   * @brief sistem saatini ayarlama fonksiyonu 
   */
  void clk_init(void);

  /** 
   * @brief UART haberlesme ayarlama fonksiyonu
   * @param baud_rate veri gonderme hiziiii (1bps, 10bps, 100bps)
   * @return true islem basarili ise
   *         false parametreler hatali ise
   */
  bool uart_init(uint8_t baud_rate, uint8_t parity_bit);

  /**
   * @brief pin ayarlama fonksiyonu
   * @param pin_num pin numarasi
   *        pin_mode 0x0 pin giris, 0x01 pin cikis
   * @return true islem basarili ise
   *         false parametreler hatali ise
   */
  bool pin_init(uint8_t pin_num, uint8_t pin_mode);

  /**
   * @brief sistem saatinde ayarlanan surenin gecip gecmedigini sorgulama fonksiyonu
   * @return true sure gecti
   *         false sure henuz gecmedi
   */
  bool clk_ticked(void);

  /** 
   * @brief pin okuma fonksiyonu
   * @param pin_num pin numarasi
   * @return -1 pin numarasi yanlis ise
   *          0 pin low
   *          1 pin high
   */
  int8_t pin_read(uint8_t pin_num);
  
  /**
   * @brief pin yazma fonksiyonu
   * @param pin_num pin numarasi
   *        pin_state pin durumu 0 Low 1 High
   * @return true islem basarili ise
   *         false parametreler hatali ise
   */
  bool pin_write(uint8_t pin_num, uint8_t pin_state);
  
  /**
   * @brief porttaki butun pinleri okuma fonksiyonu 
   * @note hem giris hem cikis pinlerini okur
   * @param port_num port numarasi
   * @return 0x00 - 0xFF pin durumlarina gore her bit bir pini temsil eder
   */
  uint8_t port_read(uint8_t port_num);
  
  /**
   * @brief porttaki butun pinleri yazma fonksiyonu
   * @note sadece cikis pinlerini etkilemeli
   * @param port_num port numarasi
   *        port_state 0x00 0xFF pin durumlarina gore her bit bir pini temsil eder
   * @return true islem basarili ise
   *         false parametreler hatali ise
   */
  bool port_write(uint8_t port_num, uint8_t port_state);

   /**
   * @brief UART portundan gelen veriyi okuma fonksiyonu
   * @return true arabellekteki veri başarılı bir şekilde okunmuş ise
   *         false arabellekte veri yoksa veya veri doğrulanamadı ise
   */
  bool uart_read(void);
  
  /**
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
    app_sequence_instructs[2] = (app_instructs_t){CMD_PORT_W, 0, 1, 1};  // Port 1, tüm pinleri HIGH yap
    app_sequence_instructs[3] = (app_instructs_t){CMD_PORT_R, 0, 1, 0};  // Port 1 durumunu oku
    app_sequence_instructs[4] = (app_instructs_t){CMD_UART_W, 0, 1, 0};  // UART üzerinden veri gönder
    app_sequence_instructs[5] = (app_instructs_t){CMD_UART_R, 0, 0, 0};  // UART üzerinden veri al
    app_sequence_instructs[6] = (app_instructs_t){CMD_PIN_W, 2, 1, 1};  // Pin 2, HIGH yap
    app_sequence_instructs[7] = (app_instructs_t){CMD_PIN_R, 2, 1, 0};  // Pin 2 durumunu oku
    app_sequence_instructs[8] = (app_instructs_t){CMD_PORT_W, 1, 2, 1};  // Port 2, düşük nibble'ı HIGH yap
    app_sequence_instructs[9] = (app_instructs_t){CMD_PORT_R, 1, 2, 0};  // Port 2 durumunu oku

    // Ana döngü
   
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
    

    return 0;
}

void clk_init(void) { 
    // Programın başlangıç zamanını kaydeder
    start_time = clock();
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
    // printf("[clk_init] fonksiyonu Sistem saati ayarlandı. Başlangıç zamanı: %ld clock ticks\n", (long)start_time);
    printf("[%s]\t[clk_init]\tSistem saati ayarlandı. \n", time_str);
    
}

bool uart_init(uint8_t baud_rate, uint8_t parity_bit) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];

    // Input validation for baud_rate and parity_bit
    if ((baud_rate != 1 && baud_rate != 10 && baud_rate != 100) || (parity_bit != 0 && parity_bit != 1)) {
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[uart_init]\tHatalı parametreler\n", time_str);
        return false;
    }
    reg_config_uart = (baud_rate << 1) | parity_bit;
    strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
    printf("[%s]\t[uart_init]\tUART ayarlandı - Baud rate: %d, Parity bit: %s\n", time_str, baud_rate, parity_bit ? "EVEN" : "OFF");
    return true;
}

bool pin_init(uint8_t pin_num, uint8_t pin_mode) {

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];

    // Input validation for pin_num and pin_mode
    if (pin_num >= 16 || (pin_mode != 0 && pin_mode != 1)) {
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[pin_init]\tHatalı parametreler pin_num: %d pin_mode: %d \n",time_str, pin_num, pin_mode);
        
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
    strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
    printf("[%s]\t[pin_init]\tPin %d ayarlandı - Mod: %s\n", time_str, pin_num, pin_mode ? "Çıkış" : "Giriş");
    return true;
}

bool clk_ticked(void) {
    static clock_t last_time = 0;
    clock_t current_time = clock();
    clock_t difference = current_time - last_time;
    double elapsed_time = (double)difference / CLOCKS_PER_SEC;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];

    strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
    printf("[%s]\t[clk_ticket]\tSistem saati ayarlandı. Başlangıç zamanı: %ld clock ticks\n", time_str, (long)start_time);

    if (elapsed_time * 1000 >= 1) {
        last_time = current_time;
        return true;
    }
    return false;
}
int8_t pin_read(uint8_t pin_num) {  
    // parametre olarak pin numarası girilir.
    // reg_config_port_1 register'larında pin numarasının input değeri mi yoksa output değeri mi olduğu kontrol edilir.
    // pin_num 1 ile reg_state_port_1 değeri ile bitwise and işlemi yapılır. Eğer sonuç 0 değilse pin_read fonksiyonu true döndürülür

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    
    // Input validation for pin_num
    if (pin_num >= 16) {
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[pin_read]\tHatalı pin numarası\n",time_str);
        return -1;
    }
    
    if (pin_num < 8) { 
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[pin_read]\tRegister port 1 değeri: 0x%02X\tstatüsü: 0x%02X\t", time_str, reg_config_port_1, reg_state_port_1);
        for (int i = 15; i >= 0; i--) {
                printf("%d", (reg_config_port_1 >> i) & 1);
                if (i % 4 == 0) printf(" ");
            }
        printf("\n");
        if (reg_config_port_1 & (1 << pin_num)) { 
            return (reg_state_port_1 & (1 << pin_num)) ? 1 : 0;
        } else {
            return rand() % 2;  // Rastgele HIGH veya LOW döndür
        }
    } 
    else {
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[pin_read] Register port 2 değeri: 0x%02X\tstatüsü: 0x%02X\t", time_str, reg_config_port_2, reg_state_port_2);
        for (int i = 15; i >= 0; i--) {
                printf("%d", (reg_config_port_2 >> i) & 1);
                if (i % 4 == 0) printf(" ");
            }
        printf("\n");
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
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    char message[50];
    snprintf(message, sizeof(message), "Pin %d durumu: 0x%02X", instr.param1, pin_state);
    strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);

    // Input validation for pin_num
    if (pin_num >= 16) {
        printf("[%s]\t[pin_write]\t%s\tHatalı pin numarası (\n", time_str, message);
        return -1;
    }

    // Input validation for pin_state
    if (!(pin_state == 0 || pin_state == 1)) {
        printf("[%s]\t[pin_write]\t%s\tHatalı pin durumu (\n", time_str, message);
        return -1;
    }
    
    printf("\n");
    if (pin_num < 8) {
        if (reg_config_port_1 & (1 << pin_num)) {
            if (pin_state) {
                reg_state_port_1 |= (1 << pin_num);
            } else {
                reg_state_port_1 &= ~(1 << pin_num);
            }
            printf("[%s]\t[pin_write]\t%s register port 1 değeri: 0x%02X statusu: 0x%02X\t", time_str, message, reg_config_port_1, reg_state_port_1);
            for (int i = 15; i >= 0; i--) {
                printf("%d", (reg_config_port_1 >> i) & 1);
                if (i % 4 == 0) printf(" ");
            }
            printf("\n");
        } else {
            printf("[%s]\t[pin_write]\t%s Pin %d giriş olarak ayarlı\n", time_str, message, pin_num);
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
            printf("[%s]\t[pin_write]\tRegister port 2 değeri: 0x%02X port 2 statusu: 0x%02X\t", time_str, reg_config_port_2,reg_state_port_2);
            for (int i = 15; i >= 0; i--) {
                printf("%d", (reg_config_port_2 >> i) & 1);
                if (i % 4 == 0) printf(" ");
            }
            printf("\n");
        } else {
            printf("[%s]\t[pin_write]\tPin %d giriş olarak ayarlı\n", time_str, pin_num);
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

    if (port_num == 1) {
        return reg_state_port_1;
    } else if (port_num == 2) {
        return reg_state_port_2;
    } else {
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[port_read]\tHatalı port numarası port_num: %d\n", time_str, port_num);
        return 0;
    }
    // Input Validation of port_num
    if (port_num >= 16) {
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[port_read]\tHatalı pin numarası (\n",time_str);
        return -1;
    }
    // Burada hangi portta olduğumu nasıl anlarım ???
    if(port_num == 1){
        printf("[%s]\t[port_read]\t%s Port 1 Read Register: 0x%04X ", time_str, message, port_state);
    }
    else{
        printf("[%s]\t[port_read]\t%s Port 2 Read Register: 0x%04X ", time_str, message, port_state);
    }
    
    for (int i = 15; i >= 0; i--) {
        printf("%d", (port_state >> i) & 1);
        if (i % 4 == 0) printf(" ");
    }
    printf("\n");
    
    
}

bool port_write(uint8_t port_num, uint8_t port_state) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    
    if (port_num == 1) {
        reg_state_port_1 = port_state;
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[port_write]\tRegister port 1 değeri: 0x%02X durumu: 0x%02X\t", time_str, reg_config_port_1,reg_state_port_1);
        for (int i = 15; i >= 0; i--) {
                printf("%d", (reg_config_port_1 >> i) & 1);
                if (i % 4 == 0) printf(" ");
            }
        for (int i = 0; i < 8; i++) {
            if (!(reg_config_port_1 & (1 << i))) {
                strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
                printf("\tPort 1, Pin %d giriş olarak ayarlı\n", i);
                return false;
            }
        }
    } else if (port_num == 2) {
        reg_state_port_2 = port_state;
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[port_write]\tRegister port 2 değeri: 0x%02X durumu: 0x%02X\t",time_str, reg_config_port_2, reg_state_port_2);
        for (int i = 15; i >= 0; i--) {
                printf("%d", (reg_config_port_2 >> i) & 1);
                if (i % 4 == 0) printf(" ");
            }
        
        for (int i = 0; i < 8; i++) {
            if (!(reg_config_port_2 & (1 << i))) {
                strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
                printf("\tPort 2, Pin %d giriş olarak ayarlı\n", i);
                return false;
            }
        }
    } 
    else {
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[port_write]\tHatalı port numarası\n", time_str);
        return false;
    }
    strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
    printf("[%s]\t[port_write]\tPort %d durumu 0x%02X olarak ayarlandı\n",time_str, port_num, port_state);
    return true;
}


bool uart_read(void) {
    // UART arabellekte veri var mı kontrolü

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];

    if ((reg_uart_chn & 0xFF) == 0) {
        // Arabellekte veri yok
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[uart_read]\tArabellekte veri yok.\n",time_str);
        return false;
    }

    // UART RX verisini oku
    data = (uint8_t)(reg_uart_chn & 0xFF);
    reg_uart_rx = data;
    strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
    printf("[%s]\t[uart_read]\tUART RX verisi: 0x%02X\t",time_str, reg_uart_rx);
    for (int i = 15; i >= 0; i--) {
                printf("%d", (reg_uart_rx>> i) & 1);
                if (i % 4 == 0) printf(" ");
            }
    printf("\n");
    // Okunan veriyi sıfırla
    reg_uart_chn = 0;

    // Veri başarılı bir şekilde okundu
    return true;
}

bool uart_write(uint8_t data_frame) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    if (reg_uart_chn != 0) {
        strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
        printf("[%s]\t[uart_write]\tUART kanalı dolu\n",time_str);
        return false;
    }
    reg_uart_chn = data_frame;
    strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", t);
    printf("[%s]\t[uart_write]\tUART CHN verisi: 0x%02X\t",time_str, reg_uart_chn);
    for (int i = 15; i >= 0; i--) {
                printf("%d", (reg_uart_chn >> i) & 1);
                if (i % 4 == 0) printf(" ");
            }
    printf("\n");
    return true;
}

