# System-Monitor

## Beschreibung
Der **System-Monitor** ist eine grafische Anwendung zur Überwachung wichtiger Systemressourcen wie CPU-Auslastung, RAM-Nutzung, Festplattenplatz und Netzwerkaktivität. 

## Funktionen
- Anzeige der aktuellen CPU-Auslastung
- Visualisierung der RAM-Nutzung
- Überwachung des verfügbaren Festplattenspeichers
- Analyse der Netzwerkaktivität
- Grafische Darstellung in einer **wxWidgets**-Oberfläche

## Technologien
- **C++** mit **wxWidgets** für die GUI
- **WX Technologien:**
   - 
   - **wxDrawEllipseArc** für Kreis Diagramm
- **sysinfo** für das Abrufen von Systemdaten(RAM)
- **statvfs** für das Abrufen von Systemdaten(Drive)
- **CPU Berechnung**: Source [stackoverflow](https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux/23376195#23376195)
- **SSID find**: Source [iwgetid](https://linux.die.net/man/8/iwgetid)
- **Timing in Network**: std::chrono
- **for primary interface get**: /proc/net/wireless + regex

## Installation & Nutzung
1. wxWidgets installieren (siehe [offizielle Anleitung](https://www.wxwidgets.org/))
2. Projekt kompilieren:
   ```shell
   mkdir build && cd build
   cmake ..
   make -j8  # use 8 processor cores when compiling
   ```
