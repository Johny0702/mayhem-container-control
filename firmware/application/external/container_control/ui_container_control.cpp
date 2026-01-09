/*
 * Container Control System - UI Implementation
 */

#include "ui_container_control.hpp"
#include "ui_container_setup.hpp"
#include "ui_security_dashboard.hpp"
#include "portapack.hpp"
#include "driver_gate/driver_gate.hpp"
#include "security/anti_jamming.hpp"
#include "security/gps_spoofing.hpp"
#include "security/threat_detection.hpp"
#include "security/forensic_evidence.hpp"
#include "security/admin_security.hpp"

using namespace portapack;

namespace ui {

ContainerControlView::ContainerControlView(NavigationView& nav)
    : nav_(nav) {

    // Initialize Driver Gate in AUTHORITY mode (RX-only)
    container_control::DriverGate::init(container_control::GateMode::MODE_AUTHORITY);

    // Initialize all security modules
    container_control::security::AntiJammingDetector::init();
    container_control::security::GPSSpoofingDetector::init();
    container_control::security::ThreatDetector::init();
    container_control::security::ForensicEvidenceManager::init();
    container_control::security::AdminSecurityManager::init();

    // Add all UI elements
    add_children({
        &rect_header,
        &text_title,
        &text_version,
        &rect_status,
        &text_status_label,
        &rect_tx_indicator,
        &text_tx_status,
        &rect_rx_indicator,
        &text_rx_status,
        &text_devices,
        &button_start,
        &button_security,
        &button_back});

    // Update TX status from Driver Gate with colors
    if (container_control::DriverGate::is_tx_allowed()) {
        text_tx_status.set("TX: ALLOWED (TEST)");
        rect_tx_indicator.set_color(Color::yellow());
    } else {
        text_tx_status.set("TX: BLOCKED");
        rect_tx_indicator.set_color(Color::red());
    }

    // Set RX status as active
    text_rx_status.set("RX: ACTIVE");
    rect_rx_indicator.set_color(Color::green());

    // Configure start button - navigate to setup screen
    button_start.on_select = [this](Button&) {
        this->on_start_scan();
    };

    // Configure security button
    button_security.on_select = [this](Button&) {
        this->on_security();
    };

    // Configure back button
    button_back.on_select = [&nav](Button&) {
        nav.pop();
    };
}

void ContainerControlView::focus() {
    button_start.focus();
}

void ContainerControlView::on_start_scan() {
    // Navigate to Container Setup screen
    nav_.push<ContainerSetupView>();
}

void ContainerControlView::on_security() {
    // Navigate to Security Dashboard
    nav_.push<SecurityDashboardView>();
}

}  // namespace ui
