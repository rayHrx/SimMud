#!/usr/bin/python3

import argparse
import cmd
import datetime
import multiprocessing
import os
import signal
import subprocess
import time

import run_client
import super_client


class SuperControlPrompt(super_client.ControlPrompt):
    def __init__(self, time, ssh_manager):
        super(SuperControlPrompt, self).__init__(time, ssh_manager)


class ServerProcessManager(run_client.ProcessManager):
    def __init__(self, process_creater):
        super(ServerProcessManager, self).__init__( process_creater)

    def stop_process(self, idx):
        assert idx < len(self.__processes)
        assert self.__processes[idx] is not None
        outs, errs = self.__processes[idx].communicate(input='q')
        print('Server:', outs)
        print('Server:', errs)
        self.__processes[idx] = None

    def __del__(self):
        for idx in filter(lambda idx: self.__processes[idx] is not None, range(len(self.__processes))):
            self.stop_process(idx)


def get_server_config(current_dir, quest, noquest, spread, static):
    config_path = None
    if quest:
        if spread:
            config_path = os.path.join(current_dir, 'config_spread_quest.ini')
        else:
            assert static
            config_path = os.path.join(current_dir, 'config_static_quest.ini')
    else:
        assert noquest
        if spread:
            config_path = os.path.join(current_dir, 'config_spread_no_quest.ini')
        else:
            assert static
            config_path = os.path.join(current_dir, 'config_static_no_quest.ini')
    
    if os.path.isfile(config_path):
        return config_path
    else:
        return None


def main(args):
    print('Info:', args)
    print('Info:')

    config_path = get_server_config(current_dir=args.current_dir, quest=args.quest, noquest=args.noquest, spread=args.spread, static=args.static)
    if config_path is None:
        print('Error:', 'Could not find server config file!')
        exit(0)

    args.client_machines = super_client.get_remote_machines(args.client_machines)

    sm = super_client.SSHManager(args.client_machines, args.username, args.password)
    if sm.get_num_machines() == 0:
        print('Error:', 'Could not connect to any of the client machines!')
        exit(0)
    print('Info:')

    def server_launcher(_):
        print('Info:', 'Launching server process')
        cmd = [os.path.join(args.current_dir, 'server'), config_path, args.port]
        print('Info:', '    ', ' '.join(cmd))
        return subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    spm = ServerProcessManager(server_launcher)
    spm.launch_process()
    time.sleep(5 * args.delay)

    print('Info:')
    launch_time = datetime.datetime.now()
    super_client.print_time(launch_time)

    super_client.launch_tasks(
        sshmanager=sm, 
        total_count=args.count, 
        remote_launcher=os.path.join(args.remote_dir, 'run_client.py'), 
        remote_cmd=os.path.join(args.remote_dir, 'client'), 
        port=args.port, 
        delay=args.delay)
    
    print('Info:')
    termination_time = None
    if args.duration is not None:
        print('Info:', 'Will terminate in', '{:.2f}'.format(args.duration), 'seconds')
        termination_time = datetime.datetime.now() + datetime.timedelta(seconds=args.duration)
        super_client.print_time(launch_time, termination_time)
        multiprocessing.Process(target=killer_process, args=(args.duration,), daemon=True).start()
    
    SuperControlPrompt((launch_time, termination_time), sm).cmdloop()


def killer_process(wait_time):
    time.sleep(wait_time)
    print('')
    print('Info:', 'Terminate!')
    os.kill(os.getppid(), signal.SIGTERM)


def parse_arguments():
    parser = argparse.ArgumentParser(description='super.py')
    parser.add_argument('--remote_dir', type=str, default='~/ece1747/SimMud', help='Remote directory')
    parser.add_argument('--current_dir', type=str, default='./', help='Current directory')

    qmode_group = parser.add_mutually_exclusive_group(required=True)
    qmode_group.add_argument('--quest', action='store_true')
    qmode_group.add_argument('--noquest', action='store_true')

    lmode_group = parser.add_mutually_exclusive_group(required=True)
    lmode_group.add_argument('--static', action='store_true')
    lmode_group.add_argument('--spread', action='store_true')

    parser.add_argument('--count', type=int, required=True, help='Number of clients to deploy')
    parser.add_argument('--delay', type=float, default=1.0, help='Delay interval between jobs launching on each machine')
    parser.add_argument('--duration', type=float, default=None, help='Time in seconds to auto terminate this script')
    
    parser.add_argument('--port', type=int, default=1747, help='Port to use')

    parser.add_argument('--client_machines', type=str, nargs='+', help='Pool of machines for client')
    parser.add_argument('--username', type=str, required=True, help='Username for SSH')
    parser.add_argument('--password', type=str, required=True, help='Password for SSH')
    
    return parser.parse_args()


if __name__ == '__main__':
    main(parse_arguments())
