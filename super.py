#!/usr/bin/python3

import argparse
import cmd
import datetime
import multiprocessing
import os
import random
import signal
import socket
import subprocess
import time

import run_client
import super_client


# Process tombstone endpoint 1
class SuperControlPrompt(super_client.ControlPrompt):
    def __init__(self, time, ssh_manager, label_message):
        super(SuperControlPrompt, self).__init__(time, ssh_manager)
        self.__label_message = label_message

    def do_load(self, arg=None):
        run_client.print_load()

    def do_label(self, arg=None):
        print('Info:', self.__label_message.get_label())
        print('Info:')


class ServerProcessManager(run_client.ProcessManager):
    def __init__(self, process_creater):
        super(ServerProcessManager, self).__init__(process_creater)

    def stop_process(self, idx):
        assert idx < len(self.get_processes())
        assert self.get_processes()[idx] is not None
        print('Info:', '    Stopping server')
        outs, errs = self.get_processes()[idx].communicate(input=bytes('q\n', 'ascii'))
        if outs:
            for line in outs.decode('utf-8').splitlines():
                print('Server STDOUT:', '    ', line)
        if errs:
            for line in errs.decode('utf-8').splitlines():
                print('Server STDERR:', '    ', line)
        self.get_processes()[idx] = None

    def __del__(self):
        for idx in filter(lambda idx: self.get_processes()[idx] is not None, range(len(self.get_processes()))):
            self.stop_process(idx)


class LabelMessenger():
    def __init__(self, quest_noquest, spread_static, count):
        self.__quest_noquest = quest_noquest
        self.__spread_static = spread_static
        self.__count = count
    
    def get_label(self):
        return (self.__quest_noquest, self.__spread_static, str(self.__count))

    def print_git_message(self):
        print('Info:')
        print('Info:', 'Don\'t forget to git!')
        print('Info:', '    ', 'git status')
        print('Info:', '    ', 'git add .')
        print('Info:', '    ', 'git commit -m \'' + ' '.join(self.get_label()) + '\'')
        print('Info:', '    ', 'git pull')
        print('Info:', '    ', 'git push')
        print('Info:')

    def __del__(self):
        self.print_git_message()


# Process tombstone endpoint 2
class SignalHandler():
    def __init__(self, ssh_manager, server_process_manager, label_message):
        self.__ssh_manager = ssh_manager
        self.__server_process_manager = server_process_manager
        self.__label_message = label_message
        signal.signal(signal.SIGTERM, self.exit_gracefully)

    def exit_gracefully(self, signum, frame):
        self.__ssh_manager.__del__()
        self.__server_process_manager.__del__()
        exit(0)


def get_server_config(path, quest, noquest, spread, static):
    config_path = None
    if quest:
        if spread:
            config_path = os.path.join(path, 'config_spread_quest.ini')
        else:
            assert static
            config_path = os.path.join(path, 'config_static_quest.ini')
    else:
        assert noquest
        if spread:
            config_path = os.path.join(path, 'config_spread_no_quest.ini')
        else:
            assert static
            config_path = os.path.join(path, 'config_static_no_quest.ini')
    
    if os.path.isfile(config_path):
        return config_path
    else:
        return None


def main(args):
    print('Info:', args)
    print('Info:')

    cur_host_name = socket.gethostname()
    print('Info:', '@' + cur_host_name)

    local_path = os.path.expanduser(args.path)
    config_path = get_server_config(path=local_path, quest=args.quest, noquest=args.noquest, spread=args.spread, static=args.static)
    if config_path is None:
        print('Error:', 'Could not find server config file in', local_path)
        exit(0)

    args.client_machines = super_client.get_remote_machines(args.client_machines)

    sm = super_client.SSHManager(args.client_machines, args.username, args.password)
    if sm.get_num_machines() == 0:
        print('Error:', 'Could not connect to any of the client machines!')
        exit(0)
    print('Info:')

    if args.port is None:
        args.port = random.randint(1500, 60000)

    server_host_port = cur_host_name + ':' + str(args.port)
    def server_launcher(_):
        print('Info:', 'Launching server process', '@' + server_host_port)
        cmd = [os.path.join(local_path, 'server'), config_path, str(args.port)]
        print('Info:', '    ', ' '.join(cmd))
        return subprocess.Popen(cmd, stdin=subprocess.PIPE)#, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    spm = ServerProcessManager(server_launcher)

    # Auto messenger on exit
    label_msger = LabelMessenger('quest' if args.quest else 'noquest', 'spread' if args.spread else 'static', args.count)

    # Register the signal handler
    sh = SignalHandler(sm, spm, label_msger)

    spm.launch_process()
    time.sleep(5 * args.delay)

    print('Info:')
    launch_time = datetime.datetime.now()
    super_client.print_time(launch_time)

    super_client.launch_tasks(
        sshmanager=sm, 
        total_count=args.count, 
        remote_launcher=os.path.join(args.path, 'run_client.py'), 
        remote_cmd=os.path.join(args.path, 'client'), 
        port=server_host_port, 
        delay=args.delay)
    
    print('Info:')
    termination_time = None
    if args.duration is not None:
        print('Info:', 'Will terminate in', '{:.2f}'.format(args.duration), 'seconds')
        termination_time = datetime.datetime.now() + datetime.timedelta(seconds=args.duration)
        super_client.print_time(launch_time, termination_time)
        multiprocessing.Process(target=killer_process, args=(args.duration,), daemon=True).start()
    
    print('Info:')
    SuperControlPrompt((launch_time, termination_time), sm, label_msger).cmdloop('DO NOT CTRL-C!')
    sh.exit_gracefully(None, None)


def killer_process(wait_time):
    time.sleep(wait_time)
    print('')
    print('Info:', 'Terminate due to --duration!')
    os.kill(os.getppid(), signal.SIGTERM)


def parse_arguments():
    parser = argparse.ArgumentParser(description='super.py')
    parser.add_argument('--path', type=str, default='~/ece1747/SimMud', help='Directory')

    qmode_group = parser.add_mutually_exclusive_group(required=True)
    qmode_group.add_argument('--quest', action='store_true')
    qmode_group.add_argument('--noquest', action='store_true')

    lmode_group = parser.add_mutually_exclusive_group(required=True)
    lmode_group.add_argument('--static', action='store_true')
    lmode_group.add_argument('--spread', action='store_true')

    parser.add_argument('--count', type=int, required=True, help='Number of clients to deploy')
    parser.add_argument('--delay', type=float, default=1.0, help='Delay interval between jobs launching on each machine')
    parser.add_argument('--duration', type=float, default=None, help='Time in seconds to auto terminate this script')
    
    parser.add_argument('--port', type=int, default=None, help='Port to use. Random by default')

    parser.add_argument('--client_machines', type=str, nargs='+', help='Pool of machines for client')
    parser.add_argument('--username', type=str, required=True, help='Username for SSH')
    parser.add_argument('--password', type=str, required=True, help='Password for SSH')
    
    return parser.parse_args()


if __name__ == '__main__':
    main(parse_arguments())
