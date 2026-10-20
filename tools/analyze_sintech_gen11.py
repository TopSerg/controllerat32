"""Compare candidate SINtech payload interpretations with Gen1.1 speed.

Correlations and fitted slopes describe this recording only. They do not
establish protocol units, physical meaning, or a command encoding.
"""
import argparse
import csv
import json
from pathlib import Path
import struct

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory', type=Path)
    args = parser.parse_args()
    p = args.directory
    with (p/'gen11.csv').open() as source:
        gen = list(csv.DictReader(source))
    with (p/'sintech.csv').open() as source:
        can = list(csv.DictReader(source))
    base = int(gen[0]['host_ns'])
    gt = np.array([(int(row['host_ns'])-base)/1e9 for row in gen])
    rpm = np.array([float(row['rpm']) for row in gen])
    candidates = []
    streams = {}
    for frame in ('0x2B0','0x3B0','0x4B0','0x1B1'):
        rows = [row for row in can if row['can_id']==frame and int(row['dlc'])==8 and int(row['msgtype'])==0]
        times = np.array([(int(row['host_ns'])-base)/1e9 for row in rows])
        formats = [('<h',0)] if frame=='0x1B1' else [('<f',0),('<f',4)]
        ref = np.interp(times,gt,rpm)
        for fmt, offset in formats:
            values = np.array([struct.unpack_from(fmt,bytes.fromhex(row['data_hex']),offset)[0] for row in rows])
            label = f'{frame} bytes {offset}..{offset+struct.calcsize(fmt)-1} ({fmt})'
            streams[(frame,offset)] = (times,values)
            record = {'frame_id':frame,'byte_offset':offset,'interpretation_hypothesis':fmt,
                      'samples':len(values),'raw_min':float(values.min()),'raw_max':float(values.max())}
            if np.std(values)>1e-10:
                centered = values-values.mean()
                slope = float(centered@(ref-ref.mean())/(centered@centered))
                intercept = float(ref.mean()-slope*values.mean())
                error = ref-(slope*values+intercept)
                record.update(correlation=float(np.corrcoef(values,ref)[0,1]),
                              r_squared=float(1-(error@error)/np.sum((ref-ref.mean())**2)),
                              empirical_slope=slope,empirical_intercept=intercept,
                              mean_absolute_error_rpm=float(np.mean(abs(error))))
            else:
                record.update(correlation=None,r_squared=None,empirical_slope=None,
                              empirical_intercept=None,mean_absolute_error_rpm=None)
            candidates.append(record)
    candidates.sort(key=lambda row: row['r_squared'] if row['r_squared'] is not None else -1, reverse=True)
    with (p/'candidate_correlations.csv').open('w',newline='',encoding='utf-8') as output:
        writer=csv.DictWriter(output,fieldnames=list(candidates[0]))
        writer.writeheader()
        writer.writerows(candidates)
    result={'status':'candidate identification only; physical units and command encoding not established',
            'alignment':'host monotonic reception timestamps; no correction for telemetry filtering/delivery delay',
            'gen11_rpm_range':[float(rpm.min()),float(rpm.max())],
            'gen11_samples':len(gen), 'can_frames':len(can),
            'gen11_read_ms_median':float(np.median([float(row['read_ms']) for row in gen])),
            'gen11_read_ms_max':max(float(row['read_ms']) for row in gen),
            'target_elapsed_ms':int(gen[-1]['target_tick_ms'])-int(gen[0]['target_tick_ms']),
            'host_elapsed_s':float(gt[-1]),'candidates':candidates}
    (p/'analysis.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    fig,axes=plt.subplots(3,1,figsize=(12,9),layout='constrained')
    axes[0].plot(gt,rpm,lw=1,color='#244b91')
    axes[0].set(ylabel='Gen1.1 speed (rpm)',title='Simultaneous recording: reference speed and SINtech candidate field')
    times,values=streams[('0x4B0',0)]
    axes[1].plot(times,values,lw=1,color='#b6531d')
    axes[1].set(ylabel='0x4B0 bytes 0..3\nfloat32 LE (units unknown)',xlabel='Time since first Gen1.1 sample (s)')
    axes[0].set_xlim(gt[0],gt[-1]); axes[1].set_xlim(gt[0],gt[-1])
    ref=np.interp(times,gt,rpm)
    points=axes[2].scatter(values[::4],ref[::4],s=7,c=times[::4],cmap='viridis',alpha=.65)
    axes[2].set(xlabel='0x4B0 bytes 0..3, float32 LE (raw)',ylabel='Gen1.1 speed (rpm)',
                title=f'Correlation in this run: r={candidates[0]["correlation"]:.4f}; units remain unverified')
    fig.colorbar(points,ax=axes[2],label='Time (s)')
    for ax in axes:
        ax.grid(alpha=.25)
    fig.savefig(p/'comparison.png',dpi=160)
    plt.close(fig)
    print(json.dumps(result,indent=2))


if __name__=='__main__':
    main()
