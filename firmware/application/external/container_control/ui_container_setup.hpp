/*
 * Container Setup Screen
 * User inputs container ID, location, and selects scan profiles
 */

#ifndef __UI_CONTAINER_SETUP_HPP__
#define __UI_CONTAINER_SETUP_HPP__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_textentry.hpp"
#include "scanner/scanner.hpp"

namespace ui {

class ContainerSetupView : public View {
   public:
    ContainerSetupView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Container Setup"; }

   private:
    NavigationView& nav_;

    // Operation mode
    enum class OperationMode : uint8_t {
        MODE_CONTAINER = 0,
        MODE_VEHICLE = 1,
        MODE_MARITIME = 2
    };
    OperationMode operation_mode_ = OperationMode::MODE_CONTAINER;

    // Container info
    char container_id_[16] = "MSCU0000000";
    char location_[32] = "Port Location";

    // Selected scan profiles
    bool profile_ism_ = true;
    bool profile_satellite_ = false;
    bool profile_cellular_ = false;
    bool profile_wifi_ = false;
    bool profile_vehicle_ = false;
    bool profile_maritime_ = false;

    // UI Elements
    Text text_title{
        {20, 20, 200, 16},
        "New Inspection"};

    Text label_mode{
        {20, 45, 200, 16},
        "Operation Mode:"};

    Button button_mode_container{
        {20, 65, 70, 24},
        "[X] Ship"};

    Button button_mode_vehicle{
        {95, 65, 70, 24},
        "[ ] Auto"};

    Button button_mode_maritime{
        {170, 65, 70, 24},
        "[ ] Boot"};

    Text label_container_id{
        {20, 100, 120, 16},
        "ID/Plate:"};

    Button button_edit_id{
        {140, 98, 80, 20},
        "MSCU..."};

    Text label_location{
        {20, 125, 120, 16},
        "Location:"};

    Button button_edit_location{
        {140, 123, 80, 20},
        "Hamburg"};

    Text label_profiles{
        {20, 155, 200, 16},
        "Scan Profiles:"};

    // Profile checkboxes (using buttons for now)
    Button button_profile_ism{
        {20, 180, 70, 24},
        "[X] ISM"};

    Button button_profile_sat{
        {95, 180, 70, 24},
        "[ ] Sat"};

    Button button_profile_cell{
        {170, 180, 70, 24},
        "[ ] Cell"};

    Button button_profile_wifi{
        {20, 210, 70, 24},
        "[ ] WiFi"};

    Button button_profile_vehicle{
        {95, 210, 70, 24},
        "[ ] Veh"};

    Button button_profile_maritime{
        {170, 210, 70, 24},
        "[ ] Mar"};

    // Action buttons
    Button button_start_scan{
        {40, 250, 160, 32},
        "START SCAN"};

    Button button_cancel{
        {40, 290, 160, 32},
        "Cancel"};

    void select_mode_container();
    void select_mode_vehicle();
    void select_mode_maritime();
    void toggle_ism();
    void toggle_satellite();
    void toggle_cellular();
    void toggle_wifi();
    void toggle_vehicle();
    void toggle_maritime();
    void start_scan();
    void update_button_text();
    void update_mode_buttons();
};

}  // namespace ui

#endif  // __UI_CONTAINER_SETUP_HPP__
