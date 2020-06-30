#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define MATRIX_PIN 10
#define MIC_PIN A7
#define MIC_SAMPLE_TIME 50
#define LED_BRIGHTNESS 255
#define ONBOARD_PIXEL 8

const bool isMEMSMic = true;

const float V_BIAS = 0.67;
const float V_INPUT = 3.3;
const int MAX_ANALOG_INPUT = 1023;

const bool MOUTH[6][8][8] = {
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
  },
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
    {0,0,1,1,1,1,0,0},
    {0,1,0,0,0,0,1,0},
    {1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1},
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

// Spin through a rainbow of colors
uint32_t colorWheel(byte wheelPosition) {
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

// Display a colorful smiling face
void smile() {
  for (int c = 0; c < 256; c++) {
    for (int x = 0; x < 8; x++) {
      for (int y = 0; y < 8; y++) {
        if (MOUTH[5][x][y]) {
          matrix.drawPixel(x, y, colorWheel(c));
        } else {
          matrix.drawPixel(x, y, matrix.Color(0,0,0));
        }
      }
    }
    matrix.show();
    delay(5);
  }
}

int micLevel;
int smileFrameCount = 0;
unsigned int maxAmplitude;
unsigned int minAmplitude;
unsigned int peakToPeakAmplitude; 
unsigned long startTime;

// Get mic volume
float findPeakToPeakVolume() {
  maxAmplitude = 0;
  minAmplitude = MAX_ANALOG_INPUT;
  peakToPeakAmplitude = 0; 

  startTime = millis();

  // Sample the mic for a period of time
  while (millis() - startTime < MIC_SAMPLE_TIME) {
    micLevel = analogRead(MIC_PIN);

    if (micLevel < MAX_ANALOG_INPUT) {
      if (micLevel > maxAmplitude) {
        maxAmplitude = micLevel;
      } else if (micLevel < minAmplitude) {
        minAmplitude = micLevel;
      }
    }
  }

  peakToPeakAmplitude = maxAmplitude - minAmplitude;

  // Serial.print(minAmplitude);
  // Serial.print(' ');
  // Serial.print(maxAmplitude);
  // Serial.print(' ');
  // Serial.println(peakToPeakAmplitude);

  return peakToPeakAmplitude;   
}

int scaledLevel;
int sampleLevel;
const int MAX_NORMAL_SPEECH = 30;

void loop() {
  sampleLevel = findPeakToPeakVolume();

  // Cap the mic input to a maximum level (found through testing)
  if (sampleLevel > MAX_NORMAL_SPEECH) sampleLevel = MAX_NORMAL_SPEECH;

  // Scale the volume to a number from 0-4 to determine which mouth shape to show
  scaledLevel = map(sampleLevel, 0, MAX_NORMAL_SPEECH, 0, 4);

  // Serial.print(level);
  // Serial.print(' ');
  // Serial.println(sampleLevel);

  // Determine we're smiling by looking for max volume for 5 sample frames
  if (sampleLevel == MAX_NORMAL_SPEECH) {
    if (smileFrameCount++ > 5) {
      // Show the smile instead
      // scaledLevel = 5;
      smileFrameCount = 0;
    }
  } else {
    smileFrameCount = 0;
  }

  // Draw the matrix
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      if (MOUTH[scaledLevel][x][y]) {
        matrix.drawPixel(x, y, matrix.Color(LED_BRIGHTNESS,LED_BRIGHTNESS,LED_BRIGHTNESS));
      } else {
        matrix.drawPixel(x, y, matrix.Color(0,0,0));
      }
    }
  }
  matrix.show();

  // Run the smile color wheel decoration if we're smiling
  // if (scaledLevel == 5) {
  //   smile();
  // }
}
