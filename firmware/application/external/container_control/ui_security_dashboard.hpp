/*
 * Security Dashboard Screen
 * Shows status of all defensive security modules
 */

#ifndef __UI_SECURITY_DASHBOARD_HPP__
#define __UI_SECURITY_DASHBOARD_HPP__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "security/anti_jamming.hpp"
#include "security/gps_spoofing.hpp"
#include "security/threat_detection.hpp"
#include "security/forensic_evidence.hpp"
#include "security/admin_security.hpp"

namespace ui {

class SecurityDashboardView : public View {
   public:
    SecurityDashboardView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Security Dashboard"; }

    void on_frame_sync() override;

   private:
    NavigationView& nav_;

    // UI Elements
    Text text_title{
        {20, 20, 200, 16},
        "SECURITY STATUS"};

    // Anti-Jamming Status
    Text text_jamming_label{
        {20, 45, 100, 16},
        "Jamming:"};

    Text text_jamming_status{
        {120, 45, 100, 16},
        "CLEAR"};

    // GPS-Spoofing Status
    Text text_spoofing_label{
        {20, 65, 100, 16},
        "GPS Spoof:"};

    Text text_spoofing_status{
        {120, 65, 100, 16},
        "AUTHENTIC"};

    // Threat Detection Status
    Text text_threat_label{
        {20, 85, 100, 16},
        "Threats:"};

    Text text_threat_status{
        {120, 85, 100, 16},
        "NONE"};

    // Evidence Session
    Text text_evidence_label{
        {20, 105, 100, 16},
        "Evidence:"};

    Text text_evidence_status{
        {120, 105, 100, 16},
        "NO SESSION"};

    // Admin Status
    Text text_admin_label{
        {20, 125, 100, 16},
        "Admin:"};

    Text text_admin_status{
        {120, 125, 100, 16},
        "NOT LOGGED IN"};

    // Statistics
    Text text_stats{
        {20, 155, 220, 48},
        ""};

    // Buttons
    Button button_admin_login{
        {40, 210, 160, 32},
        "Admin Login"};

    Button button_view_evidence{
        {40, 250, 160, 32},
        "View Evidence"};

    Button button_back{
        {40, 290, 160, 32},
        "Back"};

    void update_display();
    void on_admin_login();
    void on_view_evidence();
};

}  // namespace ui

#endif  // __UI_SECURITY_DASHBOARD_HPP__
