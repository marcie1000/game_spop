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

Run:
```console
./game_spop <GB ROM file> [--bypass-bootrom]
```
With no ROM argument, the emulator will execute the bootrom and display a scrolling black box.
