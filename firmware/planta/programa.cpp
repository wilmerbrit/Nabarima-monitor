// Planta Hielo Nabarima: setup() y loop() del maestro y de los nodos.
// El rol (maestro o nodo 1–4) se elige con NODO_ID en config.h o en platformio.ini.
#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>
#include "config.h"
#include "paquete.h"

SPIClass spiLora(HSPI);
SX1262 radio = new Module(PIN_LORA_NSS, PIN_LORA_DIO1, PIN_LORA_RST, PIN_LORA_BUSY, spiLora);

void iniciarRadio() {
  spiLora.begin(PIN_LORA_SCK, PIN_LORA_MISO, PIN_LORA_MOSI, PIN_LORA_NSS);
  int e = radio.begin(LORA_FRECUENCIA_MHZ, LORA_ANCHO_KHZ, LORA_SF, LORA_CR, LORA_SYNC_WORD, LORA_POTENCIA_DBM, LORA_PREAMBULO);
  if (e != RADIOLIB_ERR_NONE) {
    Serial.printf("No arrancó la radio LoRa (error %d). Se reinicia en 10 s.\n", e);
    delay(10000);
    ESP.restart();
  }
  radio.setCRC(true);
  Serial.printf("LoRa listo en %.1f MHz, SF%d, %d dBm\n", LORA_FRECUENCIA_MHZ, LORA_SF, LORA_POTENCIA_DBM);
}

#if NODO_ID == 0
// =====================================================================
// MAESTRO: escucha a los nodos y cada 10 s envía lo nuevo al sistema
// =====================================================================
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "certificados.h"
#if __has_include("secretos.h")
  #include "secretos.h"
#else
  #error "Falta secretos.h: copie secretos.ejemplo.h como secretos.h y ponga el WiFi, la dirección y la clave del maestro."
#endif

Registro registros[4];
bool recibido[4] = { false, false, false, false };
volatile bool llegoPaquete = false;
unsigned long ultimoEnvio = 0;
char json[1024];

void IRAM_ATTR alRecibir() { llegoPaquete = true; }

void conectarWifi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.printf("Conectando al WiFi %s…\n", WIFI_RED);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_RED, WIFI_CLAVE);
  for (int i = 0; i < 40 && WiFi.status() != WL_CONNECTED; i++) delay(250);
  if (WiFi.status() == WL_CONNECTED) Serial.printf("WiFi conectado, IP %s\n", WiFi.localIP().toString().c_str());
}

void leerPaquete() {
  uint8_t datos[64];
  size_t largo = radio.getPacketLength();
  int e = radio.readData(datos, largo < sizeof(datos) ? largo : sizeof(datos));
  if (e == RADIOLIB_ERR_NONE && paqueteValido(datos, largo)) {
    const Paquete* p = (const Paquete*)datos;
    Registro& r = registros[p->nodo - 1];
    r.paquete = *p;
    r.rssi = (int)radio.getRSSI();
    r.nuevo = true;
    recibido[p->nodo - 1] = true;
    Serial.printf("Nodo %u · #%u · %d dBm · batería %u mV%s\n", p->nodo, p->secuencia, r.rssi, p->bateria_mv,
                  (p->banderas & BANDERA_EN_BATERIA) ? " (sin luz)" : "");
  }
  radio.startReceive();
}

void enviarAlServidor() {
  int n = armarJson(json, sizeof(json), registros, 4);
  if (n <= 0) return;   // nada nuevo (o no cupo, que no debería pasar)
  conectarWifi();
  if (WiFi.status() != WL_CONNECTED) { Serial.println("Sin WiFi: se intenta en el próximo envío."); return; }
  WiFiClientSecure cliente;
  cliente.setCACert(CERTIFICADOS_RAIZ);
  HTTPClient http;
  http.setTimeout(8000);
  if (!http.begin(cliente, String(SERVIDOR_URL) + RUTA_LECTURAS)) return;
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", String("Bearer ") + CLAVE_MAESTRO);
  int codigo = http.POST((uint8_t*)json, strlen(json));
  if (codigo == 200) {
    for (auto& r : registros) r.nuevo = false;   // solo se marcan enviados si el sistema los recibió
    Serial.printf("Enviado al sistema (%d nodos): %s\n", n, http.getString().c_str());
  } else if (codigo == 401) {
    Serial.println("El sistema rechazó la clave: genere una en Más › Planta y cópiela en secretos.h.");
  } else {
    Serial.printf("El sistema respondió %d %s\n", codigo, codigo < 0 ? http.errorToString(codigo).c_str() : http.getString().c_str());
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nPlanta Hielo Nabarima · maestro");
  pinMode(PIN_VEXT, OUTPUT);
  digitalWrite(PIN_VEXT, HIGH);   // pantalla apagada
  iniciarRadio();
  radio.setPacketReceivedAction(alRecibir);
  radio.startReceive();
  conectarWifi();
}

void loop() {
  if (llegoPaquete) { llegoPaquete = false; leerPaquete(); }
  if (millis() - ultimoEnvio >= ENVIO_SERVIDOR_MS) { ultimoEnvio = millis(); enviarAlServidor(); }
  delay(5);
}

#else
// =====================================================================
// NODO: lee sus sensores y los envía por LoRa cada 10 s
// =====================================================================
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#ifdef USA_TEMPERATURA
OneWire unCable(PIN_ONEWIRE);
DallasTemperature sondas(&unCable);
#endif
uint16_t secuencia = 0;

// A02YYUW: tramas de 4 bytes 0xFF, alto, bajo, suma; con su RX sin conectar envía la distancia filtrada cada ~100 ms.
// Junta las tramas de medio segundo y devuelve la mediana en mm (o -1 si no hay).
int leerUltrasonico(HardwareSerial& puerto) {
  while (puerto.available()) puerto.read();   // descarta lo viejo
  int medidas[8], n = 0;
  uint8_t t[4];
  int k = 0;
  unsigned long inicio = millis();
  while (millis() - inicio < 500 && n < 8) {
    if (!puerto.available()) { delay(2); continue; }
    uint8_t b = puerto.read();
    if (k == 0 && b != 0xFF) continue;
    t[k++] = b;
    if (k < 4) continue;
    k = 0;
    if (((t[0] + t[1] + t[2]) & 0xFF) != t[3]) continue;
    int mm = (t[1] << 8) | t[2];
    if (mm >= 30 && mm <= 4500) medidas[n++] = mm;
  }
  if (!n) return -1;
  for (int i = 1; i < n; i++) for (int j = i; j > 0 && medidas[j - 1] > medidas[j]; j--) { int x = medidas[j]; medidas[j] = medidas[j - 1]; medidas[j - 1] = x; }
  return medidas[n / 2];
}

float nivelPorciento(int mm, int vacio, int lleno) {
  if (mm < 0) return NAN;
  float p = (float)(vacio - mm) / (float)(vacio - lleno) * 100.0f;
  return p < -10 ? -10 : p > 110 ? 110 : p;   // el sistema recorta a 0–100
}

// Corriente RMS: 200 ms de muestras (12 ciclos a 60 Hz), sin la componente continua del punto medio
float leerAmperios(int pin, float factor) {
  double suma = 0, cuadrados = 0;
  int n = 0;
  unsigned long inicio = millis();
  while (millis() - inicio < 200) {
    double mv = analogReadMilliVolts(pin);
    suma += mv;
    cuadrados += mv * mv;
    n++;
  }
  if (n < 100) return NAN;
  double media = suma / n;
  double varianza = cuadrados / n - media * media;
  float amperios = varianza > 0 ? sqrt(varianza) / 1000.0 * factor : 0;
  return amperios < PINZA_RUIDO_A ? 0 : amperios;
}

uint16_t leerBateriaMv() {
  digitalWrite(PIN_BAT_CTRL, BAT_CTRL_ACTIVO);
  delay(5);
  uint32_t mv = 0;
  for (int i = 0; i < 8; i++) mv += analogReadMilliVolts(PIN_BAT_ADC);
  digitalWrite(PIN_BAT_CTRL, !BAT_CTRL_ACTIVO);
  return (uint16_t)(mv / 8 * BAT_FACTOR);
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.printf("\nPlanta Hielo Nabarima · nodo %d\n", NODO_ID);
  WiFi.mode(WIFI_OFF);   // los nodos no usan WiFi
  btStop();
  setCpuFrequencyMhz(80);
  pinMode(PIN_VEXT, OUTPUT);
  digitalWrite(PIN_VEXT, HIGH);   // pantalla apagada
  pinMode(PIN_BAT_CTRL, OUTPUT);
  digitalWrite(PIN_BAT_CTRL, !BAT_CTRL_ACTIVO);
  if (PIN_RED >= 0) pinMode(PIN_RED, INPUT);
  analogSetAttenuation(ADC_11db);
#ifdef USA_TEMPERATURA
  sondas.begin();
#endif
#ifdef USA_ULTRA_A
  Serial1.begin(9600, SERIAL_8N1, PIN_ULTRA_A, -1);
#endif
#ifdef USA_ULTRA_B
  Serial2.begin(9600, SERIAL_8N1, PIN_ULTRA_B, -1);
#endif
  iniciarRadio();
  randomSeed(esp_random());
}

void loop() {
  Paquete p;
  p.magia[0] = PAQUETE_MAGIA_0; p.magia[1] = PAQUETE_MAGIA_1;
  p.version = PAQUETE_VERSION;
  p.nodo = NODO_ID;
  p.secuencia = secuencia++;
  p.bateria_mv = leerBateriaMv();
  p.banderas = (PIN_RED >= 0 && digitalRead(PIN_RED) == LOW) ? BANDERA_EN_BATERIA : 0;
  p.temperatura = p.nivel_a = p.nivel_b = p.corriente_a = p.corriente_b = SIN_DATO;
#ifdef USA_TEMPERATURA
  sondas.requestTemperatures();
  float t = sondas.getTempCByIndex(0);
  if (t != DEVICE_DISCONNECTED_C && t != 85.0f) p.temperatura = aDecimas(t);   // 85 °C = lectura de arranque, no válida
#endif
#ifdef USA_ULTRA_A
  p.nivel_a = aDecimas(nivelPorciento(leerUltrasonico(Serial1), ULTRA_A_VACIO_MM, ULTRA_A_LLENO_MM));
#endif
#ifdef USA_ULTRA_B
  p.nivel_b = aDecimas(nivelPorciento(leerUltrasonico(Serial2), ULTRA_B_VACIO_MM, ULTRA_B_LLENO_MM));
#endif
#ifdef USA_PINZA_A
  p.corriente_a = aDecimas(leerAmperios(PIN_PINZA_A, PINZA_A_FACTOR));
#endif
#ifdef USA_PINZA_B
  p.corriente_b = aDecimas(leerAmperios(PIN_PINZA_B, PINZA_B_FACTOR));
#endif

  int e = radio.transmit((uint8_t*)&p, sizeof(p));
  radio.sleep();
  Serial.printf("#%u enviado (%s) · bat %u mV · temp %d · nivel %d · A %d · B %d (décimas)\n", p.secuencia,
                e == RADIOLIB_ERR_NONE ? "ok" : "error", p.bateria_mv, p.temperatura, p.nivel_a, p.corriente_a, p.corriente_b);
  delay(INTERVALO_ENVIO_MS - 1500 + random(0, 2000));   // ~10 s, con desfase para que dos nodos no choquen siempre
}
#endif
