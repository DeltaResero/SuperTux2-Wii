# Building SuperTux for Nintendo Wii

## Prerequisites

1.  **devkitPro**: Install devkitPro with the `wii-dev` group.
    *   [Installation Instructions](https://devkitpro.org/wiki/Getting_Started)
2.  **Portlibs**: You need specific Wii libraries installed via `dkp-pacman`.

```bash
# Install Wii libraries
wii-sdl2 wii-sdl2_image wii-sdl2_mixer ppc-libvorbisidec ppc-libogg ppc-libmad ppc-libjpeg-turbo ppc-libpng ppc-freetype ppc-zlib wii-opusfile ppc-libmodplug
```

```bash
# Maybe in the future also
wii-mbedtls wii-curl
# https://github.com/AndrewPiroli/wii-curl
```

## Build Instructions

1.  Open a terminal in the project root.
2.  Create a build directory:
    ```bash
    mkdir build
    cd build
    ```
3.  Configure with CMake using the Wii toolchain:
    ```bash
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/Wii.cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENGL=ON -DENABLE_OPENAL=OFF
    ```
4.  Compile:
    ```bash
    make -j$(nproc)
    ```

**Output**: The resulting files are `supertux2.dol` and `data/levels/misc/menu.stl`

## Testing via Dolphin Emulator (current recommended way)

1.  **Important**: Select "Enable SD Card" in Dolphin Config -> Wii.
  
2.  **File Structure**:
    *   Dolphin looks for the SD card at: `~/.local/share/dolphin-emu/Wii/sd.raw` (Linux) or `Documents/Dolphin Emulator/Wii/sd.raw` (Windows).
    *   You must mount this virtual SD card and create the following structure:
        ```
        sd:/
        └── apps/
            └── supertux2/
                ├── supertux2.dol  (The executable)
                ├── data/          (Copy from project root/data)
                └── save/          (Empty folder for savegames)
        ```
3.  Launch using Homebrew Channel [Open_HBC_1.1.5_LULZ.wad](https://github.com/Wii-Mini-Hacking/hbc/releases)

## Running on Real Hardware (untested due to outstanding bugs)

1.  Copy the `apps/supertux2` folder structure above to a real SD card.
2.  Launch via the Homebrew Channel.

## Known Issues (POC Status)

*   **Background Glitch**: The title screen background is a checkerboard (likely non-power-of-two texture issue).
*   **Quit Button**: The in-game quit button may not function as expected; use the Home button to exit if stuck.

**Important Notice**: Currently only menus, worldmap, and the intro level (somewhat) work. Many graphical bugs, logic bugs, and out of memory issues.
