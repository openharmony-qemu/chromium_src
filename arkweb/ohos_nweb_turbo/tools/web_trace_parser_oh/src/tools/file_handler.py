import os
import datetime
from src.tools.others import save_log, catch_trace, remove_file_extension


def read_trace(input_path: str, trace_name: str) -> list[str]:
    trace_path = os.path.join(input_path, trace_name)
    if not os.path.exists(trace_path):
        print(f"trace {trace_path} 不存在， 请确认trace文件名称和路径是否正确")
        return []
    file = open(trace_path, 'r', encoding='UTF-8', errors='ignore')
    return file.readlines()


def create_archive_folder(name: str, path: str) -> str:
    oh_trace_name = remove_file_extension(name)
    timestamp = datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S')

    # 如果不存在指定目录，则创建
    if not os.path.exists(path):
        os.makedirs(path)

    # 创建文件夹
    folder_path = os.path.join(path, oh_trace_name + '-' + timestamp)
    os.makedirs(folder_path)
    print(f"已创建文件夹：{folder_path}")

    return os.path.join(folder_path, oh_trace_name + '.xlsx')
