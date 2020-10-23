import argparse

import trajectory

parser = argparse.ArgumentParser()
subparsers = parser.add_subparsers(dest='mode', required=True)

# python analyzer trajectory
parser_trajectory = subparsers.add_parser('trajectory')
parser_trajectory.set_defaults(func=trajectory.main)
trajectory.init(parser_trajectory)

# Invoke main
args = parser.parse_args()
args.func(args)
