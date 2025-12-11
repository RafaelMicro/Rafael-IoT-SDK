# Sub-GHz Examples Overview

This document summarizes all Sub-GHz examples available under the `examples/sub-g/` folder. These examples demonstrate various Sub-GHz communication features on the Rafael RT58x platform.

---

## 📂 Directory Structure

```
examples/sub-g/
├── subg-sample
├── subg-trx
├── subg-wake-on-radio
└── mesh-it-up/
    ├── miu-ftd/
    ├── miu-mtd/
    └── miu-sniffer/
```

---

## 🔍 Example Descriptions

| Example Directory         | Description                                                                 | More Info |
|---------------------------|-----------------------------------------------------------------------------|-----------|
| `subg-sample`             | A minimal Sub-GHz send/receive sample for basic packet transmission.        | –         |
| `subg-trx`                | A two-way Sub-GHz transceiver example to demonstrate bidirectional comms.   | –         |
| `subg-wake-on-radio`      | Demonstrates wake-on-radio low-power RX wakeup feature.                     | –         |
| `mesh-it-up/`             | Thread-based Sub-GHz Mesh network using OpenThread with MIU SDK.            | [MIU User Guide](mesh-it-up/README.md) |

> 📌 Note: `mesh-it-up` contains multiple device role examples:
> - `miu-ftd/` – Full Thread Device (FTD) example
> - `miu-mtd/` – Minimal Thread Device (MTD) example
> - `miu-sniffer/` – Radio Co-Processor / Sniffer example

---

## 📘 Next Steps

To get started with mesh networking, read:  
👉 [Mesh It Up User Guide](mesh-it-up/README.md)

For basic Sub-GHz communication tests, try:  
📌 [`subg-sample`](../../examples/sub-g/subg-sample), [`subg-trx`](../../examples/sub-g/subg-trx), or [`subg-subg-wake-on-radio`](../../examples/sub-g/subg-wake-on-radio)

---
