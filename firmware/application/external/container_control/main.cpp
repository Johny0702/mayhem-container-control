/*
 * Container Control System - Standalone PortaPack App
 *
 * RX-only SDR system for detecting concealed tracking devices
 * in shipping containers, vehicles, and cargo.
 *
 * Copyright (C) 2026
 * Licensed under GPL v2
 */

#include "ui.hpp"
#include "ui_container_control.hpp"
#include "ui_navigation.hpp"
#include "external_app.hpp"

namespace ui::external_app::container_control {
void initialize_app(ui::NavigationView& nav) {
    nav.push<ContainerControlView>();
}
}  // namespace ui::external_app::container_control

extern "C" {

__attribute__((section(".external_app.app_container_control.application_information"), used))
application_information_t _application_information_container_control = {
    /*.memory_location = */ (uint8_t*)0x00000000,
    /*.externalAppEntry = */ ui::external_app::container_control::initialize_app,
    /*.header_version = */ CURRENT_HEADER_VERSION,
    /*.app_version = */ VERSION_MD5,

    /*.app_name = */ "Container Ctrl",
    /*.bitmap_data = */ {
        // Icon: Container mit Antenne (16x16 bitmap)
        0x00, 0x00,  // ........
        0x7E, 0x00,  // .######.
        0x42, 0x00,  // .#....#.
        0x42, 0x00,  // .#....#.
        0x42, 0x00,  // .#....#.
        0x7E, 0x00,  // .######.
        0x18, 0x00,  // ...##...
        0x3C, 0x00,  // ..####..
        0x7E, 0x00,  // .######.
        0x18, 0x00,  // ...##...
        0x18, 0x00,  // ...##...
        0x00, 0x00,  // ........
        0x00, 0x00,  // ........
        0x00, 0x00,  // ........
        0x00, 0x00,  // ........
        0x00, 0x00,  // ........
    },
    /*.icon_color = */ ui::Color::red().v,
    /*.menu_location = */ app_location_t::RX,  // In RX apps section
    /*.desired_menu_position = */ -1,

    /*.m4_app_tag = portapack::spi_flash::image_tag_none */ {0, 0, 0, 0},
    /*.m4_app_offset = */ 0x00000000,  // will be filled at compile time
};
}
