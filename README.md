# 📊 Data Acquisition System

A modular embedded firmware project for acquiring analog data, interacting with peripherals, and demonstrating microcontroller functionalities such as **ADC**, **UART**, **LCD**, **RTC**, and **Interrupt Handling**.  
Designed for **ARM7 LPC2129** (or similar ARM Cortex-M microcontrollers).

---

## 🔧 Features

- 🟢 **ADC Readings** – Sample analog sensor inputs (`adc.c/.h`)  
- 📟 **LCD Display** – Show acquired data and system status (`lcd.c/.h`)  
- 🔌 **UART Communication** – Serial data logging and debugging (`uart.c/.h`)  
- ⏰ **Real-Time Clock (RTC)** – Timekeeping and timestamping (`rtc.c/.h`)  
- 🛎 **Interrupt Handling** – Demonstrated with external interrupt example (`eint0_test.c`)  
- ⏳ **Delay Utilities** – Simple software delays (`delay.c/.h`)  
- 📍 **Pin Configuration** – Centralized pin mapping (`pin_connect_block.*`)  
- ⌨️ **Keypad/Peripheral Handling** – Custom input interface (`kpm.c/.h`)  

---

## 📁 Project Structure

```plaintext
data_aquisition_system/
├── adc.c, adc.h, adc_defines.h
├── delay.c, delay.h
├── eint0_test.c
├── kpm.c, kpm.h, kpm_defines.h
├── lcd.c, lcd.h, lcd_defines.h
├── mini_project.c        # Main application entry
├── pin_connect_block.c, pin_connect_block.h, pin_connect_block_defines.h
├── rtc.c, rtc.h, rtc_defines.h
├── types.h
├── uart.c, uart.h, uart_defines.h
