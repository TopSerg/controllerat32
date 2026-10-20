"""Fixed 500 rpm / 1 second pulse with torque limit raw 2.

Default dry run. Execute only after operator readiness for these exact frames.
Physical torque scale is unverified; software stop is not a hardware stop.
"""
import argparse
import struct

from sintech_500rpm_pulse import Speed500Session


class Limit2Session(Speed500Session):
    STAGE='500rpm-limit2-pulse'
    TORQUE_LIMIT_RAW=2
    ON_FRAME=struct.pack('<Hhhh',9,TORQUE_LIMIT_RAW,Speed500Session.SPEED_RAW,0)


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--execute',action='store_true')
    args=parser.parse_args()
    if args.execute:
        result=Limit2Session().execute_pulse()
        raise SystemExit(0 if result['outcome']=='completed' else 1)
    print('DRY RUN; no hardware opened.')
    print('ON : 0x230 '+Limit2Session.ON_FRAME.hex(' ').upper())
    print('Nominal ~500 rpm, 1.0 s, torque-limit raw 2 (physical units unverified).')
    print('OFF: 0x230 '+Limit2Session.OFF_FRAME.hex(' ').upper())
    print('Fresh Gen1.1 required; early stop attempt above 650 rpm, stale feedback or fault.')
