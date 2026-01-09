/*
 * Forensic Evidence Module - Implementation
 */

#include "forensic_evidence.hpp"

namespace container_control {
namespace security {

// Static members
EvidenceSession ForensicEvidenceManager::current_session_ = {};
EvidenceEntry ForensicEvidenceManager::entries_[MAX_EVIDENCE_ENTRIES] = {};
uint16_t ForensicEvidenceManager::entry_count_ = 0;
GeoCoordinates ForensicEvidenceManager::current_location_ = {};
char ForensicEvidenceManager::current_operator_[32] = "UNKNOWN";
char ForensicEvidenceManager::device_serial_[32] = "HACKRF-UNKNOWN";
uint32_t ForensicEvidenceManager::next_entry_id_ = 1;

void ForensicEvidenceManager::init() {
    entry_count_ = 0;
    next_entry_id_ = 1;
    current_session_.session_id = 0;
    current_session_.finalized = false;
    current_session_.entry_count = 0;

    current_location_.valid = false;

    // Copy default operator
    const char* default_op = "AUTHORITY-001";
    size_t len = 0;
    while (default_op[len] && len < 31) {
        current_operator_[len] = default_op[len];
        len++;
    }
    current_operator_[len] = '\0';

    // TODO: Get actual HackRF serial number
    const char* default_serial = "HACKRF-0000";
    len = 0;
    while (default_serial[len] && len < 31) {
        device_serial_[len] = default_serial[len];
        len++;
    }
    device_serial_[len] = '\0';

    // Clear entries
    for (uint16_t i = 0; i < MAX_EVIDENCE_ENTRIES; i++) {
        entries_[i].active = false;
    }
}

bool ForensicEvidenceManager::start_session(const char* case_number, const char* location) {
    if (!current_session_.finalized && current_session_.session_id != 0) {
        return false;  // Session already active
    }

    current_session_.session_id = next_entry_id_++;
    current_session_.start_timestamp = 0;  // TODO: Get real timestamp
    current_session_.end_timestamp = 0;
    current_session_.entry_count = 0;
    current_session_.finalized = false;

    // Copy case number
    size_t len = 0;
    while (case_number && case_number[len] && len < 31) {
        current_session_.case_number[len] = case_number[len];
        len++;
    }
    current_session_.case_number[len] = '\0';

    // Copy location
    len = 0;
    while (location && location[len] && len < 63) {
        current_session_.location_name[len] = location[len];
        len++;
    }
    current_session_.location_name[len] = '\0';

    entry_count_ = 0;  // Reset entry count for new session

    return true;
}

bool ForensicEvidenceManager::end_session() {
    if (current_session_.finalized || current_session_.session_id == 0) {
        return false;
    }

    current_session_.end_timestamp = 0;  // TODO: Get real timestamp
    current_session_.entry_count = entry_count_;
    current_session_.session_hash = calculate_session_hash();
    current_session_.finalized = true;

    return true;
}

bool ForensicEvidenceManager::add_entry(EvidenceType type,
                                         uint32_t frequency,
                                         int8_t rssi,
                                         const char* description) {
    if (entry_count_ >= MAX_EVIDENCE_ENTRIES) {
        return false;
    }

    if (current_session_.finalized) {
        return false;  // Cannot add to finalized session
    }

    EvidenceEntry* entry = &entries_[entry_count_];
    entry->entry_id = next_entry_id_++;
    entry->timestamp = 0;  // TODO: Get real Unix timestamp
    entry->type = type;
    entry->location = current_location_;
    entry->frequency = frequency;
    entry->rssi = rssi;
    entry->integrity = IntegrityStatus::STATUS_UNVERIFIED;
    entry->active = true;

    // Copy description
    size_t len = 0;
    while (description && description[len] && len < 127) {
        entry->description[len] = description[len];
        len++;
    }
    entry->description[len] = '\0';

    // Copy operator ID
    len = 0;
    while (current_operator_[len] && len < 31) {
        entry->operator_id[len] = current_operator_[len];
        len++;
    }
    entry->operator_id[len] = '\0';

    // Copy device serial
    len = 0;
    while (device_serial_[len] && len < 31) {
        entry->device_serial[len] = device_serial_[len];
        len++;
    }
    entry->device_serial[len] = '\0';

    // Calculate hash
    entry->hash = calculate_entry_hash(entry);

    entry_count_++;
    current_session_.entry_count = entry_count_;

    return true;
}

void ForensicEvidenceManager::update_location(int32_t latitude, int32_t longitude,
                                               uint16_t altitude, uint8_t accuracy) {
    current_location_.latitude = latitude;
    current_location_.longitude = longitude;
    current_location_.altitude = altitude;
    current_location_.accuracy = accuracy;
    current_location_.valid = true;
}

void ForensicEvidenceManager::set_operator(const char* operator_id) {
    size_t len = 0;
    while (operator_id && operator_id[len] && len < 31) {
        current_operator_[len] = operator_id[len];
        len++;
    }
    current_operator_[len] = '\0';
}

const EvidenceSession* ForensicEvidenceManager::get_current_session() {
    return &current_session_;
}

const EvidenceEntry* ForensicEvidenceManager::get_entries() {
    return entries_;
}

uint16_t ForensicEvidenceManager::get_entry_count() {
    return entry_count_;
}

bool ForensicEvidenceManager::export_to_file(const char* filename) {
    // TODO: Implement SD card export
    // Format: JSON or CSV with all evidence entries
    return false;  // Not yet implemented
}

IntegrityStatus ForensicEvidenceManager::verify_entry(uint16_t entry_index) {
    if (entry_index >= entry_count_) {
        return IntegrityStatus::STATUS_CORRUPTED;
    }

    EvidenceEntry* entry = &entries_[entry_index];
    uint32_t stored_hash = entry->hash;
    uint32_t calculated_hash = calculate_entry_hash(entry);

    if (stored_hash == calculated_hash) {
        entry->integrity = IntegrityStatus::STATUS_VALID;
        return IntegrityStatus::STATUS_VALID;
    } else {
        entry->integrity = IntegrityStatus::STATUS_CORRUPTED;
        return IntegrityStatus::STATUS_CORRUPTED;
    }
}

bool ForensicEvidenceManager::verify_session() {
    uint32_t calculated = calculate_session_hash();
    return (calculated == current_session_.session_hash);
}

void ForensicEvidenceManager::clear_all() {
    entry_count_ = 0;
    current_session_.session_id = 0;
    current_session_.finalized = false;
    current_session_.entry_count = 0;

    for (uint16_t i = 0; i < MAX_EVIDENCE_ENTRIES; i++) {
        entries_[i].active = false;
    }
}

uint32_t ForensicEvidenceManager::calculate_hash(const uint8_t* data, size_t length) {
    // Simple hash function (FNV-1a variant for embedded)
    uint32_t hash = HASH_SEED;

    for (size_t i = 0; i < length; i++) {
        hash ^= data[i];
        hash *= 0x01000193;  // FNV prime
    }

    return hash;
}

uint32_t ForensicEvidenceManager::calculate_entry_hash(const EvidenceEntry* entry) {
    // Hash key fields (excluding hash field itself)
    uint32_t hash = HASH_SEED;

    hash ^= entry->entry_id;
    hash *= 0x01000193;
    hash ^= entry->timestamp;
    hash *= 0x01000193;
    hash ^= static_cast<uint32_t>(entry->type);
    hash *= 0x01000193;
    hash ^= entry->frequency;
    hash *= 0x01000193;
    hash ^= static_cast<uint32_t>(entry->rssi);
    hash *= 0x01000193;

    // Hash description
    for (size_t i = 0; entry->description[i] && i < 128; i++) {
        hash ^= entry->description[i];
        hash *= 0x01000193;
    }

    return hash;
}

uint32_t ForensicEvidenceManager::calculate_session_hash() {
    // Hash all entry hashes together
    uint32_t hash = HASH_SEED;

    for (uint16_t i = 0; i < entry_count_; i++) {
        if (entries_[i].active) {
            hash ^= entries_[i].hash;
            hash *= 0x01000193;
        }
    }

    hash ^= current_session_.session_id;
    hash *= 0x01000193;
    hash ^= current_session_.entry_count;
    hash *= 0x01000193;

    return hash;
}

}  // namespace security
}  // namespace container_control
