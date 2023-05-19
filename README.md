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
* ```--debug-info```            = at every new instruction, prints the mnemonic, the
                                  3 bytes object code, and all registers, PC, SP and
                                  register F flags values in the console. Emulator is
                                  much slower when this option is enabled.
* ```--help```, ```-h```        = show this help message and exit.