// otrFeedbaack.h
#ifndef OTRFEEDBACK_H
#define OTRFEEDBACK_H

#include <Arduino.h>
#include <lvgl.h>


void playClickGoesTheShears(int buzzerPin);
void playWaltzingMatilda(int buzzerPin);

class BUZZER {
    public:
        int buzzerPin = 16;
        int toneIndex = 0;
        unsigned long toneStartTime = 0;
        bool active = false;
        struct Tone {
            int frequency;
            int duration;
        };
        Tone toneSequence[10];
        int volume = 128;
        int interval = 50;
        const int defaultFrequency = 2700;
        const int shortBeepDuration = 100;
        const int longBeepDuration = 600;
        
        void init();
        void shortBeep();
        void longBeep();
        void successTone();
        void errorTone();
        void playTone();
        void playNextTone();
        void monitor();
};

class LED {
    public:
        int ledPin = 39;
        void init();
        void toggle();
        void flash();
        void pulsing();       
        struct dutyCycle{
            bool status = true; //on/off
            bool direction = 0; //0 down, 1 up
            uint16_t duty = 1023;
            const uint8_t increment = 8;
            const uint16_t min = 9;
            const uint16_t max = 300;
            const uint16_t flash = 700;
            const uint16_t interval = 50;
        };
        dutyCycle fade;
        
    private:

};

class VIBRATE {
    public:
        const int vibratePin = 40;
        bool status = 0; //on/off
        bool active = 0;
        bool repeat = 0;
        const uint16_t duration = 500;
        const uint16_t interval = 50;
        uint16_t count = 0;
        uint8_t cycleCount = 0;
        const uint8_t longBuzzMultiplier = 3;
        const uint8_t tapDivisor = 5;
        const uint16_t errorDuration = 300;
        const uint8_t errorGap = 100;
        const uint8_t errorCount = 3;
        uint8_t numRepeats = 0;
        uint16_t buzzLength = 0;
        uint8_t buzzGap = 0;
        void begin();
        void longBuzz();
        void tap();
        void error();
        void success();
        void toggle();
        void stop();
        void start();
        void monitor();

};

#endif
