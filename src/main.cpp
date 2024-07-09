#include <Arduino.h>
#include <Arduino.h>
#include <Adafruit_SSD1351.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include "fix_fft.h"
#include "OLED.h"

// Define pin for microphone input
#define micpin A0

// Define variables for fixed point FFT function
#define noSamplesBits 7
#define noSamples (1 << noSamplesBits)                   // Calculate number of samples (128)
char im[noSamples], data[noSamples], oldData[noSamples]; // Declare arrays to store real and imaginary components of signal

void setup()
{
  // Initialise serial monitor
  Serial.begin(9600);
  Serial.println(F("Hello world!"));

  // Initialise the display
  oledInit();
}

void loop()
{
  // Cycle data so that new data is  now old data
  for (size_t i = 0; i < noSamples; i++)
  {
    oldData[i] = data[i];
  }

  // Optionally keep track of min and max readings
  // int min = 1024, max = 0;

  // Measure new data
  for (int i = 0; i < noSamples; i++)
  {
    int val = analogRead(micpin);

    // Optionally keep track of min and max readings
    // if (val > max)
    //   max = val; // Store max & min sample
    // if (val < min)
    //   min = val;

    // Center data around 0 DC offset
    data[i] = val / 4 - (1 << noSamplesBits);
    im[i] = 0;
  }

  // Perform FFT on new data
  fix_fft(data, im, noSamplesBits, 0);

  // Draw Bar graph
  for (int i = 1; i < noSamples / 2; i++)
  { // Bar graph sketch on OLED display

    // Gain variable (Potentially to be  mapped to potentiometer in future)
    int gain = 8;

    char x = 5, ymin = 30, ymax = 10;

    // Erase old
    int dat = gain * sqrt(oldData[i] * oldData[i]);
    // Crop Magnitude to screen boundaries
    if (dat > (SCREEN_HEIGHT - (ymax + ymin)))
    {
      dat = (SCREEN_HEIGHT - (ymax + ymin));
    }
    oled.drawFastVLine(i * 2 + x, SCREEN_HEIGHT - ymin, -1 * dat, Black);

    // Draw new
    dat = gain * sqrt(data[i] * data[i]);
    // Crop Magnitude to screen boundaries
    if (dat > (SCREEN_HEIGHT - (ymax + ymin)))
    {
      dat = (SCREEN_HEIGHT - (ymax + ymin));
    }
    oled.drawFastVLine(i * 2 + x, SCREEN_HEIGHT - ymin, -1 * dat, White);
  }
}
