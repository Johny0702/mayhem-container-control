/*
 * Driver Gate - Implementation (ARM Port)
 */

#include "driver_gate.hpp"

namespace container_control {

// Static member initialization
GateMode DriverGate::current_mode_ = GateMode::MODE_AUTHORITY;
uint32_t DriverGate::blocked_count_ = 0;
uint32_t DriverGate::allowed_count_ = 0;
char DriverGate::current_operator_[16] = "SYSTEM";
bool DriverGate::initialized_ = false;

GateStatus DriverGate::init(GateMode mode) {
    current_mode_ = mode;
    blocked_count_ = 0;
    allowed_count_ = 0;
    initialized_ = true;

    // In AUTHORITY mode, log initialization
    if (mode == GateMode::MODE_AUTHORITY) {
        log_operation(OperationType::OP_RX, GateStatus::STATUS_OK, 0);
    }

    return GateStatus::STATUS_OK;
}

GateStatus DriverGate::check_operation(OperationType operation, uint32_t frequency) {
    if (!initialized_) {
        return GateStatus::STATUS_NOT_INITIALIZED;
    }

    GateStatus status = GateStatus::STATUS_OK;

    // CRITICAL: In AUTHORITY mode, BLOCK all TX operations
    if (current_mode_ == GateMode::MODE_AUTHORITY) {
        if (operation == OperationType::OP_TX) {
            blocked_count_++;
            status = GateStatus::STATUS_BLOCKED;
            log_operation(operation, status, frequency);
            return status;
        }
    }

    // RX operations are always allowed
    if (operation == OperationType::OP_RX ||
        operation == OperationType::OP_SCAN ||
        operation == OperationType::OP_SWEEP ||
        operation == OperationType::OP_CAPTURE) {
        allowed_count_++;
        return GateStatus::STATUS_OK;
    }

    // In TEST mode, TX might be allowed (not for production!)
    if (current_mode_ == GateMode::MODE_TEST) {
        allowed_count_++;
        log_operation(operation, GateStatus::STATUS_OK, frequency);
        return GateStatus::STATUS_OK;
    }

    // Default: block unknown operations
    blocked_count_++;
    status = GateStatus::STATUS_BLOCKED;
    log_operation(operation, status, frequency);
    return status;
}

bool DriverGate::is_tx_allowed() {
    if (!initialized_) {
        return false;
    }

    // CRITICAL: In AUTHORITY mode, TX is NEVER allowed
    if (current_mode_ == GateMode::MODE_AUTHORITY) {
        return false;
    }

    // Only in TEST mode (not for production)
    return (current_mode_ == GateMode::MODE_TEST);
}

GateMode DriverGate::get_mode() {
    return current_mode_;
}

uint32_t DriverGate::get_blocked_count() {
    return blocked_count_;
}

uint32_t DriverGate::get_allowed_count() {
    return allowed_count_;
}

void DriverGate::set_operator(const char* operator_id) {
    if (operator_id) {
        // Safe string copy (max 15 chars + null terminator)
        size_t len = 0;
        while (operator_id[len] && len < 15) {
            current_operator_[len] = operator_id[len];
            len++;
        }
        current_operator_[len] = '\0';
    }
}

void DriverGate::log_operation(OperationType op, GateStatus status, uint32_t freq) {
    // TODO: Implement proper logging to SD card
    // For now, just count operations
    // In full implementation, write to evidence log with timestamp
}

}  // namespace container_control
