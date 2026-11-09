# Monitoreo planta de hielo: sensores y conexiones

Planta de hielo Nabarima · Wilmer · creado el 22 de octubre de 2026

## Arquitectura

Cada nodo es un ESP32 con radio LoRa junto a lo que mide y envía sus datos cada pocos segundos. El maestro es el único que necesita WiFi: los reenvía al servidor y la aplicación los muestra en vivo.

Ver el diagrama en [01-arquitectura.md](01-arquitectura.md).

LoRa alcanza cientos de metros dentro de una planta, así los tanques lejanos no necesitan WiFi.

El programa de las placas está en la carpeta [`firmware/`](../firmware/README.md) de este repositorio (copia de la carpeta del [Sistema Nabarima](https://github.com/wilmerbrit/nabarima/tree/claude/project-thread-gkwoo5/firmware)), con la guía para cargarlo y calibrar.

## Variables y sensor de cada una

Son 11 variables repartidas en 4 nodos. Las bombas comparten nodo con el tanque subterráneo porque están en el mismo cuarto. El tanque de reserva, que está afuera, tiene nodo propio, que también mide la temperatura ambiente. La medición del tablero eléctrico quedó fuera por ahora.

| Variable | Nodo | Sensor | Qué entrega |
| --- | --- | --- | --- |
| Temperatura cuarto frío Nabarima 1 | Nodo 1 | DS18B20 con funda de acero inoxidable | °C, de -55 a 125 |
| Nivel tanque agua y sal Nabarima 1 | Nodo 1 | Ultrasónico sellado A02YYUW | Distancia en cm, se convierte a % |
| Nabarima 1 funcionando | Nodo 1 | Pinza de corriente SCT-013-000 (100 A) en una fase del cable AWG 6 | Funcionando / parada + amperios |
| Temperatura cuarto frío Nabarima 2 | Nodo 2 | DS18B20 con funda de acero inoxidable | °C |
| Nivel tanque agua y sal Nabarima 2 | Nodo 2 | Ultrasónico sellado A02YYUW | % de nivel |
| Nabarima 2 funcionando | Nodo 2 | Pinza de corriente AcuCT-075-200:333 (200 A) en una fase del cable TTU 1/0 | Funcionando / parada + amperios |
| Nivel tanque subterráneo | Nodo 3 | Ultrasónico sellado A02YYUW | % de nivel |
| Nivel tanque de reserva | Nodo 4 | Ultrasónico sellado A02YYUW | % de nivel |
| Temperatura ambiente | Nodo 4 | DS18B20 con funda de acero inoxidable, dentro de un abrigo meteorológico | °C |
| Bomba 1 funcionando | Nodo 3 | Pinza de corriente SCT-013-030 | Encendida / apagada + amperios |
| Bomba 2 funcionando | Nodo 3 | Pinza de corriente SCT-013-030 | Encendida / apagada + amperios |

Por qué estos sensores: el ultrasónico mide desde arriba sin tocar el líquido, así la salmuera no lo corroe. En las Nabarima y en las bombas, la pinza de corriente se abraza a un solo cable de fase, sin cortar ni empalmar nada. Si la corriente pasa de un umbral, la máquina está en marcha; además se ve cuántos amperios consume, lo que ayuda a detectar un motor forzado o un compresor que no arranca.

## Lista de materiales

Para usar placas que ya traen ESP32 y LoRa juntos propongo la Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262). Ahorra cableado y trae antena y pantalla. Se compra la versión de 863–928 MHz. Como Digitel usa la banda de 900 MHz (los teléfonos transmiten entre 880 y 915 MHz y las antenas entre 925 y 960 MHz), los radios se configuran en un canal fijo entre 917 y 923 MHz, que queda fuera de ambos rangos. Conviene confirmar con CONATEL el uso de 902–928 MHz sin licencia.

| Material | Cantidad | Uso | Dónde comprar (EE. UU.) |
| --- | --- | --- | --- |
| Heltec WiFi LoRa 32 V3, 915 MHz | 5 | 4 nodos + 1 maestro | eBay (Mkfrobotics, kit 915 MHz con batería) o Amazon, tienda Heltec / MakerHawk |
| Sonda DS18B20 sumergible, acero inoxidable, 3 m | 3 | Temperatura de cada cuarto frío y del ambiente (nodo 4) | Amazon: "DS18B20 waterproof 3m"; Adafruit o DigiKey |
| Sensor ultrasónico A02YYUW (UART, sellado IP67) | 4 | 2 tanques de salmuera + 2 tanques de agua | DigiKey o Mouser (DFRobot SEN0311), RobotShop |
| Pinza de corriente SCT-013-000 (100 A / 50 mA, abertura 13 mm) | 1 | Nabarima 1, en una fase del cable AWG 6 | Amazon o eBay (marca YHDC) |
| Pinza de corriente Accuenergy AcuCT-075-200:333 (200 A, salida 0,333 V, abertura 19 mm), ya comprada | 1 | Nabarima 2, en una fase del cable TTU 1/0 | Amazon: Accuenergy AcuCT-075-200:333 (equivalente, unos $40) o Global Test Supply (Magnelab); no la SCT-019 de 50 A |
| Pinza de corriente SCT-013-030 (30 A, salida de voltaje) | 2 | Bomba 1 y Bomba 2 | Amazon o eBay (marca YHDC) |
| Jack hembra 3,5 mm estéreo (3 contactos) para la pinza | 3 | Conectar las SCT-013 (se usan la punta y el cuerpo; la pinza de Nabarima 2 trae cables sueltos) | DigiKey o Amazon |
| Resistencia 10 kΩ | 12 | Divisor de polarización de cada pinza (2 por pinza) y detección de luz (1 por nodo) | Amazon: kit surtido de resistencias 1/4 W, o DigiKey |
| Resistencia 15 kΩ | 4 | Detección de luz: con una de 10 kΩ baja los 5 V de la fuente a 3 V para el GPIO 4 | En el mismo kit de resistencias |
| Capacitor electrolítico 10 µF | 4 | Filtro de cada pinza | Amazon: kit surtido de capacitores, o DigiKey |
| Resistencia de carga 22 Ω, 1 % | 1 | Convierte a voltaje la salida de la SCT-013-000 (la AcuCT ya entrega voltaje) | DigiKey o Mouser (1 %, 1/4 W) |
| Resistencia 4,7 kΩ | 3 | Pull-up de cada DS18B20 | En el kit de resistencias (o viene con la sonda) |
| Abrigo meteorológico pequeño (protector solar para sensor) | 1 | Sombra y ventilación para la sonda de temperatura ambiente | Amazon: "solar radiation shield" para sensor |
| Fuente 5 V 2 A (entrada 100 a 240 V) | 5 | Alimentar cada placa | Amazon (fuente o cargador USB-C 5 V 2 A) |
| Batería LiPo de 3000 mAh con cable JST 1,25 mm | 5 | Respaldo de cada nodo y del maestro cuando se va la luz | Viene en el kit Heltec de eBay |
| Caja estanca IP65 con prensaestopas | 5 | Proteger cada nodo de humedad y frío | Amazon (LeMotech u otra marca, con prensaestopas) |
| Cable apantallado 4 hilos, borneras, placa perforada | según distancias | Llevar los sensores hasta cada caja | Amazon o ferretería local |

Las cantidades de resistencias y capacitores son las mínimas; conviene comprar algunas de repuesto.

## Conexiones de cada nodo

Los nodos 1 a 3 usan los mismos pines, así un solo firmware sirve para todos. Los pines evitan los que la Heltec V3 ya usa para LoRa (GPIO 8 a 14), la pantalla (17, 18, 21) y la batería (1, 37). Antes de soldar se confirman contra el diagrama de pines de la placa que compres.

Diagrama con los pines y los colores de cable de cada nodo: [Conexiones de los nodos](../diagramas/conexiones-de-los-nodos.html) (ábralo en el navegador; también está en línea en Claude).

| Señal | Pin ESP32 | Nodo 1 y Nodo 2 | Nodo 3 |
| --- | --- | --- | --- |
| Bus 1-Wire (temperatura) | GPIO 5 | DS18B20 de su Nabarima | sin usar |
| Ultrasónico A (UART RX) | GPIO 47 | Tanque de salmuera | Tanque subterráneo |
| Ultrasónico B (UART RX) | GPIO 48 | sin usar | sin usar |
| Pinza de corriente A (analógica) | GPIO 6 | Una fase del motor de su Nabarima | Bomba 1 |
| Pinza de corriente B (analógica) | GPIO 7 | sin usar | Bomba 2 |
| Detección de luz (digital) | GPIO 4 | 5 V de la fuente del nodo | 5 V de la fuente del nodo |

El nodo 4, junto al tanque de reserva, usa el mismo firmware con el ultrasónico A en GPIO 47 y una DS18B20 en GPIO 5 para la temperatura ambiente. Esa sonda va dentro de un abrigo meteorológico, a la sombra, a unos 1,5 m del suelo y lejos de paredes o techos que calienta el sol; al sol directo marcaría varios grados de más. Su caja IP65 queda a la intemperie, así que conviene ponerla a la sombra y con los prensaestopas hacia abajo.

Cómo se conecta cada sensor:

- **DS18B20:** rojo a 3,3 V, negro a GND, amarillo a GPIO 5, con la resistencia de 4,7 kΩ entre el amarillo y 3,3 V.
- **A02YYUW:** alimentado a 3,3 V para que su señal no pase de 3,3 V. Su cable TX va al pin RX del ESP32. Su cable RX se deja sin conectar, así entrega la distancia ya filtrada.
- **SCT-013-030:** dos resistencias de 10 kΩ en serie entre 3,3 V y GND crean un punto medio de 1,65 V, con el capacitor de 10 µF de ese punto a GND. Un extremo de la pinza va a ese punto medio y el otro al GPIO. A 30 A la señal oscila entre 0,24 V y 3,06 V, dentro de lo que lee el ESP32.
- **Pinza en el motor o la bomba:** se abraza a un solo cable de fase, nunca al cable completo, porque las corrientes se anulan y marcaría cero. Si una bomba consume más de 30 A, se cambia por una SCT-013-000 de 100 A. Nabarima 1 lleva la SCT-013-000, porque su cable AWG 6 entra en la abertura de 13 mm. Nabarima 2 lleva la AcuCT-075-200:333, porque el TTU 1/0 mide unos 14 a 15 mm por fuera y no entra en una SCT-013; su abertura es de 19 mm. La SCT-013-000 es de salida en corriente: la resistencia de carga de 22 Ω va soldada entre sus dos cables, en la placa del nodo. La AcuCT ya entrega voltaje (0,333 V a 200 A) y se conecta como la SCT-013-030, sin resistencia de carga. Conviene medir el diámetro real de cada cable antes de comprar.
- **Alimentación:** cada fuente de 5 V entra por el USB-C o el pin 5V de la placa. El trabajo en 220 V lo debe hacer un electricista.

## Respaldo con batería

Cada placa lleva una batería 18650 de 3000 mAh, así los nodos y el maestro siguen funcionando si se va la luz. Calculo entre 1 y 2 días de autonomía por nodo midiendo cada 10 segundos; el número real se confirma midiendo el consumo en la prueba del paso 4. El router ya tiene respaldo.

- **Conexión:** la batería va al conector de batería de la Heltec V3, que la carga sola mientras hay luz y cambia a ella sin cortes cuando se va.
- **Aviso de corte:** el firmware lee el voltaje de la batería y, por el GPIO 4, si la fuente de 5 V tiene luz (10 kΩ desde los 5 V al pin y 15 kΩ del pin a GND, en todos los nodos). Avisa en la aplicación cuando un nodo pasa a batería y cuando le queda poca carga. Si todos pasan a batería a la vez, la aplicación avisa que se fue la luz, aunque ya no se mida el tablero.
- **Frío:** las baterías de litio no deben cargarse bajo 0 °C. Las cajas de los nodos 1 y 2 van fuera del cuarto frío; solo entra la sonda de temperatura.
- **Maestro:** consume más porque usa WiFi. Si queda cerca del router, conviene conectarlo también al respaldo del router.

## Datos confirmados y pendientes

- [x] Los tanques tienen como máximo 2,8 m de profundidad, dentro de los 4,5 m que mide el A02YYUW.
- [x] El tanque de salmuera tiene la superficie libre y una tapa plástica. El sensor va en un agujero de la tapa, mirando hacia abajo y al menos 3 cm por encima del nivel máximo.
- [x] La temperatura es la del cuarto frío de cada Nabarima. La sonda va colgada en el aire del cuarto, lejos de la puerta y del evaporador.
- [x] Los motores de las Nabarima son trifásicos: basta una pinza en una sola fase para saber si están en marcha y cuántos amperios consumen.
- [x] Las bombas son monofásicas de 120 V y no pasan de 30 A, así que la SCT-013-030 alcanza. Va en el cable de fase.
- [x] Las bombas están en el mismo cuarto que el tanque subterráneo. El tanque de reserva está afuera y lleva su propio nodo.
- [x] Los nodos quedan a unos 20 m del maestro y el de reserva a unos 15 m. LoRa cubre esas distancias de sobra.
- [x] Hay tomacorriente cerca del tanque de reserva, así que el nodo 4 no necesita panel solar.
