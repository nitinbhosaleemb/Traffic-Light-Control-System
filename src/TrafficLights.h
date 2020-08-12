/*******************************************************************************
 **
 ** File Name: TrafficLights.h
 ** Project: Traffic Light Control and Management System using Open CV
 ** Creation Date:
 **
 *******************************************************************************/

#ifndef TRAFFICLIGHTS_H
#define TRAFFICLIGHTS_H

#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <pthread.h>
#include <iostream>
#include <bcm2835.h>

using namespace std;

#define LANE_NUM_MAX 4

//re-definition of raspberry pi pin for lane 1 LED configurations
#define LANE_1_PIN_GREEN  RPI_BPLUS_GPIO_J8_11
#define LANE_1_PIN_YELLOW RPI_BPLUS_GPIO_J8_13
#define LANE_1_PIN_RED    RPI_BPLUS_GPIO_J8_15

//re-definition of raspberry pi pin for lane 2 LED configurations
#define LANE_2_PIN_GREEN  RPI_BPLUS_GPIO_J8_16
#define LANE_2_PIN_YELLOW RPI_BPLUS_GPIO_J8_18
#define LANE_2_PIN_RED    RPI_BPLUS_GPIO_J8_22

//re-definition of raspberry pi pin for lane 3 LED configurations
#define LANE_3_PIN_GREEN  RPI_BPLUS_GPIO_J8_29
#define LANE_3_PIN_YELLOW RPI_BPLUS_GPIO_J8_31
#define LANE_3_PIN_RED    RPI_BPLUS_GPIO_J8_32

//re-definition of raspberry pi pin for lane 4 LED configurations
#define LANE_4_PIN_GREEN  RPI_BPLUS_GPIO_J8_33
#define LANE_4_PIN_YELLOW RPI_BPLUS_GPIO_J8_36
#define LANE_4_PIN_RED    RPI_BPLUS_GPIO_J8_37

#define TIMER_NO_TRAFFIC      (30000)              //30  seconds
#define TIMER_MILD_TRAFFIC    (TIMER_NO_TRAFFIC*2) //60  seconds
#define TIMER_HIGH_TRAFFIC    (TIMER_NO_TRAFFIC*3) //90  seconds
#define TIMER_MAX_TRAFFIC     (TIMER_NO_TRAFFIC*4) //120 seconds

typedef struct{
	int PIN_GREEN;
	int PIN_YELLOW;
	int PIN_RED;
}LED_PIN_NO;

enum State
{
	OFF,
	RED,
	RED_YELLOW,
	GREEN,
	YELLOW,
};

class TrafficLights
{

public:
	//param constructor to initialise the LED pins of a traffic signal
	TrafficLights(int greenLedPin, int yellowLedPin, int redLedPin)
	{
		m_greenLedPin  = greenLedPin;
		m_yellowLedPin = yellowLedPin;
		m_redLedPin    = redLedPin;
		m_thread = 0;

		pthread_mutex_init(&m_mutex, NULL);
		pthread_mutex_init(&m_mutex_green_light, NULL);
		pthread_mutex_init(&m_mutex_time_remain, NULL);

		// Set the pins to be an outputs
//		bcm2835_gpio_fsel(_greenLedPin, BCM2835_GPIO_FSEL_OUTP);
//		bcm2835_gpio_fsel(_yellowLedPin, BCM2835_GPIO_FSEL_OUTP);
//		bcm2835_gpio_fsel(_redLedPin, BCM2835_GPIO_FSEL_OUTP);
//
//		// Set all light to off
//		bcm2835_gpio_write(_greenLedPin, LOW2);
//		bcm2835_gpio_write(_yellowLedPin, LOW2);
//		bcm2835_gpio_write(_redLedPin, LOW2);
	}
	TrafficLights();
	TrafficLights(const TrafficLights& orig);
	virtual ~TrafficLights();
	
	int initGPIO(void);
	
	void setState(State signalState);
	//void setState(const std::string& state);
	
	State getState(void);
	
	void runAsThread(void);
	
	static void* runTrafficLightThread(void* self);
	
private:
	int m_greenLedPin;
	int m_yellowLedPin;
	int m_redLedPin;

	int m_yellowToGreenDelay = 1000;
	int m_yellowToRedDelay = 1000;
	
	int m_redTime = 10000;
	int m_greenTime = 10000;
	
	int m_timeUntilStateChange = 0;
	
	State m_currentState = OFF;
	
	pthread_t m_thread;
	pthread_mutex_t m_mutex;
	pthread_mutex_t m_mutex_green_light;
	pthread_mutex_t m_mutex_time_remain;
	
public:
	void setRedTime(int time) {
		m_redTime = time;
	}
	void setGreenTime(int time) {
		pthread_mutex_lock(&m_mutex);
		m_greenTime = time;
		pthread_mutex_unlock(&m_mutex);
	}
	void setYellowToGreenDelay(int time) {
		m_yellowToGreenDelay = time;
	}
	void setYellowToRedDelay(int time) {
		m_yellowToRedDelay = time;
	}
	
	int getRedTime() {return m_redTime;}
	int getGreenTime() {
		pthread_mutex_lock(&m_mutex_green_light);
		int greenTime = m_greenTime;
		pthread_mutex_unlock(&m_mutex_green_light);
		return greenTime;
	}
	int getYellowToGreenDelay() {return m_yellowToGreenDelay;}
	int getYellowToRedDelay() {return m_yellowToRedDelay;}


	int getMTimeUntilStateChange()  {
		pthread_mutex_lock(&m_mutex_time_remain);
		int timeUntilStateChange = m_timeUntilStateChange;
		pthread_mutex_unlock(&m_mutex_time_remain);
		return timeUntilStateChange;
	}

	void setMTimeUntilStateChange(int mTimeUntilStateChange = 0) {
		m_timeUntilStateChange = mTimeUntilStateChange;
	}
};

#endif /* TRAFFICLIGHTS_H */

