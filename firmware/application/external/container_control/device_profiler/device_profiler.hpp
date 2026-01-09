/*
 * Device Profiler - Signal Consolidation Engine (ARM Port)
 * Consolidates multiple signals into logical devices
 *
 * Ported for ARM Cortex-M4 with static memory allocation
 */

#ifndef __DEVICE_PROFILER_HPP__
#define __DEVICE_PROFILER_HPP__

#include <cstdint>
#include <cstring>

namespace container_control {

// Maximum devices and frequencies (memory-constrained)
constexpr uint8_t MAX_DEVICES = 32;
constexpr uint8_t MAX_FREQUENCIES_PER_DEVICE = 8;

// Device types
enum class DeviceType : uint8_t {
    TYPE_UNKNOWN = 0,
    TYPE_SATELLITE_TRACKER = 1,
    TYPE_CELLULAR_TRACKER = 2,
    TYPE_ISM_TRACKER = 3,
    TYPE_BLE_BEACON = 4,
    TYPE_WIFI_DEVICE = 5,
    TYPE_DRONE_COMPONENT = 6,
    TYPE_KEY_FOB = 7,
    TYPE_TIRE_PRESSURE = 8,

    // Vehicle trackers (border control)
    TYPE_VEHICLE_GPS_TRACKER = 10,
    TYPE_MAGNETIC_TRACKER = 11,
    TYPE_OBD_TRACKER = 12,

    // Maritime trackers (port/coast guard)
    TYPE_AIS_TRANSPONDER = 20,
    TYPE_EPIRB_BEACON = 21,
    TYPE_MARINE_SATELLITE_TRACKER = 22,
    TYPE_MARINE_VHF = 23,
    TYPE_BOAT_GPS_TRACKER = 24
};

// Frequency info
struct FrequencyInfo {
    uint32_t frequency;  // Hz
    int8_t rssi;         // dBm (-128 to 127)
    bool active;
};

// Device profile
struct DeviceProfile {
    uint32_t device_id;
    DeviceType type;
    FrequencyInfo frequencies[MAX_FREQUENCIES_PER_DEVICE];
    uint8_t frequency_count;
    uint8_t risk_score;  // 0-100
    bool is_autonomous;
    bool has_satellite_proximity;
    bool has_cellular_proximity;
    char name[32];
    bool active;
};

// Device Profiler class
class DeviceProfiler {
   public:
    // Initialize profiler
    static void init();

    // Add detected signal
    static bool add_signal(uint32_t frequency, int8_t rssi);

    // Analyze all signals and create device profiles
    static void analyze();

    // Get device by index
    static const DeviceProfile* get_device(uint8_t index);

    // Get device count
    static uint8_t get_device_count();

    // Clear all devices
    static void clear();

    // Calculate risk score for device
    static uint8_t calculate_risk(const DeviceProfile* device);

   private:
    static DeviceProfile devices_[MAX_DEVICES];
    static uint8_t device_count_;

    // Signal consolidation helpers
    static bool signals_belong_to_same_device(uint32_t freq1, uint32_t freq2);
    static DeviceType classify_device(const DeviceProfile* device);
    static void set_device_name(DeviceProfile* device);
    static bool is_satellite_frequency(uint32_t freq);
    static bool is_cellular_frequency(uint32_t freq);
    static bool is_ism_frequency(uint32_t freq);
    static bool is_ais_frequency(uint32_t freq);
    static bool is_epirb_frequency(uint32_t freq);
    static bool is_marine_vhf_frequency(uint32_t freq);
};

}  // namespace container_control

#endif  // __DEVICE_PROFILER_HPP__
