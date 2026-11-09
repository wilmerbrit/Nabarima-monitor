# 06 · Radio LoRa y CONATEL

## Frecuencia elegida

- Placas Heltec WiFi LoRa 32 V3 en versión **863–928 MHz** (se venden como "915 MHz").
- Canal fijo de **920 MHz** (dentro de 917–923 MHz), 125 kHz de ancho, SF9, a **10 dBm** (10 mW), suficiente para 20 m.

## Por qué 920 MHz

Digitel usa la banda GSM/3G de 900 MHz en Venezuela:

| Tramo | Rango |
|---|---|
| Subida (teléfonos) | 880–915 MHz |
| Bajada (antenas) | 925–960 MHz |
| Canal de la planta | 920 MHz, entre los dos |

El canal queda fuera de ambos rangos, y con la potencia baja el riesgo de interferir es mínimo.

## WiFi

El WiFi y Bluetooth de la placa son de 2,4 GHz y solo los usa el maestro. El router debe tener una red de 2,4 GHz (el ESP32 no se conecta a 5 GHz).

## Estado regulatorio (búsqueda del 22-10-2026)

- No se encontró una norma pública de CONATEL que confirme ni que prohíba el uso libre de 902–928 MHz. La Providencia 863 (2006), en las fuentes leídas, solo cita como uso libre las bandas de 2,4 GHz y 5 GHz.
- La Providencia 217 (Gaceta Oficial 43.208, septiembre de 2025) exige homologación para importar equipos de telecomunicaciones, incluidos los de uso libre.
- **Pendiente:** consultar directamente a CONATEL.

## Decisión

El 22-10-2026 Wilmer decidió **seguir con LoRa** a pesar de la duda con CONATEL.

**Plan B** si no se permite: cambiar la comunicación entre nodos y maestro a ESP-NOW o WiFi de 2,4 GHz (la misma placa lo soporta; cambia el firmware, no el hardware de sensores).
