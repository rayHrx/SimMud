import argparse

import trajectory
import scalability

parser = argparse.ArgumentParser()
subparsers = parser.add_subparsers(dest='mode', required=True)

# python analyzer trajectory
parser_trajectory = subparsers.add_parser('trajectory', aliases=['t'])
parser_trajectory.set_defaults(func=trajectory.main)
trajectory.init(parser_trajectory)

# python analyzer scalability
parser_scalability = subparsers.add_parser('scalability', aliases=['s'])
parser_scalability.set_defaults(func=scalability.main)
scalability.init(parser_scalability)

# Invoke main
args = parser.parse_args()
args.func(args)
