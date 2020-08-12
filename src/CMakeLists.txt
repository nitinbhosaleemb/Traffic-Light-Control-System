add_executable(traffic_light_control_system Blob.cpp
Blob.h
main.cpp
main.h
OpenCV.cpp
OpenCV.h
TrafficLights.cpp
TrafficLights.h
#./Common/Utility/Utility.cpp
#./Common/Utility/Utility.h
#./Common/Utility/Utility_test.cpp
)

target_link_libraries( traffic_light_control_system -lpthread -lbcm2835 -lwiringPi -lcrypt ${OpenCV_LIBRARIES})
     
install (TARGETS traffic_light_control_system DESTINATION bin)
