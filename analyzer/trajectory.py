import argparse
import os

try:
    import matplotlib.pyplot as plt
except:
    print('Please pip install matplotlib')

import arguments
import utility


def init(parser):
    parser.description='draw graph based on .csv data'
    parser.add_argument('--path', type=str, required=True, help='Path to the directory of .csv files')
    parser.add_argument('--raw', action='store_true', help='Raw data')
    parser.add_argument('--title', type=str, help='Graph title: Type - N clients, e.g. Static - 100 clients')
    arguments.load_argument(parser)


def main(args):
    server_threads = []
    for file in os.listdir(args.path):
        if file.endswith('.csv') and 'avg' not in file:
            server_threads.append(file)
    server_threads.sort()
    
    # read one .csv, and add its data to all subplots using the same style
    # avgs5db[thread_id][dataline][avged_point]
    avgs5db = [utility.calculate_avg(filename=os.path.join(args.path, thread_file), iter_num=args.iter_num, debug=args.debug, max_row=args.max_row, raw=args.raw) for thread_file in server_threads]

    if args.title is None:
        args.title = utility.genereate_run_name(*utility.parse_label_file(args.path))

    show_fig(args.gui, args.output, args.title, avgs5db)


def show_fig(gui, output, figtitle, avgs5db, figname=None, figsize=(24,12)):
    fig = plt.figure(figname, figsize=figsize)
    suptitle = figtitle.split('_')
    suptitle = ' '.join(suptitle)
    fig.suptitle(suptitle, fontsize=16)

    # plot the same column across all server threads on one subplot
    # subfig[i] holds column i
    style = ['r', 'b', 'g', 'k']
    title = ['Number of client requests', 'Time spent processing client requests', 'Number of updates sent to clients', 'Time spent sending client updates', 'Update interval']
    ylabel = ['Number', 'Time (ms)', 'Number', 'Time (ms)', 'Time (ms)']
    subfig = []
    pos = [0, 1, 3, 4, 2]

    for i in range(5): # 4 columns in total
        subfig.append(fig.add_subplot(2, 3, i+1))

    for i in range(5):
        subfig[pos[i]].title.set_text(title[i])
        subfig[pos[i]].set(xlabel='Iteration',ylabel=ylabel[i])
    
    # read one .csv, and add its data to all subplots using the same style
    for num, avg in enumerate(avgs5db):
        # avg (2D) - [col] [avg index]
        for i in range(len(avg)):
            subfig[pos[i]].plot(avg[i], style[num])

    if output:
        filename = os.path.join(output, figtitle)
        plt.savefig(filename)
        print('Info:', 'Chart is dumped to', filename)
    if gui:
        plt.show()
