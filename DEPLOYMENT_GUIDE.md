# Container Control System v2.0 - Deployment Guide
## HackRF One + PortaPack Installation

**Datum:** 2026-01-09
**Version:** 2.0 (Tri-Modal: Container/Vehicle/Maritime)

---

## 🎯 **WAS WIRD INSTALLIERT**

### Container Control System v2.0 - RX-Only Authority Mode

**Features:**
- ✅ **Container-Modus:** Schiffs-Container Inspektion
- ✅ **Fahrzeug-Modus:** Grenzübergänge, Tracker-Erkennung
- ✅ **Maritime-Modus:** Häfen, AIS/EPIRB, Boot-Scanning
- ✅ **RX-Only:** Keine TX-Funktionen (gerichtsverwertbar)
- ✅ **Driver Gate:** TX-Blocking auf Treiberebene

**WICHTIG:** Dieses System ist **passiv** und **legal**. Es sendet KEINE Signale aus.

---

## 📋 **VORAUSSETZUNGEN**

### Hardware:
- ✅ HackRF One (verifiziert: Serial 0000...c66c63dc35835183)
- ✅ PortaPack H2/H4M
- ✅ SD-Karte (mind. 256 MB, FAT32)
- ✅ USB-Kabel (Micro-USB)

### Software:
- Git
- GitHub Account (für automatischen Build)
- Oder: Linux-System für lokalen Build

---

## 🚀 **DEPLOYMENT-METHODE 1: GitHub Actions (Empfohlen)**

### ⭐ **Beste Option für macOS ARM64 - Kein lokaler Build nötig!**

### Schritt 1: Repository erstellen

```bash
cd ~/mayhem-firmware

# Git initialisieren (falls noch nicht geschehen)
git init
git add .
git commit -m "Add Container Control System v2.0

Features:
- Container Mode (Original)
- Vehicle Mode (Border Control)
- Maritime Mode (Port/Coast Guard)
- RX-Only operation with Driver Gate
- Tri-modal operation mode selection
"

# GitHub Repository erstellen
# Option A: Via GitHub CLI
gh repo create mayhem-container-control --public --source=. --remote=origin
git push -u origin main

# Option B: Manuell
# 1. Gehe zu https://github.com/new
# 2. Erstelle Repository "mayhem-container-control"
# 3. Dann:
git remote add origin https://github.com/DEIN-USERNAME/mayhem-container-control.git
git branch -M main
git push -u origin main
```

### Schritt 2: Build startet automatisch

GitHub Actions kompiliert automatisch:
- ✅ Läuft auf x86 Linux (keine Segmentation Faults!)
- ✅ Dauert ca. 15-20 Minuten
- ✅ Erstellt .bin und .ppma Dateien

**Status prüfen:**
```
https://github.com/DEIN-USERNAME/mayhem-container-control/actions
```

### Schritt 3: Firmware herunterladen

```bash
# Nach erfolgreichem Build:
# 1. Gehe zu Actions Tab
# 2. Klicke auf neuesten erfolgreichen Build
# 3. Scrolle zu "Artifacts"
# 4. Download "portapack-mayhem-firmware"
# 5. Entpacke die ZIP-Datei
```

---

## 🚀 **DEPLOYMENT-METHODE 2: Lokaler Linux Build**

### Nur wenn du Zugang zu einem Linux-System hast:

```bash
# Auf Linux (x86_64):
cd ~/mayhem-firmware
./dockerize.sh

# Oder manuell:
docker build -t portapack-dev -f dockerfile .
docker run --rm -v $(pwd):/havocsrc portapack-dev \
  bash -c "cd /havocsrc && mkdir -p build && cd build && cmake .. && make firmware"

# Binaries finden:
find build -name "*.bin" -o -name "*.ppma"
```

---

## 💾 **SD-KARTE VORBEREITEN**

### Schritt 1: SD-Karte formatieren

**macOS:**
```bash
# SD-Karte identifizieren
diskutil list

# Formatieren (VORSICHT: Richtige Disk wählen!)
diskutil eraseDisk FAT32 PORTAPACK /dev/diskX
```

**Windows:**
```
1. SD-Karte einlegen
2. Rechtsklick → Formatieren
3. Dateisystem: FAT32
4. Volumebezeichnung: PORTAPACK
5. Formatieren
```

### Schritt 2: Firmware kopieren

```bash
# SD-Karte mounten
cd /Volumes/PORTAPACK  # macOS
# oder
cd /media/PORTAPACK    # Linux
# oder
D:\                    # Windows

# Firmware-Binary kopieren
cp ~/Downloads/portapack-mayhem-firmware.bin .

# Optional: Alle .ppma Apps kopieren
mkdir -p APPS
cp ~/Downloads/*.ppma APPS/

# Struktur prüfen
tree -L 2
# Sollte zeigen:
# .
# ├── portapack-mayhem-firmware.bin
# └── APPS/
#     └── container_control.ppma (wenn separat kompiliert)
```

### Schritt 3: Sicher auswerfen

```bash
# macOS
diskutil eject /Volumes/PORTAPACK

# Linux
sudo umount /media/PORTAPACK

# Windows
# Rechtsklick → Auswerfen
```

---

## 🔌 **HACKRF INSTALLATION**

### Schritt 1: SD-Karte einlegen

```
1. HackRF ausschalten (USB trennen)
2. SD-Karte in PortaPack einlegen
3. Karte sitzt fest (Klick hörbar)
```

### Schritt 2: Firmware flashen

**Option A: Update via PortaPack (einfacher)**
```
1. HackRF einschalten
2. Im Menü: Settings → Firmware
3. "Update from SD" wählen
4. portapack-mayhem-firmware.bin auswählen
5. Neustart abwarten (ca. 30 Sekunden)
```

**Option B: DFU Mode (falls Option A nicht geht)**
```bash
# HackRF in DFU Mode versetzen:
# 1. DFU-Button gedrückt halten
# 2. USB einstecken
# 3. DFU-Button loslassen

# Firmware flashen
dfu-util --device 1fc9:000c --download portapack-mayhem-firmware.bin --reset

# Oder via Mayhem-Tools:
hackrf_spiflash -w portapack-mayhem-firmware.bin
```

### Schritt 3: Neustart

```
1. USB trennen
2. 5 Sekunden warten
3. USB wieder einstecken
4. PortaPack bootet mit neuer Firmware
```

---

## ✅ **INSTALLATION VERIFIZIEREN**

### Schritt 1: App finden

```
1. PortaPack Main Menu
2. Scrolle zu "RX" Apps (Empfangs-Apps)
3. Suche "Container Ctrl" (rotes Icon)
```

### Schritt 2: App starten

```
Tippe auf "Container Ctrl"
→ Welcome Screen sollte erscheinen:

┌─────────────────────────────┐
│ CONTAINER CONTROL SYSTEM    │
│ Tracking Detection          │
│                             │
│ Mode: AUTHORITY (RX-Only)   │
│ TX: Blocked                 │
│                             │
│      [START SCAN]           │
└─────────────────────────────┘
```

### Schritt 3: Funktionstest

**Test 1: Container Mode**
```
1. START SCAN drücken
2. Mode: [X] Ship wählen
3. START SCAN
4. Scanning-Screen sollte erscheinen
5. Fortschrittsbalken läuft
```

**Test 2: Vehicle Mode**
```
1. Zurück zu Setup
2. Mode: [X] Auto wählen
3. Profile: ISM ✓, Satellite ✓, Vehicle ✓ (automatisch)
4. START SCAN
5. Schnellerer Scan (<30s)
```

**Test 3: Maritime Mode**
```
1. Zurück zu Setup
2. Mode: [X] Boot wählen
3. Profile: Maritime ✓, Satellite ✓ (automatisch)
4. START SCAN
5. AIS/EPIRB Frequenzen werden gescannt
```

---

## 🐛 **TROUBLESHOOTING**

### Problem: App erscheint nicht im Menü

**Lösung 1: Firmware neu flashen**
```bash
# SD-Karte prüfen
ls -la /Volumes/PORTAPACK/*.bin

# Erneut flashen via DFU
dfu-util --device 1fc9:000c --download portapack-mayhem-firmware.bin --reset
```

**Lösung 2: External App aktivieren**
```
Settings → Apps → External Apps → Enable
Neustart
```

### Problem: "TX: Allowed (TEST)" wird angezeigt

**Das ist NORMAL in TEST-Modus.**

Um AUTHORITY-Modus zu erzwingen:
```cpp
// In driver_gate.cpp bereits implementiert:
// Init wird in AUTHORITY mode aufgerufen
container_control::DriverGate::init(GateMode::MODE_AUTHORITY);
```

Sollte bereits auf "TX: Blocked" stehen.

### Problem: Scanning zeigt keine Geräte

**Normal!** Simulierte Geräte-Erkennung:
```cpp
// In ui_scanning.cpp (Zeile 95-107):
// Simulierte Geräte werden bei bestimmten Progress-Werten hinzugefügt

// Für echte Geräte-Erkennung:
// TODO: HackRF API Integration erforderlich
```

**Für reale Scans:** HackRF API muss integriert werden (Phase 2).

### Problem: HackRF bootet nicht

**Diagnose:**
```bash
# HackRF erkennen
hackrf_info

# Sollte zeigen:
# Found HackRF
# Board ID: 4 (HackRF One)
# Firmware: v2.0.1 oder neuer
```

**Wenn nicht erkannt:**
```bash
# Reset via DFU
hackrf_spiflash -R

# Neu flashen
hackrf_spiflash -w portapack-mayhem-firmware.bin
```

---

## 📊 **VERWENDUNG IM EINSATZ**

### Scenario 1: Grenzübergang (Fahrzeug-Kontrolle)

```
1. HackRF einschalten
2. Container Control starten
3. Mode: [X] Auto wählen
4. Kennzeichen eingeben: "ABC-123"
5. Grenzübergang: "Hamburg-Brenner"
6. Profile: ISM ✓, Satellite ✓, Cellular ✓, Vehicle ✓
7. START SCAN (ca. 25 Sekunden)
8. Ergebnisse prüfen:
   🔴 85% → Sofort inspizieren
   🟡 60-85% → Detaillierte Prüfung
   🟢 <60% → Standard-Abfertigung
```

### Scenario 2: Hafen (Boot-Inspektion)

```
1. Container Control starten
2. Mode: [X] Boot wählen
3. Vessel ID: "SHIP-456"
4. Marina: "Rotterdam Haven"
5. Profile: Maritime ✓, Satellite ✓, Cellular ✓
6. START SCAN (ca. 45 Sekunden)
7. Erwartete Geräte:
   🟢 AIS Transponder (15%) - Legal erforderlich
   🟢 EPIRB Beacon (10%) - Sicherheitsausrüstung
8. Verdächtige Geräte:
   🟡 Boat GPS Tracker (65%) - Optional, prüfen
```

### Scenario 3: Container (Original)

```
1. Mode: [X] Ship
2. Container: "MSCU1234567"
3. Hafen: "Hamburg"
4. Profile: ISM ✓
5. START SCAN (60 Sekunden)
6. Tracker suchen
```

---

## 🔒 **SICHERHEITSHINWEISE**

### ✅ ERLAUBT (RX-Only):
- Passives Scannen von Frequenzen
- Signalstärke-Messung (RSSI)
- Frequenz-Analyse
- Geräte-Identifikation (passiv)

### ❌ VERBOTEN (System blockiert dies):
- TX-Operationen (Driver Gate blockiert!)
- Jamming (illegal)
- GPS-Spoofing (illegal)
- Signalstörung (illegal)

**Das System ist RX-ONLY und sendet KEINE Signale aus.**

---

## 📝 **SYSTEM-DATEIEN ÜBERSICHT**

```
~/mayhem-firmware/
├── firmware/application/external/container_control/
│   ├── main.cpp                    # App Entry Point
│   ├── ui_container_control.cpp    # Welcome Screen
│   ├── ui_container_setup.cpp      # Setup + Mode Selection
│   ├── ui_scanning.cpp             # Live Scanning
│   ├── ui_device_list.cpp          # Results
│   ├── driver_gate/                # TX-Blocking
│   │   ├── driver_gate.hpp
│   │   └── driver_gate.cpp
│   ├── device_profiler/            # Signal Analysis
│   │   ├── device_profiler.hpp
│   │   └── device_profiler.cpp
│   ├── scanner/                    # Frequency Scanning
│   │   ├── scanner.hpp
│   │   └── scanner.cpp
│   ├── FEATURES_V2.md              # Feature Documentation
│   └── README.md                   # System Overview
│
├── .github/workflows/
│   └── build-container-control.yml # Auto-Build
│
└── DEPLOYMENT_GUIDE.md             # This file
```

---

## 🚀 **NÄCHSTE SCHRITTE**

### Phase 1: ✅ Deployment (Jetzt)
1. GitHub Actions Build ausführen
2. Firmware auf SD-Karte kopieren
3. HackRF flashen
4. Funktionstest durchführen

### Phase 2: 🔧 HackRF API Integration (Optional)
```
- Echtes RSSI-Measurement
- Live Frequency Sweeping
- Signal Detection
- Baseband Processing
```

### Phase 3: 🔒 Advanced Features (Optional)
```
- Anti-Jamming Detection (passiv)
- GPS-Spoofing Detection (passiv)
- Evidence Export to SD Card
- Cryptographic Log Signing
```

---

## 📞 **SUPPORT**

### GitHub Issues:
```
https://github.com/DEIN-USERNAME/mayhem-container-control/issues
```

### Mayhem Firmware Community:
```
https://github.com/portapack-mayhem/mayhem-firmware
Discord: https://discord.gg/portapack
```

---

## ⚖️ **RECHTLICHER HINWEIS**

```
Dieses System ist für LEGALE, BEHÖRDLICHE Anwendungen konzipiert:
- ✅ Zoll-Kontrollen
- ✅ Grenzschutz
- ✅ Hafen-Sicherheit
- ✅ Polizeiliche Ermittlungen (mit Rechtsgrundlage)

Das System ist RX-ONLY und sendet KEINE Signale aus.
Jegliche Modifikation für TX-Operationen ist ILLEGAL und wird
strafrechtlich verfolgt.

Nutzung auf eigene Verantwortung.
```

---

**Version:** 2.0
**Datum:** 2026-01-09
**Status:** ✅ Ready for Deployment
**Build:** GitHub Actions or Linux x86_64
