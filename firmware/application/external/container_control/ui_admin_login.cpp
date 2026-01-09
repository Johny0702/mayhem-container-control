/*
 * Admin Login Screen - Implementation
 */

#include "ui_admin_login.hpp"
#include "string_format.hpp"

namespace ui {

AdminLoginView::AdminLoginView(NavigationView& nav)
    : nav_(nav) {

    add_children({
        &text_title,
        &text_instruction,
        &text_input_display,
        &text_status,
        &button_0,
        &button_1,
        &button_2,
        &button_3,
        &button_clear,
        &button_submit,
        &button_back});

    // Number button handlers
    button_0.on_select = [this](Button&) { this->on_digit(0); };
    button_1.on_select = [this](Button&) { this->on_digit(1); };
    button_2.on_select = [this](Button&) { this->on_digit(2); };
    button_3.on_select = [this](Button&) { this->on_digit(3); };

    button_clear.on_select = [this](Button&) { this->on_clear(); };
    button_submit.on_select = [this](Button&) { this->on_submit(); };
    button_back.on_select = [&nav](Button&) { nav.pop(); };

    update_display();
}

void AdminLoginView::focus() {
    button_submit.focus();
}

void AdminLoginView::on_digit(uint8_t digit) {
    if (state_ == LoginState::STATE_ENTER_PIN) {
        pin_input_ = (pin_input_ * 10) + digit;
        if (pin_input_ > 9999) pin_input_ = pin_input_ % 10000;
    } else if (state_ == LoginState::STATE_ENTER_TOTP) {
        totp_input_ = (totp_input_ * 10) + digit;
        if (totp_input_ > 999999) totp_input_ = totp_input_ % 1000000;
    }

    update_display();
}

void AdminLoginView::on_clear() {
    if (state_ == LoginState::STATE_ENTER_PIN) {
        pin_input_ = 0;
    } else if (state_ == LoginState::STATE_ENTER_TOTP) {
        totp_input_ = 0;
    }

    update_display();
}

void AdminLoginView::on_submit() {
    if (state_ == LoginState::STATE_ENTER_PIN) {
        // Authenticate PIN (Step 1)
        auto status = container_control::security::AdminSecurityManager::authenticate_pin(
            username_, pin_input_);

        if (status == container_control::security::AuthStatus::STATUS_PIN_VERIFIED) {
            // PIN verified, need TOTP
            state_ = LoginState::STATE_ENTER_TOTP;
            totp_input_ = 0;
            text_instruction.set("Enter 6-digit TOTP:");
            text_status.set("PIN verified.\nEnter TOTP code.");
        } else if (status == container_control::security::AuthStatus::STATUS_FULLY_AUTHENTICATED) {
            // No 2FA, login complete
            state_ = LoginState::STATE_SUCCESS;
            text_instruction.set("Login successful!");
            text_status.set("Welcome, admin!");

            // Return to previous screen after 1 second (TODO: Timer)
            // For now, user must press Back
        } else if (status == container_control::security::AuthStatus::STATUS_LOCKED) {
            state_ = LoginState::STATE_FAILED;
            text_instruction.set("Account locked!");
            text_status.set("Too many failed attempts.\nWait 15 minutes.");
        } else {
            state_ = LoginState::STATE_FAILED;
            text_instruction.set("Login failed!");
            text_status.set("Invalid PIN.\nTry again.");
        }

        pin_input_ = 0;  // Clear PIN
        update_display();

    } else if (state_ == LoginState::STATE_ENTER_TOTP) {
        // Authenticate TOTP (Step 2)
        auto status = container_control::security::AdminSecurityManager::authenticate_totp(totp_input_);

        if (status == container_control::security::AuthStatus::STATUS_FULLY_AUTHENTICATED) {
            state_ = LoginState::STATE_SUCCESS;
            text_instruction.set("Login successful!");
            text_status.set("2FA verified.\nWelcome, admin!");
        } else {
            state_ = LoginState::STATE_FAILED;
            text_instruction.set("2FA failed!");
            text_status.set("Invalid TOTP code.\nTry again.");
        }

        totp_input_ = 0;  // Clear TOTP
        update_display();
    }
}

void AdminLoginView::update_display() {
    if (state_ == LoginState::STATE_ENTER_PIN) {
        char input_text[8];
        snprintf(input_text, sizeof(input_text), "%04lu", static_cast<unsigned long>(pin_input_));
        text_input_display.set(input_text);
    } else if (state_ == LoginState::STATE_ENTER_TOTP) {
        char input_text[10];
        snprintf(input_text, sizeof(input_text), "%06lu", static_cast<unsigned long>(totp_input_));
        text_input_display.set(input_text);
    } else {
        text_input_display.set("");
    }

    // Update button visibility based on state
    if (state_ == LoginState::STATE_SUCCESS || state_ == LoginState::STATE_FAILED) {
        button_submit.set_text("OK");
    } else {
        button_submit.set_text("Submit");
    }
}

}  // namespace ui
