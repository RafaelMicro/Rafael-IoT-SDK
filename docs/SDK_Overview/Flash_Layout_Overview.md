# Flash Memory Layout Specification

This document defines the Flash memory layout for the RT58x / RF13xx series, clearly separating the Bootloader, Application Image, Reserved Data, and MP Sector. It serves as a common reference for SDK, Bootloader, Factory, and Application development.

Two layout modes are provided: Basic Layout and Application Layout.

---

## 1. Layout Overview

| Layout | Description |
|---|---|
| **Basic Layout** | Single image, no OTA, highest stability |
| **OTA Layout** | Dual image (Primary / Secondary), OTA capable |

---
# Flash Basic Address

**RT581/RT582/RT53 Series**（Flash Base = `0x0000_0000`）

| Flash Size | Address Range               |
| ---------- | --------------------------- |
| 1 MB       | `0x0000_0000 ~ 0x0010_0000` |
| 2 MB       | `0x0000_0000 ~ 0x0020_0000` |

**RF1301/RT584H/RT584L/RT584HA4 Series**（Flash Base = `0x1000_0000`）

| Flash Size | Address Range               |
| ---------- | --------------------------- |
| 1 MB       | `0x1000_0000 ~ 0x1010_0000` |
| 2 MB       | `0x1000_0000 ~ 0x1020_0000` |
| 4 MB       | `0x1000_0000 ~ 0x1040_0000` |

---
# Part A. Basic Layout (Single Image )

## 2. Design Principles

- Primary image only
- MP Sector: 64KB

---

## 3. Basic Layout – Size Allocation

### 1MB Devices

| Platform | Bootloader | Primary | MP Sector |
|---|---|---|---|
| RT581 / RT582 | 32 KB | 928 KB | 64 KB |
| RF1301 | 64 KB | 896 KB |  64 KB |


### 2MB Devices

| Platform | Bootloader | Primary | MP Sector |
|---|---|---|---|
| RT583 | 32 KB | 1952 KB | 64 KB |
| RT584H / RT584L | 64 KB | 1920 KB | 64 KB |

### 4MB Devices

| Platform | Bootloader | Primary | MP Sector |
|---|---|---|---|
| RT584HA4 | 64 KB | 3968 KB | 64 KB |

---

# Part B. Application Layout (Dual Image)

## 5. Design Principles

- Primary + Secondary images
- Reserved Data (allocated when Flash ≥ 2MB)
- MP Sector (64KB)
- OTA update supported

---

## 6. Application Layout – Size Allocation

### 1MB Devices

| Platform | Bootloader | Primary | Secondary | MP Sector |
|---|---|---|---|---|
| RT581 / RT582 | 32 KB | 580 KB | 348 KB | 64 KB |
| RF1301 | 64 KB | 560  KB | 336 KB | 64 KB |

### 2MB Devices

| Platform | Bootloader | Primary | Secondary | Reserved | MP Sector |
|---|---|---|---|---|---|
| RT583 | 32 KB | 1180 KB | 708 KB | 64 KB | 64 KB |
| RT584H/RT584L | 64 KB | 1160 KB | 696 KB | 64 KB|64 KB |

### 4MB Devices

| Platform | Bootloader | Primary | Secondary | Reserved | MP Sector |
|---|---|---|---|---|---|
| RT584HA4| 64 KB | 2440 KB | 1464 KB | 64 KB | 64 KB |


---

## 7. MP Sector – Memory Map (64KB)

```
 ╭────────────────────────────────────────╮ Base
 │ User Data (16 KB)                      │
 ├────────────────────────────────────────┤
 │ System Data (40 KB)                    │
 ├────────────────────────────────────────┤
 │ Reserved Data (4 KB)                   │
 ├────────────────────────────────────────┤
 │ Information Data (4 KB)                │
 ╰────────────────────────────────────────╯ Top
```

## Notes

- Bank1 and Bank2 are symmetrically allocated for OTA operations
- Application layout is suitable for Zigbee / BLE / Matter products

---
**Document Type**: Flash Memory Layout Specification  
**Platform**: RT581/RT582/RT583/RF1301/RT584 Series
