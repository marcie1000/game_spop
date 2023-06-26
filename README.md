# game_spop
A Gameboy emulator

## Build project:

### Linux:

You need to have [SDL2](https://wiki.libsdl.org/SDL2/Installation) and [CMake](https://cmake.org/) installed.

```console
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make
```
### Windows:

In MinGW [MSYS2](https://www.msys2.org/) terminal, install the following packages (if needed):
```console
pacman -S --needed  mingw-w64-x86_64-SDL2 \
                    mingw-w64-clang-x86_64-toolchain  
```
Build:
```console
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

## How to use:

In the executable path, create a folder called `boot_rom`. Paste the Gameboy DMG boot ROM in this folder with the name `dmg_rom.bin`.

### Usage:
```console
./game_spop <ROM file> [options]
```

### Options:
* `--audio`, `-a`           = disable audio.
* `--audio-log`             = print various audio variables in a file at each sample.
* `--breakpoint`, `-p`      = enable debugging with breakpoints. The program will
                              ask to enter a PC value breakpoint at start, and will
                              ask for a new breakpoint when the previous one is
                              reached.
* `--bootrom`, `-b`         = launch the DMG bootrom before ROM. If no ROM is
                              provided, this option is always on. The file path must
                              be `boot_rom/dmg_rom.bin`.
* `--debug-info`, `-i`    = print cpu state at each instruction.
* `--gb-doctor`, `-d`    =  log cpu state into a file to be used with the [Gameboy
                            doctor tool](https://github.com/robert/gameboy-doctor) (only at launch).
                            Emulator behavior might be inaccurate since LY reading always send 0x90 in
                            this mode.
* `--log-instrs`, `-l`  = log cpu state into a file for comparison with other
                        emulators (only at launch).
* `--step`, `-s`        = enable step by step debugging. Emulator will stop
                          at each new instruction and ask to continue or edit options.
* `--help`, `-h`        = show this help message and exit.

### Controls:

Since the program uses scancodes, the keys names of the controls are different depending
on your keyboard layout, but the keys position remains the same.

Control   | QWERTY key | AZERTY key | Bépo key
---       | :---:      | :---:      | :---:
UP        | W          | Z          | é
DOWN      | S          | S          | U
LEFT      | A          | Q          | A
RIGHT     | D          | D          | I
START     | Return     | Return     | Return
SELECT    | RShift     | RShift     | RShift
A         | L          | L          | R
B         | M          | ,          | Q
Pause emulation | P | P | J
Show an option menu (during pause) | O | O | L
Next frame (during pause) | N | N | '
Toogle fast forward | SPACE | SPACE | SPACE
