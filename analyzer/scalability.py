import argparse
import os

import matplotlib.pyplot as plt


def init(parser):
    parser.description='Plot # client vs update interval with and without quest for spread and static'
    parser.add_argument('--path', type=str, default='./metrics', help='Path to the metrics directory')
    parser.add_argument('--iter_num', type=int, default=100, help='Moving average window size')
    parser.add_argument('--debug', action='store_true', help='Print debug messages when on')

def main(args):
    run_metrics = list(filter(os.path.isdir, [os.path.join(args.path, o) for o in os.listdir(args.path)]))
    print('Info:', 'Found metric data of ', len(run_metrics), 'runs')
