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
        print('List of running processes:')
        print(*sorted(self.__procs.keys()))
        print('')
    
    def do_update(self, arg=None):
        print('Updating the status of processes..')
        procs = dict()
        for idx, proc in self.__procs.items():
            if proc.poll() is None:
                procs[idx] = proc
            else:
                proc.kill()
        self.__procs = procs

        return self.check_exit()

    def do_exit(self, arg=None):
        print('Killing all running processes..')
        for idx, proc in self.__procs.items():
            proc.kill()
            print('    Killed process', idx)
        
        self.__procs.clear()
        return self.check_exit()

    def do_kill(self, arg):
        request_to_kill = sorted(map(int, arg.split()))
        print('Try to kill:', *request_to_kill)

        to_kill = sorted(set(self.__procs.keys()).intersection(set(request_to_kill)))
        print('To kill:', *to_kill)

        for idx in to_kill:
            self.__procs[idx].kill()
            del self.__procs[idx]
            print('    Killed process', idx)

        return self.check_exit()

    def check_exit(self):
        if len(self.__procs) == 0:
            return self.clean_up()
        else:
            self.do_list()

    def clean_up(self):
        print('All processes are killed. Exiting')
        return True


def main():
    parser = argparse.ArgumentParser(description='run_client.py')
    parser.add_argument('--cmd', type=str, default='./client', help='Command to run')
    parser.add_argument('--count', type=int, required=True, help='Number of clients to deploy')
    parser.add_argument('--out', type=str, help='Place to dump output files for each client. Default is no output')
    parser.add_argument('--port', type=str, default=':1747', help='Server @<IP>:<PORT>')
    args = parser.parse_args()

    print(args)

    if args.out:
        if not os.path.exists(args.out):
            os.mkdir(args.out)
        outputs = [open(os.path.join(args.out, 'client_' + str(i) + '.log'), mode='w') for i in range(args.count)]
    else:
        outputs = [sys.stdout] * args.count

    command = args.cmd.split()
    command.append(args.port)

    print("Launching all processes '" + ' '.join(command) + "'")
    def launch_job(i):
        print('    Running process', i)
        return subprocess.Popen(command, stdout=outputs[i], stderr=outputs[i])
    procs = [launch_job(i) for i in range(args.count)]

    # turn list of procs to dict of {proc_idx: prc}
    procs = dict(enumerate(procs))

    ControlPrompt(procs).cmdloop()


if __name__ == '__main__':
    main()

