#define BLYNK_TEMPLATE_ID "TMPL6aqEuHuaB"
#define BLYNK_TEMPLATE_NAME "Agriculture Sys"
#define BLYNK_AUTH_TOKEN "OD-Wx6DW1tgqEDLU7g_OiGmBzNeIf2o9"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Arduino.h>
#include <DHT.h>

// WiFi credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "A63";
char pass[] = "11223344";

// Sensor pin definitions
int moist = 34;
#define DHTTYPE DHT11
DHT dht(13, DHT11);

// Sensor class to encapsulate sensor data
class Sensor {
  public:
    Sensor(int moisturePin, int dhtPin) : moisturePin(moisturePin), dht(dhtPin, DHT11) {}

    void begin() {
      dht.begin();
      pinMode(moisturePin, INPUT);
    }

    float readMoisture() {
      int sensor_read = analogRead(moisturePin);
      return 100 - ((sensor_read / 4095.0) * 100);
    }

    float readTemperature() {
      float temperature = dht.readTemperature();
      if (isnan(temperature)) {
        Serial.println("Failed to read temperature from DHT sensor!");
        return -1;  // Return -1 to indicate error
      }
      return temperature;
    }

    float readHumidity() {
      float humidity = dht.readHumidity();
      if (isnan(humidity)) {
        Serial.println("Failed to read humidity from DHT sensor!");
        return -1;  // Return -1 to indicate error
      }
      return humidity;
    }

  private:
    int moisturePin;
    DHT dht;
};

// Blynk controller class
class BlynkController {
  public:
    BlynkController(char* authToken, char* ssid, char* pass) {
      this->authToken = authToken;
      this->ssid = ssid;
      this->pass = pass;
    }

    void begin() {
      Blynk.begin(authToken, ssid, pass, "blynk.cloud", 80);
    }

    void updateMoisture(float moisture) {
      Blynk.virtualWrite(V3, moisture);
    }

    void updateTemperature(float temperature) {
      Blynk.virtualWrite(V1, temperature);
    }

    void updateHumidity(float humidity) {
      Blynk.virtualWrite(V2, humidity);
    }

    void controlLED(int state) {
      if (state == 1) {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("LED IS ON");
      } else {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("LED IS OFF");
      }
    }

    void run() {
      Blynk.run();
    }

  private:
    char* authToken;
    char* ssid;
    char* pass;
};

// Create instances of the Sensor and BlynkController classes
Sensor sensor(moist, 13);
BlynkController blynkController(auth, ssid, pass);

// Blynk button control for LED
BLYNK_WRITE(V0) {
  int ledVal1 = param.asInt();
  blynkController.controlLED(ledVal1);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  sensor.begin();
  blynkController.begin();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WIFI. . . ");
  } else {
    Serial.println("WIFI Connected.");
  }

  float moisture = sensor.readMoisture();
  float temperature = sensor.readTemperature();
  float humidity = sensor.readHumidity();

  if (temperature != -1 && humidity != -1) {
    blynkController.updateTemperature(temperature);
    blynkController.updateHumidity(humidity);
  }

  blynkController.updateMoisture(moisture);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" degrees Celsius");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.println("Moisture: ");
  Serial.println(moisture);

  blynkController.run();
  delay(200);
}
