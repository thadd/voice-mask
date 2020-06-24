#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define MATRIX_PIN 10
#define MIC_PIN 6
#define MIC_SAMPLES 128
#define LED_BRIGHTNESS 255

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

void setup() {
  matrix.begin();
  Serial.begin(9600);
}

int smileFrameCount = 0;

uint32_t colorWheel(byte wheelPositionition) {
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

void loop() {
  int level = 0;
  int rawLevel = 0;
  int maxSampledLevel = 0;

  for (int i = 0; i < MIC_SAMPLES; i++) {
    auto rawLevel = abs(analogRead(MIC_PIN));

    maxSampledLevel = max(rawLevel, maxSampledLevel);
  }

  auto normalizedSample = maxSampledLevel;

  if (normalizedSample < 500) normalizedSample = 500;
  if (normalizedSample > 1000) normalizedSample = 1000;

  level = map(normalizedSample, 500, 1000, 0, 3);

  Serial.print(level);
  Serial.print(' ');
  Serial.println(maxSampledLevel);

  if (maxSampledLevel == 1022) {
    if (smileFrameCount++ > 5) {
      level = 4;
      smileFrameCount = 0;
    }
  } else {
    smileFrameCount = 0;
  }

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
