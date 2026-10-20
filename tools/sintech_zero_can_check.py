"""Staged SINtech CAN handover with ZERO demand and NO run request.

Dry run is default. Stages are separate operator-coordinated invocations:
--execute inspect: reviewed SDO reads only.
--execute zero: verify Analog and live PDO mapping, send ONE all-zero RPDO,
               verify all mapped demand words by SDO, then close.
--execute switch: verify the live mapping and all-zero demands, write ONLY
                 6411:08=1, read it back and check run-status bit is clear.
--execute speed-select: verify CAN and all-zero demands, send ONE speed-mode
                       selection with zero demands and RUN clear; read back.
No nonzero torque/speed demand, RUN, NMT, reset, save-to-flash, retries or
automatic mode rollback.
"""
import argparse
import csv
from datetime import datetime, timezone
import json
import time

from probe_sintech_sdo_once import (
    PCANBasic, PCAN_USBBUS1, PCAN_LISTEN_ONLY, PCAN_PARAMETER_OFF,
    PCAN_ERROR_OK, PCAN_ERROR_QRCVEMPTY, PCAN_BAUD_1M,
    PCAN_MESSAGE_EXTENDED, PCAN_MESSAGE_RTR, PCAN_MESSAGE_ERRFRAME,
    TPCANMsg, PCAN_MESSAGE_STANDARD, REQUIRED_IDS, ROOT,
)

SOURCE = (0x6411,8)
DEMANDS = [(0x6040,0),(0x6074,0),(0x606B,0),(0x6062,0)]
MAPPING = [0x60400010,0x60740010,0x606B0010,0x60620010]
ALLOWED_READS = {SOURCE,(0x6041,0),(0x1400,1),(0x1600,0)} | set(DEMANDS) | {
    (0x1600,n) for n in range(1,5)}
SWITCH = bytes.fromhex('2B 11 64 08 01 00 00 00')
SPEED_SELECT = bytes.fromhex('08 00 00 00 00 00 00 00')
assert not (int.from_bytes(SPEED_SELECT[:2],'little') & 1)
assert SPEED_SELECT[2:] == bytes(6)


def utc():
    return datetime.now(timezone.utc).isoformat()


class Session:
    def __init__(self, stage):
        self.directory=ROOT/'test_logs'/('sintech_can_'+stage+'_'+datetime.now().strftime('%Y%m%d_%H%M%S'))
        self.directory.mkdir(exist_ok=False)
        self.report=dict(stage=stage,start_utc=utc(),transactions=[],outcome='starting',
                         application_write_calls=0,rpdo_calls=0,configuration_write_calls=0,
                         rx_counts={},latest_status=None,output_dir=str(self.directory))
        self.api=PCANBasic()
        self.channel=PCAN_USBBUS1
        self.opened=False
        self.start=time.monotonic()
        self.seen=set()
        self.stream=(self.directory/'traffic.csv').open('w',newline='',encoding='utf-8')
        self.writer=csv.writer(self.stream)
        self.writer.writerow(['utc_time','host_seconds','direction','can_id','extended','rtr','error_frame','dlc','data_hex','pcan_us'])

    def save(self):
        (self.directory/'summary.json').write_text(json.dumps(self.report,indent=2),encoding='utf-8')
        self.stream.flush()

    def open(self):
        status=self.api.SetValue(self.channel,PCAN_LISTEN_ONLY,PCAN_PARAMETER_OFF)
        if status!=PCAN_ERROR_OK: raise RuntimeError(f'ACK mode failed {int(status):#x}')
        status=self.api.Initialize(self.channel,PCAN_BAUD_1M)
        if status!=PCAN_ERROR_OK: raise RuntimeError(f'Initialize failed {int(status):#x}')
        self.opened=True
        status,mode=self.api.GetValue(self.channel,PCAN_LISTEN_ONLY)
        if status!=PCAN_ERROR_OK or mode!=PCAN_PARAMETER_OFF: raise RuntimeError('Normal CAN mode not verified')
        self.receive(2)
        if not REQUIRED_IDS<=self.seen: raise RuntimeError('SINtech telemetry fingerprint missing')
        self.healthy()

    def healthy(self):
        status=self.api.GetStatus(self.channel)
        if status!=PCAN_ERROR_OK: raise RuntimeError(f'Bus status {int(status):#x}')

    def receive(self, seconds, expected=None):
        deadline=time.monotonic()+seconds
        while time.monotonic()<deadline:
            status,msg,stamp=self.api.Read(self.channel)
            if status==PCAN_ERROR_QRCVEMPTY:
                time.sleep(.001)
                continue
            if status!=PCAN_ERROR_OK: raise RuntimeError(f'RX status {int(status):#x}')
            ext=bool(msg.MSGTYPE&PCAN_MESSAGE_EXTENDED.value)
            rtr=bool(msg.MSGTYPE&PCAN_MESSAGE_RTR.value)
            err=bool(msg.MSGTYPE&PCAN_MESSAGE_ERRFRAME.value)
            raw=bytes(msg.DATA[:msg.LEN]) if not rtr else b''
            ident=f'0x{msg.ID:08X}' if ext else f'0x{msg.ID:03X}'
            micros=(stamp.millis_overflow*0x100000000+stamp.millis)*1000+stamp.micros
            self.writer.writerow([utc(),time.monotonic()-self.start,'RX',ident,int(ext),int(rtr),int(err),msg.LEN,raw.hex(' ').upper(),micros])
            self.report['rx_counts'][ident]=self.report['rx_counts'].get(ident,0)+1
            if err: raise RuntimeError('CAN error frame')
            if ext or rtr: continue
            if msg.LEN==8:
                self.seen.add(msg.ID)
                if msg.ID==0x1B0: self.report['latest_status']=raw.hex(' ').upper()
            if msg.ID==0x5B0:
                if expected is None or msg.LEN!=8 or raw[1:4]!=expected:
                    raise RuntimeError('Unsolicited/mismatched SDO reply')
                return raw
        if expected is not None: raise TimeoutError('No SDO reply within 1 s; no retry')

    def send(self, ident, raw):
        # Only these fixed non-read frames are accepted. RUN is always clear.
        is_read=ident==0x630 and len(raw)==8 and raw[0]==0x40 and raw[4:]==bytes(4) and (
            int.from_bytes(raw[1:3],'little'),raw[3]) in ALLOWED_READS
        if not (is_read or (ident==0x230 and raw in (bytes(8),SPEED_SELECT)) or (ident==0x630 and raw==SWITCH)):
            raise ValueError('Frame is outside the fixed handover whitelist')
        self.healthy()
        msg=TPCANMsg()
        msg.ID,msg.MSGTYPE,msg.LEN=ident,PCAN_MESSAGE_STANDARD.value,8
        for n,b in enumerate(raw): msg.DATA[n]=b
        kind=('read' if is_read else 'speed_mode_run_clear' if ident==0x230 and raw==SPEED_SELECT
              else 'zero_rpdo' if ident==0x230 else 'source_switch')
        record=dict(utc=utc(),id=hex(ident),data=raw.hex(' ').upper(),kind=kind)
        self.report['transactions'].append(record)
        self.report['application_write_calls']+=1
        if ident==0x230: self.report['rpdo_calls']+=1
        if raw==SWITCH: self.report['configuration_write_calls']+=1
        self.save()
        code=self.api.Write(self.channel,msg)
        record['write_status']=hex(int(code))
        self.writer.writerow([utc(),time.monotonic()-self.start,'TX_QUEUED',f'0x{ident:03X}',0,0,0,8,raw.hex(' ').upper(),''])
        self.save()
        if code!=PCAN_ERROR_OK: raise RuntimeError(f'Write failed {int(code):#x}; no retry')
        return record

    def read(self,index,sub=0):
        self.receive(.03)
        raw=bytes([0x40,index&255,index>>8,sub,0,0,0,0])
        record=self.send(0x630,raw)
        reply=self.receive(1,raw[1:4])
        record['reply']=reply.hex(' ').upper()
        if reply[0] not in (0x43,0x47,0x4B,0x4F): raise RuntimeError('Non-expedited/error SDO response: '+record['reply'])
        length=4-((reply[0]>>2)&3)
        value=int.from_bytes(reply[4:4+length],'little')
        record['value_unsigned']=value
        self.save()
        print(f'{index:04X}:{sub:02X} = {value:#x}',flush=True)
        return value

    def demands(self):
        return {f'{i:04X}:{s:02X}':self.read(i,s) for i,s in DEMANDS}

    def verify_mapping(self):
        if self.read(0x1400,1)!=0x230: raise RuntimeError('RPDO COB-ID differs or is disabled')
        if self.read(0x1600,0)!=4: raise RuntimeError('RPDO mapping count differs')
        if [self.read(0x1600,n) for n in range(1,5)]!=MAPPING: raise RuntimeError('RPDO mapping differs')

    def run(self,stage):
        try:
            self.open()
            source=self.read(*SOURCE)
            self.report['source_before']=source
            self.report['demands_before']=self.demands()
            self.report['status_before']=self.read(0x6041)
            if stage!='inspect':
                expected_source=1 if stage=='speed-select' else 0
                if source!=expected_source: raise RuntimeError(f'Expected source {expected_source} before this stage')
                if self.report['status_before'] & 4:
                    raise RuntimeError('RUN status already set; no state-changing frame sent')
                self.verify_mapping()
                if stage=='zero':
                    self.receive(.05)
                    self.send(0x230,bytes(8))
                    self.receive(.3)
                    values=self.demands()
                    self.report['demands_after']=values
                    if any(values.values()): raise RuntimeError('Zero RPDO demands not verified by readback')
                    if self.read(*SOURCE)!=0: raise RuntimeError('Source changed unexpectedly')
                elif stage=='switch':
                    if any(self.report['demands_before'].values()): raise RuntimeError('Nonzero mapped demand; source write not sent')
                    self.receive(.05)
                    record=self.send(0x630,SWITCH)
                    reply=self.receive(1,SWITCH[1:4])
                    record['reply']=reply.hex(' ').upper()
                    self.save()
                    if reply!=bytes.fromhex('60 11 64 08 00 00 00 00'): raise RuntimeError('Source write not acknowledged')
                    if self.read(*SOURCE)!=1: raise RuntimeError('CAN source not confirmed by readback')
                    values=self.demands()
                    self.report['demands_after']=values
                    if any(values.values()): raise RuntimeError('Demand changed unexpectedly')
                    status=self.read(0x6041)
                    self.report['status_after']=status
                    if status&4: raise RuntimeError('Run status bit unexpectedly set; operator should remove power')
                elif stage=='speed-select':
                    if any(self.report['demands_before'].values()):
                        raise RuntimeError('Expected all-zero mapped demands; mode frame not sent')
                    self.receive(.05)
                    self.send(0x230,SPEED_SELECT)
                    self.receive(.3)
                    values=self.demands()
                    self.report['demands_after']=values
                    if values!={'6040:00':8,'6074:00':0,'606B:00':0,'6062:00':0}:
                        raise RuntimeError('Speed-mode control word/demands not confirmed by SDO readback')
                    if self.read(*SOURCE)!=1: raise RuntimeError('CAN source no longer selected')
                    status=self.read(0x6041)
                    self.report['status_after']=status
                    self.report['speed_mode_status_bit6']=bool(status&0x40)
                    if status&4: raise RuntimeError('RUN status unexpectedly set; operator should remove power')
                    if not status&0x40: raise RuntimeError('Speed-mode status bit 6 not observed')
            self.receive(2)
            if stage=='speed-select' and self.report['latest_status']:
                final_status=int.from_bytes(bytes.fromhex(self.report['latest_status'])[:2],'little')
                self.report['final_status_word']=final_status
                if final_status&4: raise RuntimeError('RUN status unexpectedly set in final telemetry; operator should remove power')
            self.healthy()
            self.report['final_bus_status']=hex(int(self.api.GetStatus(self.channel)))
            self.report['outcome']='completed'
        except Exception as exc:
            self.report['outcome']='error'
            self.report['error']=str(exc)
        finally:
            if self.opened: self.report['uninitialize_status']=hex(int(self.api.Uninitialize(self.channel)))
            self.report['end_utc']=utc()
            self.save()
            self.stream.close()
        print(json.dumps(self.report,indent=2),flush=True)
        return self.report


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('stage',choices=['inspect','zero','switch','speed-select'])
    parser.add_argument('--execute',action='store_true')
    args=parser.parse_args()
    if args.execute:
        result=Session(args.stage).run(args.stage)
        raise SystemExit(0 if result['outcome']=='completed' else 1)
    print('DRY RUN, no hardware opened. Stage:',args.stage)
    if args.stage=='zero': print('ONE RPDO: standard 0x230 DLC 8: 00 00 00 00 00 00 00 00')
    if args.stage=='switch': print('ONE source write: standard 0x630 DLC 8: '+SWITCH.hex(' ').upper())
    if args.stage=='speed-select': print('ONE RPDO: standard 0x230 DLC 8: '+SPEED_SELECT.hex(' ').upper())
    print('Preflight/readback are fixed reviewed SDO uploads. No run request or nonzero torque/speed demand exists in this script.')
