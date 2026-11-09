# Nabarima Monitor

Sistema de adquisición de datos en tiempo real para la **planta de hielo Nabarima**: temperatura de los cuartos fríos, niveles de los tanques, máquinas de hielo y bombas funcionando (con sus amperios) y temperatura ambiente.

Los sensores se leen con placas ESP32 con radio LoRa repartidas en la planta. Un ESP32 maestro junta los datos y los envía por internet al **Sistema Nabarima** (el sistema de ventas y administración), que los muestra en vivo en la pantalla **Más › Planta**, guarda el historial y avisa por correo cuando algo falla.

```
Nodo 1 (Nabarima 1) ─┐
Nodo 2 (Nabarima 2) ─┤  LoRa 920 MHz    ┌─────────┐  WiFi + HTTPS   ┌──────────────────┐
Nodo 3 (bombas)     ─┼────────────────▶ │ Maestro │ ──────────────▶ │ Sistema Nabarima │ ──▶ Más › Planta
Nodo 4 (reserva)    ─┘                  └─────────┘                 │ (Railway)        │ ──▶ alertas por correo
                                                                    └──────────────────┘
```

## Documentación

| Documento | Qué contiene |
|---|---|
| [01 · Arquitectura](docs/01-arquitectura.md) | Nodos, maestro, servidor, flujo de datos y respaldo de energía |
| [02 · Variables](docs/02-variables.md) | Las 11 variables, su nodo, sensor, unidad y clave en el sistema |
| [03 · Sensores, materiales y conexiones](docs/03-sensores-y-conexiones.md) | Sensores elegidos, lista de materiales, pines y datos de la planta |
| [04 · Firmware de los ESP32](docs/04-firmware.md) | Programa de las 5 placas, paquete LoRa, carga y calibración |
| [05 · Integración con Sistema Nabarima](docs/05-integracion-sistema-nabarima.md) | Endpoint `POST /dispositivo/lecturas`, pantalla Planta y alertas |
| [06 · Radio LoRa y CONATEL](docs/06-radio-lora-conatel.md) | Elección de frecuencia (920 MHz), Digitel y estado regulatorio |
| [07 · Planta y equipos](docs/07-planta-y-equipos.md) | Máquinas, cuartos fríos, tanques, bombas y cables |
| [08 · Bitácora de decisiones](docs/08-bitacora-de-decisiones.md) | Qué se decidió, cuándo y qué se descartó |
| [Diagrama de conexiones](diagramas/conexiones-de-los-nodos.html) | Pines y colores de cable de cada nodo (abrir en el navegador) |
| [Capturas](capturas/README.md) | Pantalla Planta y gráficas con datos de ejemplo |
| [Firmware](firmware/README.md) | Código fuente de las placas (PlatformIO / Arduino) |
| [CHANGELOG](CHANGELOG.md) | Historial de versiones de esta documentación |

## Estado

| Paso | Estado |
|---|---|
| 1. Variables y sensores | Listo |
| 2. Firmware de los ESP32 | Escrito y compilado; falta probar en placas reales y calibrar |
| 3. Aplicación en tiempo real | Módulo Planta en [wilmerbrit/nabarima PR #1](https://github.com/wilmerbrit/nabarima/pull/1), abierto para revisión |
| 4. Alertas y pruebas en planta | Alertas por correo hechas; falta la prueba en sitio (autonomía de baterías, alcance LoRa) |

Fuera de alcance por ahora: suministro de agua, medición de energía del tablero trifásico y salinidad de la salmuera.

## Repositorios relacionados

- [wilmerbrit/nabarima](https://github.com/wilmerbrit/nabarima): Sistema Nabarima (Node 22 + Express + SQLite en Railway, detrás de Cloudflare). El módulo Planta y la copia original del firmware están en la rama `claude/project-thread-gkwoo5` (PR #1).
- [wilmerbrit/Nabarima-Variables](https://github.com/wilmerbrit/Nabarima-Variables): repositorio inicial del proyecto (vacío).

## Versiones

Cada versión se marca con una etiqueta `vX.Y` y un release en GitHub. Los cambios se anotan en [CHANGELOG.md](CHANGELOG.md).
