#pragma once

#include <simpleble/SimpleBLE.h>
#include <array>
#include <functional>

class PineTimeCommunicator
{
public:
    PineTimeCommunicator();
    ~PineTimeCommunicator();

    int ConnectToPineTime(int scan_time_ms = 1000);
    void SetCallbacks(std::function<void(std::array<int16_t, 3>)> motion_callback,
                      std::function<void(uint8_t)> heartrate_callback,
                      std::function<void(uint8_t)> battery_callback,
                      std::function<void(uint8_t)> sleep_callback);
    void EnableNotifications();
    void DisconnectFromPineTime();
    bool IsPineTimeConnected();

private:
    const std::string INFINITIME_IDENTIFIER = "InfiniTime";

    const SimpleBLE::BluetoothUUID HEARTRATE_SERVICE = "0000180d-0000-1000-8000-00805f9b34fb";
    const SimpleBLE::BluetoothUUID HEARTRATE_UUID = "00002a37-0000-1000-8000-00805f9b34fb";

    const SimpleBLE::BluetoothUUID MOTION_SERVICE = "00030000-78fc-48fe-8e23-433b3a1942d0";
    const SimpleBLE::BluetoothUUID MOTION_UUID = "00030002-78fc-48fe-8e23-433b3a1942d0";

    const SimpleBLE::BluetoothUUID BATTERY_SERVICE = "0000180f-0000-1000-8000-00805f9b34fb";
    const SimpleBLE::BluetoothUUID BATTERY_UUID = "00002a19-0000-1000-8000-00805f9b34fb";

    const SimpleBLE::BluetoothUUID SLEEP_SERVICE = "00060000-78fc-48fe-8e23-433b3a1942d0";
    const SimpleBLE::BluetoothUUID SLEEP_UUID = "00060001-78fc-48fe-8e23-433b3a1942d0";

    SimpleBLE::Peripheral pinetime;

    SimpleBLE::Adapter GetBluetoothAdapter();

    bool is_connected = false;
    bool notifications_enabled = false;
    std::function<void(std::array<int16_t, 3>)> motion_handler;
    std::function<void(uint8_t)> heartrate_handler;
    std::function<void(uint8_t)> battery_handler;
    std::function<void(uint8_t)> sleep_handler;
};
