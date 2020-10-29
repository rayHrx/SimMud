#!/usr/bin/python3

import argparse
import cmd
import copy
import datetime
import itertools
import random
import time

try:
    import paramiko
except:
    print('paramiko is not installed. Try "pip install paramiko"')


class SSHManager:
    def __init__(self, machines, username, password):
        def connect_client(machine, username, password):
            client = paramiko.SSHClient()
            client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            try:
                client.connect(machine, username=username, password=password)
                print('Info:', 'Connected to', machine, 'successfully')
                return client
            except:
                print('Error:', 'Could not connect to', machine)
                return None

        machines_connected = [(machine, connect_client(machine, username, password)) for machine in machines]
        machines_connected = list(filter(lambda x: x[1] is not None, machines_connected))
        
        self.__machine_names = None
        self.__machines = None
        self.__ioe = None
    
        if len(machines_connected) > 0:
            self.__machine_names, self.__machines = zip(*machines_connected)
            self.__ioe = [None] * len(self.__machines)

    def get_num_machines(self):
        return len(self.__machines)
    
    def get_machine(self, idx):
        assert idx < self.get_num_machines()
        return self.__machines[idx]

    def get_ioe(self, idx):
        assert idx < self.get_num_machines()
        return self.__ioe[idx]

    def refresh_ioe(self):
        def check_alive(ioe):
            if ioe is not None:
                if not ioe[2].channel.closed:
                    return ioe
            else:
                return None
        self.__ioe = list(map(check_alive, self.__ioe))

    def get_machine_name(self, idx):
        assert idx < self.get_num_machines()
        return self.__machine_names[idx]

    def get_machine_name_str(self, idx):
        assert idx < self.get_num_machines()
        return '[' + str(idx) + ']' + ' ' + self.__machine_names[idx]

    def launch_task_on_machine(self, idx, task_launcher):
        '''
        (stdin, stdout, stderr) = task_launcher(idx, machine, machine_name)
        '''
        assert idx < self.get_num_machines()
        assert task_launcher is not None
        self.__ioe[idx] = task_launcher(idx, self.get_machine(idx), self.get_machine_name(idx))

    def close_machine(self, idx):
        assert idx < self.get_num_machines()
        self.__machines[idx].close()
        print('Info:', '    Closed', self.get_machine_name(idx))

    def close_all(self):
        if self.__machines is not None:
            for idx in range(self.get_num_machines()):
                self.close_machine(idx)
    
            self.__machine_names = None
            self.__machines = None
            self.__ioe = None
    
    def __del__(self):
        self.close_all()
            

class ControlPrompt(cmd.Cmd):
    def __init__(self, launch_time, ssh_manager, args):
        '''
        ssh_manager is SSHManager
        '''
        assert isinstance(ssh_manager, SSHManager)
        super(ControlPrompt, self).__init__()
        self.__launch_time = launch_time
        self.__ssh_manager = ssh_manager
        self.__args = args

    def do_list(self, arg=None):
        self.__ssh_manager.refresh_ioe()
        num_machines = self.__ssh_manager.get_num_machines()
        print('Info:', 'List of', num_machines, 'connected machines:')
        for idx in range(num_machines):
            print('Info:', '    ' + self.__ssh_manager.get_machine_name_str(idx), ':', 'Running' if self.__ssh_manager.get_ioe(idx) is not None else 'Idling')
        print('')

    def do_run(self, arg):
        '''
        Usage: run idx <command>
        '''
        arg = arg.split()
        if len(arg) < 2:
            print('Error:', 'Wrong number of arguments')
            return

        idx = int(arg[0])

        if not self.check_machine_existance(idx):
            return

        if self.__ssh_manager.get_ioe(idx) is not None:
            print('Error:', self.__ssh_manager.get_machine_name_str(idx), 'is already running')

        def launcher(idx, machine, machine_name):
            command = arg[1:]
            command = list(map(lambda x: str(x), command))
            command = ' '.join(command)
            print('Info:', 'Launching:')
            print('Info:', '    ' + '@', '[' + str(idx) + ']', machine_name)
            print('Info:', '    ' + command)
            return machine.exec_command(command, get_pty=True)
        self.__ssh_manager.launch_task_on_machine(idx, launcher)
        print('')

    def do_launch(self, arg):
        '''
        Usage: launch idx <count>
        Info:
            1. Will launch <count> number of processes to machine idx
        ''' 
        arg = arg.split()
        if len(arg) != 2:
            print('Error:', 'Wrong number of arguments')
            return

        idx = int(arg[0])
        count = int(arg[1])
        
        if not self.check_machine_existance(idx):
            return

        if self.__ssh_manager.get_ioe(idx) is not None:
            print('Error:', self.__ssh_manager.get_machine_name_str(idx), 'is already running')

        self.__ssh_manager.launch_task_on_machine(idx, construct_launcher(remote_launcher=self.__args.remote_launcher, cmd=self.__args.cmd, count=count, port=self.__args.port, stdout=self.__args.stdout))
        print('')

    def do_talk(self, arg):
        '''
        Usage: talk idx {command}
        Info:
            1. if {command} is left empty, will simply refresh stdout
        '''
        arg = arg.split()
        if len(arg) < 1:
            print('Error:', 'Missing arguments')
            return

        idx = int(arg[0])
        forward_arg = None
        if len(arg) > 1:
            forward_arg = ' '.join(arg[1:])

        if not self.check_machine_existance(idx):
            return

        if forward_arg is not None:
            print('Info:', 'Forwarding', '"' + str(forward_arg) + '"', 'to', self.__ssh_manager.get_machine_name_str(idx))

        # Get stdin, stdout, stderr
        ioe = self.__ssh_manager.get_ioe(idx)
        if ioe is None:
            print('Warning:', 'Machine', idx, 'is not running any jobs')
            return
        else:
            i, o, e = ioe

        print('Info:')

        if forward_arg is not None:
            # Print stdout before forwarding to stdin
            o.channel.settimeout(1)
            try:
                for line in o:
                    print('        >', line.strip('\n'))
            except:
                pass

            print('        $', forward_arg)
            # Forward to stdin
            i.write(forward_arg + '\n')
            i.flush()

        # Print stdout after forwarding to stdin
        o.channel.settimeout(3)
        try:
            for line in o:
                print('        >', line.strip('\n'))
        except:
            pass

        print('Info:')
        # Reset
        o.channel.settimeout(None)
    
    def do_time(self, arg=None):
        print_launch_time(self.__launch_time, True)
        print('Info:')

    def do_exit(self, arg=None):
        print('Info:', 'Closing connections to', self.__ssh_manager.get_num_machines(), 'machines')
        self.__ssh_manager.close_all()
        print('Info: Done. Exiting')
        print('Info:')

        return True

    def check_machine_existance(self, idx):
        if idx >= self.__ssh_manager.get_num_machines():
            print('Error:', idx, 'is not a valid Machine ID')
            return False
        return True

def construct_launcher(remote_launcher, cmd, count, port, stdout):
    def launcher(idx, machine, machine_name):
        command = [remote_launcher, '--cmd', cmd, '--count', count, '--port', port]
        if stdout:
            command.append('--stdout')
        command = list(map(lambda x: str(x), command))
        command = ' '.join(command)
        print('Info:', 'Launching:')
        print('Info:', '    ' + '@', '[' + str(idx) + ']', machine_name)
        print('Info:', '    ' + command)
        return machine.exec_command(command, get_pty=True)
    return launcher

def print_launch_time(launch_time, show_elapsed=False):
    print('Info:', 'Launch :', launch_time.strftime('%Y-%m-%d %H:%M:%S'))
    if show_elapsed:
        now = datetime.datetime.now()
        print('Info:', 'Now    :', now.strftime('%Y-%m-%d %H:%M:%S'))
        print('Info:', 'Elasped:', '{:.2f}'.format((now - launch_time).total_seconds()), 'seconds')

def main(args):
    print('Info:', args)
    print('Info:')

    if args.machines is None:
        args.machines = [
            #'ug210.eecg.utoronto.ca', 
            'ug211.eecg.utoronto.ca',
            'ug212.eecg.utoronto.ca',
            'ug213.eecg.utoronto.ca', 
            'ug214.eecg.utoronto.ca', 
            'ug215.eecg.utoronto.ca', 
            'ug216.eecg.utoronto.ca', 
            'ug217.eecg.utoronto.ca', 
            'ug218.eecg.utoronto.ca', 
            'ug219.eecg.utoronto.ca', 
            'ug220.eecg.utoronto.ca', 
            'ug221.eecg.utoronto.ca', 
            'ug222.eecg.utoronto.ca', 
            'ug223.eecg.utoronto.ca', 
            'ug224.eecg.utoronto.ca',
            'ug225.eecg.utoronto.ca',
            'ug226.eecg.utoronto.ca',
            'ug227.eecg.utoronto.ca',
            'ug228.eecg.utoronto.ca',
            'ug229.eecg.utoronto.ca',
            'ug230.eecg.utoronto.ca',
            'ug231.eecg.utoronto.ca',
            'ug232.eecg.utoronto.ca',
            'ug233.eecg.utoronto.ca',
            'ug234.eecg.utoronto.ca',
            'ug235.eecg.utoronto.ca',
            'ug236.eecg.utoronto.ca',
            'ug237.eecg.utoronto.ca',
            'ug238.eecg.utoronto.ca',
            'ug239.eecg.utoronto.ca'
            ]
        random.shuffle(args.machines)

    if args.admin:
        print('Info:', 'Running in admin mode')
    
    if not args.admin:
        print('Info:', 'Runing a total of', args.count, 'processes')
        required_machines_count = (int((args.count - 1) / args.threshold) + 1)
        if required_machines_count > len(args.machines):
            print('Error:', 'Not enough machines for running', args.count, 'jobs')
            print('Info:', '    Current computing power is', args.threshold, '*', len(args.machines), '=', args.threshold * len(args.machines))
            print('Info:', '    Still needs', int(required_machines_count - len(args.machines)), 'machines')
            exit(0)

    sm = SSHManager(args.machines, args.username, args.password)
    launch_time = datetime.datetime.now()
    print('Info:')
    print_launch_time(launch_time)

    if not args.admin:
        print('Info:')
        machine_iter = itertools.cycle(range(sm.get_num_machines()))
        count_left = args.count
        while count_left > 0:
            machine_idx_to_run = next(machine_iter)
            count_to_use = min(args.threshold, count_left)

            sm.launch_task_on_machine(machine_idx_to_run, construct_launcher(remote_launcher=args.remote_launcher, cmd=args.cmd, count=count_to_use, port=args.port, stdout=args.stdout))
            time.sleep(args.delay)

            count_left = count_left - count_to_use

    print('Info:')
    ControlPrompt(launch_time, sm, args).cmdloop()


def parse_arguments():
    parser = argparse.ArgumentParser(description='super_client.py')
    parser.add_argument('--admin', action='store_true', help='SSH to all the machines, but without executing any commands')
    parser.add_argument('--remote_launcher', type=str, default='~/ece1747/SimMud/run_client.py', help='Location of remoate_launcher in remote location, aka, run_client.py')
    parser.add_argument('--count', type=int, default=1000, help='Number of processes to deploy')
    parser.add_argument('--threshold', type=int, default=500, help='Number of processes to launch for each machine')
    parser.add_argument('--delay', type=float, default=0.5, help='Delay interval between jobs launching on each machine')
    # Forwarded to remote_launcher
    parser.add_argument('--port', type=str, default=':1747', help='Forward to remote_launcher Server @<IP>:<PORT>')
    parser.add_argument('--cmd', type=str, default='~/ece1747/SimMud/client', help='Forward to remote_launcher --cmd')
    parser.add_argument('--stdout', action='store_true', help='Forward to remote_launcher --stdout')
    # SSH-related
    parser.add_argument('--machines', type=str, nargs='+', help='Pool of machines for SSH')
    parser.add_argument('--username', type=str, required=True, help='Username for SSH')
    parser.add_argument('--password', type=str, required=True, help='Password for SSH')
    
    return parser.parse_args()


if __name__ == '__main__':
    main(parse_arguments())
