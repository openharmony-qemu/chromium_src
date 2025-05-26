import subprocess
import os


# 抓取hilog
def save_log(cmd_str, out_log_path):
    print("save log start")
    os.system('hdc shell hilog -r')
    str = 'hdc shell hilog > ' + out_log_path
    os.system(str)
    log_file = open(out_log_path, 'w')
    # 有一些应用使用gbk编码格式会报错，需要替换为utf8
    subp = subprocess.Popen(cmd_str, shell=True, stdout=log_file, stderr=subprocess.PIPE,
                            encoding='gbk')
    print("save log end")
    return subp


def catch_trace():
    print("start tracing")
    cmd_str = 'hdc shell hitrace -b 81920 -t 8 ace app ohos zimage zmedia zcamera zaudio ability  distributeddatamgr graphic sched freq irq sync mmc rpc workq idle disk pagecache memreclaim binder nweb -o /data/local/ohtract.txt'
    subp = subprocess.Popen(cmd_str)
    print("stop tracing")
    return subp


def remove_file_extension(file_name):
    if "." in file_name:
        return file_name[:file_name.rindex(".")]
    else:
        return file_name