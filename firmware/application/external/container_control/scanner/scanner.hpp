/*
 * Scanner Engine - Frequency Sweep System (ARM Port)
 * Manages multi-band frequency scanning
 *
 * Ported for ARM Cortex-M4 / PortaPack Mayhem
 */

#ifndef __SCANNER_HPP__
#define __SCANNER_HPP__

#include <cstdint>

namespace container_control {

// Scan profiles
enum class ScanProfile : uint8_t {
    PROFILE_ISM = 0,          // 433/868/915 MHz
    PROFILE_SATELLITE = 1,    // L-Band (1.5-1.6 GHz)
    PROFILE_CELLULAR = 2,     // GSM/LTE bands
    PROFILE_WIFI_BLE = 3,     // 2.4/5 GHz
    PROFILE_VEHICLE = 4,      // Vehicle trackers (border control)
    PROFILE_MARITIME = 5,     // Marine trackers (boats/ships)
    PROFILE_CUSTOM = 6        // User-defined
};

// Scan status
enum class ScanStatus : uint8_t {
    STATUS_IDLE = 0,
    STATUS_SCANNING = 1,
    STATUS_PAUSED = 2,
    STATUS_COMPLETE = 3,
    STATUS_ERROR = 4
};

// Frequency range
struct FrequencyRange {
    uint32_t start_freq;  // Hz
    uint32_t end_freq;    // Hz
    uint32_t step_size;   // Hz
};

// Scan result
struct ScanResult {
    uint32_t frequency;  // Hz
    int8_t rssi;         // dBm
    bool active_signal;
};

// Scanner configuration
constexpr uint8_t MAX_SCAN_RANGES = 8;
constexpr uint16_t MAX_SCAN_RESULTS = 256;

// Scanner class
class Scanner {
   public:
    // Initialize scanner
    static void init();

    // Load predefined scan profile
    static bool load_profile(ScanProfile profile);

    // Add custom frequency range
    static bool add_range(uint32_t start_freq, uint32_t end_freq, uint32_t step);

    // Start scanning
    static bool start();

    // Pause scanning
    static void pause();

    // Resume scanning
    static void resume();

    // Stop scanning
    static void stop();

    // Get current status
    static ScanStatus get_status();

    // Get progress (0-100%)
    static uint8_t get_progress();

    // Get current frequency being scanned
    static uint32_t get_current_frequency();

    // Get scan results
    static const ScanResult* get_results();
    static uint16_t get_result_count();

    // Clear results
    static void clear_results();

   private:
    static ScanStatus status_;
    static ScanProfile current_profile_;
    static FrequencyRange ranges_[MAX_SCAN_RANGES];
    static uint8_t range_count_;
    static ScanResult results_[MAX_SCAN_RESULTS];
    static uint16_t result_count_;
    static uint32_t current_frequency_;
    static uint8_t current_range_index_;
    static uint32_t total_steps_;
    static uint32_t current_step_;

    // Helper functions
    static void setup_ism_profile();
    static void setup_satellite_profile();
    static void setup_cellular_profile();
    static void setup_wifi_ble_profile();
    static void setup_vehicle_profile();
    static void setup_maritime_profile();
    static int8_t measure_rssi(uint32_t frequency);
};

}  // namespace container_control

#endif  // __SCANNER_HPP__
