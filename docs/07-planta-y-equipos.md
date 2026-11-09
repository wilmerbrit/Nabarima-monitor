# 07 · Planta y equipos

## Máquinas de hielo y cuartos fríos

| Equipo | Máquina | Cuarto frío debajo | Volumen | Cable de alimentación | Pinza |
|---|---|---|---|---|---|
| Nabarima 1 | Hielo en escamas, 3 t / 24 h, motor trifásico | 4 × 6 × 2 m | 48 m³ | AWG 6 | SCT-013-000 100 A (abertura 13 mm) |
| Nabarima 2 | Hielo en escamas, 5 t / 24 h, motor trifásico | 4 × 11 × 2 m | 88 m³ | TTU 1/0 (14–15 mm) | Accuenergy AcuCT-075-200:333 200 A (abertura 19 mm), comprada el 23-10-2026 |

- Consigna de los cuartos en el sistema: −15 °C.
- La sonda de temperatura va colgada en el aire de cada cuarto, lejos de la puerta y del evaporador.
- Como los motores son trifásicos, basta una pinza en **una sola fase** (nunca en el cable completo, porque las corrientes se anulan).

## Tanques

| Tanque | Ubicación | Nodo |
|---|---|---|
| Agua y sal (salmuera) Nabarima 1 | Junto a la máquina, superficie libre con tapa plástica | 1 |
| Agua y sal (salmuera) Nabarima 2 | Igual | 2 |
| Subterráneo | Cuarto de bombas | 3 |
| Reserva | Afuera, con tomacorriente cerca | 4 |

- Profundidad máxima: 2,8 m (el A02YYUW mide hasta 4,5 m).
- En la salmuera el ultrasónico va en un agujero de la tapa, mirando hacia abajo, al menos 3 cm sobre el nivel máximo.
- Salmuera: 3,5 kg de sal en 210 L (≈ 1,6 %). Solo se mide el nivel.

## Bombas

- 2 bombas monofásicas de 120 V, de 30 A como máximo, en el mismo cuarto que el tanque subterráneo.
- Pinza SCT-013-030 en el cable de fase de cada una. Si alguna pasa de 30 A, se cambia por una SCT-013-000.

## Energía e internet

- Tablero de 220 V entre fases, 3 fases + neutro (sin cargas en el neutro); acometida 3 × TTU 3/0 (~200 A) + neutro AWG 6; consumo de la planta ~180 A. La medición del tablero quedó fuera del proyecto.
- El router de internet ya tiene respaldo de energía.
- El trabajo en 220 V lo debe hacer un electricista.
