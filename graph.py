import argparse
import csv
import os

def read_csv_files(path, outfile, iter_num):
    clients = []
    for file in os.listdir(path):
        if file.endswith(".csv"):
            clients.append(file)
    num_clients = len(clients)
    print("num_clients:", num_clients)
    max_iter = 0

    # compute average for each client
    avg = [] # 2D
    first_csv = True
    for i in range(num_clients):
        with open(os.path.join(path, clients[i]), mode='r') as client:
            csv_reader = csv.reader(client, delimiter=',')
            first_line = True
            line_num = 0
            col_num = 0
            iter_sum = []
            data = [] # 2D
            data_i = 0
            for row in csv_reader:
                print(row)

                if max_iter != 0 and line_num > max_iter:
                    break

                if first_line:
                    first_line = False
                    continue

                if line_num == 0:
                    col_num = len(row)
                    for j in range(col_num):
                        iter_sum.append(0)
                        data.append([])
                        if first_csv:
                            avg.append([])

                # calculate average as we read in new data
                # accumulate till we have a number of data points > iter_num
                if line_num < iter_num - 1:
                    for j in range(col_num):
                        iter_sum[j] = iter_sum[j] + float(row[j])
                        data[j].append(float(row[j]))
                else:
                    if line_num == iter_num - 1: # can calculate the first average
                        for j in range(col_num):
                            iter_sum[j] = iter_sum[j] + float(row[j])
                            data[j].append(float(row[j]))
                    else:
                        for j in range(col_num): # need to replace the oldest data with the newest
                            iter_sum[j] = iter_sum[j] + float(row[j]) - data[j][data_i]
                            data[j][data_i] = float(row[j])

                    if first_csv:
                        avg[j].append(iter_sum[j] / iter_num)
                    else:
                        avg[j][line_num - iter_num + 1] = avg[j][line_num - iter_num + 1] + iter_sum[j] / iter_num

                line_num = line_num + 1
                if data_i == iter_num - 1:
                    data_i = 0
                else:
                    data_i = data_i + 1

            first_csv = False
            print(data)
            print("-----")
            print(avg)
            
                        
    # write result to a .csv file
    # with open(os.path.join(path, outfile), mode='w') as res_file:

            

    #csv_files = []
    #return data

def compute_average(data, iter):
    return avg

def main():
    parser = argparse.ArgumentParser(description='draw graph based on .csv data')
    parser.add_argument('--path', type=str, required=True, help='Path to the directory of .csv files')
    parser.add_argument('--iter_num', type=int, required=True, help='Number of iterations to compute the average')
    parser.add_argument('--out', type=str, required=True, help='Name of the output file containing average')
    args = parser.parse_args()
    
    read_csv_files(args.path, args.out, args.iter_num)
    #data = read_csv_files(args.path)
    #avg = compute_average(data, args.iter_num)


if __name__ == '__main__':
    main()