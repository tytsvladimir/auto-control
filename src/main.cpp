#include "header.h"

bool photosensor_status;    // photosensor status
bool coil_status;           // coil status

unsigned long start_time = 0;
unsigned long finish_time = 0;
unsigned long duration_time = 0;
unsigned long hold_time = 1000;
bool last_condition = false;
bool current_condition;

char buf[50];

void setup()
{
    Serial.begin(9600);
    pinMode(photosensor_pin, INPUT_PULLUP);
    pinMode(coil_pin, INPUT_PULLUP);
    pinMode(open_pin, OUTPUT);
    pinMode(config_pin, INPUT_PULLUP);
    pinMode(close_pin, OUTPUT);
    pinMode(opened_led, OUTPUT);
    pinMode(closed_led, OUTPUT);

    digitalWrite(close_pin, 0);
    switchLed(!check());
}

void loop()
{
    if (check())
    {
        switchLed(0); //OPENED
        while (1)
        {
            if (!check())
            {
                closeCom();
                switchLed(1); //CLOSED
                break;
            }            
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool readPin(int pin)
{
    current_condition = digitalRead(pin);

    if (!current_condition && !last_condition)
    {
        start_time = millis();
        last_condition = true;
        while (1)
        {
            duration_time = millis() - start_time;
            Serial.println((String)duration_time);

            if (digitalRead(pin)) 
            {
                last_condition = false;
                return 1;
            }
            else
            {
                if (duration_time >= hold_time) break;
            }
        }
        return 0;
    }
    else if (!current_condition && last_condition);
    else if (current_condition && last_condition) last_condition = false;
    else return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

bool check(void)
{
    //if (1-0 or 0-1 or 0-0) retern 1 else 0
    photosensor_status = digitalRead(photosensor_pin);
    coil_status = digitalRead(coil_pin);
    ////////////////////////////////////////////////////////
    Serial.print("|___");
    Serial.print((String)photosensor_status);
    Serial.print("___");
    Serial.print((String)coil_status);
    Serial.println("___|");
    ////////////////////////////////////////////////////////
    if (photosensor_status && !coil_status)
    {
        bool resp = readPin(coil_pin);
        return !resp;
    }
    else if (!photosensor_status && !coil_status)
    {
        return 1;
    }


    if ((!photosensor_status && coil_status) || 
        (!photosensor_status && !coil_status)) return 1;
    else return 0;
}

void closeCom(void)
{
    // Close the barrier
    delay(delayBeforeClose);
    digitalWrite(close_pin, 1);
    delay(delayCloseSignal);
    digitalWrite(close_pin, 0);
}

void switchLed(bool st)
{
    // if st = 1 (close led - ON open led - OFF)
    // if st = 0 (close led - OFF open led - ON)
    digitalWrite(closed_led, st ? 1 : 0);
    digitalWrite(opened_led, st ? 0 : 1);
}
