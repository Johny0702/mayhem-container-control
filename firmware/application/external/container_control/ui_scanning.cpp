/*
 * Scanning Progress Screen - Implementation
 */

#include "ui_scanning.hpp"
#include "ui_device_list.hpp"
#include "string_format.hpp"

namespace ui {

ScanningView::ScanningView(NavigationView& nav, const char* container_id, const char* location)
    : nav_(nav) {

    // Copy container info
    if (container_id) {
        size_t len = 0;
        while (container_id[len] && len < 15) {
            container_id_[len] = container_id[len];
            len++;
        }
        container_id_[len] = '\0';
    }

    if (location) {
        size_t len = 0;
        while (location[len] && len < 31) {
            location_[len] = location[len];
            len++;
        }
        location_[len] = '\0';
    }

    add_children({
        &text_status,
        &text_container,
        &progress_bar,
        &text_progress,
        &text_frequency,
        &text_devices,
        &button_pause,
        &button_stop,
        &button_results});

    // Set container text
    char container_text[50];
    snprintf(container_text, sizeof(container_text), "Container: %s", container_id_);
    text_container.set(container_text);

    // Button handlers
    button_pause.on_select = [this](Button&) {
        this->pause_scan();
    };

    button_stop.on_select = [this](Button&) {
        this->stop_scan();
    };

    button_results.on_select = [this](Button&) {
        this->view_results();
    };

    // Initialize scanner and load default ISM profile
    container_control::Scanner::init();
    container_control::Scanner::load_profile(container_control::ScanProfile::PROFILE_ISM);

    // Start scanning
    container_control::Scanner::start();
    scanning_active_ = true;

    // Initialize device profiler
    container_control::DeviceProfiler::init();
}

ScanningView::~ScanningView() {
    // Ensure scanning is stopped
    if (scanning_active_) {
        container_control::Scanner::stop();
    }
}

void ScanningView::focus() {
    button_stop.focus();
}

void ScanningView::on_frame_sync() {
    if (!scanning_active_) return;

    // Process scanner (advance to next frequency)
    container_control::Scanner::process();

    // Get current scanner status
    auto status = container_control::Scanner::get_status();
    progress_ = container_control::Scanner::get_progress();
    current_frequency_ = container_control::Scanner::get_current_frequency();

    // Update display every frame
    update_display();

    // Get scan results and add to device profiler
    uint16_t result_count = container_control::Scanner::get_result_count();
    if (result_count > devices_found_) {
        // New devices found
        const container_control::ScanResult* results = container_control::Scanner::get_results();
        for (uint16_t i = devices_found_; i < result_count; i++) {
            container_control::DeviceProfiler::add_signal(results[i].frequency, results[i].rssi);
        }
        devices_found_ = result_count;
    }

    // Check if scan is complete
    if (status == container_control::ScanStatus::STATUS_COMPLETE) {
        scanning_active_ = false;
        text_status.set("Scan Complete!");
        button_pause.set_text("Pause");

        // Analyze devices
        container_control::DeviceProfiler::analyze();
        devices_found_ = container_control::DeviceProfiler::get_device_count();
    }
}

void ScanningView::update_display() {
    // Update progress bar
    progress_bar.set_value(progress_);

    // Update progress text
    char progress_text[10];
    snprintf(progress_text, sizeof(progress_text), "%d%%", progress_);
    text_progress.set(progress_text);

    // Update frequency
    char freq_text[32];
    snprintf(freq_text, sizeof(freq_text), "Freq: %.3f MHz",
             current_frequency_ / 1000000.0f);
    text_frequency.set(freq_text);

    // Update device count
    char device_text[32];
    snprintf(device_text, sizeof(device_text), "Devices: %d", devices_found_);
    text_devices.set(device_text);
}

void ScanningView::pause_scan() {
    auto status = container_control::Scanner::get_status();

    if (status == container_control::ScanStatus::STATUS_SCANNING) {
        container_control::Scanner::pause();
        button_pause.set_text("Resume");
        text_status.set("Paused");
    } else if (status == container_control::ScanStatus::STATUS_PAUSED) {
        container_control::Scanner::resume();
        button_pause.set_text("Pause");
        text_status.set("Scanning...");
    }
}

void ScanningView::stop_scan() {
    scanning_active_ = false;
    container_control::Scanner::stop();

    // Analyze devices before showing results
    container_control::DeviceProfiler::analyze();

    // Go to results
    view_results();
}

void ScanningView::view_results() {
    // Navigate to device list
    nav_.push<DeviceListView>(container_id_);
}

}  // namespace ui
