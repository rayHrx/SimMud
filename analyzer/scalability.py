import argparse
import collections
import csv
import datetime
import os

import matplotlib.pyplot as plt

import trajectory
import utility


def init(parser):
    parser.description='Plot # client vs update interval with and without quest for spread and static'
    parser.add_argument('--path', type=str, default='./metrics', help='Path to the metrics directory')
    parser.add_argument('--iter_num', type=int, default=100, help='Moving average window size')
    parser.add_argument('--debug', action='store_true', help='Print debug messages when on')
    parser.add_argument('--gui', action='store_true', help='Open charts on GUI')
    parser.add_argument('--output', type=str, help='Location to dump chart')


def main(args):
    run_names = [o for o in os.listdir(args.path) if os.path.isdir(os.path.join(args.path, o))]
    print('Info:', 'Found metric data of ', len(run_names), 'runs')

    # [(largest_update_interval, static/spread, quest/noquest, nclient, run_name, avgs5db)]
    dataset = [parse_run_metric(run_name, args) for run_name in run_names]
    dataset = [data for data in dataset if data]
    
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
        print('Info:    ', spread_static, quest_noquest, 'nclient=' + str(nclient), 'update_interval=' + str(update_interval), run_name)
        titlename = quest_noquest + '_' + spread_static + '_' + str(nclient) + '_clients'
        trajectory.show_fig(True, None, titlename, avgs5db, run_name, figsize)


    fig.canvas.callbacks.connect('pick_event', on_pick)

    if args.gui:
        plt.show()

    if args.output:
        filename = os.path.join(args.output, figname)
        plt.savefig(filename)
        print('Info:', 'Chart is dumped to', filename)


def plot_chart(ax, quest_noquest, single_chart_database):
    '''
    single_chart_database = {static_spread: sorted [(nclient, largest_update_interval, run_name, avgs5db)]}
    '''
    ax.set_title(quest_noquest)
    for static_spread, dataline in single_chart_database.items():
        ax.plot(*(list(zip(*dataline))[0:2]), label=static_spread, marker='o', picker=True, pickradius=4)
    ax.legend()
    ax.set(xlabel='Number of Clients', ylabel='Update Interval Time (ms)')
    ax.set_ylim(bottom=0.)
    ax.grid(axis='x', linestyle='--')
    ax.grid(axis='y', linestyle='-')


def parse_run_metric(run_name, args):
    '''
    (largest_update_interval, static/spread, quest/noquest, nclient, run_name, avgs5db) if data available
    None if data is not available
    '''
    print('Info:', 'Parsing', run_name)

    run_metric_dir = os.path.join(args.path, run_name)

    # Label file
    label_data = utility.parse_label_file(run_metric_dir)
    if label_data is None:
        print('Error:', run_metric_dir, 'does not have a valid label file. Data dropped')
        return None

    # CSV files
    csv_filenames = [o for o in os.listdir(run_metric_dir) if os.path.isfile(os.path.join(run_metric_dir, o)) and o[-4:] == '.csv']
    csv_filenames.sort()

    largest_update_intervals = list()
    avgs5db = list()
    for csv_filename in csv_filenames:
        avg = utility.calculate_avg(os.path.join(run_metric_dir, csv_filename), args.iter_num, args.debug)
        avgs5db.append(avg)
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
    return (largest_update_interval, *label_data, run_name, avgs5db)
