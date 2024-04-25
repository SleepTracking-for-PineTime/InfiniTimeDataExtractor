#pragma once

#include "csv_handler.hpp"
#include "pinetime_communicator.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <sstream>

class DataExtractor
{
public:
    DataExtractor(std::string file_name, int data_write_loop_delay);
    ~DataExtractor();

    int Init();
    void StartDataExtraction();
    void EndDataExtraction();

private:
    void InitCSVHandler();
    int InitPineTimeCommunicator();
    void CommandLoop();
    void WriteMotion(std::array<int16_t, 3> motion_values);
    void WriteHeartrate(uint8_t heartrate);
    void WriteBattery(uint8_t battery);
    void WriteSleep(uint8_t sleep);
    // void WriteData();
    // void DataWriteLoop();
    // void AttemptReconnect();

    std::shared_ptr<CSVHandler> csv_handler;
    std::shared_ptr<PineTimeCommunicator> communicator;

    std::string file_name;
    int data_write_loop_delay;
    bool command_loop_running = false;

    std::thread data_write_loop;
    std::thread command_loop;
};
