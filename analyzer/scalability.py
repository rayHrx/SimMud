import argparse
import collections
import csv
import os

import matplotlib.pyplot as plt

import stats


def init(parser):
    parser.description='Plot # client vs update interval with and without quest for spread and static'
    parser.add_argument('--path', type=str, default='./metrics', help='Path to the metrics directory')
    parser.add_argument('--iter_num', type=int, default=100, help='Moving average window size')
    parser.add_argument('--debug', action='store_true', help='Print debug messages when on')


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
        database[quest_noquest][static_spread].append((nclient, largest_update_interval))

    fig = plt.figure(figsize=(24, 12))
    fig.suptitle('Scalability', fontsize=16)
    for idx, quest_noquest in enumerate(database):
        plot_chart(fig.add_subplot(1, 2, idx+1), quest_noquest, database[quest_noquest])
    plt.show()


def plot_chart(ax, quest_noquest, single_chart_database):
    '''
    single_chart_database = {static_spread: [(#client, largest_update_interval)]}
    '''
    ax.set_title(quest_noquest)
    for static_spread, dataline in single_chart_database.items():
        ax.plot(*zip(*dataline), label=static_spread)
    ax.legend()
    ax.set(xlabel='Number of Clients', ylabel='Update Interval (ms)')
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
        print('Info:', '    Largest Interval for', csv_filename, 'is', large_ui)
        largest_update_intervals.append(large_ui)
    
    if len(largest_update_intervals) == 0:
        print('Error:', run_metric_dir, 'does not have any valid csv files. Data dropped')
        return None
    largest_update_interval = max(largest_update_intervals)

    print('Info:', '        (', end='')
    print(largest_update_interval, *label_data, sep=', ', end='')
    print(')')
    return (largest_update_interval, *label_data)