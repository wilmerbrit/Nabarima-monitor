# Changelog

Todos los cambios de la documentación del sistema de monitoreo de la planta Nabarima. El formato sigue [Keep a Changelog](https://keepachangelog.com/es-ES/1.1.0/) y las versiones usan `vMAYOR.MENOR`.

## [v1.0] · 2026-11-09

Primera publicación de la documentación completa del proyecto.

### Agregado

- README con la visión general, el estado de los 4 pasos y el índice.
- `docs/01-arquitectura.md`: 4 nodos LoRa + maestro WiFi + Sistema Nabarima, flujo de datos y respaldo con batería.
- `docs/02-variables.md`: las 11 variables con nodo, sensor, unidad y clave del sistema; lo que quedó fuera de alcance.
- `docs/03-sensores-y-conexiones.md`: documento de sensores, lista de materiales, pines y datos confirmados (exportado del documento "Monitoreo planta de hielo: sensores y conexiones", revisión 107).
- `docs/04-firmware.md` y `firmware/`: copia del firmware de las 5 placas Heltec (de wilmerbrit/nabarima, rama `claude/project-thread-gkwoo5`, commit `42cfadb`).
- `docs/05-integracion-sistema-nabarima.md`: endpoint `POST /dispositivo/lecturas`, tablas, pantalla Más › Planta y alertas por correo (PR #1).
- `docs/06-radio-lora-conatel.md`: canal de 920 MHz, banda de Digitel, estado ante CONATEL y plan B.
- `docs/07-planta-y-equipos.md`: máquinas, cuartos fríos, tanques, bombas y cables.
- `docs/08-bitacora-de-decisiones.md`: decisiones del 22-10 al 09-11-2026 y lo descartado.
- `diagramas/conexiones-de-los-nodos.html`: diagrama de pines y colores de cable de cada nodo.
- `capturas/`: 10 capturas de la pantalla Planta con datos de ejemplo.

[v1.0]: https://github.com/wilmerbrit/Nabarima-monitor/releases/tag/v1.0
