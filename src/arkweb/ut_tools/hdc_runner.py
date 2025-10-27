import subprocess
import time
import os
import host_path


# if device is connect to a remote PC then add remote arguments to command
def LocalToRemote(cmd):
    print("LocalToRemote cmd: " + str(cmd))
    if host_path.GetIpPort() is not None:
        cmd.insert(1, "-s")
        cmd.insert(2, host_path.GetIpPort())


def IsDeviceAvailable():
    cmd = ["hdc", "list", "targets"]
    LocalToRemote(cmd)
    try:
        output = subprocess.check_output(cmd, text=True).strip()
        if output.startswith("[Empty]"):
            print("No available OH device found in hdc.")
            return False
        elif "connect failed" in output:
            raise Exception(output)
        else:
            return True

    except subprocess.CalledProcessError:
        print("hdc Command Execute Error: {}".format(' '.join(cmd)))
        raise subprocess.CalledProcessError


def PushDepsFiles(deps_files):
    print("Pushing deps files:", flush =True)
    cmd = ["hdc", "shell", "mount", "-o", "remount,rw", "/"]
    LocalToRemote(cmd)
    if subprocess.call(cmd) != 0:
        print("hdc cannot mount device.")
        raise Exception("Fail to mount device with hdc tools.")

    device_lib_path = host_path.GetDevicePath()
    cmd_prefix = ["hdc", "file", "send"]

    for f in deps_files:
        cmd = cmd_prefix + [f, device_lib_path]
        LocalToRemote(cmd)
        result = subprocess.call(cmd)
        if result != 0:
            raise Exception('Failed to send file: {}'.format(f))


def PushBinaryFile(file_path):
    device_path = host_path.GetDevicePath()
    cmd = ["hdc", "file", "send", file_path, device_path]
    LocalToRemote(cmd)
    result = subprocess.call(cmd)
    if result != 0:
        raise Exception('Failed to send file: {}'.format(file_path))



def Reboot():
    cmd = ["hdc", "shell", "reboot"]
    LocalToRemote(cmd)
    result = subprocess.call(cmd)
    if result != 0:
        raise Exception('Failed to execute hdc reboot')

    time.sleep(50)  # normally the device will be ready after 50 second.

    start_time = time.time()
    max_timeout = 120  # 2 minute
    check_interval = 20  # check every 20s
    while not IsDeviceAvailable():
        if time.time() - start_time > max_timeout:
            raise Exception('Device cannot reboot within the maximum timeout period')
        time.sleep(check_interval)

def Sync():
    cmd = ["hdc", "shell", "sync"]
    LocalToRemote(cmd)
    result = subprocess.call(cmd)
    if result != 0:
        raise Exception('Failed to execute hdc sync')

def RunFile(file_name, filter_file, filter_str, device_flag):
    device_path = host_path.GetDevicePath()
    cmd = 'shell "cd {}; chmod a+x ./{}; export LD_LIBRARY_PATH=`pwd`:${}; ./{} "'.format(
        device_path, file_name, device_path, file_name)

    if host_path.GetIpPort() is not None:
        cmd = 'hdc -s {} '.format(host_path.GetIpPort()) + cmd
    else:
        cmd = 'hdc ' + cmd
    cmd = cmd.split()

    if filter_file:
        cmd.insert(-1,'--test-launcher-filter-file={}'.format(os.path.basename(filter_file)))
    elif filter_str:
        cmd.insert(-1,'--gtest_filter={}'.format(filter_str))
    # # a bug: must use single process, if not, this result will not display:
    # # [==========] 1203 tests from 157 test suites ran. (8275 ms total)
    # cmd.insert(-1, "--single-process-tests")
    if device_flag != []:
        cmd[-1:-1] = device_flag
    print(cmd, flush = True)
    subprocess.run(cmd)


def RecvFile(remote_path, local_relative_dir):
    cmd = ["hdc", "file", "recv", remote_path, local_relative_dir]
    LocalToRemote(cmd)
    result = subprocess.call(cmd)
    if result != 0:
        raise Exception('Failed to receive file: {}'.format(file_path))