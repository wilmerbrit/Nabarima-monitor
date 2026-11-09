# 04 · Firmware de los ESP32

El código está en [`firmware/`](../firmware/README.md), con la guía completa para preparar, cargar, comprobar y calibrar las placas. Es una copia de la carpeta `firmware/` de [wilmerbrit/nabarima](https://github.com/wilmerbrit/nabarima/tree/claude/project-thread-gkwoo5/firmware) (rama `claude/project-thread-gkwoo5`, commit `42cfadb`). Si se cambia allá, hay que actualizar esta copia y anotarlo en el CHANGELOG.

## Resumen

- **Un solo programa** para las 5 Heltec WiFi LoRa 32 V3. Lo que cambia es `NODO_ID`: 0 maestro, 1 Nabarima 1, 2 Nabarima 2, 3 cuarto de bombas, 4 tanque de reserva.
- **Radio:** 920 MHz, 125 kHz, SF9, CR 4/7, 10 dBm, palabra de sincronía `0x4E` (red privada).
- **Paquete LoRa (19 bytes):** firma `NB`, versión, nodo, secuencia, batería en mV, banderas (bit 0 = en batería), temperatura, nivel A, nivel B, corriente A y corriente B, todos en décimas. Un sensor que no responde se envía como "sin dato" y no se reporta.
- **Maestro:** cada 10 s arma el JSON con los nodos nuevos y lo envía por HTTPS (con certificados raíz en `certificados.h`) a `SERVIDOR_URL/dispositivo/lecturas`.
- **Secretos:** WiFi (2,4 GHz), dirección del sistema y clave del maestro van en `planta/secretos.h`, que se crea copiando `secretos.ejemplo.h` y **no se sube a GitHub**.
- **Herramientas:** PlatformIO (`pio run -e maestro|nodo1..nodo4 -t upload`) o IDE de Arduino con esp32 2.0.17, RadioLib 7.1.2, OneWire y DallasTemperature.

## Pines (iguales en todos los nodos)

| Señal | GPIO |
|---|---|
| DS18B20 (1-Wire) | 5 |
| Ultrasónico A (TX del sensor) | 47 |
| Ultrasónico B (reservado) | 48 |
| Pinza A | 6 |
| Pinza B | 7 |
| Detección de luz (divisor 10k/15k desde 5 V) | 4 |
| Batería (ADC / control) | 1 / 37 |
| LoRa SX1262 (NSS, SCK, MOSI, MISO, RST, BUSY, DIO1) | 8, 9, 10, 11, 12, 13, 14 |

## Calibración de las pinzas (`PINZA_x_FACTOR`, amperios por voltio RMS)

| Pinza | Uso | Factor |
|---|---|---|
| SCT-013-000 + resistencia de carga 22 Ω | Nabarima 1 | 90,9 |
| Accuenergy AcuCT-075-200:333 (0,333 V a 200 A, sin resistencia) | Nabarima 2 | 600 |
| SCT-013-030 (1 V a 30 A) | Bombas 1 y 2 | 30 |

Al instalar se ajusta cada factor comparando con una pinza amperimétrica, y se miden en cada tanque las distancias `ULTRA_A_VACIO_MM` y `ULTRA_A_LLENO_MM` (por defecto 2800 y 300 mm).

## Placa V3.2

En la Heltec V3 y V3.1 se deja `BAT_CTRL_ACTIVO LOW`; en la **V3.2** hay que cambiarlo a `HIGH` o la batería se leerá mal.

## Pendiente

- Probar en las placas reales (el formato del JSON ya se probó contra un servidor local: 15 lecturas guardadas).
- Calibrar tanques y pinzas en sitio.
- Medir la autonomía real de las baterías.
