"""Fixed nominal 250 rpm, torque limit raw 5, 10 second recording.

Dry run by default. Execute only after readiness for these exact frames.
Records SINtech CAN and Gen1.1 SWD; software stop is not a hardware stop.
"""
import argparse
import math
import struct

from sintech_500rpm_limit5_10s import TenSecondSession


class QuarterThousandSession(TenSecondSession):
    STAGE='250rpm-limit5-10s'
    SPEED_RAW=131
    ON_FRAME=struct.pack('<Hhhh',9,5,SPEED_RAW,0)
    RPM_GUARD=400


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--execute',action='store_true')
    args=parser.parse_args()
    if args.execute:
        result=QuarterThousandSession().execute_pulse()
        raise SystemExit(0 if result['outcome']=='completed' else 1)
    print('DRY RUN; no hardware opened.')
    print('ON : 0x230 '+QuarterThousandSession.ON_FRAME.hex(' ').upper())
    print(f'Nominal {QuarterThousandSession.SPEED_RAW*30/(math.pi*5):.3f} rpm, '
          '10.0 s, torque-limit raw 5 (physical units unverified).')
    print('OFF: 0x230 '+QuarterThousandSession.OFF_FRAME.hex(' ').upper())
    print('Concurrent CAN + Gen1.1 recording, including coast after RUN clear.')
    print('Early stop attempt above 400 rpm, stale Gen1.1 data or fault.')
