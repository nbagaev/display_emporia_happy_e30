/*Arduino test sketch for emporia happy e30 Display

display controller is similar ST7735 128x160
but PWCTR1 has 3 Parameters and VMCTR1 has 1 Parameter

FFC pitch 0.8mm

pinout
1-  GND
2-  BL -
3-  BL +3V 64mA
4-  114Hz 350us Pulse when BL ON 
5-  RESET
6-  C/D
7-  SDA
8-  CLK
9-  2.8V
10- 2.8V
11- CS
12- GND
*/

#include <SPI.h>
#define SLPOUT  0x11 //Sleep Out

#define FRMCTR1 0xB1 //Frame Rate Control (In normal mode/ Full colors)
#define FRMCTR2 0xB2 //Frame Rate Control (In Idle mode/ 8-colors)
#define FRMCTR3 0xB3 //Frame Rate Control (In Partial mode/ full colors)   

#define INVCTR  0xB4 //Display Inversion Control

#define PWCTR1  0xC0 //Power Control 1           3 Parameter!
#define PWCTR2  0xC1 //Power Control 2
#define PWCTR3  0xC2 //Power Control 3 (in Normal mode/ Full colors)
#define PWCTR4  0xC3 //Power Control 4 (in Idle mode/ 8-colors)
#define PWCTR5  0xC4 //Power Control 5 (in Partial mode/ full-colors)
#define VMCTR1  0xC5 //VCOM_Control 2            1 Parameter!


#define MADCTL  0x36 //This command defines read/write scanning direction of frame memory
#define GMCTRP1 0xE0 //Gamma adjustment (+ polarity)
#define GMCTRN1 0xE1 //Gamma adjustment (- polarity)
#define COLMOD  0x3A //Interface Pixel Format
#define TEON    0x35 //Tearing effect mode set & on
#define DISPON  0x29 //Display On

#define CASET   0x2A //Column address set
#define RASET   0x2B //Row address set
#define RAMWR   0x2C //Memory Write

#define SCX     10 //chip enable pin
#define DCX     9  //data|command pin
#define RES     8 //reset pin

/*
Uno, Nano..
SCL  13
SDA  11

ESP32 WROOM32
CSX  15
DCX  2
RES  4
SCL  18
SDA  23
*/

const uint8_t gamma_plus[] PROGMEM = {0x04,0x22,0x07,0x0A,0x2E,0x30,0x25,0x2A,0x28,0x26,0x2E,0x3A,0x00,0x01,0x03,0x13};
const uint8_t gamma_minus[] PROGMEM = {0x04,0x16,0x06,0x0D,0x2D,0x26,0x23,0x27,0x27,0x25,0x2D,0x3B,0x00,0x01,0x04,0x13};
        
void send_data(uint8_t bits)
{
  digitalWrite(DCX, HIGH);//set D/C HIGH = data
  SPI.transfer(bits);//send data bits   
  digitalWrite(DCX, LOW);//set D/C LOW = command
}

void send_command(uint8_t bits)
{
  SPI.transfer(bits);//send data bits 
}

void setup()
{
  //init
  pinMode(SCX, OUTPUT);
  pinMode(RES, OUTPUT); 
  pinMode(DCX, OUTPUT);   
  digitalWrite(RES, HIGH);
  //reset 
  digitalWrite(RES, LOW);
  delay(8);
  digitalWrite(RES, HIGH);
  delay(17); 
   
  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  
  send_command(SLPOUT);
  delay(120);
  
  send_command(FRMCTR1);
  send_data(0x01);
  send_data(0x08);
  send_data(0x05); 
  
  send_command(FRMCTR2);
  send_data(0x05);
  send_data(0x3C);
  send_data(0x3C);

  send_command(FRMCTR3);
  send_data(0x05);
  send_data(0x3C);
  send_data(0x3C);
  send_data(0x05);
  send_data(0x3C);
  send_data(0x3C);

  send_command(INVCTR);
  send_data(0x03);

  send_command(PWCTR1);
  send_data(0x28);
  send_data(0x08);
  send_data(0x04);

  send_command(PWCTR2);
  send_data(0xC0);

  send_command(PWCTR3);
  send_data(0x0D);
  send_data(0x00);

  send_command(PWCTR4);
  send_data(0x8D);
  send_data(0x2A);

  send_command(PWCTR5);
  send_data(0x8D);
  send_data(0xEE);

  send_command(VMCTR1);
  send_data(0x14);

  send_command(MADCTL);
  send_data(0x10);

  send_command(GMCTRP1);//Gamma adjustment (+ polarity)
  for(uint8_t i = 0; i < 16; i++)
  {
    send_data(pgm_read_byte_near(gamma_plus + i));
  }

  send_command(GMCTRN1);//Gamma adjustment (- polarity)
  for(uint8_t i = 0; i < 16; i++)
  {
    send_data(pgm_read_byte_near(gamma_minus + i));
  }

  send_command(COLMOD); 
  send_data(0x05);

  send_command(TEON); 
  send_data(0x00);

  send_command(DISPON);
  delay(8);
  
  send_command(CASET);
  send_data(0x00);  send_data(0x00);//0
  send_data(0x00);  send_data(0x7F);//127
  
  send_command(RASET);
  send_data(0x00);  send_data(0x00);//0
  send_data(0x00);  send_data(0x9F);//159
  
  send_command(RAMWR);

  //fill display black
  for(int i = 0; i < 20480; i++)//128*160
  {
    send_data(0x00); send_data(0x00);
  }
  
  //print char A
  const uint8_t a[] = {0x18, 0x24, 0x42, 0x81, 0xFF, 0x81, 0x81, 0x81};
  /*  
  00011000
  00100100
  01000010  
  10000001
  11111111
  10000001
  10000001
  10000001
  */
  uint8_t line;

  send_command(CASET);
  send_data(0x00);  send_data(0x00);//0
  send_data(0x00);  send_data(0x07);//7
  
  send_command(RASET);
  send_data(0x00);  send_data(0x00);//0
  send_data(0x00);  send_data(0x07);//7
  
  send_command(RAMWR);
  
  for(uint8_t i = 0; i < 8; i++)
  {
    line = a[i];
    for(uint8_t j = 0; j < 8; j++)
    {
      if((line & 0x80) == 0x80)
      {
        send_data(0xFF); send_data(0xFF);        
      }
      else
      {
        send_data(0x00); send_data(0x00);
      }
      line <<= 1;
    }    
  }
  SPI.endTransaction();
  SPI.end();
}

void loop()
{

}
