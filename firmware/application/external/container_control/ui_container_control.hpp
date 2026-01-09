/*
 * Container Control System - UI Header
 */

#ifndef __UI_CONTAINER_CONTROL_H__
#define __UI_CONTAINER_CONTROL_H__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "string_format.hpp"

namespace ui {

class ContainerControlView : public View {
   public:
    ContainerControlView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Container Control"; }

   private:
    NavigationView& nav_;

    // Title (shortened to save flash)
    Text text_title{
        {10, 10, 220, 20},
        "CONTAINER CONTROL"};

    // Status indicators with dots
    Rectangle rect_tx_indicator{
        {10, 40, 8, 8},
        Color::red()};

    Text text_tx_status{
        {22, 38, 180, 16},
        "TX: BLOCKED"};

    Rectangle rect_rx_indicator{
        {10, 60, 8, 8},
        Color::green()};

    Text text_rx_status{
        {22, 58, 180, 16},
        "RX: ACTIVE"};

    // Device count
    Text text_devices{
        {10, 85, 200, 16},
        "Devices: 0"};

    // Large prominent start button
    Button button_start{
        {20, 175, 200, 40},
        "START SCAN"};

    // Security Dashboard button
    Button button_security{
        {20, 225, 200, 32},
        "Security Dashboard"};

    // Back button (smaller, bottom)
    Button button_back{
        {70, 270, 100, 28},
        "Back"};

    void on_start_scan();
    void on_security();
};

}  // namespace ui

#endif /*__UI_CONTAINER_CONTROL_H__*/
