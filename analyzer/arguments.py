import argparse


def load_argument(parser):
    parser.add_argument('--iter_num', type=int, default=100, help='Moving average window size')
    parser.add_argument('--max_row', type=int, default=30000, help='Number of iterations of raw data to process')

    parser.add_argument('--debug', action='store_true', help='Print debug messages when on')
    parser.add_argument('--gui', action='store_true', help='Open charts on GUI')
    parser.add_argument('--output', type=str, help='Location to dump chart')
