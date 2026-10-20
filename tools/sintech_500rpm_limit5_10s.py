"""Fixed nominal 500 rpm, torque limit raw 5, 10 second recording.

Dry run by default. Operator readiness required before --execute.
Records raw SINtech CAN and independent Gen1.1 SWD samples together.
Software stop delivery and timing are not guaranteed on host/bus failure.
"""
import argparse

from sintech_500rpm_limit5_pulse import Limit5Session


class TenSecondSession(Limit5Session):
    STAGE='500rpm-limit5-10s'
    DURATION=10.0


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--execute',action='store_true')
    args=parser.parse_args()
    if args.execute:
        result=TenSecondSession().execute_pulse()
        raise SystemExit(0 if result['outcome']=='completed' else 1)
    print('DRY RUN; no hardware opened.')
    print('ON : 0x230 '+TenSecondSession.ON_FRAME.hex(' ').upper())
    print('Nominal ~500 rpm, 10.0 s, torque-limit raw 5 (physical units unverified).')
    print('OFF: 0x230 '+TenSecondSession.OFF_FRAME.hex(' ').upper())
    print('Concurrent CAN + Gen1.1 recording, including coast after RUN clear.')
    print('Early stop attempt above 650 rpm, stale Gen1.1 data or fault.')
