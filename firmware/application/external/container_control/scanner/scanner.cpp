/*
 * Scanner Engine - Implementation (ARM Port)
 */

#include "scanner.hpp"

namespace container_control {

// Static members
ScanStatus Scanner::status_ = ScanStatus::STATUS_IDLE;
ScanProfile Scanner::current_profile_ = ScanProfile::PROFILE_ISM;
FrequencyRange Scanner::ranges_[MAX_SCAN_RANGES] = {};
uint8_t Scanner::range_count_ = 0;
ScanResult Scanner::results_[MAX_SCAN_RESULTS] = {};
uint16_t Scanner::result_count_ = 0;
uint32_t Scanner::current_frequency_ = 0;
uint8_t Scanner::current_range_index_ = 0;
uint32_t Scanner::total_steps_ = 0;
uint32_t Scanner::current_step_ = 0;

void Scanner::init() {
    status_ = ScanStatus::STATUS_IDLE;
    range_count_ = 0;
    result_count_ = 0;
    current_frequency_ = 0;
    current_range_index_ = 0;
    total_steps_ = 0;
    current_step_ = 0;
}

bool Scanner::load_profile(ScanProfile profile) {
    if (status_ == ScanStatus::STATUS_SCANNING) {
        return false;  // Cannot load while scanning
    }

    current_profile_ = profile;
    range_count_ = 0;

    switch (profile) {
        case ScanProfile::PROFILE_ISM:
            setup_ism_profile();
            break;
        case ScanProfile::PROFILE_SATELLITE:
            setup_satellite_profile();
            break;
        case ScanProfile::PROFILE_CELLULAR:
            setup_cellular_profile();
            break;
        case ScanProfile::PROFILE_WIFI_BLE:
            setup_wifi_ble_profile();
            break;
        case ScanProfile::PROFILE_VEHICLE:
            setup_vehicle_profile();
            break;
        case ScanProfile::PROFILE_MARITIME:
            setup_maritime_profile();
            break;
        default:
            return false;
    }

    return true;
}

bool Scanner::add_range(uint32_t start_freq, uint32_t end_freq, uint32_t step) {
    if (range_count_ >= MAX_SCAN_RANGES) {
        return false;
    }

    ranges_[range_count_].start_freq = start_freq;
    ranges_[range_count_].end_freq = end_freq;
    ranges_[range_count_].step_size = step;
    range_count_++;

    return true;
}

bool Scanner::start() {
    if (range_count_ == 0) {
        return false;  // No ranges configured
    }

    status_ = ScanStatus::STATUS_SCANNING;
    current_range_index_ = 0;
    current_step_ = 0;
    result_count_ = 0;

    // Calculate total steps
    total_steps_ = 0;
    for (uint8_t i = 0; i < range_count_; i++) {
        uint32_t range_steps = (ranges_[i].end_freq - ranges_[i].start_freq) / ranges_[i].step_size;
        total_steps_ += range_steps;
    }

    current_frequency_ = ranges_[0].start_freq;

    // TODO: In full implementation, trigger actual HackRF scanning
    // For now, this is a simulation framework

    return true;
}

void Scanner::pause() {
    if (status_ == ScanStatus::STATUS_SCANNING) {
        status_ = ScanStatus::STATUS_PAUSED;
    }
}

void Scanner::resume() {
    if (status_ == ScanStatus::STATUS_PAUSED) {
        status_ = ScanStatus::STATUS_SCANNING;
    }
}

void Scanner::stop() {
    status_ = ScanStatus::STATUS_IDLE;
    current_step_ = 0;
}

ScanStatus Scanner::get_status() {
    return status_;
}

uint8_t Scanner::get_progress() {
    if (total_steps_ == 0) return 0;
    uint32_t progress = (current_step_ * 100) / total_steps_;
    return (progress > 100) ? 100 : static_cast<uint8_t>(progress);
}

uint32_t Scanner::get_current_frequency() {
    return current_frequency_;
}

const ScanResult* Scanner::get_results() {
    return results_;
}

uint16_t Scanner::get_result_count() {
    return result_count_;
}

void Scanner::clear_results() {
    result_count_ = 0;
}

void Scanner::setup_ism_profile() {
    // ISM Band 433 MHz
    add_range(433050000, 434790000, 25000);  // 433 MHz

    // ISM Band 868 MHz
    add_range(863000000, 870000000, 25000);  // 868 MHz (EU)

    // ISM Band 915 MHz
    add_range(902000000, 928000000, 25000);  // 915 MHz (US)
}

void Scanner::setup_satellite_profile() {
    // GPS L1
    add_range(1574000000, 1577000000, 100000);

    // Iridium
    add_range(1616000000, 1626500000, 100000);
}

void Scanner::setup_cellular_profile() {
    // GSM 900
    add_range(880000000, 915000000, 200000);
    add_range(925000000, 960000000, 200000);

    // GSM 1800
    add_range(1710000000, 1785000000, 200000);
    add_range(1805000000, 1880000000, 200000);
}

void Scanner::setup_wifi_ble_profile() {
    // 2.4 GHz ISM (WiFi, Bluetooth)
    add_range(2400000000, 2483500000, 1000000);
}

void Scanner::setup_vehicle_profile() {
    // VEHICLE TRACKER SCAN PROFILE (Border Control / Customs)
    // Optimized for fast vehicle pass-through scanning

    // 433 MHz ISM - Common magnetic GPS trackers
    add_range(433050000, 434790000, 50000);  // Coarser steps for speed

    // GSM 850 (Americas, parts of Asia)
    add_range(824000000, 849000000, 200000);  // Uplink
    add_range(869000000, 894000000, 200000);  // Downlink

    // GSM 900 (Europe, Asia, Africa, most of world)
    add_range(880000000, 915000000, 200000);  // Uplink
    add_range(925000000, 960000000, 200000);  // Downlink

    // GPS L1 - Primary vehicle tracker frequency
    add_range(1574000000, 1577000000, 200000);

    // Note: Quick scan optimized for <30 second vehicle stops
    // Focus on most common vehicle tracker frequencies
}

void Scanner::setup_maritime_profile() {
    // MARITIME TRACKER SCAN PROFILE (Port Control / Coast Guard)
    // Specialized for boat/ship tracking devices

    // Marine VHF (156-174 MHz) - Communication and some trackers
    add_range(156000000, 174000000, 25000);

    // AIS (Automatic Identification System) - CRITICAL for maritime
    add_range(161900000, 162100000, 5000);  // Fine resolution for AIS

    // EPIRB (Emergency Position Indicating Radio Beacon)
    add_range(406000000, 406100000, 5000);  // 406.025 MHz

    // GSM 900 (Coastal cellular trackers)
    add_range(880000000, 915000000, 200000);
    add_range(925000000, 960000000, 200000);

    // GPS L1 - Marine GPS trackers
    add_range(1574000000, 1577000000, 100000);

    // Inmarsat/Iridium - Satellite maritime trackers
    add_range(1525000000, 1559000000, 200000);  // Inmarsat
    add_range(1616000000, 1626500000, 200000);  // Iridium
}

int8_t Scanner::measure_rssi(uint32_t frequency) {
    // TODO: Implement actual RSSI measurement via HackRF
    // For now, return simulated value
    return -65;  // Typical noise floor
}

}  // namespace container_control
