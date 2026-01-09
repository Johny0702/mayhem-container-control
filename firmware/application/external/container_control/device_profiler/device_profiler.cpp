/*
 * Device Profiler - Implementation (ARM Port)
 */

#include "device_profiler.hpp"

namespace container_control {

// Static memory allocation (no malloc on ARM)
DeviceProfile DeviceProfiler::devices_[MAX_DEVICES] = {};
uint8_t DeviceProfiler::device_count_ = 0;

void DeviceProfiler::init() {
    clear();
}

void DeviceProfiler::clear() {
    // Clear all devices
    for (uint8_t i = 0; i < MAX_DEVICES; i++) {
        devices_[i].active = false;
        devices_[i].device_id = 0;
        devices_[i].frequency_count = 0;
        devices_[i].risk_score = 0;
    }
    device_count_ = 0;
}

bool DeviceProfiler::add_signal(uint32_t frequency, int8_t rssi) {
    // Try to add to existing device
    for (uint8_t i = 0; i < device_count_; i++) {
        if (!devices_[i].active) continue;

        // Check if signal belongs to this device
        for (uint8_t j = 0; j < devices_[i].frequency_count; j++) {
            if (signals_belong_to_same_device(frequency, devices_[i].frequencies[j].frequency)) {
                // Add to this device if there's space
                if (devices_[i].frequency_count < MAX_FREQUENCIES_PER_DEVICE) {
                    devices_[i].frequencies[devices_[i].frequency_count].frequency = frequency;
                    devices_[i].frequencies[devices_[i].frequency_count].rssi = rssi;
                    devices_[i].frequencies[devices_[i].frequency_count].active = true;
                    devices_[i].frequency_count++;
                    return true;
                }
                return false;  // No space
            }
        }
    }

    // Create new device
    if (device_count_ >= MAX_DEVICES) {
        return false;  // No space for new device
    }

    DeviceProfile* new_device = &devices_[device_count_];
    new_device->device_id = device_count_ + 1;
    new_device->active = true;
    new_device->frequency_count = 1;
    new_device->frequencies[0].frequency = frequency;
    new_device->frequencies[0].rssi = rssi;
    new_device->frequencies[0].active = true;
    new_device->type = DeviceType::TYPE_UNKNOWN;
    new_device->risk_score = 0;

    device_count_++;
    return true;
}

void DeviceProfiler::analyze() {
    // Analyze and classify all devices
    for (uint8_t i = 0; i < device_count_; i++) {
        if (!devices_[i].active) continue;

        // Classify device type
        devices_[i].type = classify_device(&devices_[i]);

        // Set device name
        set_device_name(&devices_[i]);

        // Check for satellite/cellular proximity
        for (uint8_t j = 0; j < devices_[i].frequency_count; j++) {
            uint32_t freq = devices_[i].frequencies[j].frequency;
            if (is_satellite_frequency(freq)) {
                devices_[i].has_satellite_proximity = true;
            }
            if (is_cellular_frequency(freq)) {
                devices_[i].has_cellular_proximity = true;
            }
        }

        // Calculate risk score
        devices_[i].risk_score = calculate_risk(&devices_[i]);
    }
}

const DeviceProfile* DeviceProfiler::get_device(uint8_t index) {
    if (index >= device_count_) {
        return nullptr;
    }
    return &devices_[index];
}

uint8_t DeviceProfiler::get_device_count() {
    return device_count_;
}

uint8_t DeviceProfiler::calculate_risk(const DeviceProfile* device) {
    if (!device || !device->active) return 0;

    uint8_t risk = 0;

    // Base risk by type
    switch (device->type) {
        case DeviceType::TYPE_SATELLITE_TRACKER:
            risk = 85;
            break;
        case DeviceType::TYPE_CELLULAR_TRACKER:
            risk = 75;
            break;
        case DeviceType::TYPE_ISM_TRACKER:
            risk = 60;
            break;
        case DeviceType::TYPE_DRONE_COMPONENT:
            risk = 70;
            break;
        case DeviceType::TYPE_BLE_BEACON:
            risk = 30;
            break;
        case DeviceType::TYPE_WIFI_DEVICE:
            risk = 20;
            break;
        case DeviceType::TYPE_KEY_FOB:
            risk = 10;
            break;

        // Vehicle trackers (border control)
        case DeviceType::TYPE_VEHICLE_GPS_TRACKER:
            risk = 80;  // High risk - covert tracking
            break;
        case DeviceType::TYPE_MAGNETIC_TRACKER:
            risk = 85;  // Very high - externally attached
            break;
        case DeviceType::TYPE_OBD_TRACKER:
            risk = 70;  // High - requires vehicle access
            break;

        // Maritime trackers (port/coast guard)
        case DeviceType::TYPE_AIS_TRANSPONDER:
            risk = 15;  // Low - required for vessels
            break;
        case DeviceType::TYPE_EPIRB_BEACON:
            risk = 10;  // Low - safety equipment
            break;
        case DeviceType::TYPE_MARINE_SATELLITE_TRACKER:
            risk = 60;  // Medium - optional tracking
            break;
        case DeviceType::TYPE_MARINE_VHF:
            risk = 15;  // Low - standard maritime comms
            break;
        case DeviceType::TYPE_BOAT_GPS_TRACKER:
            risk = 65;  // Medium-high - optional tracker
            break;

        default:
            risk = 50;
    }

    // Increase risk for multiple frequencies
    if (device->frequency_count > 2) {
        risk += 10;
    }

    // Increase risk for satellite proximity
    if (device->has_satellite_proximity) {
        risk += 15;
    }

    // Increase risk for cellular proximity
    if (device->has_cellular_proximity) {
        risk += 10;
    }

    // Cap at 100
    if (risk > 100) risk = 100;

    return risk;
}

bool DeviceProfiler::signals_belong_to_same_device(uint32_t freq1, uint32_t freq2) {
    // Simple heuristic: frequencies within 50 MHz likely same device
    uint32_t diff = (freq1 > freq2) ? (freq1 - freq2) : (freq2 - freq1);
    return (diff < 50000000);  // 50 MHz
}

DeviceType DeviceProfiler::classify_device(const DeviceProfile* device) {
    if (!device || device->frequency_count == 0) {
        return DeviceType::TYPE_UNKNOWN;
    }

    bool has_satellite = false;
    bool has_cellular = false;
    bool has_ism = false;
    bool has_24g = false;
    bool has_ais = false;
    bool has_epirb = false;
    bool has_marine_vhf = false;

    // Check frequency bands
    for (uint8_t i = 0; i < device->frequency_count; i++) {
        uint32_t freq = device->frequencies[i].frequency;

        if (is_satellite_frequency(freq)) has_satellite = true;
        if (is_cellular_frequency(freq)) has_cellular = true;
        if (is_ism_frequency(freq)) has_ism = true;
        if (is_ais_frequency(freq)) has_ais = true;
        if (is_epirb_frequency(freq)) has_epirb = true;
        if (is_marine_vhf_frequency(freq)) has_marine_vhf = true;
        if (freq >= 2400000000 && freq <= 2500000000) has_24g = true;
    }

    // Maritime-specific classification (highest priority)
    if (has_ais) {
        return DeviceType::TYPE_AIS_TRANSPONDER;
    }
    if (has_epirb) {
        return DeviceType::TYPE_EPIRB_BEACON;
    }
    if (has_marine_vhf && !has_cellular) {
        return DeviceType::TYPE_MARINE_VHF;
    }

    // Vehicle-specific classification
    if (has_ism && has_satellite) {
        return DeviceType::TYPE_MAGNETIC_TRACKER;  // Common pattern
    }
    if (has_cellular && has_satellite && device->frequency_count >= 2) {
        return DeviceType::TYPE_VEHICLE_GPS_TRACKER;
    }

    // Maritime trackers
    if (has_satellite && has_marine_vhf) {
        return DeviceType::TYPE_MARINE_SATELLITE_TRACKER;
    }
    if (has_satellite && has_cellular) {
        return DeviceType::TYPE_BOAT_GPS_TRACKER;
    }

    // Generic classification
    if (has_satellite && device->frequency_count >= 2) {
        return DeviceType::TYPE_SATELLITE_TRACKER;
    }
    if (has_cellular && device->frequency_count >= 2) {
        return DeviceType::TYPE_CELLULAR_TRACKER;
    }
    if (has_ism && device->frequency_count >= 1) {
        return DeviceType::TYPE_ISM_TRACKER;
    }
    if (has_24g) {
        return DeviceType::TYPE_BLE_BEACON;  // Or WiFi
    }

    return DeviceType::TYPE_UNKNOWN;
}

void DeviceProfiler::set_device_name(DeviceProfile* device) {
    if (!device) return;

    const char* type_name = "Unknown Device";

    switch (device->type) {
        case DeviceType::TYPE_SATELLITE_TRACKER:
            type_name = "Satellite Tracker";
            break;
        case DeviceType::TYPE_CELLULAR_TRACKER:
            type_name = "Cellular Tracker";
            break;
        case DeviceType::TYPE_ISM_TRACKER:
            type_name = "ISM Tracker";
            break;
        case DeviceType::TYPE_BLE_BEACON:
            type_name = "BLE Beacon";
            break;
        case DeviceType::TYPE_WIFI_DEVICE:
            type_name = "WiFi Device";
            break;
        case DeviceType::TYPE_DRONE_COMPONENT:
            type_name = "Drone Component";
            break;
        case DeviceType::TYPE_KEY_FOB:
            type_name = "Key Fob";
            break;
        case DeviceType::TYPE_TIRE_PRESSURE:
            type_name = "TPMS Sensor";
            break;

        // Vehicle trackers
        case DeviceType::TYPE_VEHICLE_GPS_TRACKER:
            type_name = "Vehicle GPS Tracker";
            break;
        case DeviceType::TYPE_MAGNETIC_TRACKER:
            type_name = "Magnetic Tracker";
            break;
        case DeviceType::TYPE_OBD_TRACKER:
            type_name = "OBD-II Tracker";
            break;

        // Maritime trackers
        case DeviceType::TYPE_AIS_TRANSPONDER:
            type_name = "AIS Transponder";
            break;
        case DeviceType::TYPE_EPIRB_BEACON:
            type_name = "EPIRB Beacon";
            break;
        case DeviceType::TYPE_MARINE_SATELLITE_TRACKER:
            type_name = "Marine Sat Tracker";
            break;
        case DeviceType::TYPE_MARINE_VHF:
            type_name = "Marine VHF Radio";
            break;
        case DeviceType::TYPE_BOAT_GPS_TRACKER:
            type_name = "Boat GPS Tracker";
            break;

        default:
            type_name = "Unknown Device";
    }

    // Safe string copy
    size_t len = 0;
    while (type_name[len] && len < 31) {
        device->name[len] = type_name[len];
        len++;
    }
    device->name[len] = '\0';
}

bool DeviceProfiler::is_satellite_frequency(uint32_t freq) {
    // L-Band (GPS, Iridium, Inmarsat)
    return (freq >= 1559000000 && freq <= 1630000000);
}

bool DeviceProfiler::is_cellular_frequency(uint32_t freq) {
    // GSM 900
    if (freq >= 880000000 && freq <= 960000000) return true;
    // GSM 1800
    if (freq >= 1710000000 && freq <= 1880000000) return true;
    // UMTS 2100
    if (freq >= 1920000000 && freq <= 2170000000) return true;
    // LTE bands
    if (freq >= 700000000 && freq <= 900000000) return true;

    return false;
}

bool DeviceProfiler::is_ism_frequency(uint32_t freq) {
    // 433 MHz
    if (freq >= 433000000 && freq <= 435000000) return true;
    // 868 MHz
    if (freq >= 863000000 && freq <= 870000000) return true;
    // 915 MHz
    if (freq >= 902000000 && freq <= 928000000) return true;

    return false;
}

bool DeviceProfiler::is_ais_frequency(uint32_t freq) {
    // AIS Channel A: 161.975 MHz
    // AIS Channel B: 162.025 MHz
    // Tolerance: ±10 kHz
    if (freq >= 161965000 && freq <= 161985000) return true;  // Channel A
    if (freq >= 162015000 && freq <= 162035000) return true;  // Channel B

    return false;
}

bool DeviceProfiler::is_epirb_frequency(uint32_t freq) {
    // EPIRB (Emergency Position Indicating Radio Beacon)
    // 406.025 MHz ±5 kHz
    if (freq >= 406020000 && freq <= 406030000) return true;

    return false;
}

bool DeviceProfiler::is_marine_vhf_frequency(uint32_t freq) {
    // Marine VHF: 156-174 MHz
    // Includes channels 1-88 (156.025-157.425 MHz)
    // and AIS, DSC, weather channels
    if (freq >= 156000000 && freq <= 174000000) return true;

    return false;
}

}  // namespace container_control
