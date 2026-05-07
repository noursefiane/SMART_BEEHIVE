# SMART_BEEHIVE
 STM32L476RG-based smart beehive monitor. Reads temperature, humidity (SHT31 + DHT11) and weight (HX711), displays on RGB LCD, and transmits over LoRa 868MHz to a receiver node.
 
 M1 Embedded Systems — Université Paul Sabatier, Toulouse 2025/2026.

# Emitter code
Contains the full sensor acquisition and transmission logic. Initializes and reads the SHT31 (I2C), DHT11 (bit-bang 1-Wire) and HX711 (bit-bang SPI), displays results on the Grove RGB LCD, and sends all data over LoRa via AT commands through UART4.

# Receiver code
Initializes the LoRa-E5 module in continuous RX mode and polls for incoming packets. Extracts the hex payload from the AT response, decodes it to ASCII, and forwards it to the PC via USART2 (ST-Link VCP).
