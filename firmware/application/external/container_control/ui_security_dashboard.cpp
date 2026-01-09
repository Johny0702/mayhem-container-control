/*
 * Security Dashboard Screen - Implementation
 */

#include "ui_security_dashboard.hpp"
#include "ui_admin_login.hpp"
#include "string_format.hpp"

namespace ui {

SecurityDashboardView::SecurityDashboardView(NavigationView& nav)
    : nav_(nav) {

    add_children({
        &text_title,
        &text_jamming_label,
        &text_jamming_status,
        &text_spoofing_label,
        &text_spoofing_status,
        &text_threat_label,
        &text_threat_status,
        &text_evidence_label,
        &text_evidence_status,
        &text_admin_label,
        &text_admin_status,
        &text_stats,
        &button_admin_login,
        &button_view_evidence,
        &button_back});

    // Button handlers
    button_admin_login.on_select = [this](Button&) {
        this->on_admin_login();
    };

    button_view_evidence.on_select = [this](Button&) {
        this->on_view_evidence();
    };

    button_back.on_select = [&nav](Button&) {
        nav.pop();
    };

    // Initial update
    update_display();
}

void SecurityDashboardView::focus() {
    button_back.focus();
}

void SecurityDashboardView::on_frame_sync() {
    update_display();
}

void SecurityDashboardView::update_display() {
    // Update Anti-Jamming status
    auto jamming_status = container_control::security::AntiJammingDetector::get_status();
    switch (jamming_status) {
        case container_control::security::JammingStatus::STATUS_CLEAR:
            text_jamming_status.set("CLEAR");
            break;
        case container_control::security::JammingStatus::STATUS_SUSPICIOUS:
            text_jamming_status.set("SUSPICIOUS");
            break;
        case container_control::security::JammingStatus::STATUS_CONFIRMED:
            text_jamming_status.set("CONFIRMED");
            break;
        case container_control::security::JammingStatus::STATUS_CRITICAL:
            text_jamming_status.set("CRITICAL!");
            break;
    }

    // Update GPS-Spoofing status
    auto spoofing_status = container_control::security::GPSSpoofingDetector::get_status();
    switch (spoofing_status) {
        case container_control::security::SpoofingStatus::STATUS_AUTHENTIC:
            text_spoofing_status.set("AUTHENTIC");
            break;
        case container_control::security::SpoofingStatus::STATUS_SUSPICIOUS:
            text_spoofing_status.set("SUSPICIOUS");
            break;
        case container_control::security::SpoofingStatus::STATUS_CONFIRMED:
            text_spoofing_status.set("CONFIRMED");
            break;
        case container_control::security::SpoofingStatus::STATUS_CRITICAL:
            text_spoofing_status.set("CRITICAL!");
            break;
    }

    // Update Threat Detection status
    auto threat_level = container_control::security::ThreatDetector::get_max_threat_level();
    switch (threat_level) {
        case container_control::security::ThreatLevel::LEVEL_INFO:
            text_threat_status.set("NONE");
            break;
        case container_control::security::ThreatLevel::LEVEL_LOW:
            text_threat_status.set("LOW");
            break;
        case container_control::security::ThreatLevel::LEVEL_MEDIUM:
            text_threat_status.set("MEDIUM");
            break;
        case container_control::security::ThreatLevel::LEVEL_HIGH:
            text_threat_status.set("HIGH");
            break;
        case container_control::security::ThreatLevel::LEVEL_CRITICAL:
            text_threat_status.set("CRITICAL!");
            break;
    }

    // Update Evidence status
    auto* session = container_control::security::ForensicEvidenceManager::get_current_session();
    if (session && session->session_id != 0 && !session->finalized) {
        char evidence_text[32];
        snprintf(evidence_text, sizeof(evidence_text), "ACTIVE (%d entries)",
                 container_control::security::ForensicEvidenceManager::get_entry_count());
        text_evidence_status.set(evidence_text);
    } else if (session && session->finalized) {
        text_evidence_status.set("FINALIZED");
    } else {
        text_evidence_status.set("NO SESSION");
    }

    // Update Admin status
    if (container_control::security::AdminSecurityManager::is_authenticated()) {
        auto role = container_control::security::AdminSecurityManager::get_current_role();
        const char* role_name = "UNKNOWN";
        switch (role) {
            case container_control::security::UserRole::ROLE_OPERATOR:
                role_name = "OPERATOR";
                break;
            case container_control::security::UserRole::ROLE_INVESTIGATOR:
                role_name = "INVESTIGATOR";
                break;
            case container_control::security::UserRole::ROLE_ADMIN:
                role_name = "ADMIN";
                break;
            default:
                role_name = "UNKNOWN";
                break;
        }
        text_admin_status.set(role_name);
        button_admin_login.set_text("Logout");
    } else {
        text_admin_status.set("NOT LOGGED IN");
        button_admin_login.set_text("Admin Login");
    }

    // Update statistics
    char stats_text[64];
    snprintf(stats_text, sizeof(stats_text),
             "Jamming Events: %d\nSpoofing Events: %d\nThreats: %d",
             container_control::security::AntiJammingDetector::get_event_count(),
             container_control::security::GPSSpoofingDetector::get_event_count(),
             container_control::security::ThreatDetector::get_threat_count());
    text_stats.set(stats_text);
}

void SecurityDashboardView::on_admin_login() {
    if (container_control::security::AdminSecurityManager::is_authenticated()) {
        // Logout
        container_control::security::AdminSecurityManager::logout();
        update_display();
    } else {
        // Navigate to login screen
        nav_.push<AdminLoginView>();
    }
}

void SecurityDashboardView::on_view_evidence() {
    // Check permission
    if (!container_control::security::AdminSecurityManager::can_access_evidence()) {
        // Show error or navigate to login
        text_stats.set("ERROR: Access denied\nAdmin login required");
        return;
    }

    // TODO: Navigate to evidence viewer screen
    text_stats.set("Evidence viewer\nnot yet implemented");
}

}  // namespace ui
