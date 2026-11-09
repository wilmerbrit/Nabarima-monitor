// Mensaje LoRa de un nodo al maestro y su conversión al JSON que recibe el sistema (POST /dispositivo/lecturas).
// Sin dependencias de Arduino: se prueba también en la computadora (firmware/prueba).
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PAQUETE_MAGIA_0 'N'
#define PAQUETE_MAGIA_1 'B'
#define PAQUETE_VERSION 1
#define SIN_DATO INT16_MIN   // el sensor no respondió: no se envía

// 19 bytes; los valores van multiplicados por 10 (una décima)
struct __attribute__((packed)) Paquete {
  uint8_t magia[2];
  uint8_t version;
  uint8_t nodo;
  uint16_t secuencia;
  uint16_t bateria_mv;
  uint8_t banderas;      // bit 0: funcionando con batería (se fue la luz)
  int16_t temperatura;   // °C × 10
  int16_t nivel_a;       // % × 10
  int16_t nivel_b;
  int16_t corriente_a;   // A × 10
  int16_t corriente_b;
};
#define BANDERA_EN_BATERIA 0x01

inline bool paqueteValido(const uint8_t* datos, size_t largo) {
  if (largo != sizeof(Paquete)) return false;
  const Paquete* p = (const Paquete*)datos;
  return p->magia[0] == PAQUETE_MAGIA_0 && p->magia[1] == PAQUETE_MAGIA_1 && p->version == PAQUETE_VERSION && p->nodo >= 1 && p->nodo <= 4;
}

inline int16_t aDecimas(float v) {
  if (v != v || v < -3000 || v > 3000) return SIN_DATO;   // NaN o fuera de rango
  return (int16_t)(v * 10 + (v < 0 ? -0.5f : 0.5f));
}

// Clave en el sistema de cada campo, por nodo (nullptr = el nodo no lo usa)
struct Claves { const char* temperatura; const char* nivel_a; const char* nivel_b; const char* corriente_a; const char* corriente_b; };
static const Claves CLAVES_NODO[5] = {
  { nullptr, nullptr, nullptr, nullptr, nullptr },
  { "temp_n1", "salmuera_n1", nullptr, "corriente_n1", nullptr },
  { "temp_n2", "salmuera_n2", nullptr, "corriente_n2", nullptr },
  { nullptr, "tanque_subterraneo", nullptr, "corriente_b1", "corriente_b2" },
  { "temp_ambiente", "tanque_reserva", nullptr, nullptr, nullptr },
};

// Lo último recibido de cada nodo
struct Registro {
  Paquete paquete;
  int rssi;
  bool nuevo;   // llegó desde el último envío al servidor
};

// Escribe { "nodos": [ { "nodo": 1, "bateria_v": 4.12, "en_bateria": false, "rssi": -70, "lecturas": { … } }, … ] }
// con los registros nuevos. Devuelve cuántos nodos incluyó (0 = no hay nada que enviar) o -1 si no cupo en el búfer.
// Escritor con límite para armarJson (C++11, sin plantillas variádicas en lambdas)
struct Escritor {
  char* buf; size_t tam; size_t n;
  void operator()(const char* fmt, ...) __attribute__((format(printf, 2, 3))) {
    if (n >= tam) return;
    va_list a; va_start(a, fmt);
    int w = vsnprintf(buf + n, tam - n, fmt, a);
    va_end(a);
    n = w < 0 ? tam : n + (size_t)w;
  }
  void valor(bool& primero, const char* clave, int16_t v) {
    if (!clave || v == SIN_DATO) return;
    int a = v < 0 ? -v : v;
    (*this)("%s\"%s\":%s%d.%d", primero ? "" : ",", clave, v < 0 ? "-" : "", a / 10, a % 10);
    primero = false;
  }
};

inline int armarJson(char* buf, size_t tam, const Registro* registros, int cuantos) {
  Escritor e = { buf, tam, 0 };
  int incluidos = 0;
  e("{\"nodos\":[");
  for (int i = 0; i < cuantos; i++) {
    const Registro& r = registros[i];
    if (!r.nuevo) continue;
    const Paquete& p = r.paquete;
    const Claves& c = CLAVES_NODO[p.nodo];
    e("%s{\"nodo\":%u,\"bateria_v\":%u.%02u,\"en_bateria\":%s,\"rssi\":%d,\"lecturas\":{", incluidos ? "," : "", (unsigned)p.nodo,
      (unsigned)(p.bateria_mv / 1000), (unsigned)((p.bateria_mv % 1000) / 10), (p.banderas & BANDERA_EN_BATERIA) ? "true" : "false", r.rssi);
    bool primero = true;
    e.valor(primero, c.temperatura, p.temperatura);
    e.valor(primero, c.nivel_a, p.nivel_a);
    e.valor(primero, c.nivel_b, p.nivel_b);
    e.valor(primero, c.corriente_a, p.corriente_a);
    e.valor(primero, c.corriente_b, p.corriente_b);
    e("}}");
    incluidos++;
  }
  e("]}");
  return e.n >= tam ? -1 : incluidos;
}
