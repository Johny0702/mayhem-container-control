/*
 * Advanced Threat Detection Module - Implementation
 */

#include "threat_detection.hpp"

namespace container_control {
namespace security {

// Static members
PatternDetection ThreatDetector::threats_[MAX_THREAT_DETECTIONS] = {};
uint8_t ThreatDetector::threat_count_ = 0;
ThreatLevel ThreatDetector::max_threat_level_ = ThreatLevel::LEVEL_INFO;

FrequencyHopEvent ThreatDetector::hop_events_[MAX_HOP_EVENTS] = {};
uint8_t ThreatDetector::hop_event_count_ = 0;

BurstPattern ThreatDetector::burst_patterns_[MAX_BURST_PATTERNS] = {};
uint8_t ThreatDetector::burst_pattern_count_ = 0;

ThreatDetector::SignalObservation ThreatDetector::history_[32] = {};
uint8_t ThreatDetector::history_count_ = 0;

void ThreatDetector::init() {
    threat_count_ = 0;
    max_threat_level_ = ThreatLevel::LEVEL_INFO;
    hop_event_count_ = 0;
    burst_pattern_count_ = 0;
    history_count_ = 0;

    for (uint8_t i = 0; i < MAX_THREAT_DETECTIONS; i++) {
        threats_[i].active = false;
    }
    for (uint8_t i = 0; i < MAX_HOP_EVENTS; i++) {
        hop_events_[i].active = false;
    }
    for (uint8_t i = 0; i < MAX_BURST_PATTERNS; i++) {
        burst_patterns_[i].active = false;
    }
}

void ThreatDetector::observe_signal(uint32_t frequency, int8_t rssi, uint32_t timestamp_ms, bool is_active) {
    uint8_t idx = history_count_ % 32;
    history_[idx].frequency = frequency;
    history_[idx].rssi = rssi;
    history_[idx].timestamp_ms = timestamp_ms;
    history_[idx].is_active = is_active;
    history_count_++;
}

void ThreatDetector::analyze() {
    detect_frequency_hopping();
    detect_burst_transmissions();
    detect_coordinated_devices();

    // Update max threat level
    max_threat_level_ = ThreatLevel::LEVEL_INFO;
    for (uint8_t i = 0; i < threat_count_; i++) {
        if (threats_[i].active && threats_[i].level > max_threat_level_) {
            max_threat_level_ = threats_[i].level;
        }
    }
}

const PatternDetection* ThreatDetector::get_threats() {
    return threats_;
}

uint8_t ThreatDetector::get_threat_count() {
    return threat_count_;
}

ThreatLevel ThreatDetector::get_max_threat_level() {
    return max_threat_level_;
}

const FrequencyHopEvent* ThreatDetector::get_hop_events() {
    return hop_events_;
}

uint8_t ThreatDetector::get_hop_event_count() {
    return hop_event_count_;
}

const BurstPattern* ThreatDetector::get_burst_patterns() {
    return burst_patterns_;
}

uint8_t ThreatDetector::get_burst_pattern_count() {
    return burst_pattern_count_;
}

void ThreatDetector::clear() {
    threat_count_ = 0;
    max_threat_level_ = ThreatLevel::LEVEL_INFO;
    hop_event_count_ = 0;
    burst_pattern_count_ = 0;

    for (uint8_t i = 0; i < MAX_THREAT_DETECTIONS; i++) {
        threats_[i].active = false;
    }
}

void ThreatDetector::detect_frequency_hopping() {
    if (history_count_ < 8) return;

    // Look for rapid frequency changes
    uint32_t unique_freqs[8] = {0};
    uint8_t unique_count = 0;

    // Check last 8 observations
    for (int8_t i = 0; i < 8 && i < history_count_; i++) {
        uint8_t idx = (history_count_ - 1 - i) % 32;
        uint32_t freq = history_[idx].frequency;

        bool found = false;
        for (uint8_t j = 0; j < unique_count; j++) {
            if (unique_freqs[j] == freq) {
                found = true;
                break;
            }
        }

        if (!found && unique_count < 8) {
            unique_freqs[unique_count++] = freq;
        }
    }

    // If 4+ different frequencies in short time, likely hopping
    if (unique_count >= 4 && hop_event_count_ < MAX_HOP_EVENTS) {
        FrequencyHopEvent* event = &hop_events_[hop_event_count_];
        event->hop_count = unique_count;
        for (uint8_t i = 0; i < unique_count && i < 8; i++) {
            event->frequencies[i] = unique_freqs[i];
        }
        event->hop_interval_ms = 100;  // TODO: Calculate from timestamps
        event->timestamp = history_[history_count_ % 32].timestamp_ms / 1000;
        event->active = true;
        hop_event_count_++;

        log_threat(ThreatType::THREAT_FREQUENCY_HOPPING, ThreatLevel::LEVEL_HIGH,
                   unique_freqs[0], unique_freqs[1], "Frequency hopping detected");
    }
}

void ThreatDetector::detect_burst_transmissions() {
    // Look for ON/OFF pattern in signal activity
    if (history_count_ < 6) return;

    uint8_t on_count = 0;
    uint8_t off_count = 0;

    for (int8_t i = 0; i < 6 && i < history_count_; i++) {
        uint8_t idx = (history_count_ - 1 - i) % 32;
        if (history_[idx].is_active) {
            on_count++;
        } else {
            off_count++;
        }
    }

    // If alternating pattern, likely burst transmission
    if (on_count >= 2 && off_count >= 2 && burst_pattern_count_ < MAX_BURST_PATTERNS) {
        BurstPattern* pattern = &burst_patterns_[burst_pattern_count_];
        pattern->frequency = history_[(history_count_ - 1) % 32].frequency;
        pattern->burst_duration_ms = 50;  // Estimated
        pattern->burst_interval_ms = 200;  // Estimated
        pattern->burst_count = on_count;
        pattern->timestamp = history_[(history_count_ - 1) % 32].timestamp_ms / 1000;
        pattern->active = true;
        burst_pattern_count_++;

        log_threat(ThreatType::THREAT_BURST_TRANSMISSION, ThreatLevel::LEVEL_MEDIUM,
                   pattern->frequency, 0, "Burst transmission pattern");
    }
}

void ThreatDetector::detect_coordinated_devices() {
    // Look for simultaneous transmissions on different frequencies
    if (history_count_ < 4) return;

    uint32_t concurrent_freqs[4] = {0};
    uint8_t concurrent_count = 0;
    uint32_t time_window = 1000;  // 1 second window

    uint32_t latest_time = history_[(history_count_ - 1) % 32].timestamp_ms;

    for (int8_t i = 0; i < history_count_ && i < 16; i++) {
        uint8_t idx = (history_count_ - 1 - i) % 32;
        if (latest_time - history_[idx].timestamp_ms > time_window) break;

        bool found = false;
        for (uint8_t j = 0; j < concurrent_count; j++) {
            if (concurrent_freqs[j] == history_[idx].frequency) {
                found = true;
                break;
            }
        }

        if (!found && concurrent_count < 4) {
            concurrent_freqs[concurrent_count++] = history_[idx].frequency;
        }
    }

    // If 3+ devices transmitting simultaneously, suspicious
    if (concurrent_count >= 3) {
        log_threat(ThreatType::THREAT_COORDINATED_NETWORK, ThreatLevel::LEVEL_HIGH,
                   concurrent_freqs[0], concurrent_freqs[1],
                   "Coordinated device network detected");
    }
}

void ThreatDetector::log_threat(ThreatType type, ThreatLevel level,
                                 uint32_t freq1, uint32_t freq2,
                                 const char* description) {
    if (threat_count_ >= MAX_THREAT_DETECTIONS) return;

    PatternDetection* threat = &threats_[threat_count_];
    threat->type = type;
    threat->level = level;
    threat->primary_frequency = freq1;
    threat->secondary_frequency = freq2;
    threat->pattern_confidence = 75;  // TODO: Calculate real confidence

    size_t len = 0;
    while (description[len] && len < 63) {
        threat->description[len] = description[len];
        len++;
    }
    threat->description[len] = '\0';

    threat->active = true;
    threat_count_++;
}

}  // namespace security
}  // namespace container_control
