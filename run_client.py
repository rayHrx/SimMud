#!/usr/bin/python3

import argparse
import os
import subprocess
import sys
from cmd import Cmd


class ControlPrompt(Cmd):
    def __init__(self, procs):
        '''
        procs is a dict of {idx: proc}
        '''
        super(ControlPrompt, self).__init__()
        self.__procs = procs

    def do_list(self, arg=None):
        print('Info:', 'List of running processes:')
        print('Info:', *sorted(self.__procs.keys()))
        print('')
    
    def do_update(self, arg=None):
        print('Info:', 'Updating the status of processes..')
        procs = dict()
        for idx, proc in self.__procs.items():
            if proc.poll() is None:
                procs[idx] = proc
            else:
                proc.kill()
        self.__procs = procs

        return self.check_exit()

    def do_exit(self, arg=None):
        print('Warning:', 'Killing running processes.. ALL')
        for idx, proc in self.__procs.items():
            proc.kill()
            print('Warning:', '    Killed process', idx)
        
        self.__procs.clear()
        return self.check_exit()

    def do_kill(self, arg):
        request_to_kill = sorted(map(int, arg.split()))
        print('Info:', 'To kill:', *request_to_kill)

        to_kill = sorted(set(self.__procs.keys()).intersection(set(request_to_kill)))
        print('Warning:', 'Killing running processes..', *to_kill)

        for idx in to_kill:
            self.__procs[idx].kill()
            del self.__procs[idx]
            print('Warning:', '    Killed process', idx)

        return self.check_exit()

    def check_exit(self):
        if len(self.__procs) == 0:
            return self.clean_up()
        else:
            self.do_list()

    def clean_up(self):
        print('Info:')
        print('Info:', 'All processes are killed. Exiting')
        print('Info:')
        return True


def main(args):
    print('Info:', args)
    print('Info:')

    if args.wait:
        print('Warning:', 'The program will terminate once all subprocesses are terminated')
    if args.silent: # Suppress all outputs
        outputs = [subprocess.DEVNULL] * args.count
        out_place_str = 'supressed'
    else:
        if args.output: # Redirect output to files
            if not os.path.exists(args.output):
                os.mkdir(args.output)
            outputs = [open(os.path.join(args.output, 'client_' + str(i) + '.log'), mode='w') for i in range(args.count)]
            out_place_str = 'redirected into ' + args.output
        else: # stdout
            outputs = [sys.stdout] * args.count
            out_place_str = 'redirected into stdout'
    print('Info:', 'All output of running processes are', out_place_str)

    command = args.cmd.split()
    command.append(args.port)

    print('Info:')
    print('Info:', "Launching all processes '" + ' '.join(command) + "'")
    def launch_job(i):
        print('Info:', '    Running process', i)
        return subprocess.Popen(command, stdout=outputs[i], stderr=outputs[i])
    procs = [launch_job(i) for i in range(args.count)]

    if args.wait:
        for proc in procs:
            proc.wait()
    else:
        # turn list of procs to dict of {proc_idx: prc}
        ControlPrompt(dict(enumerate(procs))).cmdloop()


def parse_arguments():
    parser = argparse.ArgumentParser(description='run_client.py')
    parser.add_argument('--count', type=int, required=True, help='Number of clients to deploy')
    parser.add_argument('--port', type=str, default=':1747', help='Server @<IP>:<PORT>')
    parser.add_argument('--cmd', type=str, default='./client', help='Command to run')

    parser.add_argument('--output', type=str, help='Place to dump output files for each client. Default is stdout.')
    parser.add_argument('--silent', action='store_true', help='Suppress all output. Default is stdout.')

    parser.add_argument('--wait', action='store_true', help='Disable the command shell, exit when all processes are done. Default is command shell.')
    
    return parser.parse_args()


if __name__ == '__main__':
    main(parse_arguments())
