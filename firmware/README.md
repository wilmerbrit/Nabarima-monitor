# Firmware de la planta (ESP32 Heltec WiFi LoRa 32 V3)

Un solo programa sirve para las 5 placas. Lo único que cambia es el número de placa, `NODO_ID`:

| NODO_ID | Placa | Qué lee |
|---|---|---|
| 0 | Maestro | Recibe por LoRa y envía al sistema por WiFi (HTTPS) |
| 1 | Nabarima 1 | Temperatura del cuarto, salmuera, pinza SCT-013-000 |
| 2 | Nabarima 2 | Temperatura del cuarto, salmuera, pinza Accuenergy AcuCT-075-200:333 |
| 3 | Cuarto de bombas | Tanque subterráneo, pinzas de bomba 1 y bomba 2 |
| 4 | Tanque de reserva | Tanque de reserva, temperatura ambiente |

Los nodos envían cada ~10 s por LoRa en **920 MHz** (fuera de la banda 900 de Digitel), a 10 dBm. El maestro junta lo recibido y cada 10 s lo manda a `POST /dispositivo/lecturas` del Sistema Nabarima. El sistema deduce "en marcha" de los amperios con el umbral de Más › Planta.

## Archivos

- `planta/config.h`: pines, radio, calibración de pinzas y medidas de los tanques. **Es lo que se ajusta al instalar.**
- `planta/programa.cpp`: el programa del maestro y de los nodos.
- `planta/paquete.h`: el mensaje LoRa (19 bytes) y cómo se convierte al JSON del sistema.
- `planta/secretos.ejemplo.h`: plantilla del WiFi, la dirección y la clave del maestro.
- `planta/certificados.h`: certificados raíz para verificar el HTTPS del servidor.
- `prueba/formato.cpp`: prueba en la computadora del JSON que arma el maestro.

## Preparar

1. **Clave del maestro:** en el sistema, Más › Planta › Generar la clave del maestro. Se muestra una sola vez.
2. Copie `planta/secretos.ejemplo.h` como `planta/secretos.h` y complete el WiFi (red de 2,4 GHz), la dirección del sistema (`https://…`, sin `/` al final) y la clave. `secretos.h` no se sube a GitHub.
3. **Versión de la placa:** mire la serigrafía. En la V3 y V3.1 deje `BAT_CTRL_ACTIVO LOW`; en la **V3.2** cámbielo a `HIGH`, o la batería se leerá mal.

## Cargar el programa

**Con PlatformIO (recomendado):** abra la carpeta `firmware` en VS Code con PlatformIO, conecte la placa por USB-C y use `pio run -e nodo1 -t upload` (o `maestro`, `nodo2`… `nodo4`). Las librerías se instalan solas.

**Con el IDE de Arduino:**
1. Agregue el gestor de placas de Espressif (`https://espressif.github.io/arduino-esp32/package_esp32_index.json`) e instale **esp32 versión 2.0.17**.
2. Instale las librerías **RadioLib 7.1.2**, **OneWire** y **DallasTemperature**.
3. Abra `planta/planta.ino`, elija la placa **Heltec WiFi LoRa 32(V3)** y cambie `#define NODO_ID` en `config.h` antes de cargar cada placa.

Etiquete cada placa con su número al cargarla.

## Comprobar

Abra el monitor serie a 115200:
- un nodo muestra cada envío: `#12 enviado (ok) · bat 4120 mV · temp -148 · nivel 632 · A 1184 …` (valores en décimas);
- el maestro muestra cada nodo recibido con su señal y `Enviado al sistema (4 nodos)`. Si dice que **rechazó la clave**, genere otra y cárguela.

En el sistema, Más › Planta debe mostrar los nodos como **Conectado** en menos de un minuto.

## Calibrar al instalar

- **Tanques:** con el sensor ya montado, mida la distancia del sensor al fondo (`ULTRA_A_VACIO_MM`) y al nivel de tanque lleno (`ULTRA_A_LLENO_MM`, al menos 3 cm bajo el sensor), en milímetros, y póngalas en el bloque del nodo en `config.h`.
- **Pinzas:** con la máquina o bomba en marcha, compare el valor del sistema con una pinza amperimétrica en el mismo cable. Ajuste `PINZA_A_FACTOR` (o `B`) en proporción: si el sistema dice 100 A y la pinza 110 A, multiplique el factor por 1,1.
- **Luz o batería:** cada nodo detecta el corte con el divisor de la fuente de 5 V en GPIO 4 (10 kΩ arriba, 15 kΩ abajo). Si no se conecta, ponga `PIN_RED -1`; el nodo nunca dirá "en batería".

## Probar el formato sin placas

```
cd firmware/prueba
g++ -std=c++11 -I../planta formato.cpp -o formato && ./formato > lecturas.json
curl -H "Authorization: Bearer <clave>" -H "Content-Type: application/json" -d @lecturas.json https://su-dominio.com/dispositivo/lecturas
```

La respuesta debe decir `"guardadas":15` y la pantalla Planta mostrará los datos de ejemplo.
