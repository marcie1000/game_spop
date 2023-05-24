# game_spop
A Gameboy emulator

### Build project:

```console
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make
```

### How to use:

In the executable path, create a folder called ```boot_rom```. Paste the Gameboy DMG boot ROM in this folder with the name ```dmg_rom.bin```.

Usage:
```console
./game_spop <ROM file> [options]
```

Options:
* ```--bypass-bootrom```        = launch directly the ROM (only if a rom is passed in argument).
                                  This option can only be provided at launch.
* ```--debug-info```            = at every new instruction, prints the mnemonic, the
                                  3 bytes object code, and all registers, PC, SP and
                                  register F flags values in the console. Emulator is
                                  much slower when this option is enabled.
* ```--breakpoint```            = enable debugging with breakpoints. The program will
                          ask to enter a PC value breakpoint at start, and will
                          ask for a new breakpoint when the previous one is
                          reached.
* ```--step```, ```-s```        = enable step by step debugging. Emulator will stop
                          at each new instruction.\n"
* ```--help```, ```-h```        = show this help message and exit.