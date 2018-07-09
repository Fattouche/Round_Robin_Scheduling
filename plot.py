import re
import os
import matplotlib.pyplot as plt


def plot_round_robin(title, ylabel, data):
    dispatch = [0, 5, 10, 15, 20, 25]
    plt.plot(dispatch, data["quantum_50"],
             color='purple', marker='|', label="quantum_50")
    plt.plot(dispatch, data["quantum_100"],
             color='green', marker='x', label="quantum_100")
    plt.plot(dispatch, data["quantum_250"],
             color='blue', marker='o', label="quantum_250")
    plt.plot(dispatch, data["quantum_500"],
             color='orange', marker='*', label="quantum_500")

    plt.xlabel('Dispatch overhead')
    plt.ylabel(ylabel)
    plt.title(title+' -- # tasks: 1000; seed value 8296')
    plt.legend()
    plt.show()


def plot_first_come(title, ylabel, data):
    dispatch = [0, 5, 10, 15, 20, 25]
    plt.plot(dispatch, data, color='purple', marker='*')

    plt.xlabel('Dispatch overhead')
    plt.ylabel(ylabel)
    plt.title(title+' -- # tasks: 1000; seed value 8296')
    plt.legend()
    plt.show()


def get_averages(path, filename):
    file = open(path+filename, 'r')
    wait = re.compile('w=(.*) ')
    turn_around = re.compile('ta=(.*)')
    wait_average = 0
    turn_around_average = 0
    counter = 0
    for line in file:
        if "EXIT" in line:
            wait_average += float(wait.search(line).group(1))
            turn_around_average += float(turn_around.search(line).group(1))
            counter += 1
    wait_average = wait_average/counter
    turn_around_average = turn_around_average/counter
    return wait_average, turn_around_average


def display_round_robin_graphs():
    dispatch_regex = re.compile('dispatch_(.*).txt')
    quantum_regex = re.compile("round_robin_(.*)_dispatch")
    wait_average_map = {}
    turn_around_average_map = {}
    for filename in os.listdir("./data/round_robin"):
        key = quantum_regex.search(filename).group(1)
        index = int(int(dispatch_regex.search(filename).group(1))/5)
        if key not in wait_average_map:
            wait_average_map[key] = [0, 0, 0, 0, 0, 0]
        if key not in turn_around_average_map:
            turn_around_average_map[key] = [0, 0, 0, 0, 0, 0]
        temp = get_averages("./data/round_robin/", filename)
        wait_average_map[key][index] = temp[0]
        turn_around_average_map[key][index] = temp[1]
    plot_round_robin("Round Robin Scheduler",
                     "Average Waiting time", wait_average_map)
    plot_round_robin("Round Robin Scheduler",
                     "Average Turn around time", turn_around_average_map)


def display_first_come_graphs():
    dispatch_regex = re.compile('serve_(.*).txt')
    wait_average = [0, 0, 0, 0, 0, 0]
    turn_around_average = [0, 0, 0, 0, 0, 0]
    for filename in os.listdir("./data/first_come"):
        index = int(int(dispatch_regex.search(filename).group(1))/5)
        temp = get_averages("./data/first_come/", filename)
        wait_average[index] = temp[0]
        turn_around_average[index] = temp[1]

    plot_first_come("First Come First Serve Scheduler",
                    "Average Waiting time", wait_average)
    plot_first_come("First Come First Serve Scheduler",
                    "Average Turn around time", turn_around_average)


# display_round_robin_graphs()
display_first_come_graphs()
