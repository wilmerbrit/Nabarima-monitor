// Prueba en la computadora: arma el JSON del maestro con paquetes de ejemplo de los 4 nodos y lo imprime.
// Compilar y probar contra el sistema:
//   g++ -std=c++11 -I../planta formato.cpp -o formato && ./formato > lecturas.json
//   curl -H "Authorization: Bearer <clave>" -H "Content-Type: application/json" -d @lecturas.json https://su-dominio.com/dispositivo/lecturas
#include "paquete.h"

static Paquete armar(uint8_t nodo, uint16_t mv, bool sinLuz, float t, float na, float ia, float ib) {
  Paquete p;
  p.magia[0] = PAQUETE_MAGIA_0; p.magia[1] = PAQUETE_MAGIA_1;
  p.version = PAQUETE_VERSION; p.nodo = nodo; p.secuencia = 1; p.bateria_mv = mv;
  p.banderas = sinLuz ? BANDERA_EN_BATERIA : 0;
  p.temperatura = aDecimas(t); p.nivel_a = aDecimas(na); p.nivel_b = SIN_DATO;
  p.corriente_a = aDecimas(ia); p.corriente_b = aDecimas(ib);
  return p;
}

int main() {
  const float N = 0.0f / 0.0f;   // sin dato
  Registro r[4] = {
    { armar(1, 4120, false, -14.86f, 63.2f, 118.44f, N), -68, true },
    { armar(2, 4085, false, -15.1f, 57.95f, 0.0f, N), -74, true },
    { armar(3, 4100, false, N, 72.0f, 14.3f, 0.0f), -81, true },
    { armar(4, 3950, true, 31.4f, 81.0f, N, N), -88, true },
  };
  if (!paqueteValido((const uint8_t*)&r[0].paquete, sizeof(Paquete))) return 1;
  char json[1024];
  int n = armarJson(json, sizeof(json), r, 4);
  if (n != 4) return 2;
  char chico[40];
  if (armarJson(chico, sizeof(chico), r, 4) != -1) return 3;   // no cabe: debe avisar
  printf("%s\n", json);
  return 0;
}
