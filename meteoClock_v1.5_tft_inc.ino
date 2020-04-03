/*
  Скетч к проекту "Домашняя метеостанция"
  Страница проекта (схемы, описания): https://alexgyver.ru/meteoclock/
  Исходники на GitHub: https://github.com/AlexGyver/MeteoClock
  Нравится, как написан и закомментирован код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  http://AlexGyver.ru/
*/

// ------------------------- НАСТРОЙКИ --------------------
#define RESET_CLOCK 0       // сброс часов на время загрузки прошивки (для модуля с несъёмной батарейкой). Не забудь поставить 0 и прошить ещё раз!
#define SENS_TIME 30000     // время обновления показаний сенсоров на экране, миллисекунд
#define LED_MODE 0          // тип RGB светодиода: 0 - главный катод, 1 - главный анод

// управление яркостью
#define BRIGHT_CONTROL 1      // 0/1 - запретить/разрешить управление яркостью (при отключении яркость всегда будет макс.)
#define BRIGHT_THRESHOLD 150  // величина сигнала, ниже которой яркость переключится на минимум (0-1023)
#define LED_BRIGHT_MAX 255    // макс яркость светодиода СО2 (0 - 255)
#define LED_BRIGHT_MIN 10     // мин яркость светодиода СО2 (0 - 255)
#define LCD_BRIGHT_MAX 255    // макс яркость подсветки дисплея (0 - 255)
#define LCD_BRIGHT_MIN 10     // мин яркость подсветки дисплея (0 - 255)

#define BLUE_YELLOW 1       // жёлтый цвет вместо синего (1 да, 0 нет) но из за особенностей подключения жёлтый не такой яркий
#define DISP_MODE 1         // в правом верхнем углу отображать: 0 - год, 1 - день недели, 2 - секунды
#define WEEK_LANG 1         // язык дня недели: 0 - английский, 1 - русский (транслит)
#define DEBUG 1             // вывод на дисплей лог инициализации датчиков при запуске. Для дисплея 1602 не работает! Но дублируется через порт!
#define CO2_SENSOR 0        // включить или выключить поддержку/вывод с датчика СО2 (1 вкл, 0 выкл)
#define RTC_CLOCK 0
#define BME280_SENSOR 1
#define PRESSURE 1          // 0 - график давления, 1 - график прогноза дождя (вместо давления). Не забудь поправить пределы гроафика
#define DISPLAY_TYPE 1      // тип дисплея: 1 - 2004 (большой), 0 - 1602 (маленький)
#define DISPLAY_ADDR 0x27   // адрес платы дисплея: 0x27 или 0x3f. Если дисплей не работает - смени адрес! На самом дисплее адрес не указан

// пределы отображения для графиков
#define TEMP_MIN 15
#define TEMP_MAX 35
#define HUM_MIN 0
#define HUM_MAX 100
#define PRESS_MIN -100
#define PRESS_MAX 100
#define CO2_MIN 300
#define CO2_MAX 2000

// адрес BME280 жёстко задан в файле библиотеки Adafruit_BME280.h
// стоковый адрес был 0x77, у китайского модуля адрес 0x76.
// Так что если юзаете НЕ библиотеку из архива - не забудьте поменять

// если дисплей не заводится - поменяйте адрес (строка 54)

// пины
#define BACKLIGHT 10
#define PHOTO A3

#define MHZ_RX 2
#define MHZ_TX 3

#define LED_COM 7
#define LED_R 1
#define LED_G 6
#define LED_B 5
#define BTN_PIN 4

#define BL_PIN 10     // пин подсветки дисплея
#define PHOTO_PIN 0   // пин фоторезистора

// библиотеки
#include <Wire.h>
#include "ST7735_TEE.h"
//#include <LiquidCrystal_I2C.h>

#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;

#if (CO2_SENSOR == 1)
#include <MHZ19_uart.h>
MHZ19_uart mhz19;
#endif

TEE_ST7735 lcd(8,9,10,11,12); //Arduino  csk,sda,A0,rst,cs

#include <GyverTimer.h>
GTimer_ms sensorsTimer(SENS_TIME);
GTimer_ms drawSensorsTimer(SENS_TIME);
GTimer_ms clockTimer(500);
GTimer_ms hourPlotTimer((long)4 * 60 * 1000);         // 4 минуты
GTimer_ms dayPlotTimer((long)1.6 * 60 * 60 * 1000);   // 1.6 часа
GTimer_ms plotTimer(240000);
GTimer_ms predictTimer((long)10 * 60 * 1000);         // 10 минут
GTimer_ms brightTimer(2000);

#include "GyverButton.h"
GButton button(BTN_PIN, LOW_PULL, NORM_OPEN);

int8_t hrs, mins, secs;
byte mode = 0;
/*
  0 часы и данные
  1 график температуры за час
  2 график температуры за сутки
  3 график влажности за час
  4 график влажности за сутки
  5 график давления за час
  6 график давления за сутки
  7 график углекислого за час
  8 график углекислого за сутки
*/

// переменные для вывода
float dispTemp;
byte dispHum;
int dispPres;
int dispCO2;
int dispRain;

// массивы графиков
int tempHour[15], tempDay[15];
int humHour[15], humDay[15];
int pressHour[15], pressDay[15];
int co2Hour[15], co2Day[15];
int delta;
uint32_t pressure_array[6];
uint32_t sumX, sumY, sumX2, sumXY;
float a, b;
byte time_array[6];

#if (LED_MODE == 0)
byte LED_ON = (LED_BRIGHT_MAX);
byte LED_OFF = (LED_BRIGHT_MIN);
#else
byte LED_ON = (255 - LED_BRIGHT_MAX);
byte LED_OFF = (255 - LED_BRIGHT_MIN);
#endif

void setup() {

  pinMode(BACKLIGHT, OUTPUT);
  pinMode(LED_COM, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  setLED(0);

  digitalWrite(LED_COM, LED_MODE);
  analogWrite(BACKLIGHT, LCD_BRIGHT_MAX);

  lcd.init(lcd.HORIZONTAL);
  lcd.fillScreen(WHITE);

#if (DEBUG == 1)
        Serial.begin(9600);
        boolean status = true;

        setLED(1);

          #if (CO2_SENSOR == 1)
            lcd.Printstr(0, 0, "CO2... ", BLACK);
            //Serial.print(F("MHZ-19... "));
            mhz19.begin(MHZ_TX, MHZ_RX);
            mhz19.setAutoCalibration(false);
            mhz19.getStatus();    // первый запрос, в любом случае возвращает -1
            delay(500);
            if (mhz19.getStatus() == 0) {
              lcd.Printstr(0, 10, "OK", BLACK);
              //Serial.println(F("OK"));
            } else {
              lcd.Printstr(0, 10, "ERROR", BLACK);
              //Serial.println(F("ERROR"));
              status = false;
            }
          #endif

          #if (RTC_CLOCK == 1)
            setLED(2);
            lcd.Printstr(0, 20, "RTC...", BLACK);
            //Serial.print(F("RTC... "));
            delay(50);
            if (rtc.begin()) {
              lcd.Printstr(0, 30, "OK", BLACK);
              //Serial.println(F("OK"));
            } else {
              lcd.Printstr(0, 30, "ERROR", BLACK);
              //Serial.println(F("ERROR"));
              status = false;
            }
          #endif

          #if (BME280_SENSOR == 1)
            setLED(3);
            lcd.Printstr(0, 40, "BME280...", BLACK);
            //Serial.print(F("BME280... "));
            delay(50);
            if (bme.begin()) {
              lcd.Printstr(0, 50, "OK", BLACK); 
              //Serial.println(F("OK"));
            } else {
              lcd.Printstr(0, 50, "ERROR", BLACK);
           //   drawDig(bme.getaddr(), 50, 50, BLACK)
              //Serial.println(F("ERROR"));
              status = false;
            }
          #endif
          
            setLED(0);
            if (status) {
              lcd.Printstr(70, 70, "All good", BLACK);
              //Serial.println(F("All good"));
            } else {
              lcd.Printstr(70, 70, "ERROR", BLACK);
              //Serial.println(F("Check wires!"));
            }
            while (1) {
              //lcd.setCursor(14, 1);
              //lcd.print("P:    ");
              //lcd.setCursor(16, 1);
              //lcd.print(analogRead(PHOTO), 1);
              //Serial.println(analogRead(PHOTO));
              delay(300);
            }
#endif

#if (CO2_SENSOR == 1)
  mhz19.begin(MHZ_TX, MHZ_RX);
  mhz19.setAutoCalibration(false);
#endif
  rtc.begin();
  bme.begin(&Wire);


  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );

  if (RESET_CLOCK || rtc.lostPower())
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  now = rtc.now();
  secs = now.second();
  mins = now.minute();
  hrs = now.hour();

  bme.takeForcedMeasurement();
  uint32_t Pressure = bme.readPressure();
  for (byte i = 0; i < 6; i++) {   // счётчик от 0 до 5
    pressure_array[i] = Pressure;  // забить весь массив текущим давлением
    time_array[i] = i;             // забить массив времени числами 0 - 5
  }

  if (DISPLAY_TYPE == 1) {
    //loadClock();
    drawClock(hrs, mins, 1);
    drawData();
  }
  readSensors();
  drawSensors();
}


void loop() {
//  if (brightTimer.isReady()) checkBrightness(); // яркость
  if (sensorsTimer.isReady()) readSensors();    // читаем показания датчиков с периодом SENS_TIME

#if (DISPLAY_TYPE == 1)
  if (clockTimer.isReady()) clockTick();        // два раза в секунду пересчитываем время и мигаем точками
  plotSensorsTick();                            // тут внутри несколько таймеров для пересчёта графиков (за час, за день и прогноз)
  modesTick();                                  // тут ловим нажатия на кнопку и переключаем режимы
  if (mode == 0) {                                  // в режиме "главного экрана"
    if (drawSensorsTimer.isReady()) drawSensors();  // обновляем показания датчиков на дисплее с периодом SENS_TIME
  } else {                                          // в любом из графиков
    if (plotTimer.isReady()) redrawPlot();          // перерисовываем график
  }
#else
  if (drawSensorsTimer.isReady()) drawSensors();
#endif
}
