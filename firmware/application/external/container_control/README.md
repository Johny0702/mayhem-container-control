# Container Control System - PortaPack App

**Version:** 1.0.0
**Status:** Initial Prototype
**Datum:** 2026-01-08

---

## Übersicht

Standalone PortaPack-App für RX-only Container-Kontrolle.

**Ziel:** Erkennung versteckter Tracking-Geräte in Containern, Fahrzeugen und Fracht.

---

## Features (v1.0 - Prototype)

### Aktuell implementiert:
- ✅ Basic UI Framework
- ✅ Welcome Screen
- ✅ TX-Blocking-Info Display
- ✅ Start Scan Button
- ✅ Device Counter (Demo)

### Geplant (v1.1+):
- 🔶 ISM-Band-Scanning (433/868/915 MHz)
- 🔶 Device Profiler Integration
- 🔶 Proximity Analysis (Sat/Cellular)
- 🔶 Risk Scoring
- 🔶 Evidence Collection
- 🔶 SD Card Export

---

## Dateien

```
external/container_control/
├── main.cpp                        # App entry point & registration
├── ui_container_control.hpp        # UI header
├── ui_container_control.cpp        # UI implementation
└── README.md                       # Diese Datei
```

---

## UI-Layout (240x320 Display)

```
┌─────────────────────────┐
│ Container Control v1.0  │  ← Title
│ RX-Only System          │  ← Subtitle
│                         │
│ Status: Ready           │  ← System info
│ TX: Blocked             │
│ Mode: Authority         │
│                         │
│ Devices: 0              │  ← Device counter
│                         │
│   [ START SCAN ]        │  ← Action button
│                         │
│     [ Back ]            │  ← Back button
└─────────────────────────┘
```

---

## Kompilierung

### 1. Zur external.cmake hinzugefügt ✅

```cmake
# In EXTCPPSRC:
external/container_control/main.cpp
external/container_control/ui_container_control.cpp

# In EXTAPPLIST:
container_control
```

### 2. Build-Befehl

```bash
cd ~/mayhem-firmware
docker run -v "/Users/gs/mayhem-firmware:/havoc" -u "501:20" --rm \
  portapack-dev make firmware
```

### 3. Ergebnis

Nach erfolgreichem Build:
- `build/firmware/application/container_control.ppma` (~XX KB)

---

## Installation auf PortaPack

### Methode 1: SD-Karte (empfohlen)

1. SD-Karte aus PortaPack entfernen
2. Am Computer mounten
3. Kopieren: `container_control.ppma` → `SD:/APPS/`
4. SD-Karte sicher auswerfen
5. In PortaPack einsetzen
6. In Mayhem: Apps → RX → Container Ctrl

### Methode 2: DFU-Mode (Advanced)

```bash
# Firmware flashen
hackrf_spiflash -w build/firmware/portapack-mayhem-firmware.bin
```

---

## Code-Struktur

### main.cpp

```cpp
// Entry point & App registration
namespace ui::external_app::container_control {
    void initialize_app(ui::NavigationView& nav);
}

// App metadata
application_information_t _application_information_container_control = {
    .app_name = "Container Ctrl",
    .icon_color = ui::Color::red().v,
    .menu_location = app_location_t::RX,
    // ...
};
```

### ui_container_control.hpp

```cpp
class ContainerControlView : public View {
public:
    ContainerControlView(NavigationView& nav);
    void focus() override;
    std::string title() const override;

private:
    // UI Elements
    Text text_title;
    Button button_start;
    Button button_back;

    // Event handlers
    void on_start_scan();
};
```

### ui_container_control.cpp

```cpp
ContainerControlView::ContainerControlView(NavigationView& nav) {
    add_children({
        &text_title,
        &button_start,
        &button_back
    });

    button_start.on_select = [this](Button&) {
        this->on_start_scan();
    };
}
```

---

## Nächste Schritte (Development Roadmap)

### Phase 1: Core Libraries (Woche 2-4) 🔶

**Portieren von ~/hackrf_container_system/libs/:**
- Driver Gate (TX-Blocking)
- Device Profiler (Signal-Konsolidierung)
- Proximity Analysis (Sat/Cellular)
- Scanner Engine (Frequency Sweep)
- Evidence System (Hash-Chain)

**Anpassungen:**
- stdlib → Mayhem HAL
- malloc → static allocation
- File I/O → SD Card API
- Floating-point → ARM FPU

### Phase 2: Advanced UI (Woche 5-6) 🔶

**Neue Screens:**
- Container Setup (ID, Location, Profile)
- Scanning Progress (real-time)
- Device List (sortable)
- Device Detail (frequencies, indicators)
- Guided Search (localization)
- Evidence Export

**Navigation:**
- Screen transitions
- Back-button handling
- Touch events
- Encoder support

### Phase 3: Hardware Integration (Woche 7-8) 🔶

**HackRF API:**
- Frequency control
- RSSI measurement
- IQ data collection
- Baseband communication

**SD Card:**
- Config file reading
- Evidence writing
- Session management

### Phase 4: Testing & Polish (Woche 9-10) 🔶

**Tests:**
- Unit tests
- Integration tests
- Field tests
- Performance profiling

**Documentation:**
- User Guide
- Developer Guide
- API Reference

---

## Memory Budget

**Target:**
- Code: < 50 KB
- RAM: < 64 KB
- Stack: < 16 KB

**Current (v1.0 prototype):**
- Code: ~5 KB (estimated)
- RAM: ~2 KB (estimated)

---

## Dependencies

### Mayhem Framework:
- `ui_widget.hpp` - UI elements (Text, Button)
- `ui_navigation.hpp` - Screen navigation
- `portapack.hpp` - Hardware abstraction

### Standard:
- `string_format.hpp` - String utilities

### External (to be added):
- Driver Gate
- Device Profiler
- Proximity Analyzer
- Scanner Engine
- Evidence System

---

## Versionshistorie

### v1.0.0 (2026-01-08) - Initial Prototype
- ✅ Basic UI framework
- ✅ Welcome screen
- ✅ App registration
- ✅ Build integration
- ⏳ Compilation test pending

### v1.1.0 (TBD) - Core Libraries
- 🔶 Driver Gate integration
- 🔶 Device Profiler
- 🔶 Scanner Engine

### v1.2.0 (TBD) - Advanced UI
- 🔶 Multi-screen navigation
- 🔶 Device list & detail
- 🔶 Progress indicators

### v2.0.0 (TBD) - Full Feature Set
- 🔶 Complete hardware integration
- 🔶 Evidence export
- 🔶 Field-tested

---

## Known Issues

### v1.0.0:
- [ ] Nicht kompiliert (noch nicht getestet)
- [ ] Scan-Funktion ist nur Demo
- [ ] Keine echte Hardware-Integration
- [ ] Kein Frequenz-Scanning

---

## Lizenz

GPL v2 (wie Mayhem Firmware)

---

## Credits

**Based on:**
- PortaPack Mayhem Firmware
- HackRF One Hardware
- Original Container Control System (USB-Tethered)

**Entwickelt von:** [Your Name]
**Datum:** 2026-01-08

---

## Support

**Bei Problemen:**
1. Mayhem Discord: https://discord.hackrf.app
2. GitHub Issues: https://github.com/portapack-mayhem/mayhem-firmware
3. Dokumentation: ~/hackrf_container_system/docs/

---

**Status:** Ready for compilation test! 🚀
