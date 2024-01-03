#include "pinetime_communicator.hpp"

#include <iostream>
#include <cstring>

PineTimeCommunicator::PineTimeCommunicator()
{
}

PineTimeCommunicator::~PineTimeCommunicator()
{
}

int PineTimeCommunicator::ConnectToPineTime(int scan_time_ms)
{
    if (!SimpleBLE::Adapter::bluetooth_enabled())
    {
        std::cout << "Bluetooth is not enabled!\n";
        return 1;
    }

    SimpleBLE::Adapter adapter = GetBluetoothAdapter();

    adapter.scan_for(scan_time_ms);

    std::vector<SimpleBLE::Peripheral> peripherals = adapter.scan_get_results();
    bool found_pinetime = false;
    for (SimpleBLE::Peripheral peripheral : peripherals)
    {
        if (INFINITIME_IDENTIFIER == peripheral.identifier())
        {
            found_pinetime = true;
            pinetime = peripheral;
        }
    }

    if (!found_pinetime)
    {
        std::cout << "PineTime was not found!\n";
        return 1;
    }

    pinetime.connect();

    return 0;
}

uint8_t PineTimeCommunicator::GetHeartRateValue()
{
    SimpleBLE::ByteArray heartrate_data = pinetime.read(HEARTRATE_SERVICE, HEARTRATE_UUID);

    // see https://github.com/InfiniTimeOrg/InfiniTime/blob/main/doc/ble.md#heart-rate
    return uint8_t(heartrate_data[1]);
}

std::array<int16_t, 3> PineTimeCommunicator::GetMotionValues()
{
    SimpleBLE::ByteArray motion_data = pinetime.read(MOTION_SERVICE, MOTION_UUID);

    // see https://github.com/InfiniTimeOrg/InfiniTime/blob/main/doc/MotionService.md
    std::array<int16_t, 3> values;
    std::memcpy(values.data(), motion_data.data(), sizeof(int16_t) * values.size());

    return values;
}

uint8_t PineTimeCommunicator::GetBatteryLevelValue()
{
    SimpleBLE::ByteArray battery_data = pinetime.read(BATTERY_SERVICE, BATTERY_UUID);

    // see https://github.com/InfiniTimeOrg/InfiniTime/blob/main/doc/ble.md#battery-level
    return uint8_t(battery_data[0]);
}

void PineTimeCommunicator::DisconnectFromPineTime()
{
    pinetime.disconnect();
}

SimpleBLE::Adapter PineTimeCommunicator::GetBluetoothAdapter()
{
    std::vector<SimpleBLE::Adapter> adapters = SimpleBLE::Adapter::get_adapters();
    if (adapters.empty())
    {
        std::cout << "No Bluetooth adapters found!\n";
        return SimpleBLE::Adapter();
    }

    return adapters[0];
}
