// Configuración de cada placa. Se elige el rol con NODO_ID:
//   0 = maestro (recibe por LoRa y envía al sistema por WiFi)
//   1 = Nabarima 1 · 2 = Nabarima 2 · 3 = cuarto de bombas · 4 = tanque de reserva
// En PlatformIO cada entorno ya lo define (pio run -e nodo1). En el IDE de Arduino se cambia aquí.
#pragma once

#ifndef NODO_ID
#define NODO_ID 1
#endif

// ---------- Radio LoRa (igual en todas las placas) ----------
// Canal fijo dentro de 902–928 MHz, fuera de la banda 900 de Digitel (880–915 subida, 925–960 bajada)
#define LORA_FRECUENCIA_MHZ 920.0
#define LORA_ANCHO_KHZ 125.0
#define LORA_SF 9              // más alto = más alcance y más tiempo en el aire
#define LORA_CR 7
#define LORA_SYNC_WORD 0x4E    // red privada de la planta (los nodos de otros no se mezclan)
#define LORA_POTENCIA_DBM 10   // 10 mW: sobra para 20 m
#define LORA_PREAMBULO 8

// Pines del SX1262 en la Heltec WiFi LoRa 32 V3
#define PIN_LORA_NSS 8
#define PIN_LORA_SCK 9
#define PIN_LORA_MOSI 10
#define PIN_LORA_MISO 11
#define PIN_LORA_RST 12
#define PIN_LORA_BUSY 13
#define PIN_LORA_DIO1 14

// ---------- Nodos ----------
#define INTERVALO_ENVIO_MS 10000   // cada cuánto envía cada nodo (más un desfase al azar para no chocar)

// Sensores (mismos pines en todos los nodos; ver el documento de conexiones)
#define PIN_ONEWIRE 5        // DS18B20
#define PIN_ULTRA_A 47       // A02YYUW: su cable TX a este pin
#define PIN_ULTRA_B 48
#define PIN_PINZA_A 6        // pinza de corriente (punto medio 1,65 V)
#define PIN_PINZA_B 7
#define PIN_RED 4            // 5 V de la fuente con divisor 10 kΩ / 15 kΩ: alto = hay luz, bajo = en batería (-1 si no se conecta)

// Batería de la Heltec V3: GPIO 1 lee la batería por un divisor 390k/100k que se activa con GPIO 37.
// En la V3 y V3.1 se activa en BAJO; en la V3.2 en ALTO (mire la versión impresa en la placa).
#define PIN_BAT_ADC 1
#define PIN_BAT_CTRL 37
#define BAT_CTRL_ACTIVO LOW
#define BAT_FACTOR 4.9

#define PIN_VEXT 36          // alimenta la pantalla; se apaga para ahorrar batería

// Pinzas: amperios por voltio RMS en el pin.
//   SCT-013-030 (salida de voltaje, 1 V a 30 A) → 30
//   SCT-013-000 con resistencia de carga de 22 Ω (100 A : 50 mA = 2000 vueltas) → 2000 / 22 = 90,9
//   AcuCT-075-200:333 o Magnelab SCT-0750-200 (salida de voltaje, 0,333 V a 200 A, sin resistencia de carga) → 200 / 0,333 = 600
//   SCT-019 200 A / 33 mA con resistencia de carga de 33 Ω (≈ 6060 vueltas) → 6060 / 33 = 183,6
// Se ajusta comparando con una pinza amperimétrica (ver firmware/README.md).
#define PINZA_RUIDO_A 0.3    // por debajo de esto se reporta 0 A

// Ultrasónicos: distancia del sensor al fondo (tanque vacío) y al nivel máximo (tanque lleno), en mm.
// Hay que medirlas en cada tanque al instalar.
#if NODO_ID == 1
  #define USA_TEMPERATURA 1
  #define USA_ULTRA_A 1
  #define ULTRA_A_VACIO_MM 2800
  #define ULTRA_A_LLENO_MM 300
  #define USA_PINZA_A 1
  #define PINZA_A_FACTOR 90.9    // Nabarima 1: SCT-013-000 + 22 Ω
#elif NODO_ID == 2
  #define USA_TEMPERATURA 1
  #define USA_ULTRA_A 1
  #define ULTRA_A_VACIO_MM 2800
  #define ULTRA_A_LLENO_MM 300
  #define USA_PINZA_A 1
  #define PINZA_A_FACTOR 600.0   // Nabarima 2: Accuenergy AcuCT-075-200:333 (0,333 V a 200 A, igual que la Magnelab SCT-0750-200)
#elif NODO_ID == 3
  #define USA_ULTRA_A 1          // tanque subterráneo
  #define ULTRA_A_VACIO_MM 2800
  #define ULTRA_A_LLENO_MM 300
  #define USA_PINZA_A 1          // bomba 1
  #define PINZA_A_FACTOR 30.0
  #define USA_PINZA_B 1          // bomba 2
  #define PINZA_B_FACTOR 30.0
#elif NODO_ID == 4
  #define USA_TEMPERATURA 1      // temperatura ambiente
  #define USA_ULTRA_A 1          // tanque de reserva
  #define ULTRA_A_VACIO_MM 2800
  #define ULTRA_A_LLENO_MM 300
#elif NODO_ID != 0
  #error "NODO_ID debe ser 0 (maestro) o de 1 a 4"
#endif

// ---------- Maestro ----------
#define ENVIO_SERVIDOR_MS 10000   // cada cuánto manda al sistema lo recibido
#define RUTA_LECTURAS "/dispositivo/lecturas"
// La dirección del sistema, el WiFi y la clave van en secretos.h (copie secretos.ejemplo.h)
