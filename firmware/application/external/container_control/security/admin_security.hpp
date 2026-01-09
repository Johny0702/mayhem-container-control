/*
 * Admin Security Layer with 2FA
 * Secure access control for sensitive operations
 *
 * LEGAL & DEFENSIVE ONLY - Admin authentication for evidence access
 */

#ifndef __ADMIN_SECURITY_HPP__
#define __ADMIN_SECURITY_HPP__

#include <cstdint>
#include <cstring>

namespace container_control {
namespace security {

// User roles
enum class UserRole : uint8_t {
    ROLE_NONE = 0,
    ROLE_OPERATOR = 1,      // Basic scanning operations
    ROLE_INVESTIGATOR = 2,  // Evidence access
    ROLE_ADMIN = 3          // Full system access
};

// Authentication status
enum class AuthStatus : uint8_t {
    STATUS_UNAUTHENTICATED = 0,
    STATUS_PIN_VERIFIED = 1,     // Step 1 of 2FA complete
    STATUS_FULLY_AUTHENTICATED = 2,  // Both steps complete
    STATUS_LOCKED = 3            // Too many failures
};

// 2FA methods
enum class TwoFAMethod : uint8_t {
    METHOD_TIME_BASED_OTP = 0,  // TOTP (Google Authenticator style)
    METHOD_CHALLENGE_CODE = 1,   // System-generated challenge
    METHOD_BACKUP_CODE = 2       // Pre-generated backup codes
};

// User account
struct UserAccount {
    char username[32];
    uint32_t pin_hash;           // Hashed 4-6 digit PIN
    UserRole role;
    bool totp_enabled;
    uint32_t totp_secret;        // TOTP shared secret (simplified)
    uint8_t backup_codes[8];     // 8 backup codes (1 byte each, simplified)
    uint8_t failed_attempts;
    uint32_t lockout_until;      // Timestamp when lockout expires
    bool active;
};

// Admin session
struct AdminSession {
    uint32_t session_id;
    char username[32];
    UserRole role;
    uint32_t login_timestamp;
    uint32_t last_activity;
    uint32_t session_timeout;    // Seconds
    AuthStatus status;
    bool active;
};

// Security audit log entry
struct AuditLogEntry {
    uint32_t timestamp;
    char username[32];
    enum EventType : uint8_t {
        EVENT_LOGIN_SUCCESS = 0,
        EVENT_LOGIN_FAILURE = 1,
        EVENT_LOGOUT = 2,
        EVENT_EVIDENCE_ACCESS = 3,
        EVENT_SETTINGS_CHANGE = 4,
        EVENT_2FA_FAILURE = 5,
        EVENT_LOCKOUT = 6
    } event_type;
    char details[64];
    bool active;
};

// Configuration
constexpr uint8_t MAX_USERS = 4;
constexpr uint8_t MAX_AUDIT_ENTRIES = 32;
constexpr uint8_t MAX_FAILED_ATTEMPTS = 3;
constexpr uint32_t LOCKOUT_DURATION = 900;  // 15 minutes
constexpr uint32_t SESSION_TIMEOUT = 1800;  // 30 minutes

// Admin Security Manager class
class AdminSecurityManager {
   public:
    // Initialize security system
    static void init();

    // User management
    static bool create_user(const char* username, uint32_t pin, UserRole role);
    static bool delete_user(const char* username);
    static bool enable_totp(const char* username, uint32_t secret);

    // Authentication (2FA)
    static AuthStatus authenticate_pin(const char* username, uint32_t pin);
    static AuthStatus authenticate_totp(uint32_t totp_code);
    static AuthStatus authenticate_backup_code(uint8_t backup_code);

    // Session management
    static const AdminSession* get_current_session();
    static bool is_authenticated();
    static UserRole get_current_role();
    static void logout();
    static void refresh_activity();  // Call on user interaction

    // Permission checks
    static bool has_permission(UserRole required_role);
    static bool can_access_evidence();
    static bool can_modify_settings();
    static bool can_export_data();

    // Audit logging
    static void log_event(AuditLogEntry::EventType type, const char* details);
    static const AuditLogEntry* get_audit_log();
    static uint8_t get_audit_count();

    // Security status
    static uint8_t get_failed_attempts(const char* username);
    static bool is_locked_out(const char* username);
    static uint32_t get_lockout_remaining(const char* username);

    // Clear (WARNING: Use only for testing!)
    static void clear_all();

   private:
    static UserAccount users_[MAX_USERS];
    static uint8_t user_count_;
    static AdminSession current_session_;
    static AuditLogEntry audit_log_[MAX_AUDIT_ENTRIES];
    static uint8_t audit_count_;
    static char pending_username_[32];  // For 2FA second step

    // Helper functions
    static uint32_t hash_pin(uint32_t pin);
    static bool verify_pin(const char* username, uint32_t pin);
    static uint32_t generate_totp(uint32_t secret, uint32_t timestamp);
    static bool verify_totp(uint32_t secret, uint32_t totp_code);
    static UserAccount* find_user(const char* username);
    static void increment_failed_attempts(const char* username);
    static void reset_failed_attempts(const char* username);
    static bool check_session_timeout();
};

}  // namespace security
}  // namespace container_control

#endif  // __ADMIN_SECURITY_HPP__
