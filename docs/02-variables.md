# 02 · Variables

Son **11 variables** en 4 nodos. Las máquinas y bombas se miden con pinzas de corriente: el sistema guarda los amperios y deduce si están funcionando cuando pasan del umbral (3 A por defecto, ajustable en Más › Planta › Configuración).

| # | Variable | Nodo | Sensor | Unidad | Clave en el sistema |
|---|---|---|---|---|---|
| 1 | Temperatura cuarto frío Nabarima 1 | 1 | DS18B20 con funda inoxidable (GPIO 5) | °C | `temp_n1` |
| 2 | Nivel tanque agua y sal Nabarima 1 | 1 | Ultrasónico A02YYUW (GPIO 47) | % | `salmuera_n1` |
| 3 | Nabarima 1 funcionando | 1 | Pinza SCT-013-000 100 A + 22 Ω (GPIO 6) | A / marcha | `corriente_n1`, `maquina_n1` |
| 4 | Temperatura cuarto frío Nabarima 2 | 2 | DS18B20 (GPIO 5) | °C | `temp_n2` |
| 5 | Nivel tanque agua y sal Nabarima 2 | 2 | A02YYUW (GPIO 47) | % | `salmuera_n2` |
| 6 | Nabarima 2 funcionando | 2 | Pinza Accuenergy AcuCT-075-200:333 (GPIO 6) | A / marcha | `corriente_n2`, `maquina_n2` |
| 7 | Nivel tanque subterráneo | 3 | A02YYUW (GPIO 47) | % | `tanque_subterraneo` |
| 8 | Bomba 1 funcionando | 3 | Pinza SCT-013-030 (GPIO 6) | A / marcha | `corriente_b1`, `bomba_1` |
| 9 | Bomba 2 funcionando | 3 | Pinza SCT-013-030 (GPIO 7) | A / marcha | `corriente_b2`, `bomba_2` |
| 10 | Nivel tanque de reserva | 4 | A02YYUW (GPIO 47) | % | `tanque_reserva` |
| 11 | Temperatura ambiente | 4 | DS18B20 en abrigo meteorológico (GPIO 5) | °C | `temp_ambiente` |

Además, de cada nodo se registran el voltaje de la batería (`bateria_v`), si está con luz o con batería (`en_bateria`) y la señal LoRa (`rssi`).

## Notas

- **Temperatura de los cuartos:** la consigna es la temperatura del cuarto registrada en el sistema (−15 °C para Nabarima 1 y 2). La alerta salta sobre la consigna más un margen (5 °C por defecto).
- **Temperatura ambiente:** solo informativa; sin consigna ni alerta.
- **Niveles:** el ultrasónico mide la distancia al líquido desde arriba y el firmware la convierte a % con las distancias de vacío y lleno de cada tanque (profundidad máxima 2,8 m).
- **Marcha:** si el maestro no envía `maquina_n1`, `maquina_n2`, `bomba_1` o `bomba_2`, el sistema los deduce de la corriente.

## Fuera de alcance

| Variable | Estado |
|---|---|
| Suministro de agua | Excluida por Wilmer (22-10-2026) |
| Presencia de energía por fase y voltaje línea-línea del tablero | Eliminada "por ahora" (22-10-2026); el corte de luz se infiere por las baterías |
| Salinidad de la salmuera | Propuesta y abortada (24-10-2026); en los tanques de salmuera solo se mide el nivel |
