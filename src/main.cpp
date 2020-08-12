/*******************************************************************************
 **
 ** File Name: main.cpp
 ** Project: Traffic Light Control and Management System using Open CV
 ** Creation Date:
 ** Author:
 **
 *******************************************************************************/

#include "main.h"

// Initialize led pins for 3 traffic lights per lane
LED_PIN_NO ledPinNum[LANE_NUM_MAX] = {
		{LANE_1_PIN_GREEN, LANE_1_PIN_YELLOW, LANE_1_PIN_RED},
		{LANE_2_PIN_GREEN, LANE_2_PIN_YELLOW, LANE_2_PIN_RED},
		{LANE_3_PIN_GREEN, LANE_3_PIN_YELLOW, LANE_3_PIN_RED},
		{LANE_4_PIN_GREEN, LANE_4_PIN_YELLOW, LANE_4_PIN_RED}
};

// Initialize video camera string array per lane with local videos for this demo
//string Video_Device[LANE_NUM_MAX]
vector<string> Video_Capture_Device 	=   {	"../../src/test1.mp4",
												"../../src/test2.mp4",
												"../../src/test3.mp4",
												"../../src/test4.mp4"
											};

//TODO : future use
//IP camera URLs
//vector<string> Video_Capture_Device = {
//		"rtsp://192.168.1.2/media.smp",
//		"rtsp://192.168.1.3/media.smp"
//};

//TODO : future use
//USB Camera
//vector<int> Video_Capture_Device = { 0, 1 };

/*
 * @brief main traffic light controller application entry
 *
 * @param argc - argument count
 * @param argv - argument vector
 * @returns - success or error
 */
int main( int argc, char* argv[] )
{
	std::vector<TrafficLights> trafficLights;
	std::vector<OpenCV> openCVCamLane;
	int carCountPerLane[LANE_NUM_MAX];
	int laneCamNum = 0;

	std::cout << "Traffic Light Control and Management System :: " << getSystemTimeNow() <<  std::endl;

	std::cout << "Setting up traffic lights output" << std::endl;
	//initialize traffic signal class for four lanes
	for(int laneNum = 0; laneNum < LANE_NUM_MAX; laneNum++)
	{
		//initialize traffic signal class constructor for 3 LED pins per lane
		trafficLights.push_back(TrafficLights(ledPinNum[laneNum].PIN_GREEN, ledPinNum[laneNum].PIN_YELLOW, ledPinNum[laneNum].PIN_RED));
		//init raspberry pi gpio
		if (trafficLights[laneNum].initGPIO() != 0)
		{
			std::cout << "Init GPIO failed" << std::endl;
			return ERROR;
		}

		std::cout << "Initializing lights to Red" << std::endl;
		//Init traffic light per lane to RED
		trafficLights[laneNum].setState(RED);

		//run thread for each traffic light lane
		trafficLights[laneNum].runAsThread();
	}

	std::cout << "Setting up four traffic lane camera" << std::endl;
	//cv::setNumThreads(8);
	//initialize opencv lane camera class for four lanes
	for(laneCamNum = 0; laneCamNum < LANE_NUM_MAX; laneCamNum++)
	{
		std::cout << "Initializing each camera lane :: " << Video_Capture_Device[laneCamNum] << endl;
		//initialize opencv lane camera class constructor for 4 lanes
		openCVCamLane.push_back(OpenCV(Video_Capture_Device[laneCamNum]));

		//run thread for each lane camera
		openCVCamLane[laneCamNum].runAsThread(Video_Capture_Device[laneCamNum]);
	}

	//loop for traffic light controller to switch as per traffic per lane
	while (1)
	{
		try
		{
			//loop 4 lane camera's to get the vehicle count per lane
			for(laneCamNum = 0; laneCamNum < LANE_NUM_MAX; laneCamNum++)
			{
				//get vehicle count from each lane
				carCountPerLane[laneCamNum] = openCVCamLane[laneCamNum].getvehicleCount();
			}

			//Start traffic control system with lane 0
			for(laneCamNum = 0; laneCamNum < LANE_NUM_MAX; laneCamNum++)
			{
				//check if lane has some vehicles
				if(carCountPerLane[laneCamNum] > 0)
				{
					//Init traffic light to green at start
					if((carCountPerLane[laneCamNum] > 30) && (carCountPerLane[laneCamNum] <= 40))
					{
						//Traffic more than camera area covered i.e. between 75% and more than 100%
						trafficLights[laneCamNum].setGreenTime(TIMER_MAX_TRAFFIC);
					}
					else if((carCountPerLane[laneCamNum] > 20) && (carCountPerLane[laneCamNum] <= 30))
					{
						//Traffic camera area covered between 50% to 75%
						trafficLights[laneCamNum].setGreenTime(TIMER_HIGH_TRAFFIC);
					}
					else if((carCountPerLane[laneCamNum] > 10) && (carCountPerLane[laneCamNum] <= 20))
					{
						//Traffic camera area covered between 25% to 50%
						trafficLights[laneCamNum].setGreenTime(TIMER_MILD_TRAFFIC);
					}
					else
					{
						//Traffic camera area covered between 0% to 25%
						if((carCountPerLane[laneCamNum] > 0) && (carCountPerLane[laneCamNum] <= 10))
						{
							trafficLights[laneCamNum].setGreenTime(TIMER_NO_TRAFFIC);
						}
					}
				}
				//Start traffic control system with lane 0 signal to go green signal
				trafficLights[laneCamNum].setState(GREEN);

				//wait while the green signal changes to red for that traffic signal lane
				while(trafficLights[laneCamNum].getMTimeUntilStateChange() > 0)
				{
					//std::cout<<"######## carCountPerLane[laneCamNum]::" <<trafficLights[laneCamNum].getMTimeUntilStateChange()<< std::endl;
					continue;
				}

				//let the light state change from Green to Yello to Red
				//for respective traffic signal which takes 1 second
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
		catch(std::exception& ex)
		{
			std::cout << getSystemTimeNow() << ": " << "Traffic Light Management Process : PID : " << getpid() << " : " << ex.what() << std::endl;
		}
		catch(...)
		{
			std::cout << getSystemTimeNow() << ": " << "Traffic Light Management Process : unknown exception : PID : " << getpid() << std::endl;
		}
	}

	return SUCCESS;
}

/*!
 * @brief get current system time
 *
 * @param void
 * @return string with date and time format as "Month DAY HH:MM:SS YEAR"
 *         i.e. ex: "Jul 23 14:58:27 2020"
 */
std::string getSystemTimeNow(void)
{
  //get the current time from the clock
  time_t my_time = time(NULL);

  // ctime() used to give the present time
  std::string fmtTime = ctime(&my_time);

  return fmtTime;
}

