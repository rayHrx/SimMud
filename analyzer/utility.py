import csv
import os


def genereate_run_name(spread_static, quest_noquest, nclient):
    return quest_noquest + '_' + spread_static + '_' + str(nclient) + '_clients'


def parse_label_file(run_metric_dir):
    '''
    (static/spread, quest/noquest, nclient) if data available
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


def calculate_avg(filename, iter_num, debug, raw=False):
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
