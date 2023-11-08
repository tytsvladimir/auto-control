#include "Arduino.h"

#define photosensor_pin 6   // coil pin
#define coil_pin 7          // photosensor pin
#define open_pin 9          // open by coil pin barrier
#define config_pin 8        //
#define close_pin 10        // close pin barrier
#define closed_led 11
#define opened_led 12
#define OPEN 1
#define CLOSE 0
#define delayBeforeClose 1000
#define durationCloseSignal 200
#define delayBeforeOpen 100
#define durationOpenSignal 200

bool readPhotosensor();
bool readCoil();
void control(int com);
void ledSwitch(int com);
