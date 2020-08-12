/*******************************************************************************
 **
 ** File Name: main.h
 ** Project: Traffic Light Control and Management System using Open CV
 ** Creation Date:
 **
 *******************************************************************************/
#ifndef MAIN_H_
#define MAIN_H_

#include <iostream>
#include <exception>
#include <sys/prctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <sys/syscall.h>
#include <memory>
#include<ctime>
#include <time.h>

#include "OpenCV.h"
#include "TrafficLights.h"

using namespace std;

std::string getSystemTimeNow(void);

typedef enum {
	ERROR = -1,
	SUCCESS,
}ErrorCode;

#endif /* MAIN_H_ */
