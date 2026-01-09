/*
 * Anti-Jamming Detection Module - Implementation
 */

#include "anti_jamming.hpp"

namespace container_control {
namespace security {

// Static members
JammingEvent AntiJammingDetector::events_[MAX_JAMMING_EVENTS] = {};
uint8_t AntiJammingDetector::event_count_ = 0;
JammingStatus AntiJammingDetector::current_status_ = JammingStatus::STATUS_CLEAR;
uint16_t AntiJammingDetector::total_detections_ = 0;
uint32_t AntiJammingDetector::last_detection_time_ = 0;

AntiJammingDetector::FrequencyBaseline AntiJammingDetector::baselines_[16] = {};
uint8_t AntiJammingDetector::baseline_count_ = 0;

void AntiJammingDetector::init() {
    event_count_ = 0;
    current_status_ = JammingStatus::STATUS_CLEAR;
    total_detections_ = 0;
    last_detection_time_ = 0;
    baseline_count_ = 0;

    // Clear all events
    for (uint8_t i = 0; i < MAX_JAMMING_EVENTS; i++) {
        events_[i].active = false;
    }

    // Clear baselines
    for (uint8_t i = 0; i < 16; i++) {
        baselines_[i].active = false;
    }
}

void AntiJammingDetector::add_measurement(uint32_t frequency, int8_t rssi, uint32_t timestamp_ms) {
    // Update baseline for this frequency
    update_baseline(frequency, rssi);

    // Check for anomaly
    if (detect_anomaly(frequency, rssi)) {
        int8_t baseline = get_baseline_rssi(frequency);
        JammingType type = classify_jamming(frequency, rssi, baseline);
        log_jamming_event(frequency, rssi, baseline, type, timestamp_ms);
    }
}

void AntiJammingDetector::analyze() {
    // Count active jamming events
    uint8_t active_count = 0;
    uint8_t critical_count = 0;

    for (uint8_t i = 0; i < event_count_; i++) {
        if (events_[i].active) {
            active_count++;
            if (events_[i].severity == JammingStatus::STATUS_CRITICAL) {
                critical_count++;
            }
        }
    }

    // Update overall status
    if (critical_count > 0) {
        current_status_ = JammingStatus::STATUS_CRITICAL;
    } else if (active_count >= 3) {
        current_status_ = JammingStatus::STATUS_CONFIRMED;
    } else if (active_count > 0) {
        current_status_ = JammingStatus::STATUS_SUSPICIOUS;
    } else {
        current_status_ = JammingStatus::STATUS_CLEAR;
    }
}

JammingStatus AntiJammingDetector::get_status() {
    return current_status_;
}

const JammingEvent* AntiJammingDetector::get_events() {
    return events_;
}

uint8_t AntiJammingDetector::get_event_count() {
    return event_count_;
}

void AntiJammingDetector::clear_events() {
    event_count_ = 0;
    current_status_ = JammingStatus::STATUS_CLEAR;

    for (uint8_t i = 0; i < MAX_JAMMING_EVENTS; i++) {
        events_[i].active = false;
    }
}

uint16_t AntiJammingDetector::get_total_detections() {
    return total_detections_;
}

uint32_t AntiJammingDetector::get_last_detection_time() {
    return last_detection_time_;
}

bool AntiJammingDetector::is_frequency_jammed(uint32_t frequency) {
    // Check if any active event affects this frequency
    for (uint8_t i = 0; i < event_count_; i++) {
        if (events_[i].active) {
            // Allow 10 MHz tolerance
            uint32_t freq_diff = (events_[i].frequency > frequency)
                                     ? (events_[i].frequency - frequency)
                                     : (frequency - events_[i].frequency);
            if (freq_diff < 10000000) {  // 10 MHz
                return true;
            }
        }
    }
    return false;
}

void AntiJammingDetector::update_baseline(uint32_t frequency, int8_t rssi) {
    // Find existing baseline for this frequency
    FrequencyBaseline* baseline = nullptr;

    for (uint8_t i = 0; i < baseline_count_; i++) {
        if (baselines_[i].active && baselines_[i].frequency == frequency) {
            baseline = &baselines_[i];
            break;
        }
    }

    // Create new baseline if needed
    if (!baseline && baseline_count_ < 16) {
        baseline = &baselines_[baseline_count_];
        baseline->frequency = frequency;
        baseline->baseline_rssi = rssi;
        baseline->sample_count = 0;
        baseline->active = true;
        baseline_count_++;
    }

    if (baseline) {
        // Add to rolling window
        uint8_t idx = baseline->sample_count % 8;
        baseline->recent_rssi[idx] = rssi;
        baseline->sample_count++;

        // Calculate average baseline (exclude outliers)
        if (baseline->sample_count >= 8) {
            int16_t sum = 0;
            uint8_t count = 0;
            for (uint8_t i = 0; i < 8; i++) {
                // Exclude extreme values
                if (baseline->recent_rssi[i] > -100 && baseline->recent_rssi[i] < -40) {
                    sum += baseline->recent_rssi[i];
                    count++;
                }
            }
            if (count > 0) {
                baseline->baseline_rssi = static_cast<int8_t>(sum / count);
            }
        }
    }
}

int8_t AntiJammingDetector::get_baseline_rssi(uint32_t frequency) {
    for (uint8_t i = 0; i < baseline_count_; i++) {
        if (baselines_[i].active && baselines_[i].frequency == frequency) {
            return baselines_[i].baseline_rssi;
        }
    }
    return -90;  // Default noise floor
}

bool AntiJammingDetector::detect_anomaly(uint32_t frequency, int8_t rssi) {
    int8_t baseline = get_baseline_rssi(frequency);

    // Anomaly if RSSI is significantly above baseline
    if (rssi > (baseline + RSSI_ANOMALY_THRESHOLD)) {
        return true;
    }

    return false;
}

JammingType AntiJammingDetector::classify_jamming(uint32_t frequency, int8_t rssi, int8_t baseline) {
    int8_t delta = rssi - baseline;

    // Simple classification based on RSSI characteristics
    if (delta > 40) {
        return JammingType::TYPE_CONTINUOUS;  // Very strong constant signal
    } else if (delta > 30) {
        return JammingType::TYPE_NOISE;  // Broadband noise
    } else if (delta > 20) {
        return JammingType::TYPE_PULSED;  // Moderate periodic signal
    } else {
        return JammingType::TYPE_UNKNOWN;
    }
}

void AntiJammingDetector::log_jamming_event(uint32_t frequency, int8_t rssi, int8_t baseline,
                                             JammingType type, uint32_t timestamp_ms) {
    // Don't overflow event buffer
    if (event_count_ >= MAX_JAMMING_EVENTS) {
        return;
    }

    JammingEvent* event = &events_[event_count_];
    event->timestamp = timestamp_ms / 1000;  // Convert to seconds
    event->frequency = frequency;
    event->rssi_baseline = baseline;
    event->rssi_peak = rssi;
    event->type = type;
    event->duration_ms = MIN_JAMMING_DURATION_MS;  // Initial duration

    // Determine severity
    int8_t delta = rssi - baseline;
    if (delta > 40) {
        event->severity = JammingStatus::STATUS_CRITICAL;
    } else if (delta > 30) {
        event->severity = JammingStatus::STATUS_CONFIRMED;
    } else {
        event->severity = JammingStatus::STATUS_SUSPICIOUS;
    }

    event->active = true;

    event_count_++;
    total_detections_++;
    last_detection_time_ = timestamp_ms;

    // Trigger analysis update
    analyze();
}

}  // namespace security
}  // namespace container_control
