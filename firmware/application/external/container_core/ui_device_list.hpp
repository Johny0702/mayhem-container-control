/*
 * Device List Screen
 * Shows all detected devices with risk scores
 */

#ifndef __UI_DEVICE_LIST_HPP__
#define __UI_DEVICE_LIST_HPP__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "device_profiler/device_profiler.hpp"
#include <cstring>

namespace ui {

class DeviceListView : public View {
   public:
    DeviceListView(NavigationView& nav, const char* container_id);
    void focus() override;
    std::string title() const override { return "Found Devices"; }

   private:
    NavigationView& nav_;
    char container_id_[16];
    uint8_t device_count_ = 0;
    uint8_t selected_device_ = 0;

    // UI Elements
    Text text_header{
        {20, 20, 200, 16},
        "Detected Devices"};

    Text text_container{
        {20, 40, 200, 16},
        ""};

    Text text_count{
        {20, 60, 200, 16},
        "Total: 0"};

    // Device list (simplified - in full version would use MenuView)
    Text text_device1{
        {20, 90, 200, 16},
        ""};

    Text text_device2{
        {20, 110, 200, 16},
        ""};

    Text text_device3{
        {20, 130, 200, 16},
        ""};

    Text text_device4{
        {20, 150, 200, 16},
        ""};

    Text text_device5{
        {20, 170, 200, 16},
        ""};

    Text text_summary{
        {20, 200, 200, 32},
        "Critical: 0\nHigh Risk: 0\nLow Risk: 0"};

    Button button_export{
        {40, 250, 180, 32},
        "Export Evidence"};

    Button button_back{
        {40, 290, 180, 32},
        "Back"};

    void populate_device_list();
    void export_evidence();
};

}  // namespace ui

#endif  // __UI_DEVICE_LIST_HPP__
