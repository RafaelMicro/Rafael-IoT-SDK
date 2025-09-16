# Mesh It Up (MIU) User Guide

This document provides an overview of the Mesh It Up (MIU) SDK architecture and directs you to usage examples for different MIU device roles:

* [`miu-router`](../../examples/sub-g/mesh-it-up/miu-router): Full Thread Device
* [`miu-sleepy`](../../examples/sub-g/mesh-it-up/miu-sleepy): Minimal Thread Device
* [`miu-sniffer`](../../examples/sub-g/mesh-it-up/miu-sniffer): Sniffer / RCP

---

## 📘 What is Mesh It Up?

**Mesh It Up (MIU)** is a development SDK based on [OpenThread](https://openthread.io/) over Sub-GHz, designed to help developers quickly build low-power mesh networking solutions. MIU provides reusable components and ready-to-use examples for various Thread device roles.

It is ideal for smart metering, industrial IoT, and low-power mesh applications requiring long-range Sub-GHz connectivity.

---

## 🧱 SDK Architecture Overview

The MIU SDK follows a modular structure:

```
examples/sub-g/mesh-it-up/
├── miu-router/   # Full Thread Device (FTD) example
├── miu-sleepy/   # Minimal Thread Device (MTD) example
└── miu-sniffer/  # Radio Co-Processor / Sniffer example
```

These examples are designed to work out-of-the-box with Rafael's RT58x series Sub-GHz Thread SoCs.

---

### Device Roles

| Device       | Type                     | Description                                                      | Use Case                           |
|--------------|--------------------------|------------------------------------------------------------------|------------------------------------|
| miu-router   | Full Thread Device (FTD) | Always-on device that forms and maintains the mesh network.      | Gateway, Coordinator               |
| miu-sleepy   | Minimal Thread Device    | Low-power node that periodically polls its parent for messages.  | Sensor, battery-powered endpoint   |
| miu-sniffer  | RCP / Sniffer            | Captures over-the-air packets for debugging and analysis.        | Packet sniffer, diagnostics        |

---

## 🔍 `miu-router`

`miu-router` is a Full Thread Device example. It can form a new mesh network or join an existing one, operating as Leader, Router, or Child depending on the topology.

📄 Path: `examples/sub-g/mesh-it-up/miu-router`

**Note:**

* Suitable for use as the network's root or routing node.
* Not suitable for low-power devices.

---

## 💥 `miu-sleepy`

`miu-sleepy` is a Minimal Thread Device (MTD) designed for low-power operation. It connects to a parent router and periodically wakes up to send or receive data.

📄 Path: `examples/sub-g/mesh-it-up/miu-sleepy`


**Note:**

* Use `ot pollperiod` to configure polling interval.
* MTD cannot route traffic and relies on FTD routers.

---

## 🔌 `miu-sniffer`

`miu-sniffer` runs as a radio co-processor (RCP) to capture and forward raw packets to a host for analysis, typically using tools like Wireshark.

📄  Path: `examples/sub-g/mesh-it-up/miu-sniffer`

**Note:**

* Use tools like `wpantund` or `pyspinel` on the host to communicate with NCP.
* Ensure UART/SPI is properly configured between host and NCP.

---

## 🚧 Future Extensions

The MIU roadmap includes BLE-capable dual-mode support:

- **miu-ble-router**: Adds BLE alongside Thread (Sub-GHz)
- **miu-ble-sleepy**: Adds BLE functionality for low-power devices

---

## 📌 Related Resources

* [OpenThread Documentation](https://openthread.io/)
* [OpenThread GitHub](https://github.com/openthread/openthread)

---
