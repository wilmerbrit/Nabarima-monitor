# 08 · Bitácora de decisiones

Decisiones tomadas por Wilmer durante el diseño, en orden. Las fechas son de 2026.

| Fecha | Decisión |
|---|---|
| 22-10 | Un ESP32 lee los sensores y envía los datos por internet a una aplicación con todas las variables en tiempo real. |
| 22-10 | Variables iniciales: temperatura Nabarima 1 y 2, nivel tanque subterráneo y de reserva, Nabarima 1 y 2 funcionando, nivel tanques de agua y sal 1 y 2 (8 variables). |
| 22-10 | Red: varios ESP32 con LoRa como nodos y un ESP32 maestro con WiFi que envía todo. Se suman 2 bombas. Suministro de agua queda fuera. |
| 22-10 | Se agrega el tablero trifásico (presencia por fase y voltaje línea-línea) con un medidor Modbus. |
| 22-10 | Pinzas de corriente para las Nabarima según el calibre del cable (AWG 6 y TTU 1/0). |
| 22-10 | Se prueba detectar el aire del fan trasero para saber si la máquina funciona. |
| 22-10 | Las bombas comparten nodo con el tanque subterráneo; el tanque de reserva (afuera) tiene nodo propio. |
| 22-10 | Datos de la planta confirmados: tanques de 2,8 m, salmuera con tapa plástica, temperatura de los cuartos fríos, motores trifásicos, bombas de 120 V ≤ 30 A, nodos a ~20 m. |
| 22-10 | Todos los nodos y el maestro llevan batería de respaldo de 3000 mAh. El router ya tiene respaldo. |
| 22-10 | **Se elimina por ahora toda la medición de energía eléctrica** (tablero, medidor, nodo). Quedan 4 nodos; el corte de luz se infiere por las baterías. |
| 22-10 | La supervisión se integra en el **Sistema Nabarima** existente (wilmerbrit/nabarima) en vez de una app aparte; el maestro envía por HTTPS con clave (Cloudflare no pasa MQTT). |
| 22-10 | Se vuelve a las **pinzas de corriente** para "Nabarima funcionando" (se descarta el sensor de aire del fan). Se muestran los amperios con un umbral ajustable. |
| 22-10 | Tarjetas con capacidad de cada máquina (3 t y 5 t / 24 h) y medidas de cada cuarto frío. |
| 22-10 | Cuadro de tiempo y % de funcionamiento junto a la gráfica de corriente, 30 % más pequeño, y que cambie con 24 h / 7 días / 30 días. |
| 22-10 | Nodo 4 suma una DS18B20 para la **temperatura ambiente** (11 variables). |
| 22-10 | Canal LoRa fijo en **920 MHz** para no cruzarse con Digitel. |
| 22-10 | Se sigue con **LoRa** a pesar de la duda con CONATEL. Plan B: ESP-NOW / WiFi 2,4 GHz. |
| 22-10 | Firmware: un programa para las 5 placas, en la carpeta `firmware/` del Sistema Nabarima. |
| 22-10 | La pinza SCT-019 de 200 A no se consigue en EE. UU.; se cambia por una de salida 0,333 V a 200 A. |
| 23-10 | Wilmer compra la Accuenergy AcuCT-075-200:333 (Amazon, ~US$40) para Nabarima 2. |
| 24-10 | Se propone medir la salinidad de la salmuera y Wilmer lo **aborta** el mismo día: solo se mide el nivel. |
| 09-11 | Se publica toda la documentación en el repositorio Nabarima-monitor (v1.0). |

## Descartado

| Idea | Por qué |
|---|---|
| Interruptor de flujo de aire en el fan trasero | Se prefirió la pinza de corriente, que además da los amperios |
| Medidor Eastron SDM630 en el tablero | Se eliminó la medición de energía por ahora; sin transformadores de corriente no daba potencia ni kWh |
| Servidor MQTT | Cloudflare no lo deja pasar; se usa HTTPS al Sistema Nabarima |
| Pinza SCT-019 200 A / 33 mA | No disponible en EE. UU. |
| Salinidad (DFRobot K=10 + DS18B20) | Abortada por Wilmer |
