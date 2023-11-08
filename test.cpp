
    if (check())
    {
        switchLed(0); //OPENED
        while (1)
        {
            if (!check())
            {
                control(open_pin, delayBeforeOpen, durationOpenSignal);
                switchLed(1); //CLOSED
                break;
            }            
        }
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////
int readPin(int pin)
{
    current_condition = digitalRead(pin);
    Serial.print((String)last_condition);
    Serial.print("-");
    Serial.print((String)current_condition);
    Serial.print("__________");

    // если кнопка была отпущена, но сейчас нажата
    if (!last_condition && current_condition == 0)
    {
        start_time = millis(); // фиксируем время старта
        last_condition = true;
        return 1;
    }
    // если кнопка была нажата, но сейчас отпущена
    else if (last_condition && current_condition == 0)
    {
        finish_time = millis(); // фиксируем врем финиша
        // если время удержания была соответствующей,
        if (finish_time - start_time >= hold_time)
        {
            last_condition = false;
            return 0;
        }
        else
        {
            last_condition = false;
            return 1;
        }
    }
    return 1; // (last_condition = 0, current_condition = 0) or (last_condition = 1, current_condition = 1)
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
int readPin(int pin)
{
    //
    if (digitalRead(pin) == 0)
    {
        // Препятствие есть
        if (obstacle_time == 0) obstacle_time = millis();  // Запоминаем время начала
    }
    else
    {
        // Препятствия нет
        if (obstacle_time > 0)
        {
            no_obstacle_time = millis();
            obstacle_duration = no_obstacle_time - obstacle_time;
            
            if (obstacle_duration >= 1000)
            {
                obstacle_time = 0;  // Сбрасываем время начала
                return 0;
            }
            else
            {
                obstacle_time = 0;  // Сбрасываем время начала
                return 1;
            }
        }
    }
}

#define BUTTON_PIN 2

int RLED = 9;
int GLED = 10;
int BLED = 11;

int bounceTime = 10;          // задержка для подавления дребезга
int holdTime = 500;           // время, в течение которого нажатие можно считать удержанием кнопки
int doubleTime = 500;         // время, в течение которого нажатия можно считать двойным
int holdTimeSum = 0;  
int i = 0;

boolean lastReading = false;  // флаг предыдущего состояния кнопки
boolean buttonSingle = false; // флаг состояния "краткое нажатие"
boolean buttonMulti = false;  // флаг состояния "двойное нажатие"
boolean buttonHold = false;   // флаг состояния "долгое нажатие"

long onTime = 0;              // переменная обработки временного интервала
long lastSwitchTime = 0;      // переменная времени предыдущего переключения состояния

void setup(){
  Serial.begin(9600);
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(BLED, OUTPUT);    
}

void loop(){
  boolean reading = digitalRead(BUTTON_PIN);

  if (reading && !lastReading){ // если кнопка нажата, но до этого не была нажата, фиксируем время старта
    onTime = millis();
  }
  
  if (reading && lastReading){ // если кнопка нажата, и до этого была нажата, сверяем разницу во времени
    if ((millis() - onTime) > holdTime){ // если время соответсвует времени удержания, значит считаем что кнопка была удержана
      holdTimeSum = (millis() - onTime);
      buttonHold = true;
    }
  }
  
  if (!reading && lastReading){
    if (((millis() - onTime) > bounceTime) && !buttonHold){
      if ((millis() - lastSwitchTime) >= doubleTime){
        lastSwitchTime = millis();
        buttonSingle = true;
        i=1;
      } else {
        i++;
        lastSwitchTime = millis();
        buttonSingle = false;
        buttonMulti = true;
      }
    }
    if (buttonHold){
      isButtonHold( holdTimeSum );
    }
  }
  
  lastReading = reading;
  
  if (buttonSingle && (millis() - lastSwitchTime) > doubleTime){
    isButtonSingle();
  }
  if (buttonMulti && (millis() - lastSwitchTime) > doubleTime){
    isButtonMulti(i);
  }
}

void isButtonSingle(){
    buttonMulti = false;
    buttonSingle = false;
    rgbRand();
}
void isButtonMulti( int count ){
    buttonSingle = false;
    buttonMulti = false;
    rgbMulti(count);
}

void isButtonHold( int count ){
    buttonHold = false;
    buttonMulti = false;
    if( count <= 5000 ){
      rgbShine();
    } else {
      rgbOff();
    }
}

void rgbMulti( int count){
  int j;
  Serial.println("MULTI");
  Serial.println(count);
  for(j=1; j <= count; j++){
    analogWrite(RLED, 127);
    analogWrite(GLED, 127);
    analogWrite(BLED, 127);
    delay(500);
    analogWrite(RLED, 0);
    analogWrite(GLED, 0);
    analogWrite(BLED, 0);
    delay(500);
  }
}

void rgbRand(){
    Serial.println("RAND");
    analogWrite(RLED, random(127));
    analogWrite(GLED, random(127));
    analogWrite(BLED, random(127));  
}

void rgbShine(){
  int j;
  Serial.println("SHINE");
  for(j=0;j<=255;j++){
    analogWrite(RLED, random(0,j));
    analogWrite(GLED, random(0,j));
    analogWrite(BLED, random(0,j));
    delay(5);
  }
  for(j=255;j>=0;j--){
    analogWrite(RLED, random(0,j));
    analogWrite(GLED, random(0,j));
    analogWrite(BLED, random(0,j));
    delay(5);
  }  
}

void rgbOff(){
    Serial.println("OFF");
    analogWrite(RLED, 0);
    analogWrite(GLED, 0);
    analogWrite(BLED, 0);  
}
