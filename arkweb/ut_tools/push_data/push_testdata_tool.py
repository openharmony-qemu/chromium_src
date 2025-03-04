#Python
# coding=utf8
import argparse
import subprocess
import json
import os
import platform
import re
from logger import logger, logger_init
from datetime import datetime
def GetOS():
    os_name = platform.system()
    return os_name.lower()

def GetNowTime():
    # 获取并格式化当前的日期和时间
    now = datetime.now()
    formatted_now = now.strftime("%Y-%m-%d-%H-%M-%S")
    return formatted_now

# 正则表达式去除C或C++风格的注释
def RemoveJsonComments(json_data):
    # 支持单行注释（//）和多行注释（/**/）
    json_data = re.sub(r'[^"//.*"]//.*', '', json_data, flags=re.MULTILINE)  # 单行注释
    json_data = re.sub(r'/\*.*?\*/', '', json_data, flags=re.DOTALL)  # 多行注释
    return json_data   

def LoadJsonWithoutComment(jsonfile_name):
    with open(jsonfile_name, 'r', encoding='utf-8') as file:
        json_data = file.read()
        json_dict = json.loads(RemoveJsonComments(json_data))
        return json_dict

# 从push_testdata_config.json加载配置
g_myscript_path = os.path.dirname(os.path.abspath(__file__))
logdir = os.path.join(g_myscript_path, 'logs')
if not os.path.isdir(logdir):
    os.makedirs(logdir)
gPushTestDataLog = os.path.join(g_myscript_path, 'logs', 'push_testdata_' + GetNowTime() + '.log')


logger, gLogFileHandler = logger_init(gPushTestDataLog)
gPushTestDataConfig = LoadJsonWithoutComment(os.path.join(g_myscript_path, 'push_testdata_config.json'))
g_push_testdata_filter = LoadJsonWithoutComment(os.path.join(g_myscript_path, 'push_testdata_filter.json'))
g_push_list = g_push_testdata_filter["push"]
g_notpush_list = g_push_testdata_filter["notpush"]


# 提取配置值
g_tmp_dir=os.path.join(g_myscript_path, 'local_tmp')
g_ssh_username = gPushTestDataConfig['ssh']['ssh_username']
g_ssh_server_ip = gPushTestDataConfig['ssh']['ssh_server_ip']
g_chromium_src_path = gPushTestDataConfig['code']['chromium_src_path']
if len(g_chromium_src_path)==0:
    g_chromium_src_path = os.path.join(g_myscript_path, '..', '..')
g_exe_path = gPushTestDataConfig['device']['exe_path']
g_project_name = gPushTestDataConfig['device']['project_name']
g_code_location = gPushTestDataConfig['code']['code_location']  # 添加获取操作系统名称
g_run_in_os = GetOS()
g_run_proxy_ip = gPushTestDataConfig['python']['proxy_ip']
g_run_proxy_port = gPushTestDataConfig['python']['proxy_port']

def IsNeedPushJson(filename):
    for pattern in g_notpush_list:
        match = re.search(pattern, filename)
        if match:
            return False
    for pattern in g_push_list:
        match = re.search(pattern, filename)
        if match:
            return True
    return False

def IsHasErrorInLog(line):
    pattern = r"\[(ERROR)|(Fail)\]"
    match = re.search(pattern, line, re.I)
    if match:
        return True
    return False

def RunCommand(command, timeout=None):
    logger.debug(f"正在执行->{command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, text=True, shell=True)
    for line in process.stdout:
        line = line.rstrip()
        if len(line) > 0:
            if IsHasErrorInLog(line):
                logger.error(f"{line} ")
            else:
                logger.warning(f"{line} ")
    return_code = process.wait()
    # 检查命令是否执行成功
    if return_code == 0:
        logger.debug(f"{command}->命令执行成功。")
    else:
        logger.error(f"{command}->命令执行失败，返回代码:{return_code}")    

# 定义将文件复制到设备的函数
def CopyAndPushToDevice(isFile, scpFullPath, dstRelativePath, dataName, is_need_tar, need_chmod, json_filename):
    proxyConfig = ''
    if g_run_in_os == "linux": #本脚本在linux下运行
        proxyConfig = f' -s {g_run_proxy_ip}:{g_run_proxy_port}'
    RunCommand(f'hdc {proxyConfig} shell mount -o remount,rw /', timeout=25)
    if not os.path.exists(g_tmp_dir):
        RunCommand(f'mkdir {g_tmp_dir}', timeout=25)
    RunCommand(f'hdc {proxyConfig} shell mkdir -p {g_exe_path}/{dstRelativePath}/', timeout=25)
    # if ("libnweb_render_webengine_so.json"==json_filename):
    #     RunCommand(f'hdc {proxyConfig} shell rm -f {g_exe_path}/{dstRelativePath}/{dataName}', timeout=50)
        
    if g_code_location == "samba": #以samba方式取得推送数据, samba时，本脚本只支持在windows下运行
        try:
            if is_need_tar:
                dataNameFolder = dataName.rstrip('.tar')
                if g_run_in_os == "windows":
                    RunCommand(f'pushd {scpFullPath}  && del /f /q {dataName}', timeout=25)
                    RunCommand(f'pushd {scpFullPath}  && tar cvf {dataName} {dataNameFolder}', timeout=25)
            RunCommand(f'hdc file send {scpFullPath}\\{dataName} {g_exe_path}/{dstRelativePath}/')
        except RuntimeError as e:
            logger.error(f"Push testdata: {json_filename}")
            logger.debug(f"RuntimeError: {e}")
    elif g_code_location == "linux_local":
        try:
            if isFile:
                if is_need_tar:
                    dataNameFolder = dataName.rstrip('.tar')
                    RunCommand(f'cd {scpFullPath} && rm -f {dataName}', timeout=25)
                    RunCommand(f'cd {scpFullPath} && tar cvf {dataName} {dataNameFolder}', timeout=25)
                if g_run_in_os == "linux":#本脚本在linux下运行
                    RunCommand(f'hdc {proxyConfig} file send {scpFullPath}/{dataName} {g_exe_path}/{dstRelativePath}/', timeout=50)
                elif g_run_in_os == "windows":
                    RunCommand(f'scp {g_ssh_username}@{g_ssh_server_ip}:{scpFullPath}/{dataName} {g_tmp_dir}/', timeout=25)
                    RunCommand(f'hdc {proxyConfig} file send {g_tmp_dir}\\{dataName} {g_exe_path}/{dstRelativePath}/', timeout=50)
                    RunCommand(f'del /f /q {g_tmp_dir}\\{dataName}', timeout=25)
            else:# is folder
                    RunCommand(f'hdc {proxyConfig} file send {scpFullPath}/{dataName} {g_exe_path}/{dstRelativePath}/', timeout=50)
        except RuntimeError as e:
            logger.error(f"Push testdata: {json_filename}")
            logger.debug(f"RuntimeError: {e}")
    elif g_code_location == "ssh_server":
        try:
            if isFile:
                if is_need_tar:
                    dataNameFolder = dataName.rstrip('.tar')
                    RunCommand(f'ssh {g_ssh_username}@{g_ssh_server_ip} "cd {scpFullPath} && rm -f {dataName}"', timeout=25)
                    RunCommand(f'ssh {g_ssh_username}@{g_ssh_server_ip} "cd {scpFullPath} && tar cvf {dataName} {dataNameFolder}"', timeout=25)                
                if g_run_in_os == "linux":#本脚本在linux下运行
                    RunCommand(f'scp {g_ssh_username}@{g_ssh_server_ip}:{scpFullPath}/{dataName} {g_tmp_dir}/', timeout=25)
                    RunCommand(f'hdc {proxyConfig} file send {g_tmp_dir}/{dataName} {g_exe_path}/{dstRelativePath}/', timeout=50)
                    RunCommand(f'rm -f {g_tmp_dir}/{dataName}', timeout=25)
                elif g_run_in_os == "windows":
                    RunCommand(f'scp {g_ssh_username}@{g_ssh_server_ip}:{scpFullPath}/{dataName} {g_tmp_dir}/', timeout=25)
                    RunCommand(f'hdc {proxyConfig} file send {g_tmp_dir}\\{dataName} {g_exe_path}/{dstRelativePath}/', timeout=50)
                    RunCommand(f'del /f /q {g_tmp_dir}\\{dataName}', timeout=25)
            else:
                if g_run_in_os == "linux":
                    RunCommand(f'scp {g_ssh_username}@{g_ssh_server_ip}:{scpFullPath}/{dataName} {g_tmp_dir}/', timeout=25)
                    RunCommand(f'hdc {proxyConfig} file send {g_tmp_dir}/{dataName} {g_exe_path}/{dstRelativePath}/', timeout=50)
                    RunCommand(f'rm -rf {g_tmp_dir}/{dataName}', timeout=25)
                elif g_run_in_os == "windows":
                    RunCommand(f'scp -r {g_ssh_username}@{g_ssh_server_ip}:{scpFullPath}/{dataName} {g_tmp_dir}/', timeout=25)
                    RunCommand(f'hdc {proxyConfig} file send {g_tmp_dir}\\{dataName} {g_exe_path}/{dstRelativePath}/', timeout=50)
                    RunCommand(f'rd /s /q {g_tmp_dir}\\{dataName}', timeout=25)
        except RuntimeError as e:
            logger.error(f"Push testdata: {json_filename}")
            logger.debug(f"RuntimeError: {e}")
    if is_need_tar:# 在target_device上解压
        RunCommand(f'hdc {proxyConfig} shell "cd {g_exe_path}/{dstRelativePath} && tar xvf {dataName}"', timeout=25)
    RunCommand(f'hdc {proxyConfig} shell sync', timeout=25)
    if need_chmod:# 在target_device上添加运行权限
        RunCommand(f'hdc {proxyConfig} shell chmod 777 -R {g_exe_path}/{dstRelativePath}/{dataName}', timeout=25)
    RunCommand(f'hdc {proxyConfig} shell ls -l {g_exe_path}/{dstRelativePath}/{dataName}', timeout=25)

def ProcessJsonData(json_data, json_filename):
    for item in json_data:
        # 提取文件名、路径等信息
        is_file = item["is_file"]
        is_need_tar = item["is_need_tar"]
        need_chmod = item["need_chmod"]
        names = item["names"]  
        src_path = os.path.join(*item["src_paths"])
        src_path = src_path.replace("$project_name", g_project_name)
        src_path = os.path.join(g_chromium_src_path, src_path)
        if g_code_location != "samba":
            src_path = src_path.replace("\\", "/")
        dst_path = item["dst_path"]

        for name in names:
            CopyAndPushToDevice(is_file, os.path.join(g_chromium_src_path, src_path), dst_path, name, is_need_tar, need_chmod, json_filename)

def printRed(txt):
    print(f"\033[91m{txt}\033[0m")

def CheckPushResult():
    logger.removeHandler(gLogFileHandler)
    logger.debug(f"=====================push result:   {gPushTestDataLog}   =================================")
    with open(gPushTestDataLog, 'r', encoding='utf-8') as file:
        # 逐行读取
        line = file.readline()
        is_has_error_in_log = False
        while line:
            if IsHasErrorInLog(line):
                is_has_error_in_log = True
                print(line)
            line = file.readline()
    if not is_has_error_in_log:
        logger.info("Push testdata Success!!!")
    pass

def main():
    parser = argparse.ArgumentParser(description=
    'push_testdata_tool:\n\
        是一个推送 testdata 的小工具,\n \
        在使用之前请：\n \
        1.配置 hdc.exe的路径到windows PATH的环境变量中, \n \
        2.配置 Ubuntu server(code和UT和编译结果所在机器)免密登录, 如果是samba方式, 请配置好免密登录\n \
        3.配置 push_testdata_config.json, \n \
        4.如果不想推送那个UT请配置 push_testdata_filter.json\n \
        5.推送的过程和结果见logs下的日志\n \
    usage: \n \
        #推送push_testdata_config.json 配置好的， 经过push_testdata_filter.json过滤的testdata\n \
        python push_testdata_tool.py \n \
    ', formatter_class=argparse.RawTextHelpFormatter)
    args = parser.parse_args()
    # 指定JSON文件所在文件夹路径
    deps_folder = "ut_data_deps"
    deps_folder = os.path.join(g_myscript_path, deps_folder)
    #UT执行需要将本地编译的libffmpeg.so推送到设备/system/lib64/路径下
    libso_name = 'libffmpeg.so'
    device_path = '/system/lib64/'
    device_name = os.path.join("out", g_project_name)
    proxyConfig = ''
    if g_run_in_os == "linux": #本脚本在linux下运行
        proxyConfig = f' -s {g_run_proxy_ip}:{g_run_proxy_port}'
    RunCommand(f'hdc {proxyConfig} shell mount -o remount,rw /', timeout=25)
    RunCommand(f'hdc {proxyConfig} file send {g_chromium_src_path}/{device_name}/{libso_name} {device_path}', timeout=50)
    #遍历文件夹中的所有JSON文件
    for filename in os.listdir(deps_folder):
        if filename.endswith(".json"):
        # 检查文件是否为JSON文件
            if IsNeedPushJson(filename):
                # 拼接文件路径
                file_path = os.path.join(deps_folder, filename)
                # 读取JSON文件内容
                with open(file_path, "r", encoding='utf-8') as json_file:
                    logger.info("正在推送："+ file_path)
                    json_data = json_file.read()
                    # 处理JSON数据
                    ProcessJsonData(json.loads(RemoveJsonComments(json_data)), filename)
    CheckPushResult()

if __name__ == "__main__":
    main()
#example:
#python3 push_testdata_tool.py    推送push_testdata_config.json 配置好的， 经过push_testdata_filter.json过滤的testdata


#                   | oh code in samba | oh code in ssh server | oh code in local linuxfilesystem
# py run in Windows |
# py run in linux   |
