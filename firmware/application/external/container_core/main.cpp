/*
 * Container Control Core - Scanner & Detection
 * External App for HackRF/PortaPack (SD Card)
 *
 * Tri-Modal RF Scanner:
 * - Container Mode (shipping containers)
 * - Vehicle Mode (border control)
 * - Maritime Mode (ports/boats)
 */

#include "ui.hpp"
#include "ui_container_control.hpp"
#include "ui_navigation.hpp"
#include "external_app.hpp"

namespace ui::external_app::container_core {
void initialize_app(ui::NavigationView& nav) {
    nav.push<ContainerControlView>();
}
}  // namespace ui::external_app::container_core

extern "C" {

__attribute__((section(".external_app.app_container_core.application_information"), used))
application_information_t _application_information_container_core = {
    /*.memory_location = */ (uint8_t*)0x00000000,
    /*.externalAppEntry = */ ui::external_app::container_core::initialize_app,
    /*.header_version = */ CURRENT_HEADER_VERSION,
    /*.app_version = */ VERSION_MD5,

    /*.app_name = */ "Cntr Core",
    /*.bitmap_data = */ {
        // Icon: Container mit Scanner (16x16 bitmap)
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
    /*.icon_color = */ ui::Color::cyan().v,
    /*.menu_location = */ app_location_t::RX,
    /*.desired_menu_position = */ -1,

    /*.m4_app_tag = */ {0, 0, 0, 0},
    /*.m4_app_offset = */ 0x00000000,
};
}
