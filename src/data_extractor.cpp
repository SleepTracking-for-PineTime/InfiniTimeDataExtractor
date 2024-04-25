#include "data_extractor.hpp"

DataExtractor::DataExtractor(std::string file_name, int data_write_loop_delay = 5)
: file_name(file_name), data_write_loop_delay(data_write_loop_delay)
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
    csv_handler.reset(new CSVHandler);
}

void DataExtractor::WriteMotion(std::array<int16_t, 3> motion_values)
{
    std::time_t current_time = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    std::vector<std::string> data_line {
        std::to_string(current_time),
        "",
        std::to_string(motion_values[0]),
        std::to_string(motion_values[1]),
        std::to_string(motion_values[2]),
        "",
        ""
    };
    csv_handler->AppendCSVLine(file_name, data_line);
}

void DataExtractor::WriteHeartrate(uint8_t heartrate)
{
    std::time_t current_time = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    std::vector<std::string> data_line {
        std::to_string(current_time),
        std::to_string(heartrate),
        "",
        "",
        "",
        "",
        ""
    };
    csv_handler->AppendCSVLine(file_name, data_line);
}

void DataExtractor::WriteBattery(uint8_t battery)
{
    std::time_t current_time = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    std::vector<std::string> data_line {
        std::to_string(current_time),
        "",
        "",
        "",
        "",
        std::to_string(battery),
        ""
    };
    csv_handler->AppendCSVLine(file_name, data_line);
}

void DataExtractor::WriteSleep(uint8_t sleep)
{
    std::time_t current_time = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    std::vector<std::string> data_line {
        std::to_string(current_time),
        "",
        "",
        "",
        "",
        "",
        std::to_string(sleep)
    };
    csv_handler->AppendCSVLine(file_name, data_line);
}

int DataExtractor::InitPineTimeCommunicator()
{
    communicator.reset(new PineTimeCommunicator);

    int ret = communicator->ConnectToPineTime();

    if (1 == ret)
      return 1;
    
    communicator->SetCallbacks(
        [&](std::array<int16_t, 3> motion_values) {
            WriteMotion(motion_values);
        },
        [&](uint8_t heartrate) { WriteHeartrate(heartrate); },
        [&](uint8_t battery) { WriteBattery(battery); },
        [&](uint8_t sleep) { WriteSleep(sleep); });

    communicator->EnableNotifications();

    std::cout << "Connected to PineTime!\nData is now being written (to exit write 'quit')...\n>> ";

    return 0;
}
