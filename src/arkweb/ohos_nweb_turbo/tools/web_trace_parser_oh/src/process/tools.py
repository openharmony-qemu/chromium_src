# 获取进程信息相关工具方法
import re

# 运行在render进程关键方法名称
render_key_function = [
    "ResourceFetcher::requestResource",
    "EvaluateScript"
]


# 获取当前应用render进程名称与进程id
def get_main_process_name_and_id(lines):
    # print(f"get_main_process_name_and_id: {lines}")
    process_name: str = ''
    process_id: str = ''
    for line in lines:
        if any(key_function in line for key_function in render_key_function):
            print(f"key line: {line}")
            process_name_id: str = line.split('(')[0]
            print(f'process_package_name_id: {process_name_id}')
            process_name_id_list: list[str] = process_name_id.strip().split("-")

            length: int = len(process_name_id_list)
            # 如果只有两项 则说明包名比较规范，数组里只有进程名和进程id
            if length == 2:
                process_name = process_name_id_list[0]
                process_id = process_name_id_list[1]
            # 如果大于两项，则说明包名不规范，使用特殊手段获取进程id与进程名称
            else:
                process_id = process_name_id_list[length - 1]
                process_name = process_name_id.replace('-' + process_id, '')
            break
    return process_name, process_id
