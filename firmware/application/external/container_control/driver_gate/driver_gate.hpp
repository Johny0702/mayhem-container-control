/*
 * Driver Gate - TX Blocking Layer (ARM Port)
 * Enforces RX-only operation at driver level
 *
 * Ported for ARM Cortex-M4 / PortaPack Mayhem
 */

#ifndef __DRIVER_GATE_HPP__
#define __DRIVER_GATE_HPP__

#include <cstdint>
#include <cstring>

namespace container_control {

// Gate operational modes
enum class GateMode : uint8_t {
    MODE_AUTHORITY = 0,  // RX-only, TX physically blocked
    MODE_TEST = 1,       // Development/testing mode
    MODE_DISABLED = 2    // Gate bypassed (not for production)
};

// Operation types to check
enum class OperationType : uint8_t {
    OP_RX = 0,      // Receive operation
    OP_TX = 1,      // Transmit operation
    OP_SCAN = 2,    // Frequency scan
    OP_SWEEP = 3,   // Spectrum sweep
    OP_CAPTURE = 4  // IQ capture
};

// Gate status codes
enum class GateStatus : uint8_t {
    STATUS_OK = 0,
    STATUS_BLOCKED = 1,
    STATUS_ERROR = 2,
    STATUS_NOT_INITIALIZED = 3
};

// Audit log entry (simplified for ARM)
struct AuditEntry {
    uint32_t timestamp;      // Seconds since boot
    OperationType operation;
    GateStatus status;
    uint32_t frequency;      // Hz
    char operator_id[16];    // Operator identifier
};

// Driver Gate class
class DriverGate {
   public:
    // Initialize gate with specified mode
    static GateStatus init(GateMode mode);

    // Check if operation is allowed
    static GateStatus check_operation(OperationType operation, uint32_t frequency);

    // Check if TX is allowed (always false in AUTHORITY mode)
    static bool is_tx_allowed();

    // Get current mode
    static GateMode get_mode();

    // Get statistics
    static uint32_t get_blocked_count();
    static uint32_t get_allowed_count();

    // Set operator ID for audit trail
    static void set_operator(const char* operator_id);

   private:
    static GateMode current_mode_;
    static uint32_t blocked_count_;
    static uint32_t allowed_count_;
    static char current_operator_[16];
    static bool initialized_;

    // Audit logging (simplified)
    static void log_operation(OperationType op, GateStatus status, uint32_t freq);
};

}  // namespace container_control

#endif  // __DRIVER_GATE_HPP__
