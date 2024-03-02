#pragma once

#include <simpleble/SimpleBLE.h>
#include <array>

class PineTimeCommunicator
{
public:
    PineTimeCommunicator();
    ~PineTimeCommunicator();

    int ConnectToPineTime(int scan_time_ms = 1000);
    void DisconnectFromPineTime();
    bool IsPineTimeConnected();
    uint8_t GetHeartRateValue();
    std::array<int16_t, 3> GetMotionValues();
    uint8_t GetBatteryLevelValue();

private:
    const std::string INFINITIME_IDENTIFIER = "InfiniTime";

    const SimpleBLE::BluetoothUUID HEARTRATE_SERVICE = "0000180d-0000-1000-8000-00805f9b34fb";
    const SimpleBLE::BluetoothUUID HEARTRATE_UUID = "00002a37-0000-1000-8000-00805f9b34fb";

    const SimpleBLE::BluetoothUUID MOTION_SERVICE = "00030000-78fc-48fe-8e23-433b3a1942d0";
    const SimpleBLE::BluetoothUUID MOTION_UUID = "00030002-78fc-48fe-8e23-433b3a1942d0";

    const SimpleBLE::BluetoothUUID BATTERY_SERVICE = "0000180f-0000-1000-8000-00805f9b34fb";
    const SimpleBLE::BluetoothUUID BATTERY_UUID = "00002a19-0000-1000-8000-00805f9b34fb";

    SimpleBLE::Peripheral pinetime;

    SimpleBLE::Adapter GetBluetoothAdapter();

    bool is_connected = false;
};
