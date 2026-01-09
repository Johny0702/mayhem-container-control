/*
 * Container Control Core - Main UI
 * Scanner & Detection (No Security Modules)
 */

#include "ui_container_control.hpp"
#include "ui_container_setup.hpp"
#include "portapack.hpp"
#include "driver_gate/driver_gate.hpp"

using namespace portapack;

namespace ui {

ContainerControlView::ContainerControlView(NavigationView& nav)
    : nav_(nav) {

    // Initialize Driver Gate in AUTHORITY mode (RX-only)
    container_control::DriverGate::init(container_control::GateMode::MODE_AUTHORITY);

    // Add all UI elements
    add_children({
        &text_title,
        &text_subtitle,
        &text_info1,
        &text_info2,
        &text_info3,
        &text_devices,
        &button_start,
        &button_back});

    // Update version number
    text_title.set("Container Control v2.0");

    // Update TX status from Driver Gate
    if (container_control::DriverGate::is_tx_allowed()) {
        text_info2.set("TX: Allowed (TEST)");
    } else {
        text_info2.set("TX: Blocked");
    }

    // Configure start button - navigate to setup screen
    button_start.on_select = [this](Button&) {
        this->on_start_scan();
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

}  // namespace ui
