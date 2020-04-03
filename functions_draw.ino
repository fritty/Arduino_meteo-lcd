#define CLK_COLOR BLUE
#define CLK_BCK   0x0f0f0f
#define CLK_X     114
#define CLK_Y     0

#define TEMP_COLOR BLUE
#define TEMP_BCK   (0x1d << 11)+(0x0c << 5)+0x00
#define TEMP_X     10
#define TEMP_Y     5

#define PRES_COLOR BLUE
#define PRES_BCK   0x0f0f0f
#define PRES_X     10
#define PRES_Y     50

#define HUM_COLOR BLUE
#define HUM_BCK   RED
#define HUM_X     10
#define HUM_Y     90

#define CO2_COLOR BLUE
#define CO2_BCK   CYAN
#define CO2_X     100
#define CO2_Y     50

#define RAIN_COLOR BLUE
#define RAIN_BCK   0x0f0f0f
#define RAIN_X     100
#define RAIN_Y     90


void drawDig(byte dig, byte x, byte y, uint16_t color) {
  lcd.drawChar_th(x, y, dig + 48, color);
}


void drawClock(byte hours, byte minutes, boolean dotState) {

  lcd.fillRect(CLK_X + 2, CLK_Y + 3, 41, 11, CLK_BCK);
  //if (hours > 23 || minutes > 59) return;
  if (hours < 10) drawDig(0, CLK_X, CLK_Y, CLK_COLOR);
  else drawDig(hours / 10, CLK_X, CLK_Y, CLK_COLOR);
  drawDig(hours % 10, CLK_X + 9, CLK_Y, CLK_COLOR);
  drawDig(10, CLK_X + 16, CLK_Y - 1, CLK_COLOR);
  drawDig(minutes / 10, CLK_X + 23, CLK_Y, CLK_COLOR);
  drawDig(minutes % 10, CLK_X + 32, CLK_Y, CLK_COLOR);
}

void drawData() {
  //lcd.setCursor(15, 0);
  if (now.day() < 10) //lcd.drawChar_th(120, 0, 48, BLUE);//lcd.print(0);
  lcd.drawChar_th(120, 0, now.day(), BLUE); //lcd.print(now.day());
  //lcd.print(".");
  if (now.month() < 10) //lcd.print(0);
  //lcd.print(now.month());

  if (DISP_MODE == 0) {
    //lcd.setCursor(16, 1);
    //lcd.print(now.year());
  } else if (DISP_MODE == 1) {
    //lcd.setCursor(16, 1);
    int dayofweek = now.dayOfTheWeek();
    //lcd.print(dayNames[dayofweek]);
  }
}

void drawInt(byte x, byte y, int i, byte len, uint16_t color, uint16_t bcolor)
{
  /*byte strl;
       if (i > 999) strl = 4;
  else if (i > 99) strl = 3;
  else if (i > 9) strl = 2; 
  else strl = 1;*/
  if (bcolor) lcd.fillRect(x-8*len+1, y, 8*len-1, 9, bcolor);
               drawDig( i % 10, x-10, y-4, color);
  if (i > 9)   drawDig((i % 100)/10, x-18, y-4, color);
  if (i > 99)  drawDig((i % 1000)/100, x-26, y-4, color);
  if (i > 999) drawDig( i/1000, x-34, y-4, color);  
}

void drawFrame(byte x, byte y, byte w, byte h, byte side, uint16_t color)
{
  byte r = (h-side)/2;
  lcd.fillRect(x+r+1, y, w-r*2-1, h, color);
  lcd.fillQuatCircle(x+r, y+r, r, color, 2);
  lcd.fillQuatCircle(x+w-r, y+r, r, color, 1);
  lcd.fillQuatCircle(x+r, y+h-r, r, color, 4);
  lcd.fillQuatCircle(x+w-r, y+h-r, r, color, 8);
  if (side)
  {
    lcd.fillRect(x, y+r, r+1, side+1, color);
    lcd.fillRect(x+w-r, y+r, r+1, side+1, color);
  }    
}

void drawTempIc(byte x, byte y)
{
  lcd.fillRect(x, y, 6, 12, WHITE);
  lcd.fillRect(x-2, y+13, 10, 7, WHITE);
  lcd.drawHorizontalLine(x+1, y-1, 4, WHITE);
  lcd.drawHorizontalLine(x-1, y+12, 8, WHITE);
  lcd.drawHorizontalLine(x-1, y+20, 8, WHITE);
  lcd.fillRect(x+2, y+4, 2, 9, RED);
  lcd.fillRect(x, y+14, 6, 4, RED);
  lcd.drawHorizontalLine(x+1, y+13, 4, RED);
  lcd.drawHorizontalLine(x+1, y+18, 4, RED);
}

void drawCO2Ic(byte x, byte y)
{
  lcd.fillCircle(x, y, 9, WHITE);
  lcd.drawString(x-7, y-5, "CO", CO2_BCK, 1);
  lcd.drawHorizontalLine(x+5, y+2, 3, CO2_BCK);
  lcd.drawPixel(x+7, y-1, CO2_BCK);
  lcd.drawHorizontalLine(x+5, y, 3, CO2_BCK);
  lcd.drawPixel(x+5, y+1, CO2_BCK);
  lcd.drawHorizontalLine(x+5, y-2, 3, CO2_BCK);
}

void drawPressIc(byte x, byte y)
{
  lcd.fillCircle(x, y, 9, WHITE);
  lcd.fillQuatCircle(x, y+1, 7, (0x1a << 11)+(0x3a << 5)+0x1a, 12);
  lcd.drawPixel(x-6, y-2, BLACK);
  lcd.drawPixel(x+6, y-2, BLACK);
  lcd.drawPixel(x-4, y-5, BLACK);
  lcd.drawPixel(x+4, y-5, BLACK);
  //lcd.drawPixel(x-5, y-2, BLACK);
  //lcd.drawPixel(x-5, y-2, BLACK);
  lcd.drawPixel(x, y-6, BLACK);

  lcd.drawHorizontalLine(x-1, y-1, 3, RED);
  lcd.drawHorizontalLine(x-1, y+1, 3, RED);
  lcd.drawHorizontalLine(x-1, y, 3, RED);

       if (dispPres < 720) lcd.drawLine(x, y, x-5, y, RED);
  else if (dispPres < 730) lcd.drawLine(x, y, x-5, y-2, RED);
  else if (dispPres < 740) lcd.drawLine(x, y, x-3, y-4, RED), lcd.drawPixel(x-1, y-2, RED);
  else if (dispPres < 750) lcd.drawLine(x, y, x, y-5, RED);
  else if (dispPres < 760) lcd.drawLine(x, y, x+3, y-4, RED), lcd.drawPixel(x+1, y-2, RED);
  else if (dispPres < 770) lcd.drawLine(x, y, x+5, y-2, RED);
  else                     lcd.drawLine(x, y, x+5, y, RED);
}

void drawRainIc(byte x, byte y)
{
  uint16_t c = (0x1f-0x1f*dispRain/120 << 11) + (0x3f-0x3f*dispRain/120 << 5) + 0x1f-0x1f*dispRain/120;
  lcd.fillRect(x+7, y+6, 12, 5, c);
  lcd.drawHorizontalLine(x+12, y+3, 3, c);
  lcd.drawHorizontalLine(x+11, y+4, 5, c);
  lcd.drawHorizontalLine(x+10, y+5, 6, c);
  lcd.drawHorizontalLine(x+8, y+11, 10, c);
  lcd.drawVerticalLine(x+6, y+7, 3, c);
  lcd.drawVerticalLine(x+19, y+7, 3, c);

  lcd.drawVerticalLine(x+11, y+13, 2, c);
  lcd.drawVerticalLine(x+14, y+15, 3, c);
  lcd.drawVerticalLine(x+17, y+14, 2, c);
  lcd.drawVerticalLine(x+8, y+15, 2, c);
}

void drawHumIc(byte x, byte y)
{
  uint16_t c = WHITE - (0x1f*dispHum/100 << 11) - (0x3f*dispHum/100 << 5);
  lcd.fillRect(x+7, y+9, 5, 6, c);
  lcd.drawVerticalLine(x+6, y+11, 3, c);
  lcd.drawVerticalLine(x+12, y+11, 3, c);
  lcd.drawHorizontalLine(x+8, y+15, 3, c);
  lcd.drawHorizontalLine(x+8, y+8, 3, c);
  lcd.drawHorizontalLine(x+8, y+7, 3, c);
  lcd.drawPixel(x+9, y+6, c);
}

void drawSensors() {
  //temp
    drawFrame(TEMP_X, TEMP_Y, 50, 35, 7, TEMP_BCK);
    drawTempIc(TEMP_X+8, TEMP_Y-6+13);
    drawInt(TEMP_X+39, TEMP_Y+13, dispTemp, 2, TEMP_COLOR, 0);
    lcd.drawString(TEMP_X+41, TEMP_Y+2+13, "C", TEMP_COLOR, 1);
  //press
  //dispPres = 740;
    drawFrame(PRES_X-48+48, PRES_Y-10+10, 50, 30, 7, PRES_BCK);
    drawPressIc(PRES_X-36+48, PRES_Y+4+10);
    drawInt(PRES_X+48, PRES_Y+10, dispPres, 4, PRES_COLOR, 0);
  //humid
  //dispHum = 50;
    drawFrame(HUM_X, HUM_Y, 50, 20, 4, HUM_BCK);
    drawHumIc(HUM_X+2, HUM_Y-1);
    drawInt(HUM_X+39,  HUM_Y+5,  dispHum,  2, HUM_COLOR, 0);
    lcd.drawString(HUM_X+42, HUM_Y+6, "%", HUM_COLOR, 1);
  //CO2
  //dispCO2 = 500;
    drawFrame(CO2_X-44+44, CO2_Y-10+10, 55, 30, 7, CO2_BCK);
    drawCO2Ic(CO2_X-32+44, CO2_Y+4+10);
    drawInt(CO2_X+10+44,  CO2_Y+10,  dispCO2,  4, CO2_COLOR,  0);
  //rain
  //dispRain = 40;
    drawFrame(RAIN_X, RAIN_Y, 50, 20, 4, RAIN_BCK);
    drawRainIc(RAIN_X, RAIN_Y);
    drawInt(RAIN_X+39,  RAIN_Y+5,  dispRain,  2, RAIN_COLOR, 0);
    lcd.drawString(RAIN_X+42, RAIN_Y+6, "%", RAIN_COLOR, 1);
}

void drawPlotFrame( void )
{
  lcd.drawVerticalLine(10, 20, 90, BLUE);
  lcd.drawHorizontalLine(10, 110, 150, BLUE);

  
}

void drawPlot(byte pos, byte row, byte width, byte height, int min_val, int max_val, int *plot_array, String label)
{
  drawPlotFrame();

  for (byte i = 0; i < 14; i++)
  {
    if (plot_array[i]) lcd.drawLine(10*i+5, plot_array[i], 10*i+15, plot_array[i+1], RED);
  }

}


void redrawPlot() {
  lcd.fillScreen(WHITE);
  switch (mode) {
    case 1: drawPlot(0, 3, 15, 4, TEMP_MIN, TEMP_MAX, (int*)tempHour, "t hr");
      break;
    case 2: drawPlot(0, 3, 15, 4, TEMP_MIN, TEMP_MAX, (int*)tempDay, "t day");
      break;
    case 3: drawPlot(0, 3, 15, 4, HUM_MIN, HUM_MAX, (int*)humHour, "h hr");
      break;
    case 4: drawPlot(0, 3, 15, 4, HUM_MIN, HUM_MAX, (int*)humDay, "h day");
      break;
    case 5: drawPlot(0, 3, 15, 4, PRESS_MIN, PRESS_MAX, (int*)pressHour, "p hr");
      break;
    case 6: drawPlot(0, 3, 15, 4, PRESS_MIN, PRESS_MAX, (int*)pressDay, "p day");
      break;
    case 7: drawPlot(0, 3, 15, 4, CO2_MIN, CO2_MAX, (int*)co2Hour, "c hr");
      break;
    case 8: drawPlot(0, 3, 15, 4, CO2_MIN, CO2_MAX, (int*)co2Day, "c day");
      break;
  }
}

void drawdots(byte x, byte y, uint16_t color) {
  //byte code;
  //if (state) code = 165;
  //else code = 32;
  lcd.drawPixel(x, y, color);
  //lcd.write(code);
  //lcd.setCursor(x, y + 1);
  //lcd.write(code);
}
