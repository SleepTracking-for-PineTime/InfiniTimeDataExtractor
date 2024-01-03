#include "pinetime_communicator.hpp"
#include "csv_handler.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char** argv)
{
   PineTimeCommunicator communicator;
   if (1 == communicator.ConnectToPineTime())
      return 1;

   CSVHandler csv;
   std::vector<std::string> first_line {"Time", "Heartrate", "MotionX", "MotionY", "MotionZ", "Battery Level"};
   csv.AppendCSVLine("test.csv", first_line);

   for (size_t i = 0; i < 5; i++)
   {
      std::cout << "Heartrate: " << unsigned(communicator.GetHeartRateValue()) << "\n";

      std::array<int16_t, 3> motion_values = communicator.GetMotionValues();
      std::cout << "X Value: " << motion_values[0] <<
      " | Y Value: " << motion_values[1] <<
      " | Z Value: " << motion_values[2] << "\n";

      std::cout << "Battery Level: " << unsigned(communicator.GetBatteryLevelValue()) << "\n\n";

      if (4 == i)
         break;
      
      std::this_thread::sleep_for(std::chrono::seconds(1));
   }

   communicator.DisconnectFromPineTime();

   return 0;
}