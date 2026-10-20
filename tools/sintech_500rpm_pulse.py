"""Fixed 500 rpm nominal test. Default dry run; operator readiness required.

Keeps the previously tested torque limit raw 1. Physical torque units and
speed conversion remain unvalidated. Software stop is not a hardware stop.
"""
import argparse
import math
import struct

from sintech_low_speed_pulse import LowSpeedSession


class Speed500Session(LowSpeedSession):
    STAGE='500rpm-pulse'
    SPEED_RAW=262
    TORQUE_LIMIT_RAW=1
    ON_FRAME=struct.pack('<Hhhh',9,TORQUE_LIMIT_RAW,SPEED_RAW,0)
    DURATION=1.0
    RPM_GUARD=650


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--execute',action='store_true')
    args=parser.parse_args()
    if args.execute:
        result=Speed500Session().execute_pulse()
        raise SystemExit(0 if result['outcome']=='completed' else 1)
    print('DRY RUN; no hardware opened.')
    print('ON : 0x230 '+Speed500Session.ON_FRAME.hex(' ').upper())
    print(f'Nominal {Speed500Session.SPEED_RAW*30/(math.pi*5):.3f} rpm, '
          '1.0 s, torque-limit raw 1 (physical units unverified).')
    print('OFF: 0x230 '+Speed500Session.OFF_FRAME.hex(' ').upper())
    print('Fresh Gen1.1 required; early stop attempt above 650 rpm, stale feedback or fault.')
