"""Fixed first low-speed pulse, requiring operator readiness before --execute.

0x230 ON 09 00 01 00 14 00 00 00, about 0.5 s, then OFF 08 00...00.
Speed raw 20 is approximately 38 rpm from code/config, not calibrated yet.
Torque limit raw 1 is the minimum positive integer; physical units unverified.
Gen1.1 SWD monitoring is in a separate thread so a blocked SWD read does not
delay the main thread's CAN stop attempt. This is not a hardware safety loop.
"""
import argparse
import csv
import math
import threading
import time

from sintech_run_zero_pulse import PulseSession,OFF_ZERO
from sintech_zero_can_check import ALLOWED_READS
from capture_sintech_gen11 import RunningTarget

# Only additional read objects; state-changing SDO writes are not added.
ALLOWED_READS.update({(0x6410,5),(0x6410,6)})
ON=bytes.fromhex('09 00 01 00 14 00 00 00')


class SpeedMonitor:
    def __init__(self):
        self.samples=[]
        self.latest=None
        self.error=None
        self.firmware_sha256=None
        self.ready=threading.Event()
        self.stop=threading.Event()
        self.thread=threading.Thread(target=self.worker,name='Gen11-read-only',daemon=True)

    def worker(self):
        target=RunningTarget()
        try:
            self.firmware_sha256=target.open()
            while not self.stop.is_set():
                target.require_running()
                sample=target.sample()
                self.samples.append(sample)
                self.latest=sample
                self.ready.set()
                self.stop.wait(.015)
        except BaseException as exc:
            self.error=str(exc)
            self.ready.set()
        finally:
            target.close()

    def require_fresh(self):
        if self.error: raise RuntimeError('Gen1.1 monitor: '+self.error)
        item=self.latest
        if item is None: raise RuntimeError('Gen1.1 sample unavailable')
        age=(time.monotonic_ns()-item['host_ns'])/1e9
        if age>.100: raise RuntimeError('Gen1.1 sample older than 100 ms')
        return item


class LowSpeedSession(PulseSession):
    ON_FRAME=ON
    OFF_FRAME=OFF_ZERO
    DURATION=.500
    STAGE='low-speed-pulse'
    SPEED_RAW=20
    TORQUE_LIMIT_RAW=1
    RPM_GUARD=100

    def __init__(self):
        super().__init__(self.STAGE)
        self.monitor=SpeedMonitor()
        self.report['speed_request_raw']=self.SPEED_RAW
        self.report['torque_limit_raw']=self.TORQUE_LIMIT_RAW
        self.report['torque_units']='internal controller-output units; physical scale unverified'
        self.report['software_rpm_guard']=self.RPM_GUARD
        self.report['run_on_host_ns']=None
        self.report['run_off_host_ns']=None

    def before_pulse(self):
        poles=self.read(0x6410,5)
        limit=self.read(0x6410,6)
        self.report.update(configured_pole_pairs=poles,electrical_speed_limit=limit)
        if poles!=5 or limit!=800:
            raise RuntimeError('Motor settings changed from reviewed 5 pole pairs / limit 800')
        self.report['estimated_target_rpm_from_code']=self.SPEED_RAW*30/(math.pi*poles)
        self.monitor.thread.start()
        if not self.monitor.ready.wait(5):
            raise RuntimeError('Gen1.1 monitor initialization timed out; no RUN sent')
        deadline=time.monotonic()+.25
        while time.monotonic()<deadline:
            sample=self.monitor.require_fresh()
            if abs(sample['rpm'])>1:
                raise RuntimeError('Shaft not stationary before pulse; no RUN sent')
            self.receive(.01)
        samples=list(self.monitor.samples)
        if len(samples)<5 or samples[-1]['target_tick_ms']==samples[0]['target_tick_ms']:
            raise RuntimeError('Gen1.1 samples/ticks did not advance; no RUN sent')
        if max(abs(x['resolver_sin'])+abs(x['resolver_cos']) for x in samples)<1:
            raise RuntimeError('Resolver samples absent; no RUN sent')
        self.report['gen11_baseline_max_abs_rpm']=max(abs(x['rpm']) for x in samples)
        self.report['gen11_firmware_sha256']=self.monitor.firmware_sha256

    def pulse_frame(self,raw):
        if raw==self.ON_FRAME: self.report['run_on_host_ns']=time.monotonic_ns()
        if raw==self.OFF_FRAME: self.report['run_off_host_ns']=time.monotonic_ns()
        return super().pulse_frame(raw)

    def monitor_pulse(self):
        item=self.monitor.require_fresh()
        if abs(item['rpm'])>self.RPM_GUARD:
            raise RuntimeError(f'Gen1.1 exceeded {self.RPM_GUARD} rpm software guard; ending pulse early')

    def after_stop(self):
        # Record free deceleration, without applying further torque commands.
        deadline=time.monotonic()+2
        while time.monotonic()<deadline:
            self.monitor.require_fresh()
            self.receive(.01)

    def cleanup_observer(self):
        self.monitor.stop.set()
        if self.monitor.thread.ident is not None: self.monitor.thread.join(timeout=1)
        samples=list(self.monitor.samples)
        self.report['gen11_monitor_error']=self.monitor.error
        self.report['gen11_thread_still_running']=self.monitor.thread.is_alive()
        self.report['gen11_samples']=len(samples)
        if samples:
            with (self.directory/'gen11.csv').open('w',newline='',encoding='utf-8') as output:
                writer=csv.DictWriter(output,fieldnames=list(samples[0]))
                writer.writeheader()
                writer.writerows(samples)
            self.report['gen11_peak_abs_rpm']=max(abs(x['rpm']) for x in samples)
            self.report['gen11_final_rpm']=samples[-1]['rpm']
            on,off=self.report['run_on_host_ns'],self.report['run_off_host_ns']
            active=[s for s in samples if on is not None and s['host_ns']>=on and (off is None or s['host_ns']<=off)]
            self.report['gen11_active_samples']=len(active)
            if active:
                self.report['gen11_active_peak_abs_rpm']=max(abs(s['rpm']) for s in active)
                self.report['gen11_active_last_rpm']=active[-1]['rpm']


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--execute',action='store_true')
    args=parser.parse_args()
    if args.execute:
        result=LowSpeedSession().execute_pulse()
        raise SystemExit(0 if result['outcome']=='completed' else 1)
    print('DRY RUN; no hardware opened.')
    print('ON : 0x230 '+ON.hex(' ').upper())
    print('0.5 s nominal duration, speed raw 20 (~38 rpm), torque-limit raw 1 (units unverified).')
    print('OFF: 0x230 '+OFF_ZERO.hex(' ').upper())
    print('Fresh Gen1.1 SWD required; software early stop for >100 rpm, stale data or fault.')
