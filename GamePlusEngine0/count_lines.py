import os

def count_lines(filename):
    with open(filename, 'r') as file:
        return sum(1 for line in file)

def count_lines_in_directory(directory):
    cpp_files = [f for f in os.listdir(directory) if f.endswith('.cpp')]
    h_files = [f for f in os.listdir(directory) if f.endswith('.h')]

    total_lines = 0
    file_lines = []

    for cpp_file in cpp_files:
        file_path = os.path.join(directory, cpp_file)
        lines = count_lines(file_path)
        total_lines += lines
        file_lines.append((cpp_file, lines))

    for h_file in h_files:
        file_path = os.path.join(directory, h_file)
        lines = count_lines(file_path)
        total_lines += lines
        file_lines.append((h_file, lines))

    total_files = len(os.listdir(directory))
    total_cpp_files = len(cpp_files)
    total_h_files = len(h_files)

    return total_lines, file_lines, total_files, total_cpp_files, total_h_files

def display_table(total_lines, file_lines, total_files, total_cpp_files, total_h_files):
    print("{:<30} {:<15}".format("File", "Lines"))
    print("="*45)
    for file, lines in file_lines:
        print("{:<30} {:<15}".format(file, lines))
    print("="*45)
    print("{:<30} {:<15}".format("Total", total_lines))
    print("{:<30} {:<15}".format("Total Files", total_files))
    print("{:<30} {:<15}".format("Total .cpp Files", total_cpp_files))
    print("{:<30} {:<15}".format("Total .h Files", total_h_files))

if __name__ == "__main__":
    directory_path = input("Enter the directory path: ")
    total_lines, file_lines, total_files, total_cpp_files, total_h_files = count_lines_in_directory(directory_path)
    display_table(total_lines, file_lines, total_files, total_cpp_files, total_h_files)
