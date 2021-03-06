import argparse
import collections
import csv
import datetime
import multiprocessing
import os
import time

try:
    import matplotlib.pyplot as plt
except:
    print('Please pip install matplotlib')
try:
    import numpy as np
except:
    print('Please pip install numpy')

import arguments
import trajectory
import utility


def float_fmt(num):
    return '{:.2f}'.format(num)


def init(parser):
    parser.description='Plot # client vs update interval with and without quest for spread and static'
    parser.add_argument('--path', type=str, default='./metrics', help='Path to the metrics directory')
    black_white_group = parser.add_mutually_exclusive_group(required=False)
    black_white_group.add_argument('--whitelist', type=str, nargs='+', help='List of groups to include. Check comma separated group_strs inside group.txt')
    black_white_group.add_argument('--blacklist', type=str, nargs='+', help='List of groups to exclude. Check comma separated group_strs inside group.txt')
    arguments.load_argument(parser)


def main(args):
    run_names = [o for o in os.listdir(args.path) if os.path.isdir(os.path.join(args.path, o))]
    print('Info:', 'Found metric data of', len(run_names), 'runs')

    print('Info:', 'Parsing in parallel...')
    start = time.time()
    dataset = multiprocessing.Pool().map(parse_run_metric_wrapper, map(lambda run_name: (run_name, args), run_names))
    end = time.time()
    print('Info:')
    print('Info:', 'Parsing took', float_fmt(end - start), 'seconds')

    # (largest_update_interval, static/spread, quest/noquest, nclient, run_name, avgs5db)
    dataset = [data for data in dataset if data]
    # Check for data validity
    check_data_validity(dataset)

    # Reorganize data
    # {quest_noquest: {static_spread: sorted [(nclient, largest_update_interval, run_name, avgs5db)]}}
    database = collections.defaultdict(lambda:collections.defaultdict(list))
    for row in dataset:
        largest_update_interval, static_spread, quest_noquest, nclient, run_name, avgs5db = row
        database[quest_noquest][static_spread].append((int(nclient), largest_update_interval, run_name, avgs5db))
    for datachart in database.values():
        for dataline in datachart.values():
            dataline.sort(key=lambda x: (x[0], x[1]))
    
    # Printing Stats
    print('Info:')
    print('Info:', 'Stats:')
    for quest_noquest, chart_database in database.items():
        for static_spread, dataline in chart_database.items():
            print('Info:', '    ' + str(len(dataline)), 'data points are available for', '[' + quest_noquest + ']', '[' + static_spread + ']')
    print('Info:', '    ' + str(len(dataset)), 'data points are available in TOTAL')
    print('Info:')

    figsize = (16,8)
    figname = 'scalability_' + str(len(dataset)) + '_' + datetime.datetime.now().strftime('%y%m%d_%H%M%S')
    fig = plt.figure(figname, figsize=figsize)
    fig.suptitle('Scalability of Update Interval Time with varying Number of Clients', fontsize=16)
    for idx, quest_noquest in enumerate(database):
        plot_chart(fig.add_subplot(1, len(database), idx+1), quest_noquest, database[quest_noquest])


    def on_pick(event):
        print('Info:')
        artist = event.artist
        xmouse, ymouse = event.mouseevent.xdata, event.mouseevent.ydata
        x, y = artist.get_xdata(), artist.get_ydata()
        ind = event.ind
        indx = ind[0]
        print('Info:', 'Clicked: {:.2f}, {:.2f}'.format(xmouse, ymouse))
        print('Info:', 'Picked {} vertices: '.format(len(ind)), end='')
        if len(ind) != 1:
            print('Pick between vertices {} and {}'.format(min(ind), max(ind)+1))
        else:
            print('Picked vertice index:', indx)

        spread_static = artist.get_label()
        quest_noquest = artist.axes.get_title()
        nclient = x[indx]
        update_interval = y[indx]
        run_data = database[quest_noquest][spread_static][indx]
        assert (nclient, update_interval) == (*run_data[0:2],)
        run_name = run_data[2]
        avgs5db = run_data[3]
        print('Info:    ', quest_noquest, spread_static, 'nclient=' + str(nclient), 'update_interval=' + str(update_interval), run_name)
        titlename = utility.genereate_run_name(spread_static, quest_noquest, nclient)
        trajectory.show_fig(True, None, titlename, avgs5db, run_name, figsize)


    fig.canvas.callbacks.connect('pick_event', on_pick)

    if args.output:
        filename = os.path.join(args.output, figname)
        plt.savefig(filename)
        print('Info:', 'Chart is dumped to', filename)

    if args.gui:
        plt.show()


def plot_chart(ax, quest_noquest, single_chart_database):
    '''
    single_chart_database = {static_spread: sorted [(nclient, largest_update_interval, run_name, avgs5db)]}
    '''
    ax.set_title(quest_noquest)
    for static_spread, dataline in single_chart_database.items():
        x, y, run_name = list(zip(*dataline))[0:3]
        
        conflicts = sorted([(x[idx], run_name[idx], y[idx]) for idx in range(len(x)) if x.count(x[idx]) > 1])
        for xx, rr, yy in conflicts:
            print('Warning:', rr, '(' + quest_noquest + ', ' + static_spread + ')','has conflicting data (' + str(xx) + ', ' + float_fmt(yy) + ')')
        if len(conflicts) > 0:
            print('Info:')
                
        ax.plot(x, y, label=static_spread, marker='o', picker=True, pickradius=2)
    ax.legend()
    ax.set(xlabel='Number of Clients', ylabel='Update Interval Time (ms)')
    ax.set_ylim(bottom=0.)
    ax.grid(axis='x', linestyle='--')
    ax.grid(axis='y', linestyle='-')


def parse_run_metric_wrapper(single_arg):
    return parse_run_metric(*single_arg)


def parse_run_metric(run_name, args):
    '''
    (largest_update_interval, static/spread, quest/noquest, nclient, run_name, avgs5db) if data available
    None if data is not available
    '''
    if args.debug:
        print('Debug:', 'Parsing', run_name)

    run_metric_dir = os.path.join(args.path, run_name)

    # Check for whitelist and blacklist
    def check_white(group_strs, whitelist):
        '''
        True if should include
        '''
        for white_group in whitelist:
            if white_group in group_strs:
                return True
        return False

    def check_black(group_strs, blacklist):
        '''
        True if should include
        '''
        for black_group in blacklist:
            if black_group in group_strs:
                return False
        return True

    if args.whitelist or args.blacklist:
        group_strs = utility.parse_group_file(run_metric_dir)
        if args.whitelist:
            if group_strs:
                if not check_white(group_strs, args.whitelist):
                    print('Info:', run_name, 'is ignored due to --whitelist')
                    return None
            else:
                print('Info:', run_name, 'is ignored due to --whitelist')
                return None
        if args.blacklist:
            if group_strs:
                if not check_black(group_strs, args.blacklist):
                    print('Info:', run_name, 'is ignored due to --blacklist')
                    return None

    # Label file
    label_data = utility.parse_label_file(run_metric_dir)
    if label_data is None:
        print('Error:', 'Parsing', run_name + '.', run_metric_dir, 'does not have a valid label file. Data dropped')
        return None

    # CSV files
    csv_filenames = [o for o in os.listdir(run_metric_dir) if os.path.isfile(os.path.join(run_metric_dir, o)) and o[-4:] == '.csv']
    csv_filenames.sort()

    largest_update_intervals = list()
    avgs5db = list()
    for csv_filename in csv_filenames:
        avg = utility.calculate_avg(filename=os.path.join(run_metric_dir, csv_filename), iter_num=args.iter_num, debug=args.debug, max_row=args.max_row)
        avgs5db.append(avg)
        update_interval = avg[4]
        large_ui = max(update_interval)
        if args.debug:
            print('Debug:', '    Largest Interval for', csv_filename, 'is', large_ui)
        largest_update_intervals.append(large_ui)
    
    if len(largest_update_intervals) == 0:
        print('Error:', 'Parsing', run_name + '.', run_metric_dir, 'does not have any valid csv files. Data dropped')
        return None
    largest_update_interval = max(largest_update_intervals)

    info_str = 'Info: Parsing ' + run_name + ' (' + '{0}'.format(', '.join([float_fmt(largest_update_interval)] + label_data)) + ')'
    print(info_str)
    return (largest_update_interval, *label_data, run_name, avgs5db)


def check_data_validity(dataset):
    '''
    (largest_update_interval, static/spread, quest/noquest, nclient, run_name, avgs5db)
    '''
    # Main data structure to work with
    datasize_list = [(run_name, (static_spread, quest_noquest, nclient), max(map(lambda perthread: len(perthread[4]), avgs5db))) for _, static_spread, quest_noquest, nclient, run_name, avgs5db in dataset]
    datasize_list.sort(key=lambda p: p[2])

    if len(datasize_list) == 0:
        return

    # Compute the high cutoff
    _, _, size_list = zip(*datasize_list)
    high_cutoff = np.percentile(np.unique(size_list), 85)
    original_size = len(size_list)

    # Calculate the mean and std on data < high_cutoff
    cutoffed_size_list = list(filter(lambda t: t[2] < high_cutoff, datasize_list))
    if len(cutoffed_size_list) == 0:
        return
    _, _, size_list = zip(*cutoffed_size_list)
    post_high_cutoff_size = len(size_list)
    size_mean = np.mean(size_list)
    size_std = np.std(size_list)
    print('Info:', 'Data sizes have', 'mean=' + float_fmt(size_mean), 'std=' + float_fmt(size_std), 'high_cutoff=' + float_fmt(high_cutoff), '(' + str(original_size) + '->' + str(post_high_cutoff_size) + ')')

    size_warning_threshold = size_mean - 1.5 * size_std
    warning_list = [(run_name, label, size) for run_name, label, size in datasize_list if size < size_warning_threshold]
    
    for run_name, label, size in warning_list:
        print('Warning:', run_name, label, 'only contains', size, 'data')
    print('Warning:', len(warning_list), 'data sets don\'t have enough data')
