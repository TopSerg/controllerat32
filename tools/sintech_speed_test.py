"""Manual SINtech speed test with concurrent CAN and Gen1.1 recording.

Default is a hardware-free preview. --execute starts one finite test.
Uses the existing finally-protected RUN-clear path, including Ctrl+C.
Process termination, PC failure or CAN loss can prevent stop delivery.
"""
import argparse
import math
import struct

from sintech_low_speed_pulse import LowSpeedSession


def make_plan(rpm,seconds,torque_limit=5):
    if not math.isfinite(rpm) or not 0<rpm<=1000:
        raise ValueError('RPM must be finite, positive and at most 1000 (current test range).')
    if not math.isfinite(seconds) or not 0<seconds<=10:
        raise ValueError('Duration must be finite, positive and at most 10 seconds (current test range).')
    if type(torque_limit) is not int or not 0<=torque_limit<=10:
        raise ValueError('Torque limit must be an integer from 0 to 5 (current test range; physical units unverified).')
    raw=round(rpm*2*math.pi*5/60)
    if raw<1:
        raise ValueError('Requested RPM rounds to zero; use at least 1 rpm.')
    nominal=raw*30/(math.pi*5)
    return dict(requested_rpm=rpm,speed_raw=raw,nominal_rpm=nominal,
                seconds=seconds,torque_limit_raw=torque_limit,
                rpm_guard=min(1200,max(400,math.ceil(nominal*1.3))),
                on=struct.pack('<Hhhh',9,torque_limit,raw,0),
                off=bytes.fromhex('08 00 00 00 00 00 00 00'))


class ManualSpeedSession(LowSpeedSession):
    def __init__(self,plan):
        self.STAGE='manual-speed-test'
        self.SPEED_RAW=plan['speed_raw']
        self.TORQUE_LIMIT_RAW=plan['torque_limit_raw']
        self.ON_FRAME=plan['on']
        self.OFF_FRAME=plan['off']
        self.DURATION=plan['seconds']
        self.RPM_GUARD=plan['rpm_guard']
        super().__init__()
        self.report['operator_requested_rpm']=plan['requested_rpm']


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rpm',required=True,type=float,help='Positive nominal rpm, max 1000; no reverse.')
    parser.add_argument('--seconds',default=10,type=float,help='RUN duration in seconds, max 10 (default 10).')
    parser.add_argument('--torque-limit',default=5,type=int,help='Integer limit 0..5 in internal controller-output units, not calibrated Nm/A (default 5).')
    parser.add_argument('--execute',action='store_true',help='Start hardware test; omit for preview.')
    args=parser.parse_args()
    try:
        plan=make_plan(args.rpm,args.seconds,args.torque_limit)
    except ValueError as exc:
        parser.error(str(exc))
    print(f"Requested {plan['requested_rpm']:g} rpm; code-derived {plan['nominal_rpm']:.3f} rpm "
          f"(raw {plan['speed_raw']}), {plan['seconds']:g} seconds; "
          f"torque limit raw {plan['torque_limit_raw']} (physical units unverified).",flush=True)
    print('ON : 0x230 '+plan['on'].hex(' ').upper(),flush=True)
    print('OFF: 0x230 '+plan['off'].hex(' ').upper(),flush=True)
    print(f"Software stop-attempt threshold: {plan['rpm_guard']} rpm; fresh Gen1.1 required.",flush=True)
    if not args.execute:
        print('DRY RUN; no hardware opened. Add --execute only when ready to run.')
        return 0
    print('STARTING. Keep manual power cutoff available. Ctrl+C requests early stop; '
          'do not force-close the process or disconnect PCAN.',flush=True)
    result=ManualSpeedSession(plan).execute_pulse()
    print('Logs: '+result['output_dir'],flush=True)
    confirmed=(result.get('run_off_write_ok') is True
               and result.get('demands_after')=={'6040:00':8,'6074:00':0,'606B:00':0,'6062:00':0}
               and 'status_after' in result and not result['status_after']&4)
    if confirmed:
        print('RUN clear and zero demands confirmed. Verify shaft has stopped.',flush=True)
    elif result.get('run_on_host_ns') is not None:
        print('STOP NOT CONFIRMED: CHECK SHAFT AND REMOVE POWER MANUALLY IF NEEDED.',flush=True)
    return 0 if result['outcome']=='completed' else 1


if __name__=='__main__':
    raise SystemExit(main())
