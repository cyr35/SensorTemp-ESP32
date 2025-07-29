Sensor de Temperatura ESP32 con Servidor Web y Configuración
Este repositorio contiene el firmware para un dispositivo ESP32 que mide la temperatura utilizando un sensor DS18B20, muestra los datos en una pantalla LCD I2C, y expone una interfaz de configuración y estado a través de un servidor web. Además, el dispositivo envía periódicamente los datos de temperatura a un servidor remoto.

Características
Medición de Temperatura: Utiliza un sensor DS18B20 para lecturas precisas de temperatura.

Pantalla LCD: Muestra la temperatura actual, el estado de la conexión WiFi, la dirección IP del dispositivo y el ID del dispositivo en una pantalla LCD 16x2 I2C.

Servidor Web Integrado:

Página de estado que muestra la temperatura, el ID del dispositivo y la IP local.

Portal de configuración para establecer las credenciales WiFi (SSID y Contraseña), el ID del dispositivo y la contraseña de administración.

Modo de Configuración Automático: Si no hay credenciales WiFi guardadas o la conexión falla, el ESP32 inicia un punto de acceso (AP) para permitir la configuración inicial a través de un navegador web.

Almacenamiento Persistente: Utiliza el sistema de preferencias (NVS) del ESP32 para guardar las configuraciones de WiFi, el ID del dispositivo y la contraseña de administración, persistiendo entre reinicios.

Envío de Datos Remoto: Envía la temperatura medida a un endpoint HTTP (https://temperature.zeronetwork.cl/insert.php) para el registro o monitoreo externo.

Autenticación Básica: Protege el portal de configuración con un nombre de usuario y contraseña.

Componentes Necesarios
Para replicar este proyecto, necesitarás los siguientes componentes de hardware:

ESP32 Dev Kit: Cualquier placa de desarrollo ESP32 compatible.

Sensor de Temperatura DS18B20: (con resistencia pull-up de 4.7kΩ, si no está integrada en el módulo).

Pantalla LCD 16x2 con módulo I2C: Asegúrate de que tenga un módulo I2C para simplificar el cableado.

Cables de conexión (Dupont).

Fuente de alimentación: Cable USB para el ESP32.

Diagrama de Conexión (Ejemplo)
ESP32         DS18B20         LCD I2C
-----------   -----------     -----------
GPIO 4 (DATA) -- Data        SDA (GPIO 21) -- SDA
GND           -- GND         SCL (GPIO 22) -- SCL
3.3V / 5V     -- VCC         GND           -- GND
                               VCC           -- VCC (5V)

Asegúrate de verificar los pines SDA y SCL específicos de tu placa ESP32, ya que pueden variar.

Requisitos de Software
Arduino IDE o PlatformIO.

Placa ESP32 configurada en el IDE.

Librerías de Arduino (Instalación desde el Gestor de Librerías)
Asegúrate de tener instaladas las siguientes librerías en tu Arduino IDE:

WiFi: (Generalmente incluida con el soporte de placa ESP32)

WebServer: (Generalmente incluida con el soporte de placa ESP32)

Preferences: (Generalmente incluida con el soporte de placa ESP32)

Wire: (Generalmente incluida con el soporte de placa ESP32)

LiquidCrystal_I2C: Búscala en el Gestor de Librerías (por Juan Batiz-Benet o similar).

OneWire: Búscala en el Gestor de Librerías (por Paul Stoffregen).

DallasTemperature: Búscala en el Gestor de Librerías (por Miles Burton).

HTTPClient: (Generalmente incluida con el soporte de placa ESP32)

Instrucciones de Configuración y Uso
Clonar el Repositorio:

git clone https://github.com/tu-usuario/nombre-de-tu-repo.git
cd nombre-de-tu-repo

Abrir en Arduino IDE:
Abre el archivo .ino principal en el Arduino IDE.

Instalar Librerías:
Asegúrate de que todas las librerías mencionadas en la sección "Requisitos de Software" estén instaladas. Puedes usar el "Gestor de Librerías" en el Arduino IDE (Sketch > Incluir Librería > Administrar Librerías...).

Seleccionar Placa y Puerto:
En el Arduino IDE, selecciona tu placa ESP32 (e.g., ESP32 Dev Module) y el puerto COM/USB correcto.

Cargar el Código:
Sube el código a tu ESP32.

Primer Inicio y Configuración
Al encender el ESP32 por primera vez (o si no encuentra una red WiFi guardada):

El dispositivo iniciará un punto de acceso (AP) llamado ESP32_Config.

La pantalla LCD mostrará "Modo Config" y la dirección IP del AP (generalmente 192.168.4.1).

Conéctate a la red ESP32_Config desde tu teléfono o computadora.

Abre un navegador web y navega a http://192.168.4.1.

Verás la página de estado. Haz clic en el enlace "Configuración".

Se te pedirá un nombre de usuario y contraseña. Los valores por defecto son:

Usuario: admin

Contraseña: admin

En el formulario de configuración:

Ingresa el SSID de tu red WiFi.

Ingresa la Contraseña de tu red WiFi.

Define un ID del dispositivo (ej. "SensorSalon").

(Opcional) Ingresa una Nueva clave admin para cambiar la contraseña por defecto.

Haz clic en "Guardar". El ESP32 se reiniciará y intentará conectarse a la red WiFi configurada.

Operación Normal
Una vez conectado a tu red WiFi:

La pantalla LCD mostrará la temperatura, y alternará entre "WiFi OK", la IP asignada y el ID del dispositivo.

Puedes acceder a la página de estado del dispositivo abriendo un navegador web y navegando a la dirección IP que se muestra en la LCD (ej. http://192.168.1.100).

La temperatura se enviará automáticamente al servidor remoto (https://temperature.zeronetwork.cl/insert.php) cada 500ms (definido por delay(500) en el loop).

Endpoint de Envío de Temperatura
El dispositivo envía los datos de temperatura a la siguiente URL:

https://temperature.zeronetwork.cl/insert.php?id=<ID_DISPOSITIVO>&temp=<TEMPERATURA>

Donde:

<ID_DISPOSITIVO> es el ID que configuraste para el dispositivo.

<TEMPERATURA> es la lectura de temperatura actual en grados Celsius.

Contribuciones
¡Las contribuciones son bienvenidas! Si tienes ideas para mejorar, por favor:

Haz un "fork" de este repositorio.

Crea una nueva rama (git checkout -b feature/nueva-funcionalidad).

Realiza tus cambios y haz "commit" (git commit -am 'Add new feature').

Sube tus cambios (git push origin feature/nueva-funcionalidad).

Abre un "Pull Request".

Licencia
Este proyecto está bajo la Licencia MIT. Consulta el archivo LICENSE para más detalles.

Aquí podrías insertar una imagen de tu configuración de hardware o un diagrama de conexión más detallado.

Aquí podrías insertar una captura de pantalla de la página web de estado.

Aquí podrías insertar una captura de pantalla del portal de configuración web.