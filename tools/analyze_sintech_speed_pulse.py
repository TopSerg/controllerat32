"""Offline comparison of pulse traffic.csv and Gen1.1 speed.

Float interpretations and regression coefficients are experimental candidates,
not a DBC or a calibrated conversion. No hardware access.
"""
import argparse
import csv
import json
import struct
from pathlib import Path

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory',type=Path)
    p=parser.parse_args().directory
    summary=json.loads((p/'summary.json').read_text())
    on=next(t for t in summary['transactions'] if t['kind'].endswith('_on'))
    off=next(t for t in summary['transactions'] if t['kind'].endswith('_off'))
    duration=off['host_seconds']-on['host_seconds']
    with (p/'gen11.csv').open() as source: gen=list(csv.DictReader(source))
    with (p/'traffic.csv').open() as source: can=list(csv.DictReader(source))
    gt=np.array([(int(r['host_ns'])-summary['run_on_host_ns'])/1e9 for r in gen])
    rpm=np.array([float(r['rpm']) for r in gen])
    active=(gt>=0)&(gt<=duration)
    tail=(gt>=duration-3)&(gt<=duration)
    reached=np.flatnonzero(active & (abs(rpm)>=450))
    stopped=np.flatnonzero((gt>duration)&(abs(rpm)<1))
    results=[]
    streams={}
    for ident in ('0x2B0','0x3B0','0x4B0'):
        rows=[r for r in can if r['direction']=='RX' and r['can_id']==ident and r['dlc']=='8' and r['extended']=='0']
        times=np.array([float(r['host_seconds'])-on['host_seconds'] for r in rows])
        overlap=(times>=gt[0])&(times<=gt[-1])
        times=times[overlap]
        for offset in (0,4):
            values=np.array([struct.unpack_from('<f',bytes.fromhex(r['data_hex']),offset)[0] for r in rows])[overlap]
            valid=np.isfinite(values)
            t,x=times[valid],values[valid]
            y=np.interp(t,gt,rpm)
            item=dict(can_id=ident,byte_offset=offset,interpretation='float32 little-endian hypothesis',samples=len(x),raw_min=float(x.min()),raw_max=float(x.max()))
            if np.std(x)>1e-10:
                dx=x-x.mean()
                slope=float(dx@(y-y.mean())/(dx@dx))
                intercept=float(y.mean()-slope*x.mean())
                error=y-(slope*x+intercept)
                item.update(correlation=float(np.corrcoef(x,y)[0,1]),r_squared=float(1-error@error/np.sum((y-y.mean())**2)),empirical_rpm_slope=slope,empirical_rpm_intercept=intercept,fit_mae_rpm=float(np.mean(abs(error))))
            results.append(item)
            streams[(ident,offset)]=(t,x)
    results.sort(key=lambda r:r.get('r_squared',-1),reverse=True)
    report=dict(alignment='Host monotonic timestamps, no delay/filter correction; fit only within overlapping recordings.',interpretation='Empirical correlation only. Does not establish field identity or engineering units.',duration_s=duration,gen11_samples=len(gen),gen11_active_samples=int(active.sum()),active_peak_abs_rpm=float(abs(rpm[active]).max()),last_three_seconds_mean_abs_rpm=float(abs(rpm[tail]).mean()),last_three_seconds_min_abs_rpm=float(abs(rpm[tail]).min()),last_three_seconds_max_abs_rpm=float(abs(rpm[tail]).max()),time_to_450rpm_s=float(gt[reached[0]]) if len(reached) else None,first_below_1rpm_after_stop_s=float(gt[stopped[0]]-duration) if len(stopped) else None,candidates=results)
    (p/'speed_comparison.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
    best=results[0]
    t,x=streams[(best['can_id'],best['byte_offset'])]
    fig,axes=plt.subplots(2,1,figsize=(11,7),sharex=True,layout='constrained')
    axes[0].plot(gt,abs(rpm),label='Gen1.1 measured speed magnitude',lw=1.6)
    axes[0].plot(t,abs(best['empirical_rpm_slope']*x+best['empirical_rpm_intercept']),label=f"SINtech {best['can_id']} bytes {best['byte_offset']}..{best['byte_offset']+3}: empirical fit",alpha=.75,lw=1)
    axes[0].hlines(summary['estimated_target_rpm_from_code'],0,duration,colors='gray',linestyles='dashed',label='Command: code-derived RPM')
    axes[0].set(ylabel='Speed magnitude (rpm)',title='SINtech speed pulse and independent Gen1.1 measurement')
    axes[0].legend(fontsize=9)
    axes[1].plot(t,x,color='#ae5624',label=f"{best['can_id']}, float32 LE candidate; units unknown")
    axes[1].set(ylabel='Raw candidate field',xlabel='Time since RUN request (s)')
    axes[1].legend(fontsize=9)
    for ax in axes:
        ax.axvline(0,color='green',ls=':',label='RUN on')
        ax.axvline(duration,color='red',ls=':',label='RUN off')
        ax.grid(alpha=.25)
        ax.set_xlim(gt[0],gt[-1])
    fig.savefig(p/'speed_comparison.png',dpi=160)
    plt.close(fig)
    print(json.dumps(report,indent=2))


if __name__=='__main__': main()
