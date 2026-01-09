/*
 * Scanning Progress Screen
 * Shows real-time scanning progress and found devices
 */

#ifndef __UI_SCANNING_HPP__
#define __UI_SCANNING_HPP__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "scanner/scanner.hpp"
#include "device_profiler/device_profiler.hpp"
#include <cstring>

namespace ui {

class ScanningView : public View {
   public:
    ScanningView(NavigationView& nav, const char* container_id, const char* location);
    ~ScanningView();

    void focus() override;
    std::string title() const override { return "Scanning"; }

   private:
    NavigationView& nav_;
    char container_id_[16];
    char location_[32];

    // Scanning state
    bool scanning_active_ = false;
    uint8_t progress_ = 0;
    uint32_t current_frequency_ = 0;
    uint8_t devices_found_ = 0;

    // UI Elements
    Text text_status{
        {20, 20, 200, 16},
        "Scanning..."};

    Text text_container{
        {20, 45, 200, 16},
        ""};

    ProgressBar progress_bar{
        {20, 75, 200, 16}};

    Text text_progress{
        {20, 95, 100, 16},
        "0%"};

    Text text_frequency{
        {20, 120, 200, 16},
        "Freq: 433.000 MHz"};

    Text text_devices{
        {20, 145, 200, 16},
        "Devices: 0"};

    // Removed text_info to save flash space

    Button button_pause{
        {40, 220, 80, 32},
        "Pause"};

    Button button_stop{
        {140, 220, 80, 32},
        "Stop"};

    Button button_results{
        {40, 265, 180, 32},
        "View Results"};

    // Update timer
    MessageHandlerRegistration message_handler_frame_sync{
        Message::ID::DisplayFrameSync,
        [this](const Message* const) {
            this->on_frame_sync();
        }};

    void on_frame_sync();
    void update_display();
    void pause_scan();
    void stop_scan();
    void view_results();
};

}  // namespace ui

#endif  // __UI_SCANNING_HPP__
