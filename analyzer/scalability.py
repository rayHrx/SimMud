import argparse
import collections
import csv
import datetime
import os

import matplotlib.pyplot as plt

import stats


def init(parser):
    parser.description='Plot # client vs update interval with and without quest for spread and static'
    parser.add_argument('--path', type=str, default='./metrics', help='Path to the metrics directory')
    parser.add_argument('--iter_num', type=int, default=100, help='Moving average window size')
    parser.add_argument('--debug', action='store_true', help='Print debug messages when on')
    parser.add_argument('--gui', action='store_true', help='Open charts on GUI')
    parser.add_argument('--output', type=str, help='Location to dump chart')


def main(args):
    run_metric_dirs = list(filter(os.path.isdir, [os.path.join(args.path, o) for o in os.listdir(args.path)]))
    print('Info:', 'Found metric data of ', len(run_metric_dirs), 'runs')

    # [(largest_update_interval, static/spread, quest/noquest, #client)]
    dataset = [parse_run_metric(run_metric_dir, args) for run_metric_dir in run_metric_dirs]
    dataset = [data for data in dataset if data]
    
    # Reorganize data
    # {quest_noquest: {static_spread: [(#client, largest_update_interval)]}}
    database = collections.defaultdict(lambda:collections.defaultdict(list))
    for row in dataset:
        largest_update_interval, static_spread, quest_noquest, nclient = row
        database[quest_noquest][static_spread].append((int(nclient), largest_update_interval))
    
    # Printing Stats
    print('Info:')
    print('Info:', 'Stats:')
    for quest_noquest, chart_database in database.items():
        for static_spread, dataline in chart_database.items():
            print('Info:', '    ' + str(len(dataline)), 'data points are available for', '[' + quest_noquest + ']', '[' + static_spread + ']')
    print('Info:', '    ' + str(len(dataset)), 'data points are available in TOTAL')
    print('Info:')

    fig = plt.figure('Scalability Charts', figsize=(16, 8))
    fig.suptitle('Scalability of Update Interval Time with varying Number of Clients', fontsize=16)
    for idx, quest_noquest in enumerate(database):
        plot_chart(fig.add_subplot(1, len(database), idx+1), quest_noquest, database[quest_noquest])
    
    if args.gui:
        plt.show()

    if args.output:
        filename = 'scalability_' + str(len(dataset)) + '_' + datetime.datetime.now().strftime('%y%m%d_%H%M%S') + '.png'
        plt.savefig(filename)
        print('Info:', 'Chart is dumped to', filename)


def plot_chart(ax, quest_noquest, single_chart_database):
    '''
    single_chart_database = {static_spread: [(#client, largest_update_interval)]}
    '''
    ax.set_title(quest_noquest)
    for static_spread, dataline in single_chart_database.items():
        ax.plot(*zip(*sorted(dataline)), label=static_spread, marker='.')
    ax.legend()
    ax.set(xlabel='Number of Clients', ylabel='Update Interval Time (ms)')
    ax.set_ylim(bottom=0.)
    ax.grid(axis='x', linestyle='--')
    ax.grid(axis='y', linestyle='-')


def parse_label_file(run_metric_dir):
    '''
    (static/spread, quest/noquest, #client) if data available
    None if not
    '''
    label_file_name = 'label.txt'
    label_file_path = os.path.join(run_metric_dir, label_file_name)
    
    if not os.path.isfile(label_file_path):
        return None
    
    with open(label_file_path, mode='r') as f:
        csv_reader = csv.reader(f, delimiter=',')
        row = next(iter(csv_reader))

        assert len(row) == 3
        return row


def parse_run_metric(run_metric_dir, args):
    '''
    (largest_update_interval, static/spread, quest/noquest, #client) if data available
    None if data is not available
    '''
    print('Info:', 'Parsing', run_metric_dir)

    # Label file
    label_data = parse_label_file(run_metric_dir)
    if label_data is None:
        print('Error:', run_metric_dir, 'does not have a valid label file. Data dropped')
        return None

    # CSV files
    csv_filenames = [o for o in os.listdir(run_metric_dir) if os.path.isfile(os.path.join(run_metric_dir, o)) and o[-4:] == '.csv']

    largest_update_intervals = list()
    for csv_filename in csv_filenames:
        avg = stats.calculate_avg(os.path.join(run_metric_dir, csv_filename), args.iter_num, args.debug)
        update_interval = avg[4]
        large_ui = max(update_interval)
        if args.debug:
            print('Debug:', '    Largest Interval for', csv_filename, 'is', large_ui)
        largest_update_intervals.append(large_ui)
    
    if len(largest_update_intervals) == 0:
        print('Error:', run_metric_dir, 'does not have any valid csv files. Data dropped')
        return None
    largest_update_interval = max(largest_update_intervals)

    print('Info:', '    (', end='')
    print(largest_update_interval, *label_data, sep=', ', end='')
    print(')')
    return (largest_update_interval, *label_data)
