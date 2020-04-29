#include <WiFi.h>
#define ESPALEXA_DEBUG
#include <Espalexa.h>
#include <M5StickC.h>

// prototypes
boolean connectWifi();

//callback functions
void firstLightChanged(uint8_t brightness);
void colorLightChanged(uint8_t brightness, uint32_t rgb);

// Change this!!
const char* ssid = "...";
const char* password = "wifipassword";

boolean wifiConnected = false;

Espalexa espalexa;

#define ID_M5_LED "M5 LED"
#define ID_M5_LCD "M5 LCD"

uint16_t screen_color = BLACK;
uint8_t led_brightness = 0;
uint8_t lcd_brightness = 0;
uint32_t lcd_rgb = 0;

void setup() {
  // put your setup code here, to run once:  
  Serial.begin(115200);
  // Initialise wifi connection
  wifiConnected = connectWifi();

  if(wifiConnected){

    // Define your devices here. 
//    espalexa.addDevice(ID_M5_LED, firstLightChanged); //simplest definition, default state off
    espalexa.addDevice(ID_M5_LED, firstLightChanged, 0); //third parameter is beginning state (here fully on)
    espalexa.addDevice(ID_M5_LCD, colorLightChanged);

    espalexa.begin();
    
  } else
  {
    while (1) {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }

  M5.begin();

  pinMode(M5_LED, OUTPUT);
  digitalWrite(M5_LED, HIGH);

  M5.Lcd.setRotation(3);
  update_screen();
}

void loop() {
  // put your main code here, to run repeatedly:
   espalexa.loop();
   delay(1);
}

//our callback functions
void firstLightChanged(uint8_t brightness) {
  //do what you need to do here
  Serial.print("Brightness: ");
  Serial.println(brightness);
  
  digitalWrite(M5_LED, brightness ? LOW : HIGH);

  led_brightness = brightness;
  update_screen();
}

//the color device callback function has two parameters
void colorLightChanged(uint8_t brightness, uint32_t rgb) {
  //do what you need to do here, for example control RGB LED strip
  Serial.print("Brightness: ");
  Serial.print(brightness);
  Serial.print(", Red: ");
  Serial.print((rgb >> 16) & 0xFF); //get red component
  Serial.print(", Green: ");
  Serial.print((rgb >>  8) & 0xFF); //get green
  Serial.print(", Blue: ");
  Serial.println(rgb & 0xFF); //get blue

  M5.Axp.ScreenBreath((brightness / 42) + 7);

  lcd_brightness = brightness;
  lcd_rgb = rgb;
  screen_color = M5.Lcd.color565(((rgb >> 16) & 0xFF), ((rgb >> 8) & 0xFF), (rgb & 0xFF));
  update_screen();
}

void update_screen() {
  M5.Lcd.fillScreen(screen_color);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("%s\n", ID_M5_LED);
  M5.Lcd.printf(" Bright: %d\n", led_brightness);
  M5.Lcd.printf("%s\n", ID_M5_LCD);
  M5.Lcd.printf(" Bright: %d\n", lcd_brightness);
  M5.Lcd.printf(" R: %d\n", (lcd_rgb >> 16) & 0xFF);
  M5.Lcd.printf(" G: %d\n", (lcd_rgb >> 8) & 0xFF);
  M5.Lcd.printf(" B: %d\n", lcd_rgb & 0xFF);
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}
