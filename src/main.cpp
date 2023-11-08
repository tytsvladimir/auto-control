#include "header.h"

bool photosensor_status;    // photosensor status
bool coil_status;           // coil status
bool openedOrClosed;        // 0 - closed, 1 - opened

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
    check();
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
                return 0;
            }
            else
            {
                if (duration_time >= hold_time) break;
            }
        }
        return 1;
    }
    else if (!current_condition && last_condition);
    else if (current_condition && last_condition) last_condition = false;
    else return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 1. Функция на выезд (redPin уже реализована) (убеждаемся что сигнал длится непрерывно в течении указанного тайминга > открываем)
// 2. Функция на въезд (реакция на фотик > флаг "открыто" > ожидание состояния датчиков 0-0 > закрытие)
// 3. Функция
int check(void)
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
    if ((photosensor_status && !coil_status) && openedOrClosed) //0-1
    {
        if (int n = readPin(coil_pin))
        {
            control(open_pin, delayBeforeOpen, duration_time);
            switchLed(1);
            openedOrClosed = 1;
        } 
    }
    else if (!photosensor_status && coil_status)                //1-0
    {
        if (readPin(photosensor_pin))
        {
            switchLed(1);
            openedOrClosed = 1;
        }
    }
    else if (!photosensor_status && !coil_status)               //1-1
    {
        while (1)
        {
            if (photosensor_status && coil_status)
            {
                control(close_pin, delayBeforeClose, durationCloseSignal);
                switchLed(0);
                openedOrClosed = 0;
                break;
            }
        }
    }
}

void control(int pin, int delay_, int duration)
{
    // Control the output
    delay(delay_);
    digitalWrite(pin, 1);
    delay(duration);
    digitalWrite(pin, 0);
}

void switchLed(bool st)
{
    // if st = 0 (close led - ON open led - OFF)
    // if st = 1 (close led - OFF open led - ON)
    digitalWrite(closed_led, st ? 0 : 1);
    digitalWrite(opened_led, st ? 1 : 0);
}
