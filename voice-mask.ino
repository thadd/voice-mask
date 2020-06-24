#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define MATRIX_PIN 10
#define MIC_PIN A7
#define MIC_SAMPLES 50
#define LED_BRIGHTNESS 150
#define ONBOARD_PIXEL 8

const bool isMEMSMic = true;

const bool MOUTH[5][8][8] = {
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,0},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {0,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,0,0},
    {0,1,0,0,0,0,1,0},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {0,1,0,0,0,0,1,0},
    {0,0,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0}
  },
  {
    {0,0,1,1,1,1,0,0},
    {0,1,0,0,0,0,1,0},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {0,1,0,0,0,0,1,0},
    {0,0,1,1,1,1,0,0}
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1},
    {0,1,0,0,0,0,1,0},
    {0,0,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
  }
};

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, MATRIX_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, ONBOARD_PIXEL, NEO_GRB + NEO_KHZ800);

int lowLevel = 1024;
int highLevel = 0;

void setup() {
  Serial.begin(9600);

  matrix.begin();
}

int smileFrameCount = 0;

uint32_t colorWheel(byte wheelPosition) {
  wheelPosition = 255 - wheelPosition;

  if (wheelPosition < 85) {
    return matrix.Color(255 - wheelPosition * 3, 0, wheelPosition * 3);
  }
  else if (wheelPosition < 170) {
    wheelPosition -= 85;
    return matrix.Color(0, wheelPosition * 3, 255 - wheelPosition * 3);
  }
  else {
    wheelPosition -= 170;
    return matrix.Color(wheelPosition * 3, 255 - wheelPosition * 3, 0);
  }
}

void smile() {
  for (int c = 0; c < 256; c++) {
    for (int x = 0; x < 8; x++) {
      for (int y = 0; y < 8; y++) {
        if (MOUTH[4][x][y]) {
          matrix.drawPixel(x, y, colorWheel(c));
        } else {
          matrix.drawPixel(x, y, matrix.Color(0,0,0));
        }
      }
    }
    matrix.show();
  }
}

float findPeakToPeakVolume() {
   unsigned int peakToPeakAmplitude = 0; 

   unsigned int maxAmplitude = 0;
   unsigned int minAmplitude = 1023;
   int micLevel;

   for (int i = 0; i < MIC_SAMPLES; i++) { 
      micLevel = analogRead(MIC_PIN);

      if (micLevel < 1023) {
        if (micLevel > maxAmplitude) {
          maxAmplitude = micLevel;
        } else if (micLevel < minAmplitude) {
          minAmplitude = micLevel;
        }
      }
   }

  peakToPeakAmplitude = maxAmplitude - minAmplitude;
  float volume = map(peakToPeakAmplitude*3.3, 0, 1024, 0, 1);

  Serial.print(minAmplitude);
  Serial.print(' ');
  Serial.print(maxAmplitude);
  Serial.print(' ');
  Serial.print(peakToPeakAmplitude);
  Serial.print(' ');
  Serial.println(volume);

  return volume;   
}

void loop() {

  int level = 0;
  int sampleLevel = findPeakToPeakVolume();
  level = map(sampleLevel, 0, 1, 0, 3);

  // Serial.print(level);
  // Serial.print(' ');
  // Serial.println(sampleLevel);

  // if (sampleLevel == 1022) {
  //   if (smileFrameCount++ > 5) {
  //     level = 4;
  //     smileFrameCount = 0;
  //   }
  // } else {
  //   smileFrameCount = 0;
  // }

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      if (MOUTH[level][x][y]) {
        matrix.drawPixel(x, y, matrix.Color(LED_BRIGHTNESS,LED_BRIGHTNESS,LED_BRIGHTNESS));
      } else {
        matrix.drawPixel(x, y, matrix.Color(0,0,0));
      }
    }
  }
  matrix.show();

  if (level == 4) {
    smile();
    delay(2000);
  }
}
