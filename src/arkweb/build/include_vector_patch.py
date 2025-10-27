import os
import sys

def process(path):
    use_string = 'std::vector'
    include_string = '#include <vector>'
    try:
        file = open(path, 'r', encoding="utf-8")
    except:
        print('file %s read error' % path)
        return

    lines = file.read()
    if include_string in lines:
        return
    if use_string in lines and include_string in lines:
        return
    if not use_string in lines:
        return

    file.close()

    line_list = lines.split('\n')
    # find first #include
    i = 0
    is_find = False
    for line in line_list:
        if '#include ' in line:
            is_find = True
            break
        i += 1

    try:
        file = open(path, 'w', encoding="utf-8")
    except:
        print('file %s write error' % path)
        return

    if not is_find:
        i = -1
        file.write(include_string)
        file.write('\n')


    for j in range(len(line_list)):
        file.write(line_list[j])
        file.write('\n')

        if j == i:
            file.write(include_string)
            file.write('\n')

    file.close()


def recursive_list_directory(directory):
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.h'):
                file_path = os.path.join(root, file)
                print('processing ' + file_path)
                process(file_path)

# 检查是否提供了足够的参数
if len(sys.argv) < 1:
    print("argument size should be 1")
    sys.exit(1)
first_argument = sys.argv[1]
recursive_list_directory(first_argument)

