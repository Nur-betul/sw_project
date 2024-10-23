# C Kullanarak Basit Bir MCU Simülasyonu Gerçekleştirme
## MCU özellikleri:
- Register boyutu: 8-bit 16-bit
- Saat hızı: 10Hz
- IO sayısı: 2 port her portta 8 pin (Toplam 16 pin)
- Haberleşme arayüzü: UART
- Birim ayarları:
  - IO:
    - Giriş/Çıkış ayarları
  - UART:
    - Haberleşme hızı (1bps, 10bps, 100bps) 
    - Parity biti (OFF, EVEN)
- Registerlar: 8 tane 
  1. Ayarlar için: 3 tane
     1. 1.Port Pin ayarlama register 8-bit
     2. 2.Port Pin ayarlama register 8-bit
     3. UART ayarlama register 8-bit
        - Hız ve parity için bir register
   2. Birimler için: 5 tane
      1. 1.port için pin yazma ve okuma register 8-bit
      2. 2.port için pin yazma ve okuma register 8-bit
      3. UART Tx register  16-bit
      4. UART RX register 16-bit
      5. UART Channel register 16-bit
