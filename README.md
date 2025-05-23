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
- **wxDrawEllipseArc** für Kreis Diagramm
- **sysinfo** für das Abrufen von Systemdaten(RAM)
- **statvfs** für das Abrufen von Systemdaten(Drive)

## Installation & Nutzung
1. wxWidgets installieren (siehe [offizielle Anleitung](https://www.wxwidgets.org/))
2. Abhängigkeiten wie `sysinfo` oder `psutil` einbinden
3. Projekt kompilieren:
   ```shell
   mkdir build && cd build
   cmake ..
   make -j8  # use 8 processor cores when compiling
   ```
