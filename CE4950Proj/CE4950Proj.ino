#include <TimerOne.h>

/**
 * Definitions 
 */
#define TRANSMISSION_RATE 1000 //transmission rate in bits/sec
/*
 * Pin declarations
 */
#define RX_PIN 2 //only pins 2 and 3 can trigger interrupts
#define TX_PIN 3
#define LED_IDLE 13
#define LED_BUSY 12
#define LED_COLL 11
/*
 * State Definitions for the channel monitor
 */
enum CHANNEL_MON_STATES {MON_BUSY, MON_IDLE, MON_COLL}; //idle is already defined somewhere else, so I just did this
enum CHANNEL_MON_STATES monitor_state;
/*
 * Other monitor definitions
 */
#define MON_BUS_TIME_TO_IDLE 8.3
#define MON_BUS_COL_DET_TIME 0.52
TimerOne timer;
float timerTime = 0.0;
void setup() {
    Serial.begin(250000);
    Serial.println("Connected to node");
    timer.initialize();
    //Set State to IDLE
    monitor_state = MON_IDLE;
    setLED(LED_IDLE);
    //set interrupt on RX_PIN on a rising edge
    attachInterrupt(digitalPinToInterrupt(RX_PIN), MON_RX_LOW_TO_HIGH, RISING);
    //setup LED out pins
    pinMode(RX_PIN, INPUT_PULLUP);
    pinMode(TX_PIN, OUTPUT);
    pinMode(LED_IDLE, OUTPUT);
    pinMode(LED_BUSY, OUTPUT);
    pinMode(LED_COLL, OUTPUT);
    interrupts();
}

void loop() {
  
}


/**
 * ISR for the RX pin going low to high
 */
void MON_RX_LOW_TO_HIGH(){
    //set interrupt on RX_PIN from high to low
    attachInterrupt(digitalPinToInterrupt(RX_PIN), MON_RX_HIGH_TO_LOW, FALLING);
    //schedule timer interrupt to occur 0.6ms
    setTimer(MON_BUS_COL_DET_TIME);
    //set busy state
    monitor_state = MON_BUSY;
    setLED(LED_BUSY);
}

/**
 * ISR for the RX pin going high to low
 */
void MON_RX_HIGH_TO_LOW(){
    //set interrupt on RX_PIN from low to high
    attachInterrupt(digitalPinToInterrupt(RX_PIN), MON_RX_LOW_TO_HIGH, RISING);
    //schedule timer interrupt to occur 7.7ms
    setTimer(MON_BUS_TIME_TO_IDLE);
    //set busy state
    monitor_state = MON_BUSY;
    setLED(LED_BUSY);
}

/**
 * Sets the timer to a specified time
 */
void setTimer(double millisec){
    timerTime = millisec;
    timer.attachInterrupt(timerISR, millisec*1000.0); //library uses us not ms
}

/**
 * Timer ISR, fires off when timer expires
 */
void timerISR(){
  if(timerTime == MON_BUS_COL_DET_TIME){
    if(digitalRead(RX_PIN) == HIGH){
      monitor_state = MON_COLL;
      setLED(LED_COLL);
      attachInterrupt(digitalPinToInterrupt(RX_PIN), MON_RX_LOW_TO_HIGH, RISING);
      setTimer(MON_BUS_TIME_TO_IDLE);
    }
  }
  if(timerTime == MON_BUS_TIME_TO_IDLE){
    if(digitalRead(RX_PIN) == LOW){
      monitor_state = MON_IDLE;
      setLED(LED_IDLE);
      attachInterrupt(digitalPinToInterrupt(RX_PIN), MON_RX_HIGH_TO_LOW, FALLING);
      setTimer(MON_BUS_COL_DET_TIME);
    }
   }
}

/**
 * Sets the LED indicator
 */
void setLED(int led){
     digitalWrite(led, HIGH);
     
     if(led != LED_IDLE){
        digitalWrite(LED_IDLE, LOW);
     }
     if(led != LED_COLL){
        digitalWrite(LED_COLL, LOW);
     }
     if(led != LED_BUSY){
        digitalWrite(LED_BUSY, LOW);
     }
}


