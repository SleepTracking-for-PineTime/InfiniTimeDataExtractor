#include "data_extractor.hpp"

DataExtractor::DataExtractor(std::string file_name, int data_write_loop_delay = 5)
: file_name(file_name),
  data_write_loop_delay(data_write_loop_delay),
  latest_motion_values({0, 0, 0}),
  latest_heartrate(0),
  latest_battery(0),
  latest_sleep(0)
{
}

DataExtractor::~DataExtractor()
{
}

int DataExtractor::Init()
{
    InitCSVHandler();

    if (1 == InitPineTimeCommunicator())
        return 1;
    
    return 0;
}

void DataExtractor::CommandLoop()
{
    command_loop_running = true;
    while (command_loop_running)
    {
        std::string command;
        std::cin >> command;

        if ("quit" == command)
            command_loop_running = false;
        else
            std::cout << "Invalid Command. Write 'quit' to exit program.\n>> ";
    }

    EndDataExtraction();
}


void DataExtractor::StartDataExtraction()
{
    command_loop = std::thread(&DataExtractor::CommandLoop, this);
    // data_write_loop = std::thread(&DataExtractor::DataWriteLoop, this);
    command_loop.join();
    // data_write_loop.join();
}

void DataExtractor::EndDataExtraction()
{
    if (communicator->IsPineTimeConnected())
        communicator->DisconnectFromPineTime();

    command_loop_running = false;
}

void DataExtractor::InitCSVHandler()
{
    csv_handler.reset(new CSVHandler(file_name));
}

void DataExtractor::WriteData()
{
    std::time_t current_time = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    std::vector<std::string> data_line {
        std::to_string(current_time),
        std::to_string(latest_heartrate),
        std::to_string(latest_motion_values[0]),
        std::to_string(latest_motion_values[1]),
        std::to_string(latest_motion_values[2]),
        std::to_string(latest_battery),
        std::to_string(latest_sleep)
    };
    csv_handler->AppendCSVLine(data_line);
}

int DataExtractor::InitPineTimeCommunicator()
{
    communicator.reset(new PineTimeCommunicator);

    int ret = communicator->ConnectToPineTime();

    if (1 == ret)
      return 1;

    communicator->SetCallbacks(
        [&](std::array<int16_t, 3> motion_values) {
          latest_motion_values = motion_values;
          WriteData();
        },
        [&](uint8_t heartrate) {
          latest_heartrate = heartrate;
          WriteData();
        },
        [&](uint8_t battery) {
          latest_battery = battery;
          WriteData();
        },
        [&](uint8_t sleep) {
          latest_sleep = sleep;
          WriteData();
        });

    communicator->EnableNotifications();

    std::cout << "Connected to PineTime!\nData is now being written (to exit write 'quit')...\n>> ";

    return 0;
}
