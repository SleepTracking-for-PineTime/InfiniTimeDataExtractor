#!/usr/bin/env python3
"""Visualize sensor data collected by InfiniTimeDataExtractor"""
import argparse
import datetime
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as md


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--infile", "-i", required=True, help="Input CSV file")
    parser.add_argument("--raw", action="store_true",
                        help="Disable accelerometer conversion from signed 12-bit to g")
    parser.add_argument("--outfile", "-o")
    parser.add_argument("--battery", action="store_true", help="Also plot battery level")
    args = parser.parse_args()

    data = np.genfromtxt(args.infile, delimiter=",", names=True)
    dt = np.array([datetime.datetime.fromtimestamp(x, datetime.timezone.utc) for x in data['Time']])

    if args.battery:
        fig = plt.figure(figsize=(12, 12))
        gs = fig.add_gridspec(3, 1)
        ax1 = fig.add_subplot(gs[0, :])
        ax2 = fig.add_subplot(gs[1, :], sharex=ax1)
        ax3 = fig.add_subplot(gs[2, :], sharex=ax1)
        axes = [ax1, ax2, ax3]
    else:
        fig = plt.figure(figsize=(12, 8))
        gs = fig.add_gridspec(2, 1)
        ax1 = fig.add_subplot(gs[0, :])
        ax2 = fig.add_subplot(gs[1, :], sharex=ax1)
        axes = [ax1, ax2]
    xfmt = md.DateFormatter('%Y-%m-%d %H:%M:%S')

    for accel_axis in ['X', 'Y', 'Z']:
        x = data[f'Motion{accel_axis}']
        if not args.raw:
            x = x * 2 / 2048  # convert to g
        ax1.plot(dt, x, label=accel_axis)
    ax1.set_ylabel("Accelerometer" + (" [g]" if not args.raw else ""))
    ax1.legend()

    ax2.plot(dt, data['Heartrate'], label="Heart rate")
    ax2.set_ylabel("Heart rate [bpm]")

    if args.battery:
        ax3.plot(dt, data['Battery_Level'])
        ax3.set_ylabel("Battery level [%]")

    for ax in axes:
        ax.set_xlabel("Date/time")
        ax.grid()
        ax.xaxis.set_major_formatter(xfmt)

    if args.outfile:
        plt.tight_layout()
        plt.savefig(args.outfile)
    else:
        plt.show()
