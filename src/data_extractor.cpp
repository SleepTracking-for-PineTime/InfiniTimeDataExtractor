#include "pinetime_communicator.hpp"
#include "csv_handler.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

void DataWriteLoop(CSVHandler &csv, PineTimeCommunicator &communicator, bool &can_run, std::string file_name, int loop_delay)
{
    auto time = std::chrono::system_clock::now();
    while (can_run)
    {
        std::time_t current_time = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
        std::array<int16_t, 3> motion_values = communicator.GetMotionValues();

        std::vector<std::string> data_line {
            std::to_string(current_time),
            std::to_string(communicator.GetHeartRateValue()),
            std::to_string(motion_values[0]),
            std::to_string(motion_values[1]),
            std::to_string(motion_values[2]),
            std::to_string(communicator.GetBatteryLevelValue())
        };
        csv.AppendCSVLine(file_name, data_line);
        std::this_thread::sleep_for(std::chrono::seconds(loop_delay));
    }
}

int main(int argc, char** argv)
{
    std::string file_name = "data.csv";
    if (argc > 1)
        file_name = argv[1];

    int data_write_loop_delay = 1;
    if (argc > 2)
        data_write_loop_delay = argv[2][0] - '0';

    PineTimeCommunicator communicator;
    if (1 == communicator.ConnectToPineTime())
      return 1;
    std::cout << "Connected to PineTime!\nData is now being written (to exit write 'quit')...\n>> ";

    CSVHandler csv;
    std::vector<std::string> first_line {"Time", "Heartrate", "MotionX", "MotionY", "MotionZ", "Battery Level"};
    csv.AppendCSVLine(file_name, first_line);
    
    bool can_run = true;
    std::thread data_write_loop(DataWriteLoop, std::ref(csv), std::ref(communicator), std::ref(can_run), file_name, data_write_loop_delay);

    while (true)
    {
        std::string command;
        std::cin >> command;

        if ("quit" == command)
            break;
        else
            std::cout << "Invalid Command. Write 'quit' to exit program.\n>> ";
    }

    can_run = false;
    data_write_loop.join();

    communicator.DisconnectFromPineTime();

    return 0;
}
