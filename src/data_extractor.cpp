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
    data_write_loop = std::thread(&DataExtractor::DataWriteLoop, this);
    command_loop.join();
    data_write_loop.join();
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

int DataExtractor::InitPineTimeCommunicator()
{
    communicator.reset(new PineTimeCommunicator);

    if (1 == communicator->ConnectToPineTime())
      return 1;
    
    std::vector<std::string> first_line {"Time", "Heartrate", "MotionX", "MotionY", "MotionZ", "Battery Level", "Sleep Stage"};
    csv_handler->AppendCSVLine(file_name, first_line);
    
    std::cout << "Connected to PineTime!\nData is now being written (to exit write 'quit')...\n>> ";

    return 0;
}

void DataExtractor::WriteData()
{
    std::time_t current_time = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
    std::array<int16_t, 3> motion_values = communicator->GetMotionValues();

    std::vector<std::string> data_line {
        std::to_string(current_time),
        std::to_string(communicator->GetHeartRateValue()),
        std::to_string(motion_values[0]),
        std::to_string(motion_values[1]),
        std::to_string(motion_values[2]),
        std::to_string(communicator->GetBatteryLevelValue()),
        std::to_string(communicator->GetSleepStage())
    };
    csv_handler->AppendCSVLine(file_name, data_line);
    std::this_thread::sleep_for(std::chrono::seconds(data_write_loop_delay));
}

void DataExtractor::DataWriteLoop()
{
    
    auto time = std::chrono::system_clock::now();
    while (command_loop_running)
    {
        try
        {
            WriteData();
        }

        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            std::cout << "Attempting to reconnect PineTime!\n";

            // We disconnect to prevent unexpected behaviour that could occur else
            communicator->DisconnectFromPineTime();
            AttemptReconnect();
        }
    }
}

void DataExtractor::AttemptReconnect()
{
    uint32_t kill_count = 0;

    while (kill_count != 100)
    {
        std::cout << "Attempt " << kill_count+1 << ": ";
        if (0 == communicator->ConnectToPineTime(3000))
        {
            std::cout << "Successfully reconnected to PineTime!\n";
            return;
        }

        kill_count++;
    }

    std::cout << "Unable to reconnect to PineTime. Exiting Program.\n";
    std::terminate();
}
