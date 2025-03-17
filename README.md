# System-Monitor

## Beschreibung
Der **System-Monitor** ist eine grafische Anwendung zur Überwachung wichtiger Systemressourcen wie CPU-Auslastung, RAM-Nutzung, Festplattenplatz und Netzwerkaktivität. 

## Funktionen
- Anzeige der aktuellen CPU-Auslastung
- Visualisierung der RAM-Nutzung
- Überwachung des verfügbaren Festplattenspeichers
- Analyse der Netzwerkaktivität
- Grafische Darstellung mit **wxGauge** in einer **wxWidgets**-Oberfläche

## Technologien
- **C++** mit **wxWidgets** für die GUI
- **wxGauge** für Balkendiagramme
- **sysinfo** oder **psutil** für das Abrufen von Systemdaten

## Installation & Nutzung
1. wxWidgets installieren (siehe [offizielle Anleitung](https://www.wxwidgets.org/))
2. Abhängigkeiten wie `sysinfo` oder `psutil` einbinden
3. Projekt kompilieren:
   ```sh
   g++ main.cpp -o system-monitor `wx-config --cxxflags --libs`
