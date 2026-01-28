# Projekt unter Windows mit WSL einrichten

Diese Anleitung erklärt, wie du das Repository auf einem Windows‑PC mit WSL (Windows Subsystem for Linux) einrichtest und ausführst.


## 1) WSL und Ubuntu installieren

Führe die folgenden Schritte in PowerShell (als Administrator) aus:

```powershell
wsl --install
# optional: bestimmte Distribution wählen (z. B. Ubuntu)
wsl --install -d Ubuntu
```

Starte Windows neu, öffne danach „Ubuntu“ aus dem Startmenü und lege einen Linux‑Benutzer an.

Prüfe anschließend die Version und Distribution:

```bash
wsl.exe -l -v    # Windows-Seite
uname -a         # in Ubuntu
```

Empfohlen sind WSL2 und Ubuntu 22.04 oder 24.04.

## 2) Repository unter WSL klonen

Klone das Projekt in ein Verzeichnis deiner Wahl.
Hinweis: Das Projekt sollte im Linux-Dateisystem liegen, da Builds sonst langsamer sind.

```bash
git clone https://github.com/Lukas-Wollmann/IT-Projekt-WS25.git
cd IT-Projekt-WS25
```

## 3) VS Code mit WSL verbinden

Jetzt richtest du VS Code ein, um das Projekt komfortabel zu bearbeiten:

1. Installiere in Windows Visual Studio Code (falls noch nicht vorhanden).
2. Installiere die Erweiterung „WSL" (Publisher: Microsoft) in VS Code.
3. Starte VS Code und wähle „WSL: Connect to WSL" (unten links auf das grüne WSL‑Symbol klicken oder `F1` → „WSL").
4. Klicke auf „Open Folder…" und wähle den Ordner des geklonten Projekts (z. B. `~/IT-Projekt-WS25`).
5. Installiere die empfohlene Erweiterung:
   - **C/C++** (Microsoft) für IntelliSense, Debugging und Code-Navigation

Ab jetzt arbeitest du im VS Code Remote-WSL-Fenster. Das integrierte Terminal in VS Code läuft automatisch in WSL.

## 4) Abhängigkeiten installieren

Das Repo enthält ein Skript, das die benötigten Pakete installiert (Clang, Make, LLVM, GDB, clang-format).
Öffne das integrierte Terminal in VS Code (`` Strg+` `` oder Menü „Terminal" → „New Terminal") und führe aus:

```bash
chmod +x ./dependencies.sh
./dependencies.sh
```

Falls du manuell installieren möchtest:

```bash
sudo apt-get update
sudo apt-get install -y clang make llvm gdb clang-format
```

## 5) Bauen, ausführen und testen

Das `Makefile` stellt Ziele zum Bauen der App und der Tests bereit.
Nutze das integrierte Terminal in VS Code:

```bash
# Build und App starten
make           # baut bin/app
make run       # startet bin/app

# Tests bauen und ausführen
make test      # baut und startet bin/test_runner
```

Artefakte liegen unter `bin/` (ausführbare Dateien) und `build/` (Objekte). Quellen liegen in `src/`, Tests in `test/`.

## 6) Typischer Workflow

Wenn du regelmäßig am Projekt arbeitest:

```bash
git pull          # Änderungen holen
make -j           # schnell bauen (parallel)
make test         # Tests ausführen
```

## 7) Hilfe

- Offizielles WSL‑Doc: https://learn.microsoft.com/windows/wsl/
- VS Code WSL‑Doc: https://code.visualstudio.com/docs/remote/wsl