/*
 * Anti-Jamming Detection Module
 * Passively detects jamming attacks through RSSI anomaly analysis
 *
 * LEGAL & DEFENSIVE ONLY - No TX capabilities
 */

#ifndef __ANTI_JAMMING_HPP__
#define __ANTI_JAMMING_HPP__

#include <cstdint>
#include <cstring>

namespace container_control {
namespace security {

// Jamming detection status
enum class JammingStatus : uint8_t {
    STATUS_CLEAR = 0,       // No jamming detected
    STATUS_SUSPICIOUS = 1,  // Possible jamming
    STATUS_CONFIRMED = 2,   // High confidence jamming
    STATUS_CRITICAL = 3     // Severe jamming attack
};

// Jamming type classification
enum class JammingType : uint8_t {
    TYPE_UNKNOWN = 0,
    TYPE_CONTINUOUS = 1,    // Constant carrier wave
    TYPE_SWEEPING = 2,      // Frequency sweeping jammer
    TYPE_PULSED = 3,        // Pulse jamming
    TYPE_NOISE = 4,         // Broadband noise
    TYPE_BARRAGE = 5        // Multi-frequency barrage
};

// Jamming event record
struct JammingEvent {
    uint32_t timestamp;          // Seconds since boot
    uint32_t frequency;          // Hz
    int8_t rssi_baseline;        // Normal RSSI (dBm)
    int8_t rssi_peak;            // Peak RSSI during jamming (dBm)
    JammingType type;
    JammingStatus severity;
    uint16_t duration_ms;        // Duration in milliseconds
    bool active;
};

// Configuration
constexpr uint8_t MAX_JAMMING_EVENTS = 16;
constexpr int8_t RSSI_ANOMALY_THRESHOLD = 20;  // dBm above baseline
constexpr uint16_t MIN_JAMMING_DURATION_MS = 100;

// Anti-Jamming Detector class
class AntiJammingDetector {
   public:
    // Initialize detector
    static void init();

    // Add RSSI measurement for analysis
    static void add_measurement(uint32_t frequency, int8_t rssi, uint32_t timestamp_ms);

    // Update analysis (call periodically)
    static void analyze();

    // Get current jamming status
    static JammingStatus get_status();

    // Get detected jamming events
    static const JammingEvent* get_events();
    static uint8_t get_event_count();

    // Clear event history
    static void clear_events();

    // Get statistics
    static uint16_t get_total_detections();
    static uint32_t get_last_detection_time();

    // Check if specific frequency is being jammed
    static bool is_frequency_jammed(uint32_t frequency);

   private:
    static JammingEvent events_[MAX_JAMMING_EVENTS];
    static uint8_t event_count_;
    static JammingStatus current_status_;
    static uint16_t total_detections_;
    static uint32_t last_detection_time_;

    // RSSI baseline tracking
    struct FrequencyBaseline {
        uint32_t frequency;
        int8_t baseline_rssi;
        int8_t recent_rssi[8];  // Rolling window
        uint8_t sample_count;
        bool active;
    };
    static FrequencyBaseline baselines_[16];
    static uint8_t baseline_count_;

    // Analysis helpers
    static void update_baseline(uint32_t frequency, int8_t rssi);
    static int8_t get_baseline_rssi(uint32_t frequency);
    static bool detect_anomaly(uint32_t frequency, int8_t rssi);
    static JammingType classify_jamming(uint32_t frequency, int8_t rssi, int8_t baseline);
    static void log_jamming_event(uint32_t frequency, int8_t rssi, int8_t baseline,
                                   JammingType type, uint32_t timestamp_ms);
};

}  // namespace security
}  // namespace container_control

#endif  // __ANTI_JAMMING_HPP__
