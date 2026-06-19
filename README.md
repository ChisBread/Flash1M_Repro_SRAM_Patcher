# GBA_FlashID_Bypass_Patcher
GBA ROM patcher that bypasses Flash save chip ID checks.


## Introduction

Some GBA games using Flash saves verify the Flash manufacturer/device ID before using the save chip.
This branch patches those checks so the game uses the expected ID value without depending on the cartridge hardware returning the original chip ID.

This tool does not convert saves to SRAM. The generated ROM keeps the original Flash save routines except for the ID detection path.

## Supported save libraries

- FLASH1M_V102
- FLASH1M_V103
- FLASH512
- FLASH_V120 / FLASH_V121
- FLASH_V123 / FLASH_V124 / FLASH_V125 / FLASH_V126

## Build

Use CMake to build the sources
```cmake . && cmake --build .```

## Use

```./GBA_FlashID_Bypass_Patcher <ROMFILE>.gba```

The patched ROM is written next to the input file with a `[FLASHID]` prefix.
