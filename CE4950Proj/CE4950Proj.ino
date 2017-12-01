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
#define MON_BUS_TIME_TO_IDLE 8.01
#define MON_BUS_COL_DET_TIME 0.52
TimerOne timer;
void setup() {
    Serial.begin(9600);
    Serial.println("Connected to node");
    timer.initialize();
    //Set State to IDLE
    monitor_state = MON_IDLE;
    //set interrupt on RX_PIN on a rising edge
    attachInterrupt(digitalPinToInterrupt(RX_PIN), MON_RX_LOW_TO_HIGH, RISING);
    //setup LED out pins
    pinMode(LED_IDLE, OUTPUT);
    pinMode(LED_BUSY, OUTPUT);
    pinMode(LED_COLL, OUTPUT);
}

void loop() {
  switch (monitor_state){
    case MON_IDLE:
      Serial.println("LINE IDLE");
    case MON_BUSY:
      Serial.println("LINE BUSY");
    case MON_COLL:
      Serial.println("COLLISION");
  }

}


/**
 * ISR for the RX pin going low to high
 */
void MON_RX_LOW_TO_HIGH(){
    //set busy state
    monitor_state = MON_BUSY;
    //set interrupt on RX_PIN from high to low
    attachInterrupt(digitalPinToInterrupt(RX_PIN), MON_RX_HIGH_TO_LOW, FALLING);
    //schedule timer interrupt to occur 0.6ms
    setTimer(MON_BUS_COL_DET_TIME);
}

/**
 * ISR for the RX pin going high to low
 */
void MON_RX_HIGH_TO_LOW(){
    //set busy state
    monitor_state = MON_BUSY;
    //set interrupt on RX_PIN from low to high
    attachInterrupt(digitalPinToInterrupt(RX_PIN), MON_RX_LOW_TO_HIGH, RISING);
    //schedule timer interrupt to occur 7.7ms
    setTimer(MON_BUS_TIME_TO_IDLE);
}


void setTimer(float millisec){
    timer.attachInterrupt(timerISR, millisec*1000.0); //library uses us not ms
}

void timerISR(){
    
}


