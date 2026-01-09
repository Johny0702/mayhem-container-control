# Container Control System v2.0 - Feature Documentation
## Tri-Modal Authority Tracking Detection

**Version:** 2.0
**Datum:** 2026-01-09
**Erweiterungen:** Vehicle Mode (Grenzübergänge), Maritime Mode (Häfen/Boote)

---

## 📋 OPERATION MODES

### 1. CONTAINER-MODUS (Original)
**Anwendung:** Schiffs-Container Inspektion (Häfen, Zoll)

**Scan-Profile:**
- ISM Bands (433/868/915 MHz)
- Satelliten-Tracker (GPS, Iridium)
- Cellular-Tracker (GSM/LTE)

**Typische Einsatzzeit:** 60 Sekunden
**Fokus:** Verdeckte Tracking-Geräte in Fracht

---

### 2. FAHRZEUG-MODUS ⭐ NEU
**Anwendung:** Grenzübergänge, Zoll, Polizei-Kontrollen

**Scan-Profile:**
```
433.05 - 434.79 MHz    ISM (Magnetische GPS-Tracker)
824 - 894 MHz          GSM 850 (Americas, Asien)
880 - 960 MHz          GSM 900 (Europa, Afrika, Asien)
1574 - 1577 MHz        GPS L1 (Hauptfrequenz für Tracker)
```

**Optimierungen:**
- **Schneller Scan:** <30 Sekunden
- **Gröbere Schritte:** 50-200 kHz für schnelle Durchfahrt
- **Fokus:** Externe Magnettracker und versteckte GPS-Geräte

**Erkannte Geräte:**
| Device Type | Risk Score | Beschreibung |
|-------------|------------|--------------|
| Magnetic Tracker | 85% 🔴 | Extern angebracht, höchste Priorität |
| Vehicle GPS Tracker | 80% 🔴 | GPS+Cellular Kombination |
| OBD-II Tracker | 70% 🟡 | Fahrzeugintegration erforderlich |

---

### 3. MARITIME-MODUS ⚓ NEU
**Anwendung:** Häfen, Küstenwache, Marina-Kontrollen

**Scan-Profile:**
```
156 - 174 MHz          Marine VHF (Kommunikation & Tracker)
161.965 - 161.985 MHz  AIS Channel A (5 kHz Präzision!)
162.015 - 162.035 MHz  AIS Channel B (5 kHz Präzision!)
406.020 - 406.030 MHz  EPIRB Notfall-Bake (5 kHz Präzision)
880 - 960 MHz          GSM 900 (Küsten-Cellular)
1574 - 1577 MHz        GPS L1 (Marine GPS)
1525 - 1559 MHz        Inmarsat (Satelliten-Telefonie)
1616 - 1626.5 MHz      Iridium (Satelliten-Telefonie)
```

**Besonderheiten:**
- **AIS-Präzision:** 5 kHz Schritte für exakte Kanaltrennung
- **EPIRB-Erkennung:** Notfall-Baken (Sicherheitsausrüstung)
- **Maritime VHF:** Standard-Funkgeräte (niedrige Risk-Scores)

**Erkannte Geräte:**
| Device Type | Risk Score | Beschreibung |
|-------------|------------|--------------|
| AIS Transponder | 15% 🟢 | Legal vorgeschrieben für Schiffe >300 GT |
| EPIRB Beacon | 10% 🟢 | Sicherheitsausrüstung, SOLAS-konform |
| Marine VHF Radio | 15% 🟢 | Standard-Kommunikation |
| Boat GPS Tracker | 65% 🟡 | Optional, potenziell verdächtig |
| Marine Sat Tracker | 60% 🟡 | Optional, Überwachungs-Indikator |

---

## 🎨 USER INTERFACE

### Mode Selection Screen
```
╔═══════════════════════════════════╗
║  New Inspection                    ║
║                                    ║
║  Operation Mode:                   ║
║  [X] Ship  [ ] Auto  [ ] Boot     ║
║                                    ║
║  ID/Plate: __________              ║
║  Location: __________              ║
║                                    ║
║  Scan Profiles:                    ║
║  [X] ISM   [ ] Sat   [ ] Cell     ║
║  [ ] WiFi  [ ] Veh   [ ] Mar      ║
║                                    ║
║         [START SCAN]               ║
╚═══════════════════════════════════╝
```

### Intelligente Label-Anpassung

**Ship Mode:**
- ID-Feld: "ID/Container:"
- Location: "Port:"
- Default Profile: ISM ✓

**Auto Mode:**
- ID-Feld: "Plate/VIN:"
- Location: "Border:"
- Default Profiles: ISM ✓, Satellite ✓, Cellular ✓, Vehicle ✓

**Boot Mode:**
- ID-Feld: "Vessel ID:"
- Location: "Port/Marina:"
- Default Profiles: Satellite ✓, Cellular ✓, Maritime ✓

---

## 🔍 DEVICE CLASSIFICATION

### Klassifizierungs-Logik

```
Priority 1: Maritime-Specific (höchste Priorität)
  if has_ais_frequency():
      → TYPE_AIS_TRANSPONDER
  if has_epirb_frequency():
      → TYPE_EPIRB_BEACON
  if has_marine_vhf() AND NOT has_cellular():
      → TYPE_MARINE_VHF

Priority 2: Vehicle-Specific
  if has_ism() AND has_satellite():
      → TYPE_MAGNETIC_TRACKER
  if has_cellular() AND has_satellite() AND freq_count >= 2:
      → TYPE_VEHICLE_GPS_TRACKER

Priority 3: Maritime Trackers
  if has_satellite() AND has_marine_vhf():
      → TYPE_MARINE_SATELLITE_TRACKER
  if has_satellite() AND has_cellular():
      → TYPE_BOAT_GPS_TRACKER

Priority 4: Generic Classification
  → Fallback zu Standard-Klassifizierung
```

---

## 📊 RISK ASSESSMENT MATRIX

### Container/Generic Devices
```
TYPE_SATELLITE_TRACKER        85%  🔴  Kritisch
TYPE_CELLULAR_TRACKER         75%  🟡  Hoch
TYPE_ISM_TRACKER              60%  🟡  Medium
TYPE_DRONE_COMPONENT          70%  🟡  Hoch
TYPE_BLE_BEACON               30%  🟢  Niedrig
TYPE_WIFI_DEVICE              20%  🟢  Niedrig
TYPE_KEY_FOB                  10%  🟢  Sehr niedrig
```

### Vehicle Trackers (Border Control)
```
TYPE_MAGNETIC_TRACKER         85%  🔴  Sehr hoch - extern angebracht
TYPE_VEHICLE_GPS_TRACKER      80%  🔴  Hoch - verdecktes Tracking
TYPE_OBD_TRACKER              70%  🟡  Medium - Fahrzeugzugang nötig
```

### Maritime Devices (Port/Coast Guard)
```
TYPE_AIS_TRANSPONDER          15%  🟢  Niedrig - vorgeschrieben
TYPE_EPIRB_BEACON             10%  🟢  Niedrig - Sicherheit
TYPE_MARINE_VHF               15%  🟢  Niedrig - Standard-Funk
TYPE_BOAT_GPS_TRACKER         65%  🟡  Medium-hoch - optional
TYPE_MARINE_SATELLITE_TRACKER 60%  🟡  Medium - optional
```

### Risk Modifiers
```
+10%  Multiple Frequenzen (>2)
+15%  Satellite Proximity
+10%  Cellular Proximity
Max:  100%
```

---

## 🔧 TECHNISCHE SPEZIFIKATIONEN

### Scanner-Konfiguration

**ISM Profile:**
- 433.05 - 434.79 MHz (25 kHz steps)
- 863 - 870 MHz (25 kHz steps)
- 902 - 928 MHz (25 kHz steps)

**Satellite Profile:**
- 1574 - 1577 MHz (100 kHz steps, GPS L1)
- 1616 - 1626.5 MHz (100 kHz steps, Iridium)

**Cellular Profile:**
- 880 - 915 MHz (200 kHz steps, GSM 900 UL)
- 925 - 960 MHz (200 kHz steps, GSM 900 DL)
- 1710 - 1785 MHz (200 kHz steps, GSM 1800 UL)
- 1805 - 1880 MHz (200 kHz steps, GSM 1800 DL)

**Vehicle Profile:** ⭐ NEU
- 433.05 - 434.79 MHz (50 kHz steps, schneller)
- 824 - 849 MHz (200 kHz steps, GSM 850 UL)
- 869 - 894 MHz (200 kHz steps, GSM 850 DL)
- 880 - 915 MHz (200 kHz steps, GSM 900 UL)
- 925 - 960 MHz (200 kHz steps, GSM 900 DL)
- 1574 - 1577 MHz (200 kHz steps, GPS L1)

**Maritime Profile:** ⚓ NEU
- 156 - 174 MHz (25 kHz steps, Marine VHF)
- 161.965 - 161.985 MHz (5 kHz steps, AIS A)
- 162.015 - 162.035 MHz (5 kHz steps, AIS B)
- 406.02 - 406.03 MHz (5 kHz steps, EPIRB)
- 880 - 960 MHz (200 kHz steps, GSM 900)
- 1574 - 1577 MHz (100 kHz steps, GPS L1)
- 1525 - 1559 MHz (200 kHz steps, Inmarsat)
- 1616 - 1626.5 MHz (200 kHz steps, Iridium)

### Memory Footprint
```
Static Allocation (ARM Cortex-M4):
- DeviceProfile[32]           ~4 KB
- ScanResult[256]             ~2 KB
- FrequencyRange[8]           <1 KB
- UI Screens                  ~2 KB
Total Estimated:              ~9 KB RAM
```

### Scan Performance
```
Mode       Steps  Time    Priority  Use Case
─────────────────────────────────────────────
Container  ~500   60s     Medium    Thorough inspection
Vehicle    ~300   25s     Fast      Quick border check
Maritime   ~800   45s     Precise   AIS/EPIRB detection
```

---

## 🔒 SECURITY & COMPLIANCE

### TX-Blocking (Driver Gate)
```
AUTHORITY Mode (Default):
  ✓ All TX operations blocked at driver level
  ✓ RX-only enforcement
  ✓ No signal interference possible
  ✓ Court-admissible evidence

TEST Mode (Development):
  ⚠ TX allowed for testing
  ⚠ Not for production use
```

### Evidence Integrity
- Timestamp logging
- Frequency/RSSI recording
- Device classification metadata
- Risk score calculation audit trail

### Legal Compliance
- **Passive Scanning Only:** Keine aktive Aussendung
- **EMVG-konform:** Keine Störung fremder Funkdienste
- **Gerichtsverwertbar:** Dokumentierte Messmethodik

---

## 📈 OPERATIONAL GUIDELINES

### Best Practices - Grenzübergang (Vehicle)

1. **Vorbereitung:**
   - Modus: [X] Auto
   - Profile: ISM + Satellite + Cellular + Vehicle
   - Zeit einplanen: 30 Sekunden pro Fahrzeug

2. **Durchführung:**
   - HackRF nahe Fahrzeug positionieren
   - Vollständiger Scan (25-30s)
   - Ergebnisse dokumentieren

3. **Bewertung:**
   - 🔴 85%+ → Sofortige Inspektion
   - 🟡 60-85% → Detaillierte Prüfung
   - 🟢 <60% → Standard-Abfertigung

### Best Practices - Hafen (Maritime)

1. **Vorbereitung:**
   - Modus: [X] Boot
   - Profile: Satellite + Cellular + Maritime
   - AIS-Abgleich vorbereiten

2. **Durchführung:**
   - AIS-Transponder erwarten (legal required)
   - EPIRB prüfen (Sicherheitsausrüstung)
   - Zusätzliche Tracker identifizieren

3. **Bewertung:**
   - AIS + EPIRB vorhanden: ✓ Standard
   - Zusätzliche Tracker >60%: ⚠ Prüfen
   - Fehlende AIS bei Pflichtschiff: 🔴 Kritisch

---

## 🚀 NEXT STEPS

### Deployment

1. **Kompilierung abwarten:**
   - Build läuft im Docker-Container
   - Output: `portapack-mayhem-firmware.bin`

2. **SD-Karte vorbereiten:**
   ```bash
   cp build/firmware/portapack-mayhem-firmware.bin /Volumes/BOOT/
   ```

3. **HackRF Neustart:**
   - SD-Karte einlegen
   - PortaPack neustarten
   - App im Menü unter "RX" finden

### Testing

1. **Container Mode:** Original-Funktionalität testen
2. **Vehicle Mode:** Mit bekanntem GPS-Tracker testen
3. **Maritime Mode:** AIS-Erkennung mit echtem Transponder verifizieren

---

## 📝 VERSION HISTORY

### v2.0 (2026-01-09) - Tri-Modal Extension
- ✅ Vehicle Mode für Grenzübergänge
- ✅ Maritime Mode für Häfen/Boote
- ✅ 5 neue Device Types (Fahrzeuge)
- ✅ 5 neue Device Types (Maritime)
- ✅ Operation Mode Selection UI
- ✅ Intelligente Profil-Vorauswahl
- ✅ Angepasste Risk Scores
- ✅ AIS/EPIRB Präzisions-Scanning

### v1.0 (2026-01-08) - Initial Release
- Container Control System (Behördenmodus)
- Driver Gate TX-Blocking
- Device Profiler mit Risk Scores
- 4 UI Screens (Welcome, Setup, Scanning, Device List)

---

**Entwickelt für:** PortaPack Mayhem Firmware
**Hardware:** HackRF One + PortaPack H2
**Lizenz:** GNU GPL v2.0 or later
**Status:** ✅ Code Complete - Build läuft
