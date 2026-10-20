# QS138 / Gen1.1 braking trials, 2026-10-01

Left QS138: Gen1.1, `resolverSwap=1`, `resolverShift=0.675 rad`.
Right motor: Sintech, driven by pedal. Both inverters share a NICE-POWER
SPS3010-2KD supply. The installed commissioning firmware limits phase current
to 10 A, DC charging current to 1 A, and trips raw overcurrent at 20 A.

The host set `n_max=1700 rpm`, logged the selected raw CAN traffic and 50 ms
telemetry, and sent `Id=0`. The stronger trials armed positive `Iq` only after
the right motor reached -700 rpm, ramped at 1 A per 0.2 s, and released the
request below -600 rpm. Host abort limits included `Udc>=62 V` on the 8 A run,
measured dq-current magnitude >=11 A, a fault, or a loss of `GateDrv=2`.

| Command | Right-motor speed | Measured `Iq` | Charge-current derating | Supply display / operator observation |
| --- | --- | --- | --- | --- |
| 0 A | About 1180 rpm | -0.4 A mean, 1100-1250 rpm | About 1% | About 61 V, 3 A |
| +1 A | About 1150 rpm | 0.74 A mean, 1100-1250 rpm | 31% | About 61 V, 2.8 A |
| +3 A | About 1100-1300 rpm | 2.82 A mean, 1100-1250 rpm | 15% | Approximately the same display and pedal feel |
| +5 A | About 1100-1300 rpm | 4.69 A mean, 1100-1250 rpm | 63% | Approximately the same display and pedal feel |
| Ramp to +8 A | About 700-1000 rpm during ramp | Briefly 11.5 A at -888 rpm | Active at the peak | Host aborted at 11 A; operator reported no obvious change |

The 8 A request was **not** sustained: after the 11.5 A sample the host sent
`Iq=0` and kept streaming CAN commands while the shaft stopped. The measured
current exceeded the 10 A functional phase-current setting briefly; no MCU
fault latched. The run peaked at 1595 rpm after current release, below the
1700 rpm host speed limit. The inverter's reported DC bus stayed at 59-61 V.
The final passive `0x2C6` payload was all zeros.

All moving samples in the +1 A, +3 A, and +5 A runs had `GateDrv=2`; their
raw `0x2C6` payloads were zero. The zero-current run contained one transient
`McuCANFault=0x02` frame only at CAN startup, before motor motion. It cleared
without a gate transition. The +8 A run had no raw `0x2C6` fault either.

The shared supply display shows **net** current of the two inverters. If the
right inverter consumes power supplied in part by the left inverter, the
display can stay near 3 A while left braking current rises. It is not a
measurement of left regenerative current or coupling torque. In firmware,
`Control.IdcFilt` is assigned to the unsigned legacy `McuOfsAl` field of CAN
`0x07D`; negative current is clipped by its DBC packing. Therefore that field
cannot be used to quantify regeneration. `McuActualTorque` in `0x07A` is
quantized to whole N m and was mostly 0 or 1 in these short trials.

Before requesting a larger braking torque, expose signed `Control.IdcFilt` on
CAN and verify its sign and scale, establish the Gen1.1 power-stage current
rating and the DC link's ability to absorb a transient, and identify the
provisional QS138 motor parameters. A torque transducer or calibrated
mechanical load is needed to measure coupling torque directly. The supply
manual describes a source with 60 V / 10 A series output; it does not specify
four-quadrant operation or a current-sinking rating. See the
[NICE-POWER SPS3010-2KD product page](https://www.nice-power.com.cn/productinfo/1156265.html)
and the [supplier's manual](https://cdn.shopify.com/s/files/1/0705/8027/3329/files/Dual_Channel_DC_Power_Supply_English_Manual.pdf?v=1747620232).

## Evidence

- `test_logs/measure_zero_{can.csv,telemetry.jsonl}`
- `test_logs/measure_brake1_{can.csv,telemetry.jsonl}`
- `test_logs/brake3_limited_{can.csv,telemetry.jsonl}`
- `test_logs/brake5_armed_{can.csv,telemetry.jsonl}`
- `test_logs/brake8_armed_{can.csv,telemetry.jsonl}`

The capture/abort script is `tools/qs138_stand_capture.py`.
