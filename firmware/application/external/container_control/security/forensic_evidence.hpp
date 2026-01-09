/*
 * Forensic Evidence Module
 * Court-admissible evidence collection with cryptographic integrity
 *
 * LEGAL & DEFENSIVE ONLY - Evidence collection for legal proceedings
 */

#ifndef __FORENSIC_EVIDENCE_HPP__
#define __FORENSIC_EVIDENCE_HPP__

#include <cstdint>
#include <cstring>

namespace container_control {
namespace security {

// Evidence entry types
enum class EvidenceType : uint8_t {
    TYPE_DEVICE_DETECTION = 0,
    TYPE_JAMMING_EVENT = 1,
    TYPE_SPOOFING_EVENT = 2,
    TYPE_THREAT_DETECTION = 3,
    TYPE_SCAN_COMPLETE = 4,
    TYPE_SYSTEM_EVENT = 5
};

// Evidence integrity status
enum class IntegrityStatus : uint8_t {
    STATUS_VALID = 0,       // Signature valid
    STATUS_UNVERIFIED = 1,  // Not yet verified
    STATUS_CORRUPTED = 2    // Signature mismatch
};

// Geographic coordinates
struct GeoCoordinates {
    int32_t latitude;   // Degrees * 1e7 (e.g., 52.5200000 = 525200000)
    int32_t longitude;  // Degrees * 1e7
    uint16_t altitude;  // Meters above sea level
    uint8_t accuracy;   // GPS accuracy in meters
    bool valid;         // GPS fix obtained
};

// Evidence entry
struct EvidenceEntry {
    uint32_t entry_id;            // Unique ID
    uint32_t timestamp;           // Unix timestamp (seconds since epoch)
    EvidenceType type;
    GeoCoordinates location;      // GPS coordinates
    uint32_t frequency;           // Frequency involved (Hz)
    int8_t rssi;                  // Signal strength (dBm)
    char description[128];        // Human-readable description
    char operator_id[32];         // Operator who collected evidence
    char device_serial[32];       // HackRF serial number
    uint32_t hash;                // Cryptographic hash (simple for embedded)
    IntegrityStatus integrity;
    bool active;
};

// Evidence session
struct EvidenceSession {
    uint32_t session_id;
    uint32_t start_timestamp;
    uint32_t end_timestamp;
    char case_number[32];         // Case/inspection ID
    char location_name[64];       // Human-readable location
    uint16_t entry_count;
    bool finalized;               // Session closed, no more entries
    uint32_t session_hash;        // Hash of entire session
};

// Configuration
constexpr uint8_t MAX_EVIDENCE_ENTRIES = 64;
constexpr uint32_t HASH_SEED = 0x5A5A5A5A;  // Simple hash seed

// Forensic Evidence Manager class
class ForensicEvidenceManager {
   public:
    // Initialize evidence system
    static void init();

    // Start new evidence session
    static bool start_session(const char* case_number, const char* location);

    // End current session
    static bool end_session();

    // Add evidence entry
    static bool add_entry(EvidenceType type,
                          uint32_t frequency,
                          int8_t rssi,
                          const char* description);

    // Update GPS location (call when GPS fix obtained)
    static void update_location(int32_t latitude, int32_t longitude,
                                uint16_t altitude, uint8_t accuracy);

    // Set operator ID
    static void set_operator(const char* operator_id);

    // Get current session
    static const EvidenceSession* get_current_session();

    // Get evidence entries
    static const EvidenceEntry* get_entries();
    static uint16_t get_entry_count();

    // Export evidence to SD card (TODO: Implementation)
    static bool export_to_file(const char* filename);

    // Verify integrity
    static IntegrityStatus verify_entry(uint16_t entry_index);
    static bool verify_session();

    // Clear evidence (WARNING: Use only for testing!)
    static void clear_all();

   private:
    static EvidenceSession current_session_;
    static EvidenceEntry entries_[MAX_EVIDENCE_ENTRIES];
    static uint16_t entry_count_;
    static GeoCoordinates current_location_;
    static char current_operator_[32];
    static char device_serial_[32];
    static uint32_t next_entry_id_;

    // Hash calculation helpers
    static uint32_t calculate_hash(const uint8_t* data, size_t length);
    static uint32_t calculate_entry_hash(const EvidenceEntry* entry);
    static uint32_t calculate_session_hash();
};

}  // namespace security
}  // namespace container_control

#endif  // __FORENSIC_EVIDENCE_HPP__
