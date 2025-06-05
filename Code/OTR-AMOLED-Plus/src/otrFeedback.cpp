#include "otrFeedback.h"
#include <map>
#include <string>
#include <vector>
#include <esp_adc_cal.h>
#include <driver/gpio.h>
#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <lvgl.h>
#include "ui/ui.h"

extern LilyGo_Class amoled;
extern LED led;
extern VIBRATE vibrate;
extern BUZZER buzzer;
int buzzTime;
int startBuzzTime;






void BUZZER::init(){
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);
    ledcAttachPin(buzzerPin, 2);
    ledcSetup(2, defaultFrequency, 8);
}
void BUZZER::shortBeep() {
    toneSequence[0].frequency = defaultFrequency;
    toneSequence[0].duration = shortBeepDuration;
    toneIndex = 0;
    playTone();
}

void BUZZER::longBeep() {
    toneSequence[0].frequency = defaultFrequency;
    toneSequence[0].duration = longBeepDuration;
    toneIndex = 0;
    playTone();
}

void BUZZER::successTone() {
    toneSequence[0].frequency = 3000;
    toneSequence[0].duration = 100;
    toneIndex = 0;
    playTone();
}

void BUZZER::errorTone() {
    toneSequence[0].frequency = 2000;
    toneSequence[0].duration = 100;
    toneSequence[1].frequency = 1000;
    toneSequence[1].duration = 300;
    toneIndex = 0;
    playTone();
}

void BUZZER::playTone() {
    toneIndex = 0;
    toneStartTime = millis();
    active = true;
    playNextTone();
}

void BUZZER::playNextTone() {
    if (toneIndex < sizeof(toneSequence) / sizeof(toneSequence[0])) {
        ledcWriteTone(2, toneSequence[toneIndex].frequency);
        toneStartTime = millis();
        toneIndex++;
    } else {
        ledcWriteTone(2, 0); // Stop the tone
        active = false;
    }
}

void BUZZER::monitor() {
    if (active) {
        if (toneIndex < sizeof(toneSequence) / sizeof(toneSequence[0])) {
            if (millis() - toneStartTime >= toneSequence[toneIndex - 1].duration) {
                playNextTone();
            }
        }
    }
}

void LED::init(){
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    ledcAttachPin(ledPin, 1);
    ledcSetup(1, 5000, 10);
}

void LED::toggle(){
    if (fade.status) {
        fade.status = 0;
        fade.duty = fade.min;
        fade.direction = 0;
        
    } else {
        if(fade.duty == fade.min){
            fade.duty = fade.max;

        } else {
            fade.duty = fade.min;
        }
    }
    ledcWrite(1, fade.duty);
    
}

void LED::flash(){
    fade.status = 1;
    fade.direction = 1;
    fade.duty = fade.flash;
    ledcWrite(1, fade.duty);

}
void LED::pulsing(){
    if (fade.status) {
        if (fade.direction) { 
            fade.duty += fade.increment;
            if (fade.duty >= fade.max) {
                fade.direction = 0;
            }
        } else {
            fade.duty -= fade.increment;
            if (fade.duty <= fade.min) {
                fade.duty = fade.min;
                fade.direction = 1;
            } else if (fade.duty >= fade.max) {
                fade.duty -= (fade.increment * 4);
                fade.direction = 0;
            }
        }
        //Serial.print("fade.duty: "); Serial.println(led.fade.duty);
        ledcWrite(1, fade.duty); // Update the LED brightness
    }
}

void VIBRATE::begin() {
    pinMode(vibratePin, OUTPUT);
    digitalWrite(vibratePin, LOW);
}
void VIBRATE::longBuzz() {
    digitalWrite(vibratePin, HIGH);
    status = 1;
    active = 1;
    count = 0;
    repeat = 0;
    buzzLength = duration * longBuzzMultiplier;
    #ifdef OTR_DEBUG
        Serial.println("VIBRATE::longBuzz()");
    #endif
}

void VIBRATE::error() {
    digitalWrite(vibratePin, HIGH);
    status = 1;
    active = 1;
    count = 0;
    repeat = 1;
    buzzLength = errorDuration;
    buzzGap = errorGap;
    numRepeats = errorCount;
    
}
void VIBRATE::success() {
    digitalWrite(vibratePin, HIGH);
    status = 1;
    active = 1;
    count = 0;
    repeat = 0;
    buzzLength = duration;
}


void VIBRATE::tap() {
    digitalWrite(vibratePin, HIGH);
    status = 1;
    active = 1;
    count = 0;
    repeat = 0;
    buzzLength = duration / tapDivisor;
}

void VIBRATE::stop() {
    digitalWrite(vibratePin, LOW);
    status = 0;
    repeat = 0;
    active = 0;
    
}
void VIBRATE::toggle() {
     digitalWrite(vibratePin, !digitalRead(vibratePin));
     status = !status;
     active = !active;
}

void VIBRATE::start() {
    digitalWrite(vibratePin, HIGH);
    status = 1;
    active = 1;
    repeat = 1;
    buzzLength = duration;
    buzzGap = 0;
}

void VIBRATE::monitor() {
    if (active) {
        count = count + interval;
        if (repeat) {
            if (status) {
                if (count >= buzzLength) {
                    digitalWrite(vibratePin, LOW);
                    status = 0;
                    count = 0;
                    cycleCount++;
                    if (cycleCount >= numRepeats) {
                        repeat = 0;
                        cycleCount = 0;
                        stop();
                    }
                } 
            } else {
                    if (count >= buzzGap) {
                        digitalWrite(vibratePin, HIGH);
                        status = 1;
                        count = 0;
                    }
             }
        }  else {
            if (count >= buzzLength) {
                stop();
            }
        }
    }

}