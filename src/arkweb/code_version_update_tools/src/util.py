#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2025 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import subprocess


def INFO(info):
    output = "\033[1;32m" + info + "\033[0m"
    print(output)


def WARNING(info):
    output = "\033[1;33m" + info + "\033[0m"
    print(output)


def ERROR(info):
    output = "\033[1;31m" + info + "\033[0m"
    print(output)


def exec_and_output(cmd, is_print=False):
    if is_print:
        print(cmd)
    cmd = ['/bin/sh', '-c', cmd]
    pipe = subprocess.Popen(cmd, shell=False, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = pipe.communicate()
    if stderr:
        ERROR(stderr)
    return stdout


def exec_and_stderr_stdout(cmd, is_print=False):
    if is_print:
        print(cmd)
    cmd = ['/bin/sh', '-c', cmd]
    pipe = subprocess.Popen(cmd, shell=False, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = pipe.communicate()
    ERROR(stderr)
    return stderr, stdout


def get_work_dir_list(path):
    dir_list = []
    dirs = os.listdir(path)
    for cur_dir in dirs:
        if cur_dir[-4:] == ".zip":
            continue
        dir_list.append(cur_dir)
    return dir_list


def mkdir_data_folder(path):
    cmd = "mkdir " + path + " > /dev/null 2>&1"
    exec_and_output(cmd)


def run_one_cmd(cmd):
    cmd = ['/bin/sh', '-c', cmd]
    with subprocess.Popen(cmd, shell=False, text=True,\
        stdout=subprocess.PIPE, stderr=subprocess.PIPE) as p:
        print(f'\033[94m******************************\nruning cmd: {cmd}\n******************************\033[0m')
        stdout, stderr = p.communicate()
        ERROR(stderr)
        return stdout, stderr, p.returncode
