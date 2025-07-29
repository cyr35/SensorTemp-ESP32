#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <HTTPClient.h>

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensor DS18B20
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temperature = 0.0;

// Web + almacenamiento
WebServer server(80);
Preferences preferences;

String deviceName = "";
String adminUser = "admin";
String adminPass = "admin";

// Mostrar temperatura en la página principal
void handleRoot() {
  IPAddress ip = WiFi.localIP();
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Status</title>"
                "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                "<style>body{font-family:Arial;text-align:center;background:#f4f4f4;padding-top:50px;}"
                "div{background:#fff;display:inline-block;padding:20px;border-radius:10px;box-shadow:0 0 10px #ccc;}</style></head><body>"
                "<div><h2>Estado del dispositivo</h2>"
                "<p><strong>Temperatura:</strong> " + String(temperature, 1) + " &deg;C</p>"
                "<p><strong>ID:</strong> " + deviceName + "</p>"
                "<p><strong>IP:</strong> " + ip.toString() + "</p>"
                "<button onclick=\"location.reload();\">Actualizar</button><br><br>"
                "<a href='/configuracion'>Configuración</a>"
                "</div></body></html>";
  server.send(200, "text/html", html);
}

// Formulario de configuración con autenticación
void handleConfigForm() {
  if (!server.authenticate(adminUser.c_str(), adminPass.c_str())) {
    return server.requestAuthentication();
  }
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="es">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Configuración</title>
      <style>
        body { font-family: Arial, sans-serif; background: #f4f4f4; text-align: center; padding-top: 50px; }
        form { background: white; display: inline-block; padding: 20px; border-radius: 8px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
        input[type="text"], input[type="password"] {
          width: 100%; padding: 10px; margin: 8px 0; border: 1px solid #ccc; border-radius: 4px;
        }
        input[type="submit"] {
          background-color: #4CAF50; color: white; padding: 10px 15px; border: none; border-radius: 4px; cursor: pointer; margin-top: 10px;
        }
        input[type="submit"]:hover { background-color: #45a049; }
      </style>
    </head>
    <body>
      <h2>Configuración</h2>
      <form action="/save" method="POST">
        <label for="ssid">SSID:</label><br>
        <input type="text" id="ssid" name="ssid" required><br>
        <label for="password">Contraseña:</label><br>
        <input type="password" id="password" name="password" required><br>
        <label for="device">ID del dispositivo:</label><br>
        <input type="text" id="device" name="device" required><br>
        <label for="adminpass">Nueva clave admin:</label><br>
        <input type="password" id="adminpass" name="adminpass"><br>
        <input type="submit" value="Guardar">
      </form>
    </body>
    </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

// Guardar configuración
void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("device")) {
    preferences.begin("wifi", false);
    preferences.putString("ssid", server.arg("ssid"));
    preferences.putString("password", server.arg("password"));
    preferences.putString("device", server.arg("device"));
    preferences.end();

    deviceName = server.arg("device");

    if (server.hasArg("adminpass") && server.arg("adminpass") != "") {
      preferences.begin("admin", false);
      preferences.putString("password", server.arg("adminpass"));
      preferences.end();
      adminPass = server.arg("adminpass");
    }

    server.send(200, "text/html", "<html><body><h2>Guardado!</h2><p>Reiniciando...</p></body></html>");
    delay(3000);
    ESP.restart();
  } else {
    server.send(400, "text/plain", "Faltan datos");
  }
}

void startConfigPortal() {
  WiFi.softAP("ESP32_Config");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Portal de config: http://");
  Serial.println(IP);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Modo Config");
  lcd.setCursor(0, 1);
  lcd.print(IP);

  server.on("/", handleRoot);
  server.on("/configuracion", handleConfigForm);
  server.on("/save", handleSave);
  server.begin();
}

void connectToWiFi() {
  preferences.begin("wifi", true);
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  deviceName = preferences.getString("device", "ESP32");
  preferences.end();

  preferences.begin("admin", true);
  adminPass = preferences.getString("password", "admin");
  preferences.end();

  if (ssid == "") {
    Serial.println("No hay WiFi guardado. Modo config.");
    startConfigPortal();
    return;
  }

  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Conectando a ");
  Serial.println(ssid);

  lcd.setCursor(0, 0);
  lcd.print("Conectando...");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi OK");
    delay(1000);
    server.on("/", handleRoot);
    server.on("/configuracion", handleConfigForm);
    server.on("/save", handleSave);
    server.begin();
    Serial.println("Servidor web iniciado en modo STA.");
  } else {
    Serial.println("\nError de conexión. Modo config.");
    startConfigPortal();
  }
}

void enviarTemperaturaAlServidor() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://temperature.yourdomain.com/insert.php?id=" + deviceName + "&temp=" + String(temperature, 2);
   http.begin(url);
    int httpCode = http.GET();
    http.end();
    Serial.print("Envío HTTP: ");
    Serial.println(httpCode);
  } else {
    Serial.println("No conectado a Wi-Fi. No se puede enviar temperatura.");
  }
}

void setup() {
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");
  sensors.begin();
  connectToWiFi();
}

void loop() {
  if (WiFi.getMode() == WIFI_MODE_AP) {
    server.handleClient();
    return;
  }

  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  enviarTemperaturaAlServidor();

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C    ");

  static unsigned long lastSwitch = 0;
  static int displayState = 0;
  unsigned long now = millis();

  if (now - lastSwitch > 3000) {
    lastSwitch = now;
    lcd.setCursor(0, 1);
    if (displayState == 0) {
      lcd.print("WiFi OK        ");
    } else if (displayState == 1) {
      IPAddress ip = WiFi.localIP();
      lcd.print("IP:");
      lcd.print(ip);
    } else {
      lcd.print("ID:");
      lcd.print(deviceName);
      lcd.print("           ");
    }
    displayState = (displayState + 1) % 3;
  }

  server.handleClient();
  delay(500);
}