/*
 * Admin Security Layer - Implementation
 */

#include "admin_security.hpp"

namespace container_control {
namespace security {

// Static members
UserAccount AdminSecurityManager::users_[MAX_USERS] = {};
uint8_t AdminSecurityManager::user_count_ = 0;
AdminSession AdminSecurityManager::current_session_ = {};
AuditLogEntry AdminSecurityManager::audit_log_[MAX_AUDIT_ENTRIES] = {};
uint8_t AdminSecurityManager::audit_count_ = 0;
char AdminSecurityManager::pending_username_[32] = "";

void AdminSecurityManager::init() {
    user_count_ = 0;
    audit_count_ = 0;
    current_session_.active = false;
    current_session_.status = AuthStatus::STATUS_UNAUTHENTICATED;

    // Clear users
    for (uint8_t i = 0; i < MAX_USERS; i++) {
        users_[i].active = false;
    }

    // Create default admin (PIN: 0000, CHANGE THIS!)
    create_user("admin", 0, UserRole::ROLE_ADMIN);
}

bool AdminSecurityManager::create_user(const char* username, uint32_t pin, UserRole role) {
    if (user_count_ >= MAX_USERS) return false;

    UserAccount* user = &users_[user_count_];

    // Copy username
    size_t len = 0;
    while (username && username[len] && len < 31) {
        user->username[len] = username[len];
        len++;
    }
    user->username[len] = '\0';

    user->pin_hash = hash_pin(pin);
    user->role = role;
    user->totp_enabled = false;
    user->totp_secret = 0;
    user->failed_attempts = 0;
    user->lockout_until = 0;
    user->active = true;

    // Initialize backup codes to 0
    for (uint8_t i = 0; i < 8; i++) {
        user->backup_codes[i] = 0;
    }

    user_count_++;
    log_event(AuditLogEntry::EVENT_SETTINGS_CHANGE, "User created");
    return true;
}

bool AdminSecurityManager::delete_user(const char* username) {
    UserAccount* user = find_user(username);
    if (!user) return false;

    user->active = false;
    log_event(AuditLogEntry::EVENT_SETTINGS_CHANGE, "User deleted");
    return true;
}

bool AdminSecurityManager::enable_totp(const char* username, uint32_t secret) {
    UserAccount* user = find_user(username);
    if (!user) return false;

    user->totp_enabled = true;
    user->totp_secret = secret;
    log_event(AuditLogEntry::EVENT_SETTINGS_CHANGE, "TOTP enabled");
    return true;
}

AuthStatus AdminSecurityManager::authenticate_pin(const char* username, uint32_t pin) {
    UserAccount* user = find_user(username);
    if (!user) {
        log_event(AuditLogEntry::EVENT_LOGIN_FAILURE, "User not found");
        return AuthStatus::STATUS_UNAUTHENTICATED;
    }

    // Check lockout
    if (is_locked_out(username)) {
        log_event(AuditLogEntry::EVENT_LOCKOUT, "Account locked");
        return AuthStatus::STATUS_LOCKED;
    }

    // Verify PIN
    if (!verify_pin(username, pin)) {
        increment_failed_attempts(username);
        log_event(AuditLogEntry::EVENT_LOGIN_FAILURE, "Invalid PIN");
        return AuthStatus::STATUS_UNAUTHENTICATED;
    }

    // PIN verified - store username for 2FA step
    size_t len = 0;
    while (username[len] && len < 31) {
        pending_username_[len] = username[len];
        len++;
    }
    pending_username_[len] = '\0';

    // If TOTP not enabled, full authentication
    if (!user->totp_enabled) {
        reset_failed_attempts(username);

        // Create session
        current_session_.session_id = 1;  // TODO: Generate unique ID
        len = 0;
        while (username[len] && len < 31) {
            current_session_.username[len] = username[len];
            len++;
        }
        current_session_.username[len] = '\0';
        current_session_.role = user->role;
        current_session_.login_timestamp = 0;  // TODO: Real timestamp
        current_session_.last_activity = 0;
        current_session_.session_timeout = SESSION_TIMEOUT;
        current_session_.status = AuthStatus::STATUS_FULLY_AUTHENTICATED;
        current_session_.active = true;

        log_event(AuditLogEntry::EVENT_LOGIN_SUCCESS, "Login successful (no 2FA)");
        return AuthStatus::STATUS_FULLY_AUTHENTICATED;
    }

    log_event(AuditLogEntry::EVENT_SETTINGS_CHANGE, "PIN verified, awaiting 2FA");
    return AuthStatus::STATUS_PIN_VERIFIED;
}

AuthStatus AdminSecurityManager::authenticate_totp(uint32_t totp_code) {
    if (pending_username_[0] == '\0') {
        return AuthStatus::STATUS_UNAUTHENTICATED;
    }

    UserAccount* user = find_user(pending_username_);
    if (!user || !user->totp_enabled) {
        log_event(AuditLogEntry::EVENT_2FA_FAILURE, "TOTP not configured");
        return AuthStatus::STATUS_UNAUTHENTICATED;
    }

    // Verify TOTP
    if (!verify_totp(user->totp_secret, totp_code)) {
        increment_failed_attempts(pending_username_);
        log_event(AuditLogEntry::EVENT_2FA_FAILURE, "Invalid TOTP code");
        return AuthStatus::STATUS_UNAUTHENTICATED;
    }

    // 2FA successful
    reset_failed_attempts(pending_username_);

    // Create session
    current_session_.session_id = 1;
    size_t len = 0;
    while (pending_username_[len] && len < 31) {
        current_session_.username[len] = pending_username_[len];
        len++;
    }
    current_session_.username[len] = '\0';
    current_session_.role = user->role;
    current_session_.login_timestamp = 0;
    current_session_.last_activity = 0;
    current_session_.session_timeout = SESSION_TIMEOUT;
    current_session_.status = AuthStatus::STATUS_FULLY_AUTHENTICATED;
    current_session_.active = true;

    pending_username_[0] = '\0';  // Clear pending

    log_event(AuditLogEntry::EVENT_LOGIN_SUCCESS, "Login successful (2FA)");
    return AuthStatus::STATUS_FULLY_AUTHENTICATED;
}

AuthStatus AdminSecurityManager::authenticate_backup_code(uint8_t backup_code) {
    if (pending_username_[0] == '\0') {
        return AuthStatus::STATUS_UNAUTHENTICATED;
    }

    UserAccount* user = find_user(pending_username_);
    if (!user) return AuthStatus::STATUS_UNAUTHENTICATED;

    // Check backup codes
    for (uint8_t i = 0; i < 8; i++) {
        if (user->backup_codes[i] == backup_code && backup_code != 0) {
            // Code valid - invalidate it
            user->backup_codes[i] = 0;

            // Create session (same as TOTP success)
            current_session_.session_id = 1;
            size_t len = 0;
            while (pending_username_[len] && len < 31) {
                current_session_.username[len] = pending_username_[len];
                len++;
            }
            current_session_.username[len] = '\0';
            current_session_.role = user->role;
            current_session_.status = AuthStatus::STATUS_FULLY_AUTHENTICATED;
            current_session_.active = true;

            pending_username_[0] = '\0';

            log_event(AuditLogEntry::EVENT_LOGIN_SUCCESS, "Backup code used");
            return AuthStatus::STATUS_FULLY_AUTHENTICATED;
        }
    }

    log_event(AuditLogEntry::EVENT_2FA_FAILURE, "Invalid backup code");
    return AuthStatus::STATUS_UNAUTHENTICATED;
}

const AdminSession* AdminSecurityManager::get_current_session() {
    check_session_timeout();
    return &current_session_;
}

bool AdminSecurityManager::is_authenticated() {
    check_session_timeout();
    return (current_session_.active &&
            current_session_.status == AuthStatus::STATUS_FULLY_AUTHENTICATED);
}

UserRole AdminSecurityManager::get_current_role() {
    if (!is_authenticated()) return UserRole::ROLE_NONE;
    return current_session_.role;
}

void AdminSecurityManager::logout() {
    current_session_.active = false;
    current_session_.status = AuthStatus::STATUS_UNAUTHENTICATED;
    log_event(AuditLogEntry::EVENT_LOGOUT, "User logged out");
}

void AdminSecurityManager::refresh_activity() {
    if (current_session_.active) {
        current_session_.last_activity = 0;  // TODO: Real timestamp
    }
}

bool AdminSecurityManager::has_permission(UserRole required_role) {
    if (!is_authenticated()) return false;
    return (current_session_.role >= required_role);
}

bool AdminSecurityManager::can_access_evidence() {
    return has_permission(UserRole::ROLE_INVESTIGATOR);
}

bool AdminSecurityManager::can_modify_settings() {
    return has_permission(UserRole::ROLE_ADMIN);
}

bool AdminSecurityManager::can_export_data() {
    return has_permission(UserRole::ROLE_INVESTIGATOR);
}

void AdminSecurityManager::log_event(AuditLogEntry::EventType type, const char* details) {
    if (audit_count_ >= MAX_AUDIT_ENTRIES) {
        audit_count_ = 0;  // Wrap around
    }

    AuditLogEntry* entry = &audit_log_[audit_count_];
    entry->timestamp = 0;  // TODO: Real timestamp
    entry->event_type = type;

    // Copy username
    size_t len = 0;
    const char* username = current_session_.active ? current_session_.username : "SYSTEM";
    while (username[len] && len < 31) {
        entry->username[len] = username[len];
        len++;
    }
    entry->username[len] = '\0';

    // Copy details
    len = 0;
    while (details && details[len] && len < 63) {
        entry->details[len] = details[len];
        len++;
    }
    entry->details[len] = '\0';

    entry->active = true;
    audit_count_++;
}

const AuditLogEntry* AdminSecurityManager::get_audit_log() {
    return audit_log_;
}

uint8_t AdminSecurityManager::get_audit_count() {
    return audit_count_;
}

uint8_t AdminSecurityManager::get_failed_attempts(const char* username) {
    UserAccount* user = find_user(username);
    return user ? user->failed_attempts : 0;
}

bool AdminSecurityManager::is_locked_out(const char* username) {
    UserAccount* user = find_user(username);
    if (!user) return false;

    if (user->failed_attempts >= MAX_FAILED_ATTEMPTS) {
        // TODO: Check if lockout expired
        return true;
    }
    return false;
}

uint32_t AdminSecurityManager::get_lockout_remaining(const char* username) {
    // TODO: Calculate remaining lockout time
    return 0;
}

void AdminSecurityManager::clear_all() {
    user_count_ = 0;
    audit_count_ = 0;
    current_session_.active = false;

    for (uint8_t i = 0; i < MAX_USERS; i++) {
        users_[i].active = false;
    }
}

uint32_t AdminSecurityManager::hash_pin(uint32_t pin) {
    // Simple hash (for embedded - NOT cryptographically secure!)
    uint32_t hash = 0x5A5A5A5A;
    hash ^= pin;
    hash *= 0x01000193;
    hash ^= (pin >> 16);
    hash *= 0x01000193;
    return hash;
}

bool AdminSecurityManager::verify_pin(const char* username, uint32_t pin) {
    UserAccount* user = find_user(username);
    if (!user) return false;

    return (hash_pin(pin) == user->pin_hash);
}

uint32_t AdminSecurityManager::generate_totp(uint32_t secret, uint32_t timestamp) {
    // Simplified TOTP (30-second window)
    // Real implementation would use HMAC-SHA1
    uint32_t time_step = timestamp / 30;
    uint32_t hash = secret ^ time_step;
    hash *= 0x01000193;
    return (hash % 1000000);  // 6-digit code
}

bool AdminSecurityManager::verify_totp(uint32_t secret, uint32_t totp_code) {
    uint32_t current_time = 0;  // TODO: Get real timestamp

    // Check current window and ±1 window for clock skew
    for (int offset = -1; offset <= 1; offset++) {
        uint32_t test_time = current_time + (offset * 30);
        uint32_t expected = generate_totp(secret, test_time);
        if (expected == totp_code) {
            return true;
        }
    }

    return false;
}

UserAccount* AdminSecurityManager::find_user(const char* username) {
    for (uint8_t i = 0; i < user_count_; i++) {
        if (users_[i].active) {
            bool match = true;
            for (uint8_t j = 0; j < 32; j++) {
                if (users_[i].username[j] != username[j]) {
                    match = false;
                    break;
                }
                if (username[j] == '\0') break;
            }
            if (match) return &users_[i];
        }
    }
    return nullptr;
}

void AdminSecurityManager::increment_failed_attempts(const char* username) {
    UserAccount* user = find_user(username);
    if (user) {
        user->failed_attempts++;
        if (user->failed_attempts >= MAX_FAILED_ATTEMPTS) {
            user->lockout_until = 0;  // TODO: Set real lockout timestamp
        }
    }
}

void AdminSecurityManager::reset_failed_attempts(const char* username) {
    UserAccount* user = find_user(username);
    if (user) {
        user->failed_attempts = 0;
        user->lockout_until = 0;
    }
}

bool AdminSecurityManager::check_session_timeout() {
    if (!current_session_.active) return false;

    // TODO: Check if session expired
    // For now, keep session active
    return true;
}

}  // namespace security
}  // namespace container_control
