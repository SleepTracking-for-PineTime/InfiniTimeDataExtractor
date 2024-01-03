#include "pinetime_communicator.hpp"

#include <simpleble/SimpleBLE.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

int main(int argc, char** argv)
{
   PineTimeCommunicator communicator;
   if (1 == communicator.ConnectToPineTime())
      return 1;

   for (size_t i = 0; i < 5; i++)
   {
      std::cout << "Heartrate: " << unsigned(communicator.GetHeartRateValue()) << "\n";

      std::array<int16_t, 3> motion_values = communicator.GetMotionValues();
      std::cout << "X Value: " << motion_values[0] <<
      " | Y Value: " << motion_values[1] <<
      " | Z Value: " << motion_values[2] << "\n\n";

      if (4 == i)
         break;
      
      std::this_thread::sleep_for(std::chrono::seconds(1));
   }

   communicator.DisconnectFromPineTime();

   return 0;
}