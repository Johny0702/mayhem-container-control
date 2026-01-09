/*
 * GPS-Spoofing Detection Module
 * Passively detects GPS spoofing through signal analysis
 *
 * LEGAL & DEFENSIVE ONLY - No TX capabilities
 */

#ifndef __GPS_SPOOFING_HPP__
#define __GPS_SPOOFING_HPP__

#include <cstdint>
#include <cstring>

namespace container_control {
namespace security {

// GPS constellation types
enum class GNSSConstellation : uint8_t {
    GPS = 0,        // USA GPS
    GLONASS = 1,    // Russian GLONASS
    GALILEO = 2,    // European Galileo
    BEIDOU = 3      // Chinese BeiDou
};

// Spoofing detection status
enum class SpoofingStatus : uint8_t {
    STATUS_AUTHENTIC = 0,   // Signals appear authentic
    STATUS_SUSPICIOUS = 1,  // Possible spoofing
    STATUS_CONFIRMED = 2,   // High confidence spoofing
    STATUS_CRITICAL = 3     // Severe spoofing attack
};

// Spoofing indicators
struct SpoofingIndicators {
    bool clock_drift_anomaly;          // Clock drift doesn't match expected
    bool signal_strength_uniform;      // All satellites have similar strength (suspicious)
    bool doppler_shift_missing;        // Missing expected Doppler shift
    bool multi_constellation_mismatch; // GPS/GLONASS/Galileo don't agree
    bool sudden_position_jump;         // Position jumped unrealistically
    bool single_source_correlation;    // All signals correlate (same transmitter)
    uint8_t anomaly_count;            // Total anomalies detected
};

// GPS signal measurement
struct GPSSignalMeasurement {
    GNSSConstellation constellation;
    uint32_t frequency;          // Hz (L1: ~1575 MHz)
    int8_t rssi;                 // Signal strength (dBm)
    uint32_t timestamp_ms;       // Measurement time
    int16_t doppler_shift;       // Hz (if available)
    bool active;
};

// Spoofing event record
struct SpoofingEvent {
    uint32_t timestamp;               // Seconds since boot
    SpoofingStatus severity;
    SpoofingIndicators indicators;
    uint32_t affected_frequency;      // Primary frequency affected
    char description[64];             // Human-readable description
    bool active;
};

// Configuration
constexpr uint8_t MAX_SPOOFING_EVENTS = 8;
constexpr uint8_t MAX_GPS_MEASUREMENTS = 16;
constexpr int8_t RSSI_UNIFORMITY_THRESHOLD = 5;  // dBm tolerance

// GPS Spoofing Detector class
class GPSSpoofingDetector {
   public:
    // Initialize detector
    static void init();

    // Add GPS signal measurement
    static void add_measurement(GNSSConstellation constellation,
                                uint32_t frequency,
                                int8_t rssi,
                                uint32_t timestamp_ms);

    // Analyze measurements for spoofing
    static void analyze();

    // Get current spoofing status
    static SpoofingStatus get_status();

    // Get detected spoofing events
    static const SpoofingEvent* get_events();
    static uint8_t get_event_count();

    // Get current indicators
    static SpoofingIndicators get_indicators();

    // Clear event history
    static void clear_events();

    // Get statistics
    static uint16_t get_total_detections();
    static uint32_t get_last_detection_time();

   private:
    static SpoofingEvent events_[MAX_SPOOFING_EVENTS];
    static uint8_t event_count_;
    static SpoofingStatus current_status_;
    static SpoofingIndicators current_indicators_;
    static uint16_t total_detections_;
    static uint32_t last_detection_time_;

    // Signal measurements
    static GPSSignalMeasurement measurements_[MAX_GPS_MEASUREMENTS];
    static uint8_t measurement_count_;

    // Analysis helpers
    static bool check_signal_uniformity();
    static bool check_multi_constellation_consistency();
    static bool check_rssi_realism();
    static void update_indicators();
    static void log_spoofing_event(const char* description);
};

}  // namespace security
}  // namespace container_control

#endif  // __GPS_SPOOFING_HPP__
