# Flash Memory Layout Specification

This document defines the flash memory layout for the RT58x, RT584, and RF1301 series. It
separates the flash into the Bootloader, Application image, Reserved data, and MP Sector.

There are two layouts: Basic Layout (single image) and Application Layout (dual image, for OTA).

## 1. Layout Overview

| Layout | Description |
|---|---|
| **Basic Layout** | Single image. No OTA. The most stable option. |
| **Application Layout** | Dual image (Primary and Secondary banks). Supports OTA. |

## Flash base address

**RT581 / RT582 / RT583** (flash base = `0x0000_0000`)

| Flash size | Address range |
|---|---|
| 1 MB | `0x0000_0000` – `0x0010_0000` |
| 2 MB | `0x0000_0000` – `0x0020_0000` |

**RF1301 / RT584H / RT584L / RT584HA4** (flash base = `0x1000_0000`)

| Flash size | Address range |
|---|---|
| 1 MB | `0x1000_0000` – `0x1010_0000` |
| 2 MB | `0x1000_0000` – `0x1020_0000` |
| 4 MB | `0x1000_0000` – `0x1040_0000` |

# Part A. Basic Layout (single image)

## 2. Design Principles

- One application image only.
- The MP Sector is 64 KB, at the top of flash.

## 3. Basic Layout — Size Allocation

### 1 MB devices

| Platform | Bootloader | Application | MP Sector |
|---|---|---|---|
| RT581 / RT582 | 32 KB | 928 KB | 64 KB |
| RF1301 | 64 KB | 896 KB | 64 KB |

### 2 MB devices

| Platform | Bootloader | Application | MP Sector |
|---|---|---|---|
| RT583 | 32 KB | 1952 KB | 64 KB |
| RT584H / RT584L | 64 KB | 1920 KB | 64 KB |

### 4 MB devices

| Platform | Bootloader | Application | MP Sector |
|---|---|---|---|
| RT584HA4 | 64 KB | 1984 KB | 64 KB |

⚠️ RT584HA4 has a flash-controller limitation: code above the 2 MB mark (`0x10200000`) is not
reliably executable. The remaining 2048 KB above the Application region is not available for
code. Part B below describes what that space holds instead.

# Part B. Application Layout (dual image, OTA)

## 4. Design Principles

- A Primary bank and a Secondary bank. The bootloader can boot from either bank.
- On RT581/RT582, RF1301, RT583, and RT584H/RT584L, Primary and Secondary are always the same
  size. This lets the bootloader swap between them freely.
- RT584HA4 breaks that symmetry. See the warning below.
- 2 MB and 4 MB devices also reserve extra flash outside the two banks (see the Reserved column).
- The MP Sector is 64 KB, on every device.

## 5. Application Layout — Size Allocation

### 1 MB devices

| Platform | Bootloader | Primary | Secondary | MP Sector |
|---|---|---|---|---|
| RT581 / RT582 | 32 KB | 464 KB | 464 KB | 64 KB |
| RF1301 | 64 KB | 448 KB | 448 KB | 64 KB |

### 2 MB devices

| Platform | Bootloader | Primary | Secondary | Reserved | MP Sector |
|---|---|---|---|---|---|
| RT583 | 32 KB | 944 KB | 944 KB | 64 KB | 64 KB |
| RT584H / RT584L | 64 KB | 928 KB | 928 KB | 64 KB | 64 KB |

### 4 MB devices

| Platform | Bootloader | Primary | Secondary | Reserved | MP Sector |
|---|---|---|---|---|---|
| RT584HA4 | 64 KB | 1984 KB | 1856 KB | 128 KB | 64 KB |

⚠️ **Primary and Secondary are not the same size on RT584HA4.** Primary (1984 KB) ends exactly
at the 2 MB mark. The flash controller cannot reliably execute code above that address. Secondary
(1856 KB) is used as OTA staging space, not as a second directly-bootable bank. Do not raise the
Primary size past 1984 KB.

## 6. MP Sector — Memory Map (64 KB)

The internal layout differs slightly between the two chip families.

**RT581 / RT582 / RT583** — no gaps:

```
 ╭────────────────────────────────────────╮ Base
 │ EFD — user key-value store (16 KB)     │
 ├────────────────────────────────────────┤
 │ Zigbee NVRAM (32 KB)                   │
 ├────────────────────────────────────────┤
 │ BLE bonding data (8 KB)                │
 ├────────────────────────────────────────┤
 │ Calibration data + MP Sector info (4 KB)│
 ╰────────────────────────────────────────╯ Top (end of flash)
```

**RT584H / RT584L / RT584HA4** — a 4 KB unused gap, between Zigbee NVRAM and BLE bonding:

```
 ╭────────────────────────────────────────╮ Base
 │ EFD — user key-value store (16 KB)     │
 ├────────────────────────────────────────┤
 │ Zigbee NVRAM (32 KB)                   │
 ├────────────────────────────────────────┤
 │ 4 KB gap (unused)                      │
 ├────────────────────────────────────────┤
 │ BLE bonding data (8 KB)                │
 ├────────────────────────────────────────┤
 │ Calibration data + MP Sector info (4 KB)│
 ╰────────────────────────────────────────╯ Top (end of flash)
```

On RT584HA4, Zigbee NVRAM is 64 KB and lives outside this 64 KB block, in the Reserved region
from Part B instead.

- **EFD** — a 16 KB key-value store, for general settings. Application code uses it directly.
  Thread also stores its settings here instead of using its own flash region.
- **Zigbee NVRAM** — 32 KB (64 KB on RT584HA4).
- **BLE bonding data** — 8 KB. It ends exactly at the boundary reserved for the MP Sector.
- **Calibration data + MP Sector info** — 4 KB combined, at the very top of flash. Calibration
  holds RF, crystal, and power trim values written by the mass-production tool. MP Sector info
  is a small header: version, size, and pointers to the other sub-blocks.

## Notes

- Primary and Secondary are the same size on every chip except RT584HA4. See the warning above.
- The Application Layout supports Zigbee, Thread, BLE, and Matter products.

---
**Document type**: Flash Memory Layout Specification
**Platforms**: RT581, RT582, RT583, RF1301, RT584H, RT584L, RT584HA4
