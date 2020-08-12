/*******************************************************************************
 **
 ** File Name: TrafficLights.cpp
 ** Project: Traffic Light Control and Management System using Open CV
 ** Creation Date:
 ** Author:
 **
 *******************************************************************************/

#include "TrafficLights.h"

TrafficLights::TrafficLights() {
}

TrafficLights::TrafficLights(const TrafficLights& orig) {
}

TrafficLights::~TrafficLights() {
	pthread_mutex_destroy(&m_mutex);
	pthread_mutex_destroy(&m_mutex_green_light);
	pthread_mutex_destroy(&m_mutex_time_remain);
}

/*!
 * @brief init gpio for bmc 2835 raspberry pi
 * @param void
 * @return success or error
 */
int TrafficLights::initGPIO(void)
{
	cout << "initGPIO" << endl;
	return (0/*bcm2835_init()*/);
}

/*!
 * @brief set state of traffic light signal
 * @param colorState - traffic signal color
 * @return void
 */
void TrafficLights::setState(State colorState)
{
	//commented code below which will work on raspberry pi

	pthread_mutex_lock(&m_mutex);
	m_currentState = colorState;
	switch (colorState)
	{
		case RED:
//			bcm2835_gpio_write(_redLedPin, HIGH2);
//			bcm2835_gpio_write(_yellowLedPin, LOW2);
//			bcm2835_gpio_write(_greenLedPin, LOW2);
			m_timeUntilStateChange = m_redTime;
			break;
		case RED_YELLOW:
//			bcm2835_gpio_write(_redLedPin, HIGH2);
//			bcm2835_gpio_write(_yellowLedPin, HIGH2);
//			bcm2835_gpio_write(_greenLedPin, LOW2);
			m_timeUntilStateChange = m_yellowToGreenDelay;
			break;
		case GREEN:
//			bcm2835_gpio_write(_redLedPin, LOW2);
//			bcm2835_gpio_write(_yellowLedPin, LOW2);
//			bcm2835_gpio_write(_greenLedPin, HIGH2);
			m_timeUntilStateChange = m_greenTime;
			break;
		case YELLOW:
//			bcm2835_gpio_write(_redLedPin, LOW2);
//			bcm2835_gpio_write(_yellowLedPin, HIGH2);
//			bcm2835_gpio_write(_greenLedPin, LOW2);
			m_timeUntilStateChange = m_yellowToRedDelay;
			break;
		case OFF:
//			bcm2835_gpio_write(_redLedPin, LOW2);
//			bcm2835_gpio_write(_yellowLedPin, LOW2);
//			bcm2835_gpio_write(_greenLedPin, LOW2);
			m_timeUntilStateChange = m_yellowToRedDelay;
			break;
	}
	pthread_mutex_unlock(&m_mutex);
}

/*!
 * @brief get current state of traffic light signal
 * @param void
 * @return traffic signal state color
 */
State TrafficLights::getState(void)
{
	return m_currentState;
}

/*!
 * @brief inline sleep function
 * @param milliseconds
 * @return void
 */
inline void mySleep(int milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

/*!
 * @brief spawn a thread to controll traffic light
 * @param void
 * @return void
 */
void TrafficLights::runAsThread(void)
{
	pthread_create(&m_thread, NULL, &TrafficLights::runTrafficLightThread, this);
	return;
}

/*!
 * @brief function handler thread to controll traffic light
 * @param void * - parameters needed to access static function
 * @return void
 */
void* TrafficLights::runTrafficLightThread(void* self)
{
	TrafficLights* pTrafficLights = static_cast<TrafficLights *>(self);
	while (1)
	{
		int updateTime = 100;
		// Get thread Id of calling thread
		pthread_t thId = pthread_self();
		//std::cout << "Thread Id from thread function : " << thId << std::endl;
		//std::cout<<"pTrafficLights->m_timeUntilStateChange -- "<< pTrafficLights->m_timeUntilStateChange << std::endl;
		//timer expires, and current state switches from GREEN to YELLOW to RED
		if ((pTrafficLights->m_timeUntilStateChange) <= 0)
		{
			// Change to next state
			switch ((pTrafficLights->m_currentState))
			{
				case RED:
					//setState(RED_YELLOW);
					break;
				case RED_YELLOW:
					//setState(GREEN);
					break;
				case GREEN:
					//reset green signal time to default
					pTrafficLights->setGreenTime(TIMER_NO_TRAFFIC);
					pTrafficLights->setState(YELLOW);
					break;
				case YELLOW:
					pTrafficLights->setState(RED);
					break;
				case OFF:
					(pTrafficLights->m_timeUntilStateChange) = updateTime;
					break;
			}
		}
		else
		{
			(pTrafficLights->m_timeUntilStateChange) -= updateTime;
			mySleep(100);
		}
	}
	// exit the current thread
	pthread_exit(NULL);
}
