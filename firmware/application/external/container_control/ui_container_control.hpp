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

    // Header with colored background
    Rectangle rect_header{
        {0, 0, 240, 50},
        Color::dark_blue()};

    // Title
    Text text_title{
        {10, 8, 220, 20},
        "CONTAINER CONTROL"};

    Text text_version{
        {10, 28, 220, 12},
        "v2.0 - Authority Mode"};

    // Status Panel
    Rectangle rect_status{
        {10, 60, 220, 70},
        Color::dark_grey()};

    Text text_status_label{
        {20, 68, 100, 16},
        "SYSTEM STATUS"};

    // TX Status with colored indicator
    Rectangle rect_tx_indicator{
        {20, 90, 12, 12},
        Color::red()};

    Text text_tx_status{
        {36, 88, 180, 16},
        "TX: BLOCKED"};

    // RX Status with colored indicator
    Rectangle rect_rx_indicator{
        {20, 108, 12, 12},
        Color::green()};

    Text text_rx_status{
        {36, 106, 180, 16},
        "RX: ACTIVE"};

    // Device count with icon
    Text text_devices{
        {20, 145, 200, 20},
        "Detected Devices: 0"};

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
