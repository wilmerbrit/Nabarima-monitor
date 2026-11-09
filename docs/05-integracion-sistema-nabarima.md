# 05 · Integración con Sistema Nabarima

En lugar de una aplicación aparte, la supervisión de la planta es un módulo del **Sistema Nabarima** ([wilmerbrit/nabarima](https://github.com/wilmerbrit/nabarima)), el sistema de ventas y administración que ya funciona en Railway detrás de Cloudflare. Así ventas, administración y planta están en un solo lugar y con los mismos usuarios.

Implementación: [PR #1 · Módulo Planta](https://github.com/wilmerbrit/nabarima/pull/1) (archivo `planta.js`, pantalla Más › Planta, pruebas en `test/planta.test.js`). Estado al 09-11-2026: abierto, sin fusionar.

## Entrada de datos del maestro

```
POST https://SU-DOMINIO/dispositivo/lecturas
Authorization: Bearer CLAVE-DEL-MAESTRO
Content-Type: application/json

{"nodos":[
  {"nodo":1,"bateria_v":4.12,"en_bateria":false,"rssi":-68,"lecturas":{"temp_n1":-14.9,"salmuera_n1":63.2,"corriente_n1":118.4}},
  {"nodo":2,"bateria_v":4.08,"en_bateria":false,"rssi":-74,"lecturas":{"temp_n2":-15.1,"salmuera_n2":58.0,"corriente_n2":0.0}},
  {"nodo":3,"bateria_v":4.10,"en_bateria":false,"rssi":-81,"lecturas":{"tanque_subterraneo":72.0,"corriente_b1":14.3,"corriente_b2":0.0}},
  {"nodo":4,"bateria_v":3.95,"en_bateria":true,"rssi":-88,"lecturas":{"temp_ambiente":31.4,"tanque_reserva":81.0}}
]}
```

- La ruta está fuera de `/api`: no usa sesión, cookies ni la protección CSRF de mismo origen.
- **Clave del maestro:** el admin la genera en Más › Planta › Configuración. Se muestra una sola vez y el sistema guarda solo su SHA-256; generar otra invalida la anterior.
- Las lecturas desconocidas, fuera de rango o de otro nodo se ignoran y la respuesta lo informa junto con cuántas se guardaron.
- Claves por nodo: 1 `temp_n1`, `salmuera_n1`, `corriente_n1`, `maquina_n1` · 2 `temp_n2`, `salmuera_n2`, `corriente_n2`, `maquina_n2` · 3 `tanque_subterraneo`, `corriente_b1`, `bomba_1`, `corriente_b2`, `bomba_2` · 4 `tanque_reserva`, `temp_ambiente`.

## Datos

| Tabla | Contenido |
|---|---|
| `planta_estado` | Último valor de cada variable |
| `planta_historial` | Un punto por minuto por variable y cada arranque o parada; se borra lo de más de un año |
| `planta_nodos` | Batería, luz o batería y señal de cada nodo |
| `planta_alertas` | Alertas abiertas y cerradas |

## Pantalla Más › Planta

La ven todos los roles (admin, ventas, llenador); solo el admin la configura. Se refresca cada 10 s.

- Tarjetas Nabarima 1 y Nabarima 2 (capacidad de la máquina y medidas del cuarto), Agua (tanques y bombas) y Ambiente.
- Amperios debajo de "Funcionando" o "Parada" de cada máquina y bomba.
- Estado de cada nodo: conectado, batería y señal.
- Gráfica de historial de 24 h, 7 días o 30 días, con la consigna o el umbral de marcha marcado.
- En las gráficas de corriente, un cuadro con el tiempo y el % en marcha del período elegido (por ejemplo "Funcionó en 24 h · 23 h 20 min · 97 %"), y el tiempo sin datos si faltaron.

Capturas en [capturas/](../capturas/README.md).

## Alertas por correo

| Alerta | Condición |
|---|---|
| Temperatura alta | Cuarto sobre la consigna más el margen (5 °C por defecto) |
| Tanque bajo | Nivel bajo el mínimo (20 % por defecto) |
| Nabarima detenida | Máquina parada (se puede desactivar) |
| Nodo sin datos | Más de 5 min sin recibir (se avisa de inmediato) |
| Batería baja | Batería del nodo bajo 3,4 V |
| Se fue la luz | Todos los nodos en batería |
| Maestro sin enviar | Internet o maestro caído |

Cada alerta espera el tiempo configurado (10 min por defecto), avisa una sola vez y envía otro correo cuando se normaliza. Los correos van a los de la configuración de la planta o, si no hay, a los del reporte de ventas.

## Despliegue

No requiere variables de entorno nuevas. Después de desplegar: Más › Planta › Generar la clave del maestro, y cargar esa clave, el WiFi y la dirección en `firmware/planta/secretos.h` antes de cargar el maestro.
