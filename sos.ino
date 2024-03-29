#include <WiFi.h>
#include <Wire.h>
#include <MPU6050.h>
#include <math.h>

const char* ssid = "duharia";
const char* password = "abhishek";

MPU6050 mpu;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connect to Wi-Fi network
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize MPU6050 sensor
  Wire.begin();
  mpu.initialize();
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);

  // Start the web server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    String request = client.readStringUntil('\r');
    Serial.println(request);
    
    if (request.indexOf("/data") != -1) {
      String data = getSensorData();
      if(!data) {
        client.stop();
        Serial.println("Client disconnected");
      }
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println(data);
    }
    
  }
}

String getSensorData() {
  // Read raw accelerometer and gyroscope data
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Calculate pitch and roll angles (in degrees)
  float pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / M_PI;
  float roll = atan2(ay, az) * 180.0 / M_PI;

  // Format sensor data as a JSON string
  String data = "{\"pitch\":" + String(pitch) + ",\"roll\":" + String(roll) + "}";
  return data;
}
