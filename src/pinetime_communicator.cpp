#include "pinetime_communicator.hpp"

#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

PineTimeCommunicator::PineTimeCommunicator() :
    motion_handler(nullptr),
    heartrate_handler(nullptr),
    battery_handler(nullptr),
    sleep_handler(nullptr)
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
    is_connected = true;

    pinetime.set_callback_on_disconnected([&]() {
        if (!is_connected)
            return;

        std::cout << "Unintended disconnect, trying to reconnect" << std::endl;
        is_connected = false;
        int retries = 100;
        while (!is_connected && retries--) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            pinetime.connect();
            is_connected = pinetime.is_connected();
        }

        if (!retries) {
            std::cerr << "cannot reconnect" << std::endl;
            return;
        }

        std::cout << "reconnected" << std::endl;

        if (notifications_enabled)
            EnableNotifications();
    });

    return 0;
}

void PineTimeCommunicator::SetCallbacks(std::function<void(std::array<int16_t, 3>)> motion_callback,
                                        std::function<void(uint8_t)> heartrate_callback,
                                        std::function<void(uint8_t)> battery_callback,
                                        std::function<void(uint8_t)> sleep_callback)
{
    motion_handler = motion_callback;
    heartrate_handler = heartrate_callback;
    battery_handler = battery_callback;
    sleep_handler = sleep_callback;
}

void PineTimeCommunicator::EnableNotifications()
{
    pinetime.notify(MOTION_SERVICE, MOTION_UUID, [&](SimpleBLE::ByteArray bytes) {
        std::array<int16_t, 3> values;
        std::memcpy(values.data(), bytes.data(), sizeof(int16_t) * values.size());
        if (motion_handler != nullptr)
            motion_handler(values);
    });

    pinetime.notify(HEARTRATE_SERVICE, HEARTRATE_UUID, [&](SimpleBLE::ByteArray bytes) {
        // see https://github.com/InfiniTimeOrg/InfiniTime/blob/main/doc/ble.md#heart-rate
        if (heartrate_handler != nullptr)
            heartrate_handler(uint8_t(bytes[1]));
    });

    pinetime.notify(BATTERY_SERVICE, BATTERY_UUID, [&](SimpleBLE::ByteArray bytes) {
        // see https://github.com/InfiniTimeOrg/InfiniTime/blob/main/doc/ble.md#battery-level
        if (battery_handler != nullptr)
            battery_handler(uint8_t(bytes[0]));
    });

    pinetime.notify(SLEEP_SERVICE, SLEEP_UUID, [&](SimpleBLE::ByteArray bytes) {
        std::cerr << "got sleep data" << std::endl;
        if (sleep_handler != nullptr)
            sleep_handler(uint8_t(bytes[0]));
    });

    notifications_enabled = true;
}

bool PineTimeCommunicator::IsPineTimeConnected()
{
    return is_connected;
}

void PineTimeCommunicator::DisconnectFromPineTime()
{
    pinetime.unsubscribe(MOTION_SERVICE, MOTION_UUID);
    pinetime.unsubscribe(HEARTRATE_SERVICE, HEARTRATE_UUID);
    pinetime.unsubscribe(BATTERY_SERVICE, BATTERY_UUID);
    pinetime.unsubscribe(SLEEP_SERVICE, SLEEP_UUID);

    is_connected = false;
    pinetime.disconnect();
    motion_handler = nullptr;
    heartrate_handler = nullptr;
    battery_handler = nullptr;
    sleep_handler = nullptr;
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
