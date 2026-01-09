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
        &text_info,
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

    // Update display every frame
    update_display();

    // Simulate scanning progress (in real implementation, get from Scanner)
    auto status = container_control::Scanner::get_status();
    if (status == container_control::ScanStatus::STATUS_SCANNING) {
        progress_ = container_control::Scanner::get_progress();
        current_frequency_ = container_control::Scanner::get_current_frequency();

        // Simulate finding devices (in real implementation, add from scan results)
        if (progress_ > 20 && devices_found_ == 0) {
            // Simulate first device found
            container_control::DeviceProfiler::add_signal(433920000, -55);
            devices_found_++;
        }
        if (progress_ > 50 && devices_found_ == 1) {
            container_control::DeviceProfiler::add_signal(868300000, -60);
            devices_found_++;
        }
        if (progress_ > 80 && devices_found_ == 2) {
            container_control::DeviceProfiler::add_signal(915000000, -58);
            devices_found_++;
        }

        // Check if complete
        if (progress_ >= 100) {
            scanning_active_ = false;
            text_status.set("Scan Complete!");
            button_pause.set_text("Pause");

            // Analyze devices
            container_control::DeviceProfiler::analyze();
            devices_found_ = container_control::DeviceProfiler::get_device_count();
        }
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
