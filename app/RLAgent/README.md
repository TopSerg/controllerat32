# H64 RL agent port for AT32

This directory contains a shadow-mode port of the FP32 `9 -> 64 -> 64 -> 2` H64 actor using unrolled-by-4 inner products.

## Memory placement

- H64 parameters: 4,930 FP32 values = 19,720 bytes.
- Weight arrays use `.data.rl64_weights`, so the existing `*(.data*)` linker rule copies them to SRAM at startup.
- The hot inference entry point uses the existing `.code_in_ram` section by default. Define `RL64_CODE_IN_FLASH` to keep it in Flash.
- W1 is split into eight C translation units only to keep source files manageable; neuron/weight order is unchanged.

## Build integration

Compile these files:

- `rl64_port.c`
- `rl64_actor_weights_small.c`
- `rl64_w1_0.c` ... `rl64_w1_7.c`

Add `app/RLAgent` to the include path and include:

```c
#include "rl64_port.h"
```

Initialize once after `ControlSystem_v2_initialize()` and before interrupt-driven control starts:

```c
rl64_at32_init();
```

For a shadow timing test, call immediately after `isrADC()` in `adcCall()`:

```c
const float rl_torque_ref_nm = extRef.refValue; /* verify active mode uses torque here */
(void)rl64_at32_shadow_step(rl_torque_ref_nm, -1.0e31f);
```

`extRef.refValue` is mode-dependent, therefore the bridge deliberately takes the torque reference as an explicit argument.

## Debug variables

- `rl64_step_time_us`
- `rl64_step_count`
- `rl64_valid_count`
- `rl64_last_output`

The bridge is shadow-only: it does not assign `Control.Ud`, `Control.Uq`, or PWM outputs.

For clean timing measurements, disable the existing legacy `TrqCalc()` benchmark load in `main.c` before testing the RL agent.
