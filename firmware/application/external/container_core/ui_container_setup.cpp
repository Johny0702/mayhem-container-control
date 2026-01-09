/*
 * Container Setup Screen - Implementation (Extended for Vehicle/Maritime)
 */

#include "ui_container_setup.hpp"
#include "ui_scanning.hpp"

namespace ui {

ContainerSetupView::ContainerSetupView(NavigationView& nav)
    : nav_(nav) {

    add_children({
        &text_title,
        &label_mode,
        &button_mode_container,
        &button_mode_vehicle,
        &button_mode_maritime,
        &label_container_id,
        &button_edit_id,
        &label_location,
        &button_edit_location,
        &label_profiles,
        &button_profile_ism,
        &button_profile_sat,
        &button_profile_cell,
        &button_profile_wifi,
        &button_profile_vehicle,
        &button_profile_maritime,
        &button_start_scan,
        &button_cancel});

    // Mode selection handlers
    button_mode_container.on_select = [this](Button&) {
        this->select_mode_container();
    };

    button_mode_vehicle.on_select = [this](Button&) {
        this->select_mode_vehicle();
    };

    button_mode_maritime.on_select = [this](Button&) {
        this->select_mode_maritime();
    };

    // Profile toggle handlers
    button_profile_ism.on_select = [this](Button&) {
        this->toggle_ism();
    };

    button_profile_sat.on_select = [this](Button&) {
        this->toggle_satellite();
    };

    button_profile_cell.on_select = [this](Button&) {
        this->toggle_cellular();
    };

    button_profile_wifi.on_select = [this](Button&) {
        this->toggle_wifi();
    };

    button_profile_vehicle.on_select = [this](Button&) {
        this->toggle_vehicle();
    };

    button_profile_maritime.on_select = [this](Button&) {
        this->toggle_maritime();
    };

    // Action buttons
    button_start_scan.on_select = [this](Button&) {
        this->start_scan();
    };

    button_cancel.on_select = [&nav](Button&) {
        nav.pop();
    };

    // Initialize button text
    update_button_text();
    update_mode_buttons();
}

void ContainerSetupView::focus() {
    button_start_scan.focus();
}

void ContainerSetupView::select_mode_container() {
    operation_mode_ = OperationMode::MODE_CONTAINER;
    // Set default profiles for container scanning
    profile_ism_ = true;
    profile_satellite_ = false;
    profile_cellular_ = false;
    profile_wifi_ = false;
    profile_vehicle_ = false;
    profile_maritime_ = false;
    update_button_text();
    update_mode_buttons();

    // Update labels
    label_container_id.set("ID/Container:");
    label_location.set("Port:");
}

void ContainerSetupView::select_mode_vehicle() {
    operation_mode_ = OperationMode::MODE_VEHICLE;
    // Set default profiles for vehicle scanning (border control)
    profile_ism_ = true;
    profile_satellite_ = true;
    profile_cellular_ = true;
    profile_wifi_ = false;
    profile_vehicle_ = true;
    profile_maritime_ = false;
    update_button_text();
    update_mode_buttons();

    // Update labels
    label_container_id.set("Plate/VIN:");
    label_location.set("Border:");
}

void ContainerSetupView::select_mode_maritime() {
    operation_mode_ = OperationMode::MODE_MARITIME;
    // Set default profiles for maritime scanning (boats)
    profile_ism_ = false;
    profile_satellite_ = true;
    profile_cellular_ = true;
    profile_wifi_ = false;
    profile_vehicle_ = false;
    profile_maritime_ = true;
    update_button_text();
    update_mode_buttons();

    // Update labels
    label_container_id.set("Vessel ID:");
    label_location.set("Port/Marina:");
}

void ContainerSetupView::toggle_ism() {
    profile_ism_ = !profile_ism_;
    update_button_text();
}

void ContainerSetupView::toggle_satellite() {
    profile_satellite_ = !profile_satellite_;
    update_button_text();
}

void ContainerSetupView::toggle_cellular() {
    profile_cellular_ = !profile_cellular_;
    update_button_text();
}

void ContainerSetupView::toggle_wifi() {
    profile_wifi_ = !profile_wifi_;
    update_button_text();
}

void ContainerSetupView::toggle_vehicle() {
    profile_vehicle_ = !profile_vehicle_;
    update_button_text();
}

void ContainerSetupView::toggle_maritime() {
    profile_maritime_ = !profile_maritime_;
    update_button_text();
}

void ContainerSetupView::update_mode_buttons() {
    button_mode_container.set_text(
        operation_mode_ == OperationMode::MODE_CONTAINER ? "[X] Ship" : "[ ] Ship");
    button_mode_vehicle.set_text(
        operation_mode_ == OperationMode::MODE_VEHICLE ? "[X] Auto" : "[ ] Auto");
    button_mode_maritime.set_text(
        operation_mode_ == OperationMode::MODE_MARITIME ? "[X] Boot" : "[ ] Boot");
}

void ContainerSetupView::update_button_text() {
    button_profile_ism.set_text(profile_ism_ ? "[X] ISM" : "[ ] ISM");
    button_profile_sat.set_text(profile_satellite_ ? "[X] Sat" : "[ ] Sat");
    button_profile_cell.set_text(profile_cellular_ ? "[X] Cell" : "[ ] Cell");
    button_profile_wifi.set_text(profile_wifi_ ? "[X] WiFi" : "[ ] WiFi");
    button_profile_vehicle.set_text(profile_vehicle_ ? "[X] Veh" : "[ ] Veh");
    button_profile_maritime.set_text(profile_maritime_ ? "[X] Mar" : "[ ] Mar");
}

void ContainerSetupView::start_scan() {
    // Check if at least one profile is selected
    if (!profile_ism_ && !profile_satellite_ && !profile_cellular_ &&
        !profile_wifi_ && !profile_vehicle_ && !profile_maritime_) {
        // Show error - no profiles selected
        return;
    }

    // Initialize scanner with selected profiles
    container_control::Scanner::init();

    if (profile_ism_) {
        container_control::Scanner::load_profile(container_control::ScanProfile::PROFILE_ISM);
    }
    if (profile_satellite_) {
        container_control::Scanner::load_profile(container_control::ScanProfile::PROFILE_SATELLITE);
    }
    if (profile_cellular_) {
        container_control::Scanner::load_profile(container_control::ScanProfile::PROFILE_CELLULAR);
    }
    if (profile_wifi_) {
        container_control::Scanner::load_profile(container_control::ScanProfile::PROFILE_WIFI_BLE);
    }
    if (profile_vehicle_) {
        container_control::Scanner::load_profile(container_control::ScanProfile::PROFILE_VEHICLE);
    }
    if (profile_maritime_) {
        container_control::Scanner::load_profile(container_control::ScanProfile::PROFILE_MARITIME);
    }

    // Navigate to scanning screen
    nav_.push<ScanningView>(container_id_, location_);
}

}  // namespace ui
