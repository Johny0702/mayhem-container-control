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

    // Title
    Text text_title{
        {20, 20, 200, 16},
        "Container Control v1.0"};

    Text text_subtitle{
        {20, 40, 200, 16},
        "RX-Only System"};

    // System Info
    Text text_info1{
        {20, 70, 200, 16},
        "Status: Ready"};

    Text text_info2{
        {20, 90, 200, 16},
        "TX: Blocked"};

    Text text_info3{
        {20, 110, 200, 16},
        "Mode: Authority"};

    // Device count display
    Text text_devices{
        {20, 140, 200, 16},
        "Devices: 0"};

    // Start button
    Button button_start{
        {60, 180, 120, 32},
        "START SCAN"};

    // Back button
    Button button_back{
        {60, 220, 120, 32},
        "Back"};

    void on_start_scan();
};

}  // namespace ui

#endif /*__UI_CONTAINER_CONTROL_H__*/
