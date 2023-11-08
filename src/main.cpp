#include "header.h"

bool photosensor_status;    // photosensor status
bool coil_status;           // coil status
bool openedOrClosed;        // 0 - closed, 1 - opened

unsigned long start_time = 0;
unsigned long finish_time = 0;
unsigned long duration_time = 0;
unsigned long hold_time = 1000;
bool last_condition_photosensor = false;
bool current_condition_photosensor;
bool last_condition_coil = false;
bool current_condition_coil;

void setup()
{
    pinMode(photosensor_pin, INPUT_PULLUP);
    pinMode(coil_pin, INPUT_PULLUP);
    pinMode(open_pin, OUTPUT);
    pinMode(config_pin, INPUT_PULLUP);
    pinMode(close_pin, OUTPUT);
    pinMode(opened_led, OUTPUT);
    pinMode(closed_led, OUTPUT);
    digitalWrite(close_pin, 0);
}

void loop()
{
    //if (1-0 or 0-1 or 0-0) retern 1 else 0
    photosensor_status = digitalRead(photosensor_pin);
    coil_status = digitalRead(coil_pin);
    if ((photosensor_status && !coil_status) && !openedOrClosed) // EXIT low coil signal
    {
        if (!readCoil())
        {
            control(OPEN);
            openedOrClosed = 1;
        } 
    }
    else if ((photosensor_status && coil_status) && openedOrClosed) // EXIT coil signal is gone
    {
        if (readCoil())
        {
            control(CLOSE);
            openedOrClosed = 0;
        }
    }
    else if ((!photosensor_status && coil_status) && !openedOrClosed) // ENTRY low photosens signal
    {
        if (!readPhotosensor())
        {
            ledSwitch(1);
            openedOrClosed = 1;
        }
    }
    else if ((photosensor_status && coil_status) && openedOrClosed) // ENTRY photosens signal is gone
    {
        if (readPhotosensor())
        {
            control(CLOSE);
            openedOrClosed = 0;
        }
    }
    else if (!photosensor_status && !coil_status)                   // BOTH signals is low
    {
        while (1)
        {
            if (digitalRead(photosensor_pin) && digitalRead(coil_pin)) // BOTH signals is HIGH => CLOSE
            {
                control(CLOSE);
                openedOrClosed = 0;
                break;
            }
        }
    }
}

bool readPhotosensor()
{
    current_condition_photosensor = digitalRead(photosensor_pin);
    if (!current_condition_photosensor && !last_condition_photosensor)
    {
        start_time = millis();
        last_condition_photosensor = true;
        while (1)
        {
            duration_time = millis() - start_time;
            Serial.println((String)duration_time);

            if (digitalRead(photosensor_pin)) 
            {
                last_condition_photosensor = false;
                return 1;
            }
            else
            {
                if (duration_time >= hold_time) break;
            }
        }
        last_condition_photosensor = false;
        return 0;
    }
    else if (!current_condition_photosensor && last_condition_photosensor);
    else if (current_condition_photosensor && last_condition_photosensor) last_condition_photosensor = false;
    else return 1;
}

bool readCoil()
{
    current_condition_coil = digitalRead(coil_pin);
    if (!current_condition_coil && !last_condition_coil)
    {
        start_time = millis();
        last_condition_coil = true;
        while (1)
        {
            duration_time = millis() - start_time;
            Serial.println((String)duration_time);

            if (digitalRead(coil_pin)) 
            {
                last_condition_coil = false;
                return 1;
            }
            else
            {
                if (duration_time >= hold_time) break;
            }
        }
        last_condition_coil = false;
        return 0;
    }
    else if (!current_condition_coil && last_condition_coil);
    else if (current_condition_coil && last_condition_coil) last_condition_coil = false;
    else return 1;
}

void control(int com)
{
    // Control the output
    delay(com ? delayBeforeOpen : delayBeforeClose);
    digitalWrite(com ? open_pin : close_pin, 1);
    delay(com ? durationOpenSignal : durationCloseSignal);
    digitalWrite(com ? open_pin : close_pin, 0);
    digitalWrite(closed_led, com ? 0 : 1);
    digitalWrite(opened_led, com ? 1 : 0);    
}

void ledSwitch(int com)
    {
        digitalWrite(closed_led, com ? 0 : 1);
        digitalWrite(opened_led, com ? 1 : 0);
    }
