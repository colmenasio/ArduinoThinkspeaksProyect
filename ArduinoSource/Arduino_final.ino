#include <Servo.h>
#include <DHT.h>
#include <LiquidCrystal.h>

#define AUTO_BUTTON_PIN 6
#define TOGGLE_BUTTON_PIN 7
#define LED_PIN 8
#define LIGTH_SENSOR_PIN A0

#define DHTPIN 11          // Pin del sensor DHT11
#define DHTTYPE DHT11     // Tipo de sensor DHT que estás utilizando

DHT dht(DHTPIN, DHTTYPE);

Servo servo;             // Definir el objeto servo
const int rs = 13, en = 12, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // definimos los pines de la pantalla
int modo = 0; // 0 manual, 1 automatico

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);    // Iniciar comunicación serial
  dht.begin();           // Iniciar el sensor DHT
  servo.attach(9);       // Conectar el servo al pin 9
  lcd.clear();           // borramos la pantalla
  lcd.begin(16, 2);
}


bool is_manual = false;
bool curr_status = false;

int long last_update_auto_button = millis();
int long last_update_toggle_button = millis();
int long last_serial_print = millis();
int long last_lcd_cooldown = millis();

int long last_update_time_servo = millis();
int servo_step = 0;

void loop() {
  float humedad = dht.readHumidity();  // Leer la humedad del sensor DHT
  float temperatura = dht.readTemperature();
  int readed_value = analogRead(LIGTH_SENSOR_PIN); // Leer el valor del sensor
  float oscuridad = map(readed_value, 0, 1023, 0, 100); // poner en porcentaje

  if ((digitalRead(AUTO_BUTTON_PIN) == 1) && (millis() - last_update_auto_button >= 1500)) {
    is_manual = !is_manual;
    last_update_auto_button = millis();
  }

  if (millis() - last_update_toggle_button >= 1500) {
    curr_status = get_servo_status(is_manual, curr_status, humedad);
    last_update_toggle_button = millis();
  }

  //no hace falta cooldown
  bool do_fast_swipe = use_fast_mode(humedad);
 

  if (millis() - last_lcd_cooldown >= 1000) {
    update_screen(is_manual, curr_status);
    last_lcd_cooldown = millis();
  }
  
  //add mover servo logic
  int cooldown = do_fast_swipe ? 5 : 10;
  if ((curr_status == true) && ((millis() - last_update_time_servo) >= cooldown)) {
    servo_step = (servo_step + 1) % 250;
    int angle = 0;
    if (servo_step < 125) {
      angle = servo_step;
    } else {
      angle = 250 - servo_step;
    }
    servo.write(angle);
    last_update_time_servo = millis();
  }

  if (millis() - last_serial_print >= 1000) {
    print_data(humedad, temperatura, oscuridad, is_manual);
    last_serial_print = millis();
  }

  update_ligth(oscuridad); // oscuro
}

bool get_servo_status(bool is_manual, bool curr_status, float humedad) {
  if (is_manual == false) { //Automatico
    return humedad >= 60; // Parametro a cambiar
  } else { //Manual
    return curr_status ^ (digitalRead(TOGGLE_BUTTON_PIN) == 1);
  }
}

bool use_fast_mode(float humedad) {
  return humedad >= 70;
}

void print_data(float humedad, float temperatura, float oscuridad, bool is_manual) {
  //Sending Data 
  Serial.print(round(temperatura));
  Serial.print(',');
  Serial.print(round(humedad));
  Serial.print(',');
  Serial.print(round(100-oscuridad));
  Serial.println();
}

void update_screen(bool is_manual, bool curr_status) {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (is_manual == false) { //Auto
    lcd.print("Mode: Auto");
  } else { //Manual
    lcd.print("Mode: Manual");
    lcd.setCursor(1, 1);
    if (curr_status == true) { //encendido
      lcd.print("Working");
    } else {
      lcd.print("off");
    }
  }
}

void update_ligth(float oscuridad) {
  
  if (oscuridad > 20 ) {
    digitalWrite(LED_PIN, HIGH);

  }
  else {
    digitalWrite(LED_PIN, LOW);
  }
}
