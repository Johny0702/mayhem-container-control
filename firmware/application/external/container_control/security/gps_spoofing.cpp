/*
 * GPS-Spoofing Detection Module - Implementation
 */

#include "gps_spoofing.hpp"

namespace container_control {
namespace security {

// Static members
SpoofingEvent GPSSpoofingDetector::events_[MAX_SPOOFING_EVENTS] = {};
uint8_t GPSSpoofingDetector::event_count_ = 0;
SpoofingStatus GPSSpoofingDetector::current_status_ = SpoofingStatus::STATUS_AUTHENTIC;
SpoofingIndicators GPSSpoofingDetector::current_indicators_ = {};
uint16_t GPSSpoofingDetector::total_detections_ = 0;
uint32_t GPSSpoofingDetector::last_detection_time_ = 0;

GPSSignalMeasurement GPSSpoofingDetector::measurements_[MAX_GPS_MEASUREMENTS] = {};
uint8_t GPSSpoofingDetector::measurement_count_ = 0;

void GPSSpoofingDetector::init() {
    event_count_ = 0;
    current_status_ = SpoofingStatus::STATUS_AUTHENTIC;
    total_detections_ = 0;
    last_detection_time_ = 0;
    measurement_count_ = 0;

    // Clear indicators
    current_indicators_.clock_drift_anomaly = false;
    current_indicators_.signal_strength_uniform = false;
    current_indicators_.doppler_shift_missing = false;
    current_indicators_.multi_constellation_mismatch = false;
    current_indicators_.sudden_position_jump = false;
    current_indicators_.single_source_correlation = false;
    current_indicators_.anomaly_count = 0;

    // Clear events
    for (uint8_t i = 0; i < MAX_SPOOFING_EVENTS; i++) {
        events_[i].active = false;
    }

    // Clear measurements
    for (uint8_t i = 0; i < MAX_GPS_MEASUREMENTS; i++) {
        measurements_[i].active = false;
    }
}

void GPSSpoofingDetector::add_measurement(GNSSConstellation constellation,
                                           uint32_t frequency,
                                           int8_t rssi,
                                           uint32_t timestamp_ms) {
    // Circular buffer for measurements
    uint8_t idx = measurement_count_ % MAX_GPS_MEASUREMENTS;

    measurements_[idx].constellation = constellation;
    measurements_[idx].frequency = frequency;
    measurements_[idx].rssi = rssi;
    measurements_[idx].timestamp_ms = timestamp_ms;
    measurements_[idx].doppler_shift = 0;  // TODO: Calculate if receiver supports
    measurements_[idx].active = true;

    measurement_count_++;

    // Trigger analysis after collecting enough samples
    if (measurement_count_ >= 4) {
        analyze();
    }
}

void GPSSpoofingDetector::analyze() {
    update_indicators();

    // Count total anomalies
    uint8_t anomaly_count = 0;
    if (current_indicators_.signal_strength_uniform) anomaly_count++;
    if (current_indicators_.multi_constellation_mismatch) anomaly_count++;
    if (current_indicators_.clock_drift_anomaly) anomaly_count++;
    if (current_indicators_.doppler_shift_missing) anomaly_count++;
    if (current_indicators_.single_source_correlation) anomaly_count++;

    current_indicators_.anomaly_count = anomaly_count;

    // Determine overall status
    if (anomaly_count >= 4) {
        current_status_ = SpoofingStatus::STATUS_CRITICAL;
        log_spoofing_event("Multiple spoofing indicators detected");
    } else if (anomaly_count >= 2) {
        current_status_ = SpoofingStatus::STATUS_CONFIRMED;
        log_spoofing_event("GPS spoofing likely");
    } else if (anomaly_count == 1) {
        current_status_ = SpoofingStatus::STATUS_SUSPICIOUS;
    } else {
        current_status_ = SpoofingStatus::STATUS_AUTHENTIC;
    }
}

SpoofingStatus GPSSpoofingDetector::get_status() {
    return current_status_;
}

const SpoofingEvent* GPSSpoofingDetector::get_events() {
    return events_;
}

uint8_t GPSSpoofingDetector::get_event_count() {
    return event_count_;
}

SpoofingIndicators GPSSpoofingDetector::get_indicators() {
    return current_indicators_;
}

void GPSSpoofingDetector::clear_events() {
    event_count_ = 0;
    current_status_ = SpoofingStatus::STATUS_AUTHENTIC;

    for (uint8_t i = 0; i < MAX_SPOOFING_EVENTS; i++) {
        events_[i].active = false;
    }
}

uint16_t GPSSpoofingDetector::get_total_detections() {
    return total_detections_;
}

uint32_t GPSSpoofingDetector::get_last_detection_time() {
    return last_detection_time_;
}

bool GPSSpoofingDetector::check_signal_uniformity() {
    // Real GPS satellites have varying signal strengths due to different distances
    // Spoofed signals from single transmitter tend to have uniform strength

    if (measurement_count_ < 4) return false;

    int16_t rssi_sum = 0;
    uint8_t count = 0;

    // Calculate average RSSI
    for (uint8_t i = 0; i < MAX_GPS_MEASUREMENTS && count < 8; i++) {
        if (measurements_[i].active) {
            rssi_sum += measurements_[i].rssi;
            count++;
        }
    }

    if (count < 4) return false;

    int8_t rssi_avg = static_cast<int8_t>(rssi_sum / count);

    // Check variance
    uint8_t within_threshold = 0;
    for (uint8_t i = 0; i < MAX_GPS_MEASUREMENTS && i < count; i++) {
        if (measurements_[i].active) {
            int8_t diff = measurements_[i].rssi - rssi_avg;
            if (diff < 0) diff = -diff;

            if (diff <= RSSI_UNIFORMITY_THRESHOLD) {
                within_threshold++;
            }
        }
    }

    // If >75% of signals are within tight tolerance, suspicious
    return (within_threshold * 4 > count * 3);
}

bool GPSSpoofingDetector::check_multi_constellation_consistency() {
    // Check if GPS and GLONASS/Galileo measurements exist but don't correlate
    // Real signals should be consistent across constellations

    bool has_gps = false;
    bool has_other = false;

    for (uint8_t i = 0; i < MAX_GPS_MEASUREMENTS; i++) {
        if (measurements_[i].active) {
            if (measurements_[i].constellation == GNSSConstellation::GPS) {
                has_gps = true;
            } else {
                has_other = true;
            }
        }
    }

    // If we only have one constellation, can't check consistency
    if (!has_gps || !has_other) {
        return false;
    }

    // TODO: Implement cross-constellation position verification
    // For now, return false (assume consistent)
    return false;
}

bool GPSSpoofingDetector::check_rssi_realism() {
    // Real GPS signals are very weak (-130 to -150 dBm typical)
    // Spoofed signals often too strong

    for (uint8_t i = 0; i < MAX_GPS_MEASUREMENTS; i++) {
        if (measurements_[i].active) {
            // GPS L1 signals should be weak
            if (measurements_[i].rssi > -100) {
                // Too strong for real GPS satellite
                return false;  // Unrealistic (suspicious)
            }
        }
    }

    return true;  // Realistic
}

void GPSSpoofingDetector::update_indicators() {
    // Check signal uniformity
    current_indicators_.signal_strength_uniform = check_signal_uniformity();

    // Check multi-constellation consistency
    current_indicators_.multi_constellation_mismatch = check_multi_constellation_consistency();

    // Check RSSI realism
    bool rssi_realistic = check_rssi_realism();
    if (!rssi_realistic) {
        current_indicators_.single_source_correlation = true;
    }

    // TODO: Implement clock drift analysis (requires GPS receiver integration)
    // For now, placeholder
    current_indicators_.clock_drift_anomaly = false;

    // TODO: Implement Doppler shift analysis
    current_indicators_.doppler_shift_missing = false;

    // TODO: Implement position jump detection
    current_indicators_.sudden_position_jump = false;
}

void GPSSpoofingDetector::log_spoofing_event(const char* description) {
    if (event_count_ >= MAX_SPOOFING_EVENTS) {
        return;
    }

    SpoofingEvent* event = &events_[event_count_];
    event->timestamp = last_detection_time_ / 1000;  // Convert to seconds
    event->severity = current_status_;
    event->indicators = current_indicators_;
    event->affected_frequency = 1575420000;  // GPS L1

    // Copy description
    size_t len = 0;
    while (description[len] && len < 63) {
        event->description[len] = description[len];
        len++;
    }
    event->description[len] = '\0';

    event->active = true;

    event_count_++;
    total_detections_++;
}

}  // namespace security
}  // namespace container_control
