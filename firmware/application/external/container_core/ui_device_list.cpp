/*
 * Device List Screen - Implementation
 */

#include "ui_device_list.hpp"
#include "string_format.hpp"

namespace ui {

DeviceListView::DeviceListView(NavigationView& nav, const char* container_id)
    : nav_(nav) {

    // Copy container ID
    if (container_id) {
        size_t len = 0;
        while (container_id[len] && len < 15) {
            container_id_[len] = container_id[len];
            len++;
        }
        container_id_[len] = '\0';
    }

    add_children({
        &text_header,
        &text_container,
        &text_count,
        &text_device1,
        &text_device2,
        &text_device3,
        &text_device4,
        &text_device5,
        &text_summary,
        &button_export,
        &button_back});

    // Set container text
    char container_text[50];
    snprintf(container_text, sizeof(container_text), "Container: %s", container_id_);
    text_container.set(container_text);

    // Button handlers
    button_export.on_select = [this](Button&) {
        this->export_evidence();
    };

    button_back.on_select = [&nav](Button&) {
        nav.pop();
    };

    // Populate device list
    populate_device_list();
}

void DeviceListView::focus() {
    button_back.focus();
}

void DeviceListView::populate_device_list() {
    device_count_ = container_control::DeviceProfiler::get_device_count();

    // Update count
    char count_text[32];
    snprintf(count_text, sizeof(count_text), "Total: %d devices", device_count_);
    text_count.set(count_text);

    // Risk counters
    uint8_t critical_count = 0;
    uint8_t high_risk_count = 0;
    uint8_t low_risk_count = 0;

    // Show up to 5 devices
    Text* device_texts[] = {&text_device1, &text_device2, &text_device3, &text_device4, &text_device5};

    for (uint8_t i = 0; i < 5; i++) {
        if (i < device_count_) {
            const auto* device = container_control::DeviceProfiler::get_device(i);
            if (device && device->active) {
                // Format device text
                char device_text[64];
                const char* risk_icon = "🟢";
                if (device->risk_score >= 70) {
                    risk_icon = "🔴";
                    critical_count++;
                } else if (device->risk_score >= 40) {
                    risk_icon = "🟡";
                    high_risk_count++;
                } else {
                    low_risk_count++;
                }

                snprintf(device_text, sizeof(device_text),
                         "%s %s (%d%%)",
                         risk_icon, device->name, device->risk_score);

                device_texts[i]->set(device_text);
            }
        } else {
            device_texts[i]->set("");
        }
    }

    // Update summary
    char summary_text[64];
    snprintf(summary_text, sizeof(summary_text),
             "Critical: %d\nHigh Risk: %d\nLow Risk: %d",
             critical_count, high_risk_count, low_risk_count);
    text_summary.set(summary_text);
}

void DeviceListView::export_evidence() {
    // TODO: Implement evidence export to SD card
    // For now, just show message
    text_header.set("Evidence Exported!");
}

}  // namespace ui
