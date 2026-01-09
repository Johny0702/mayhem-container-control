/*
 * Container Control Security - Defensive Monitoring
 * External App for HackRF/PortaPack (SD Card)
 *
 * Security Modules:
 * - Anti-Jamming Detection
 * - GPS-Spoofing Detection
 * - Advanced Threat Detection
 * - Forensic Evidence Logging
 * - Admin Security (2FA)
 */

#include "ui.hpp"
#include "ui_security_dashboard.hpp"
#include "ui_navigation.hpp"
#include "external_app.hpp"

namespace ui::external_app::container_security {
void initialize_app(ui::NavigationView& nav) {
    nav.push<SecurityDashboardView>();
}
}  // namespace ui::external_app::container_security

extern "C" {

__attribute__((section(".external_app.app_container_security.application_information"), used))
application_information_t _application_information_container_security = {
    /*.memory_location = */ (uint8_t*)0x00000000,
    /*.externalAppEntry = */ ui::external_app::container_security::initialize_app,
    /*.header_version = */ CURRENT_HEADER_VERSION,
    /*.app_version = */ VERSION_MD5,

    /*.app_name = */ "Cntr Security",
    /*.bitmap_data = */ {
        // Icon: Shield mit Checkmark (16x16 bitmap)
        0x00, 0x00,  // ........
        0x18, 0x00,  // ...##...
        0x3C, 0x00,  // ..####..
        0x7E, 0x00,  // .######.
        0xFF, 0x00,  // ########
        0xFF, 0x01,  // #########
        0xFF, 0x01,  // #########
        0xCF, 0x01,  // ##..####
        0x87, 0x01,  // ###....#
        0x03, 0x01,  // ##......
        0x01, 0x00,  // #.......
        0x00, 0x00,  // ........
        0x00, 0x00,  // ........
        0x00, 0x00,  // ........
        0x00, 0x00,  // ........
        0x00, 0x00,  // ........
    },
    /*.icon_color = */ ui::Color::red().v,
    /*.menu_location = */ app_location_t::RX,
    /*.desired_menu_position = */ -1,

    /*.m4_app_tag = */ {0, 0, 0, 0},
    /*.m4_app_offset = */ 0x00000000,
};
}
