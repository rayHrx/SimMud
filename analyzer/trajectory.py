import argparse
import os

import matplotlib.pyplot as plt

import stats


def init(parser):
    parser.description='draw graph based on .csv data'
    parser.add_argument('--path', type=str, required=True, help='Path to the directory of .csv files')
    parser.add_argument('--iter_num', type=int, required=True, help='Number of iterations to compute the average')
    parser.add_argument('--debug', type=bool, default=False, help='Print debug messages when on')
    parser.add_argument('--raw', type=bool, default=False, help='Raw data')
    parser.add_argument('--title', type=str, required=True, help="Graph title: Type - N clients, e.g. Static - 100 clients")


def main(args):
    fig = plt.figure(figsize=(24,12))
    suptitle = args.title.split('_')
    suptitle = ' '.join(suptitle)
    fig.suptitle(suptitle, fontsize=16)

    server_threads = []
    for file in os.listdir(args.path):
        if file.endswith(".csv") and "avg" not in file:
            server_threads.append(file)
    num_threads = len(server_threads)

    # plot the same column across all server threads on one subplot
    # subfig[i] holds column i
    style = ["r", "b", "g", "k"]
    title = ["Number of client requests", "Time spent processing client requests", "Number of updates sent to clients", "Time spent sending client updates", "Update interval"]
    ylabel = ["Number", "Time (ms)", "Number", "Time (ms)", "Time (ms)"]
    subfig = []
    pos = [0, 1, 3, 4, 2]

    for i in range(5): # 4 columns in total
        subfig.append(fig.add_subplot(2, 3, i+1))

    for i in range(5):
        subfig[pos[i]].title.set_text(title[i])
        subfig[pos[i]].set(xlabel="Iteration",ylabel=ylabel[i])
    
    # read one .csv, and add its data to all subplots using the same style
    for num in range(num_threads):
        filename = server_threads[num]
        # avg (2D) - [col] [avg index]
        avg = stats.calculate_avg(os.path.join(args.path, filename), args.iter_num, args.debug, args.raw)
        for i in range(len(avg)):
            subfig[pos[i]].plot(avg[i], style[num])

    #plt.show()
    plt.savefig(args.title)
