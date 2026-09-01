# TF0060DCA

Dual-channel digital audio attenuation board. Two LM1972 digital attenuators
driven from an 8-bit parallel volume bus by a CH32V203C8T6 RISC-V MCU, with a
measured calibration mapping.

- `eagle/` - EAGLE schematic/board plus CAM, BOM/CPL and check tooling
  (`make help` for targets; gerbers and PDF build via the terriblefire78
  Docker images)
- `firmware/` - CH32V203 firmware (builds in `terriblefire78/mrs:latest`,
  or `make firmware` from the repo root)
- `testfixture/` - EAGLE test fixture board
- `measured/` - measured attenuation data and calibration table generator
- `gerbers/` - manufactured revisions (JLCPCB)

## License

Copyright (c) 2026 Stephen Leary (TerribleFire).

This design and its firmware are licensed under
[Creative Commons Attribution 4.0 International](LICENSE) (CC BY 4.0): you may
use, modify, manufacture and sell derivatives for any purpose, provided you
credit the original design and work.

WCH vendor sources under `firmware/` (Nanjing Qinheng Microelectronics) retain
their own copyright notices.
