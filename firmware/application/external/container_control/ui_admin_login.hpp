/*
 * Admin Login Screen (2FA)
 * Secure authentication for admin access
 */

#ifndef __UI_ADMIN_LOGIN_HPP__
#define __UI_ADMIN_LOGIN_HPP__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "security/admin_security.hpp"

namespace ui {

class AdminLoginView : public View {
   public:
    AdminLoginView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Admin Login"; }

   private:
    NavigationView& nav_;

    enum class LoginState : uint8_t {
        STATE_ENTER_PIN = 0,
        STATE_ENTER_TOTP = 1,
        STATE_SUCCESS = 2,
        STATE_FAILED = 3
    };

    LoginState state_ = LoginState::STATE_ENTER_PIN;
    char username_[32] = "admin";  // Default username
    uint32_t pin_input_ = 0;
    uint32_t totp_input_ = 0;

    // UI Elements
    Text text_title{
        {20, 20, 200, 16},
        "ADMIN LOGIN (2FA)"};

    Text text_instruction{
        {20, 50, 200, 32},
        "Enter 4-digit PIN:"};

    Text text_input_display{
        {20, 90, 200, 24},
        "****"};

    Text text_status{
        {20, 120, 220, 48},
        ""};

    // Number buttons (simplified - in real app, use proper input)
    Button button_0{
        {20, 180, 40, 32},
        "0"};

    Button button_1{
        {70, 180, 40, 32},
        "1"};

    Button button_2{
        {120, 180, 40, 32},
        "2"};

    Button button_3{
        {170, 180, 40, 32},
        "3"};

    Button button_clear{
        {20, 220, 90, 32},
        "Clear"};

    Button button_submit{
        {120, 220, 90, 32},
        "Submit"};

    Button button_back{
        {40, 260, 160, 32},
        "Cancel"};

    void on_digit(uint8_t digit);
    void on_clear();
    void on_submit();
    void update_display();
};

}  // namespace ui

#endif  // __UI_ADMIN_LOGIN_HPP__
