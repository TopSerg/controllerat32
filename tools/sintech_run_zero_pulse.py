"""One operator-coordinated RUN pulse with zero speed and zero limit.

Default is dry run. --execute requires fresh CAN/mapping/demand/status checks.
RUN is requested for about 300 ms; a fixed RUN-clear frame is attempted in
finally before any readback or adapter close. Software timing/stop delivery
is not guaranteed if the process, adapter, bus or host fails.
"""
import argparse
import csv
import json
import time

from sintech_zero_can_check import Session, SOURCE, SPEED_SELECT, utc
from probe_sintech_sdo_once import TPCANMsg, PCAN_MESSAGE_STANDARD, PCAN_ERROR_OK

RUN_ZERO=bytes.fromhex('09 00 00 00 00 00 00 00')
OFF_ZERO=SPEED_SELECT
PULSE_SECONDS=.300
ZERO_EXPECTED={'6040:00':8,'6074:00':0,'606B:00':0,'6062:00':0}


class PulseSession(Session):
    ON_FRAME=RUN_ZERO
    OFF_FRAME=OFF_ZERO
    DURATION=PULSE_SECONDS

    def before_pulse(self):
        pass

    def monitor_pulse(self):
        pass

    def after_stop(self):
        pass

    def cleanup_observer(self):
        pass

    def pulse_frame(self,raw):
        if raw not in (self.ON_FRAME,self.OFF_FRAME):
            raise ValueError('Only the fixed pulse ON/OFF frames accepted')
        msg=TPCANMsg()
        msg.ID,msg.MSGTYPE,msg.LEN=0x230,PCAN_MESSAGE_STANDARD.value,8
        for n,b in enumerate(raw): msg.DATA[n]=b
        record=dict(utc=utc(),id='0x230',data=raw.hex(' ').upper(),
                    kind=self.report['stage']+'_on' if raw==self.ON_FRAME else self.report['stage']+'_off',
                    host_seconds=time.monotonic()-self.start)
        self.report['transactions'].append(record)
        self.report['application_write_calls']+=1
        self.report['rpdo_calls']+=1
        # Stop is always attempted even if the bus status is already an error.
        code=self.api.Write(self.channel,msg)
        record['write_status']=hex(int(code))
        record['write_return_host_seconds']=time.monotonic()-self.start
        self.writer.writerow([record['utc'],record['host_seconds'],'TX_QUEUED' if code==PCAN_ERROR_OK else 'TX_ERROR',
                             '0x230',0,0,0,8,raw.hex(' ').upper(),''])
        if code!=PCAN_ERROR_OK:
            raise RuntimeError(f'{record["kind"]} Write failed: {int(code):#x}')
        return record

    def execute_pulse(self):
        attempted=False
        try:
            self.open()
            if self.read(*SOURCE)!=1:
                raise RuntimeError('CAN source not selected; no RUN frame sent')
            values=self.demands()
            self.report['demands_before']=values
            if values!=ZERO_EXPECTED:
                raise RuntimeError('Expected speed-mode word 8 and all-zero demands; no RUN frame sent')
            status=self.read(0x6041)
            self.report['status_before']=status
            if status&0xC or not status&2 or not status&0x40:
                raise RuntimeError('Unexpected ready/RUN/fault/speed status; no RUN frame sent')
            self.verify_mapping()
            self.before_pulse()
            self.receive(.05)
            self.healthy()
            self.save()
            self.monitor_pulse()
            self.report['planned_pulse_seconds']=self.DURATION
            # No SDO, SWD, filesystem flush, or unbounded wait is inserted
            # between this request and the finally-protected stop attempt.
            attempted=True
            on_time=time.monotonic()
            self.report['run_status_seen_during_pulse']=False
            try:
                self.pulse_frame(self.ON_FRAME)
                while time.monotonic()-on_time<self.DURATION:
                    self.receive(min(.01,max(0,self.DURATION-(time.monotonic()-on_time))))
                    self.monitor_pulse()
                    if self.report['latest_status']:
                        status=int.from_bytes(bytes.fromhex(self.report['latest_status'])[:2],'little')
                        if status&4: self.report['run_status_seen_during_pulse']=True
                        if status&8 or not status&2:
                            raise RuntimeError('Fault or ready loss observed during RUN pulse')
            finally:
                self.report['run_off_attempted']=True
                self.report['elapsed_before_stop_attempt_seconds']=time.monotonic()-on_time
                try:
                    self.pulse_frame(self.OFF_FRAME)
                    self.report['run_off_write_ok']=True
                except BaseException as stop_error:
                    self.report['run_off_write_ok']=False
                    self.report['run_off_error']=str(stop_error)
                    print('RUN-CLEAR WRITE FAILED: REMOVE POWER MANUALLY.',flush=True)
                    raise
                finally:
                    self.save()
            self.receive(.3)
            values=self.demands()
            self.report['demands_after']=values
            if values!=ZERO_EXPECTED:
                raise RuntimeError('RUN-clear demand readback mismatch; remove power manually')
            status=self.read(0x6041)
            self.report['status_after']=status
            if status&4:
                raise RuntimeError('RUN status remains set after stop; remove power manually')
            if self.read(*SOURCE)!=1: raise RuntimeError('Source changed unexpectedly')
            self.receive(2)
            self.healthy()
            final_status=int.from_bytes(bytes.fromhex(self.report['latest_status'])[:2],'little')
            self.report['final_status']=final_status
            if final_status&4: raise RuntimeError('RUN present in final telemetry; remove power manually')
            self.report['final_bus_status']=hex(int(self.api.GetStatus(self.channel)))
            self.after_stop()
            self.report['outcome']='completed'
        except BaseException as exc:
            self.report['outcome']='error'
            self.report['error']=str(exc)
            if attempted:
                print('Pulse interrupted; check RUN-clear confirmation and be ready to REMOVE POWER.',flush=True)
        finally:
            try:
                self.cleanup_observer()
            except Exception as cleanup_error:
                self.report['observer_cleanup_error']=str(cleanup_error)
                self.report['outcome']='error'
            if self.opened:
                self.report['uninitialize_status']=hex(int(self.api.Uninitialize(self.channel)))
            self.report['end_utc']=utc()
            self.save()
            self.stream.close()
        # Preserve every status transition from the raw recording, including
        # frames which arrived between the 10-ms monitoring iterations.
        transitions=[]
        with (self.directory/'traffic.csv').open(encoding='utf-8') as source:
            for row in csv.DictReader(source):
                if row['direction']=='RX' and row['can_id']=='0x1B0':
                    data=bytes.fromhex(row['data_hex'])
                    status=int.from_bytes(data[:2],'little')
                    if not transitions or transitions[-1]['status']!=hex(status):
                        transitions.append(dict(host_seconds=float(row['host_seconds']),
                                                status=hex(status),run=bool(status&4),data=row['data_hex']))
        self.report['status_transitions']=transitions
        (self.directory/'summary.json').write_text(json.dumps(self.report,indent=2),encoding='utf-8')
        print(json.dumps(self.report,indent=2),flush=True)
        return self.report


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--execute',action='store_true')
    options=parser.parse_args()
    if options.execute:
        result=PulseSession('run-zero-pulse').execute_pulse()
        raise SystemExit(0 if result['outcome']=='completed' else 1)
    print('DRY RUN: no hardware opened.')
    print('Preflight: CAN=1, CW=8, speed=0, torque limit=0, mapping and ready/status checked.')
    print('ON : 0x230 '+RUN_ZERO.hex(' ').upper())
    print('Approximately 300 ms, then finally-protected OFF (even on a Python-level pulse error).')
    print('OFF: 0x230 '+OFF_ZERO.hex(' ').upper())
    print('SDO/status readback follows OFF. No automatic retries, source changes, or nonzero demands.')
