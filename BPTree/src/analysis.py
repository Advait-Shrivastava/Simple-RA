import random
import subprocess
import os
import matplotlib.pyplot as plt
from collections import Counter

def generate_insert_file():
    numbers = list(range(1, 101))
    random.shuffle(numbers)
    with open("../data/analysis_insert", "w") as file:
        for number in numbers:
            file.write(f"INSERT {number}\n")


def generate_range_execution_file():
    with open("../data/analysis_range_execution", "w") as file:
        for i in range(1, 100):
            file.write(f"RANGE {i} {i+1}\n")


def run_BPTree_program():
    BPTree_executable = os.path.join(os.pardir, "build", "BPTree")
    output_file = os.path.join(os.pardir, "data", "analysis_output")

    try:
        process = subprocess.Popen([BPTree_executable], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)

        process.stdin.write("SOURCE analysis_insert\n")
        process.stdin.flush()

        process.stdin.write("SOURCE analysis_range_execution\n")
        process.stdin.flush()

        process.stdin.close()
        process.wait()

        with open(output_file, 'w') as outfile:
            output_content = process.stdout.read()[4:-2]
            outfile.write(output_content)

    except subprocess.CalledProcessError as e:
        print(f"Error executing BPTree: {e}")


def analysis_plot(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    a_values = [int(line.split()[0]) for line in lines]
    b_values = [int(line.split()[1]) for line in lines]

    a_frequencies = Counter(a_values)
    b_frequencies = Counter(b_values)

    unique_a_numbers, a_counts = zip(*sorted(a_frequencies.items()))
    unique_b_numbers, b_counts = zip(*sorted(b_frequencies.items()))

    total_entries = len(lines)
    a_percentages = [(count / total_entries) * 100 for count in a_counts]
    b_percentages = [(count / total_entries) * 100 for count in b_counts]

    plt.bar(unique_a_numbers, a_percentages, label='B+ Tree', color='blue')
    plt.xlabel('B+ Tree Block Access')
    plt.ylabel('Percentage')
    plt.title('B+ Tree Block Access Frequency')
    plt.legend()
    plt.xticks(unique_a_numbers)
    plt.show()

    plt.bar(unique_b_numbers, b_percentages, label='Heap', color='orange')
    plt.xlabel('Heap Block Access')
    plt.ylabel('Percentage')
    plt.title('Heap Block Access Frequency')
    plt.legend()
    plt.xticks(unique_b_numbers)
    plt.show()


generate_insert_file()
generate_range_execution_file()
run_BPTree_program()
analysis_plot("../data/analysis_output")

