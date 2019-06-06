
#include <string.h>
#include "RainbowSlide.h"
#include "RainbowSpectrum.h"
#include "ArtistGradients.h"
#include "Common.h"
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
// #include "packetparser.cpp"


#include "BluefruitConfig.h"
#define LED_STRING_1 10


//instantiate LEDMatrix so that you can assign to it
static cLEDMatrix<ROW_LENGTH, NUM_ROWS, HORIZONTAL_MATRIX> ledBuffer;


//packet buffer for bluefruit
extern uint8_t packetbuffer[];
//Bluefruit Device
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


//input manager

//call whenever you wanna refresh the strip
void UpdateLEDStrip();

//print some element iteratedover the matrix
void PrintMatrix();

//prints over the ledString
void PrintString();

//fills the buffer with 0's
void InitializeMatrix();


//Check bluetooth for input, 
boolean CheckForInput();

//sets up the bluetooth device
void SetupBluetooth(); 

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);













void setup() {
  Serial.begin(9600);
  // while (!Serial); 
  FastLED.addLeds<NEOPIXEL, LED_STRING_1>(ledString, NUM_LEDS);

  
  InitializeMatrix();
  SetupBluetooth();

}

void loop() {  
  UpdateLEDStrip();
  PrintMatrix();





  if (CheckForInput()) {
    
    if (packetbuffer[1] == 'B') {
      uint8_t button = packetbuffer[2];
      if (button == 0x31) CurrentPattern = (LEDPattern)0;
      if (button == 0x32) CurrentPattern = (LEDPattern)1;
      if (button == 0x33) CurrentPattern = (LEDPattern)2;
    }
  };

  switch (CurrentPattern)
  {
    case PSpectrumWave:{SpectrumWave(ledBuffer);
                        break;}
    case PSpectrumStatic:{SpectrumStatic(ledBuffer);
                        break;}
    case PMadeonGradient:{MadeonGradient(ledBuffer);
                        break;}
    
    default:
      for(int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j< ROW_LENGTH; j++) {
          ledBuffer(j, i) = 0;
        }
      }
      break;
  }

  // delay(1);
}













void SetupBluetooth() {  
  if (!ble.begin()) Serial.println("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?");

  /* Perform a factory reset to make sure everything is in a known state */
  Serial.println(F("Performing a factory reset: "));
  if ( ! ble.factoryReset() ){
    Serial.println(F("Couldn't factory reset"));
  }

  ble.echo(false);
  
  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  Serial.println(F("***********************"));

  // Set Bluefruit to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("***********************"));
};

boolean CheckForInput() {
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0) return false;
  printHex(packetbuffer, len);
  return true;
}


void InitializeMatrix() {
  for(int i = 0; i < NUM_ROWS; i++) {
    for (int j = 0; j< ROW_LENGTH; j++) {
      ledBuffer(j, i) = 0;
    }
  }
  Serial.println("Matrix Initialized!");
}

void PrintString() {
  int ledIndex = 0;
  for(int i = 0; i < NUM_LEDS; i++) {
    // Serial.println("%i ", ledString[i].g);
  }
  Serial.println();
}

void PrintMatrix() {
  int ledIndex = 0;
  for(int i = 0; i < NUM_ROWS; i++) {
    for (int j = 0; j < ROW_LENGTH; j++) {
      ledIndex = ledMatrixLayout[i][j];
      Serial.print(" " + (String)ledBuffer(j, i).r + ", " + (String)ledBuffer(j, i).g + ", " + (String)ledBuffer(j, i).b);
    }
    Serial.println();
  }
}

void UpdateLEDStrip() {
  int ledIndex;
  for(int i = 0; i < NUM_ROWS; i++) {
    for (int j = 0; j < ROW_LENGTH; j++) {
      ledIndex = ledMatrixLayout[i][j];
      if (ledIndex != -1) {
        ledString[ledIndex-1].setRGB(ledBuffer(j, i).r, ledBuffer(j, i).g, ledBuffer(j, i).b);
      }
    }
  }
  FastLED.show();
}