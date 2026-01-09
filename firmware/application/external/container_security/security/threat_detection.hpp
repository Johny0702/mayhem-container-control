/*
 * Advanced Threat Detection Module
 * Pattern-based threat analysis and coordinated device detection
 *
 * LEGAL & DEFENSIVE ONLY - No TX capabilities
 */

#ifndef __THREAT_DETECTION_HPP__
#define __THREAT_DETECTION_HPP__

#include <cstdint>
#include <cstring>

namespace container_control {
namespace security {

// Threat types
enum class ThreatType : uint8_t {
    THREAT_NONE = 0,
    THREAT_FREQUENCY_HOPPING = 1,    // Frequency-hopping device detected
    THREAT_BURST_TRANSMISSION = 2,    // Burst transmissions
    THREAT_COORDINATED_NETWORK = 3,   // Multiple coordinated devices
    THREAT_UNUSUAL_PATTERN = 4,       // Anomalous transmission pattern
    THREAT_COVERT_CHANNEL = 5         // Potential covert communication
};

// Threat severity
enum class ThreatLevel : uint8_t {
    LEVEL_INFO = 0,      // Informational
    LEVEL_LOW = 1,       // Low threat
    LEVEL_MEDIUM = 2,    // Medium threat
    LEVEL_HIGH = 3,      // High threat
    LEVEL_CRITICAL = 4   // Critical threat
};

// Pattern detection result
struct PatternDetection {
    ThreatType type;
    ThreatLevel level;
    uint32_t primary_frequency;      // Hz
    uint32_t secondary_frequency;    // Hz (if applicable)
    uint16_t pattern_confidence;     // 0-100%
    char description[64];
    bool active;
};

// Frequency hopping detection
struct FrequencyHopEvent {
    uint32_t frequencies[8];     // Detected hop frequencies
    uint8_t hop_count;
    uint32_t hop_interval_ms;    // Average time between hops
    uint32_t timestamp;
    bool active;
};

// Burst transmission tracking
struct BurstPattern {
    uint32_t frequency;
    uint16_t burst_duration_ms;
    uint16_t burst_interval_ms;
    uint8_t burst_count;
    uint32_t timestamp;
    bool active;
};

// Configuration
constexpr uint8_t MAX_THREAT_DETECTIONS = 16;
constexpr uint8_t MAX_HOP_EVENTS = 4;
constexpr uint8_t MAX_BURST_PATTERNS = 8;

// Threat Detector class
class ThreatDetector {
   public:
    // Initialize detector
    static void init();

    // Add signal observation for pattern analysis
    static void observe_signal(uint32_t frequency, int8_t rssi, uint32_t timestamp_ms, bool is_active);

    // Analyze patterns
    static void analyze();

    // Get detected threats
    static const PatternDetection* get_threats();
    static uint8_t get_threat_count();

    // Get highest threat level
    static ThreatLevel get_max_threat_level();

    // Get frequency hopping events
    static const FrequencyHopEvent* get_hop_events();
    static uint8_t get_hop_event_count();

    // Get burst patterns
    static const BurstPattern* get_burst_patterns();
    static uint8_t get_burst_pattern_count();

    // Clear detections
    static void clear();

   private:
    static PatternDetection threats_[MAX_THREAT_DETECTIONS];
    static uint8_t threat_count_;
    static ThreatLevel max_threat_level_;

    static FrequencyHopEvent hop_events_[MAX_HOP_EVENTS];
    static uint8_t hop_event_count_;

    static BurstPattern burst_patterns_[MAX_BURST_PATTERNS];
    static uint8_t burst_pattern_count_;

    // Signal history for pattern analysis
    struct SignalObservation {
        uint32_t frequency;
        int8_t rssi;
        uint32_t timestamp_ms;
        bool is_active;
    };
    static SignalObservation history_[32];
    static uint8_t history_count_;

    // Analysis helpers
    static void detect_frequency_hopping();
    static void detect_burst_transmissions();
    static void detect_coordinated_devices();
    static void log_threat(ThreatType type, ThreatLevel level,
                           uint32_t freq1, uint32_t freq2,
                           const char* description);
};

}  // namespace security
}  // namespace container_control

#endif  // __THREAT_DETECTION_HPP__
