## Quick context for AI coding agents

This repository implements an open-source animal EID tag reader built for ESP32-based LilyGo AMOLED Plus hardware (OTR-AMOLED-Plus). Primary language is C++ using the Arduino framework and PlatformIO. The UI is built with LVGL and generated UI source lives under `Code/OTR-AMOLED-Plus/src/ui` (SquareLine exports .sll/.spj into the repo).

Key locations and responsibilities:
- `Code/OTR-AMOLED-Plus/platformio.ini` — primary PlatformIO project config. Use this to discover default environment, build flags, platform version (`espressif32@6.7.0`) and library dependencies.
- `Code/OTR-AMOLED-Plus/src/` — application C++ sources. Look for `otrUI.*`, `otrWebInterface.*`, `otrData.*`, and `initSequence.*` for startup and main features.
- `Code/OTR-AMOLED-Plus/src/ui/` — LVGL UI C sources generated from SquareLine. Screens are in `screens/` and fonts in `fonts/` (example: `ui_font_BIG.c`).
- `Code/OTR-AMOLED-Plus/boards/` — custom board JSONs referenced by `platformio.ini` via `boards_dir` (used to target specific LilyGo hardware).
- `Code/OTR-AMOLED-Plus/data/` and `Code/OTR-AMOLED-Plus/html/` — static web UI and CSV files used at runtime (LittleFS/SD). CSVs such as `tag_status.csv` are read from LittleFS.
- `build_instructions/build_instructions.md` — higher-level steps for building and casting images; consult for device-specific notes.

Why things are organized this way
- UI is designed visually in SquareLine and exported as plain C into `src/ui` — edits to UI should be made with SquareLine and then re-exported rather than hand-editing the generated files.
- The project uses PlatformIO to manage toolchain, locked `platform = espressif32@6.7.0`, and a custom partition table `default_16MB.csv` (16MB flash + LittleFS). Look at `platformio.ini` for build flags (e.g. `BOARD_HAS_PSRAM`, `DOTR_DEBUG`).

Build / run / debug notes (Windows PowerShell)
- Build: `pio run -e <env>` (use the default env listed in `Code/OTR-AMOLED-Plus/platformio.ini`).
- Upload: `pio run -t upload -e <env> -p COM3` (replace `COM3` with your device port and `<env>` with the env name from `platformio.ini`).
- Serial monitor: `pio device monitor -e <env>` (the project sets `monitor_filters` to include the `esp32_exception_decoder` — use the same env to get decoded traces).
- VS Code / PlatformIO: use the PlatformIO extension. The repo contains `.vscode/launch.json` that points to the `.pio/build/<env>/firmware.elf` for native debugging steps.

Integration points and patterns an AI should follow
- RFID input: tags are displayed in the UI (see `Code/OTR-AMOLED-Plus/src/ui/*` where labels such as `ui_Main_Label_RFID` exist). If you need to find the serial/RFID handler, search under `Code/OTR-AMOLED-Plus/src/` for `Serial`/`Serial1` or functions named `validateRFID`/`read_tags` — the handler may be colocated with other device code in `src/`.
- Persistence: static CSV and HTML assets are stored under `Code/OTR-AMOLED-Plus/data/` and served from LittleFS or SD at runtime. When adding persistence, follow existing patterns of storing small CSVs in the `data/` folder and opening with filesystem APIs.
- Web UI: an async webserver exposes static files from SD/LittleFS (see `Code/OTR-AMOLED-Plus/src/otrWebInterface.cpp` and the `Code/OTR-AMOLED-Plus/html/` folder). When adding endpoints, mirror existing patterns (serveStatic, small JSON payloads, simple handlers that log with Serial).
- UI edits: avoid editing generated `Code/OTR-AMOLED-Plus/src/ui/*` files directly. Instead, update the SquareLine project used to generate the UI assets and re-export into `src/ui`.

Conventions and small rules to follow
- Prefer PlatformIO env flags for target-specific behavior. Do not hardcode board-related defines; use `platformio.ini` build_flags when possible.
- Logging uses `Serial.println(...)`. Keep logs lightweight for flash/heap-limited builds. Many modules gate debug with `-DDOTR_DEBUG` in `platformio.ini`.
- When you touch fonts or icons, update the `assets/` scripts and regenerate LVGL font C files (see header in `Code/OTR-AMOLED-Plus/src/ui/fonts/ui_font_BIG.c` for the img/font generation opts).

Quick examples to reference
- UI label updated with tag example: `Code/OTR-AMOLED-Plus/src/ui/screens/ui_Main.c` contains `lv_label_set_text(ui_Main_Label_RFID, "900 123456789012");` — change UI strings via LVGL API.
- File & asset locations: static web assets live in `Code/OTR-AMOLED-Plus/html/` and device `data/` — see `Code/OTR-AMOLED-Plus/src/otrWebInterface.cpp` for how files are served.
- Webserver static file serving: `Code/OTR-AMOLED-Plus/src/otrWebInterface.cpp` uses `server.serveStatic("/images/BLACK.png", SD, "/html/tag_images/Black.png");`.

If you need more context
- I merged available README and project files; there were no existing `.github/copilot-instructions.md` or AGENT.md files to merge.
- Ask for any missing hardware details (which RFID module/serial pins are connected) or preferred serial port naming on CI. If you want, I can add an example debug workflow for JTAG/SWD using the `.vscode/launch.json` already present.

Next steps (ask me before proceeding):
- Add troubleshooting section for common build problems (PlatformIO package download issues, missing PSRAM flags) if you want.
- Add a short checklist to the top of this file for on-boarding new contributors.

---
Please review and tell me any sections that are unclear or where you want more examples (specific file references or code snippets to include). 
