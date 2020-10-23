import argparse
import csv
import os
import matplotlib.pyplot as plt

def calculate_avg(filename, iter_num, debug, raw):

    # compute average for each server thread
    avg = [] # (2D) [col][index]
    col_num = 0
    line_num = 0

    with open(filename, mode='r') as f:
        csv_reader = csv.reader(f, delimiter=',')
        first_line = True
        iter_sum = []
        data = [] # 2D, [col][iter_num]
        data_i = 0 # index of where in data to write - 0 <= data_i < iter_num
        max_row = 8000

        for row in csv_reader:
            if debug: print("row", line_num, row)

            if first_line:
                first_line = False
                continue

            if line_num == 0:
                col_num = len(row)+1
                for j in range(col_num):
                    iter_sum.append(0)
                    data.append([])
                    avg.append([])

            if line_num == max_row:
                break

            if len(row)+1 < col_num:
                break

            row.append(float(row[1]) + float(row[3]))

            if raw:
                for j in range(col_num):
                    avg[j].append(float(row[j]))
                    continue

            # calculate average as we read in new data
            # accumulate till we have a number of data points > iter_num
            if line_num < iter_num - 1:
                for j in range(col_num):
                    iter_sum[j] = iter_sum[j] + float(row[j])
                    data[j].append(float(row[j]))
            # have enough data
            else:
                for j in range(col_num):
                    if line_num == iter_num - 1: # can calculate the first average
                        iter_sum[j] = iter_sum[j] + float(row[j])
                        data[j].append(float(row[j]))
                    else: # need to replace the oldest data with the newest
                        iter_sum[j] = iter_sum[j] + float(row[j]) - data[j][data_i]
                        data[j][data_i] = float(row[j])
                    avg[j].append(iter_sum[j] / iter_num)

            line_num = line_num + 1
            if data_i == iter_num - 1:
                data_i = 0
            else:
                data_i = data_i + 1
        
        if debug:
            print("-----------")
            print("avg:", avg)
            print("-----------")

    '''
    # write result to a .csv file
    if not raw:
        outfile = os.path.splitext(filename)[0] + "_avg.csv"
        if debug: print(outfile)
        dump = [] # (2D) [index][col_num]
        count = len(avg[0])
        for i in range(count):
            dump.append([])
            for j in range(col_num):
                dump[i].append(avg[j][i])
        if debug: print(dump)

        with open(outfile, mode='w') as res_file:
            csv_writer = csv.writer(res_file, delimiter=',')
            for row in dump:
                csv_writer.writerow(row)
    '''
    conversion = [1, 3, 4]
    for i in conversion:
        for j in range(len(avg[0])):
            avg[i][j] = avg[i][j]/float(1000)
    return avg

def main():
    parser = argparse.ArgumentParser(description='draw graph based on .csv data')
    parser.add_argument('--path', type=str, required=True, help='Path to the directory of .csv files')
    parser.add_argument('--iter_num', type=int, required=True, help='Number of iterations to compute the average')
    parser.add_argument('--debug', type=bool, default=False, help='Print debug messages when on')
    parser.add_argument('--raw', type=bool, default=False, help='Raw data')
    parser.add_argument('--title', type=str, required=True, help="Graph title: Type - N clients, e.g. Static - 100 clients")
    args = parser.parse_args()
    
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
        avg = calculate_avg(os.path.join(args.path, filename), args.iter_num, args.debug, args.raw)
        for i in range(len(avg)):
            subfig[pos[i]].plot(avg[i], style[num])

    #plt.show()
    plt.savefig(args.title)

if __name__ == '__main__':
    main()