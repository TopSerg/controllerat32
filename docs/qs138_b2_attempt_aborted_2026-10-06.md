# QS138 Stage B2 attempt — stopped before PWM test (2026-10-06)

## Prepared image

The `test/overcurrent-flight-recorder` worktree was fast-forwarded to `f54dfc0de155d87a11fbf10caa200f97427eeded`. Both the updated [B2 procedure](qs138_next_test_overcurrent_2026-10-06.md) and the [previous test results](qs138_overcurrent_test_results_2026-10-06.md) were read. `cmake --build build/debug -j 8` succeeded. The resulting `build/debug/AT32_Base.bin` is 163956 bytes, SHA-256 `881FCE6F482DD76587216628CB3232918C69229209A983893FC486FDFBFCFFBD`, and embeds the branch and exact HEAD hash. The matching ELF contains `g_qs138BackEmfFeedForwardEnable`, `g_qs138BackEmfFeedForwardGain`, `VqPlay`, and `g_qs138FlightRecorder` of size `0x6000` (256 × 96 bytes). Source checks confirmed `OverCurrent=20 A`, `DecouplingEnable=0`, gain `0.75`, and the existing provisional QS138 constants. No regulator or motor parameter was edited.

## Why B2 was not run

The Nvert PCAN loader was started at 1 Mbit/s and waited for bootloader device 240. After two operator resets of the controller's logic supply, the loader did not observe device 240; its [scan log](../test_logs/qs138_b2_flash_20261006.log) ended in a timeout. A separate five-second PCAN receive check also saw zero CAN frames while the 60 V motor supply was off. Read-only JTAG access remained available. Reading flash firmware metadata at `0x08028000` proved that the target was still running the **previous** `de8e969f736b5228ef2f431f54ff58d3183324da` image. **The new B2 image was not flashed.**

The operator briefly enabled motor power to retry communication and reported that the **left QS138 hummed while the right drive was off and the pedal was untouched**. After a small right-drive pedal input, the operator identified the left motor as continuing to drive the common shaft after the pedal was released. The operator then switched off motor power and confirmed the shaft had stopped. No host control server was running and no PWM command was sent by these test tools. The cause of left-motor torque during this interval was not established from telemetry. No external-spin plateau, B2 feed-forward validation, or nonzero current command was attempted.

Using an ELF rebuilt from the installed old revision, read-only JTAG [status after power-off](../test_logs/qs138_b2_abort_old_firmware_jtag_20261006.json) showed `modActive=0`, `globalError=0`, `overCurrent=0`, `hardwareFault=0`, `triggered=0`, `frozen=0`, `armed=1`, and about 2.2 V on the DC-bus measurement. Since no fault was latched and the recorder was not frozen, there was no fault dump to decode. This post-stop status does not establish whether PWM was active during the brief powered interval.

## Current disposition

The stand is left with motor power off and the shaft stopped. Do not repeat the powered run or treat it as a B2 result until the source of motion is identified and PCAN bootloader access is verified. The installed firmware is still `de8e969`; the B2 image exists only as the local build artifact. No reset or fault clear was performed after the reported motion, and `main` was not merged or modified.
