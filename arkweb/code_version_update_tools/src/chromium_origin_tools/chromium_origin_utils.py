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

import sys
import platform

import os
import subprocess
import datetime
import shutil

if sys.version_info.major != 3:
    sys.stderr.write('Python3 is required!')
    sys.exit(1)
if platform.system().lower() != 'linux':
    sys.stderr.write(f'This is for linux! not for {platform.system()}')
    sys.exit(1)


def get_blue(text):
    return '\033[94m' + text + '\033[0m'


def get_red(text):
    return '\033[91m' + text + '\033[0m'


def print_error(text):
    print(get_red('*' * 100))
    print(get_red(text))
    print(get_red('*' * 100))


def print_blue(text):
    print(get_blue('*' * 100))
    print(f'{get_blue(text)}')
    print(get_blue('*'*100))


def run_one_cmd(cmd):
    print_blue(f'running {cmd}')
    cmd = ['/bin/sh', '-c', cmd]
    result = subprocess.run(cmd, shell=False, text=True)
    return result


def ERROR(info):
    output = "\033[1;31m" + info + "\033[0m"
    print(output)


def recursive_rm_all_git(folder_path):
    files = os.listdir(folder_path)
    for filename in files:
        if filename == '.git':
            shutil.rmtree(os.path.join(folder_path, filename))
        elif os.path.isdir(os.path.join(folder_path, filename)):
            recursive_rm_all_git(os.path.join(folder_path, filename))


def download_depot_tools():
    result = run_one_cmd(f'git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git')
    if result.returncode != 0:
        print_error(
            f'error! error return code: {result.returncode}. error msg: 如上所示\n请确认:(1)请确认是否配置能访问国外网站的代理！')
    return result


def update_depot_tools(local_depot_tools_path):
    result = run_one_cmd(
        f'cd {local_depot_tools_path} && git reset --hard HEAD && git clean -fxd && git fetch origin main && git reset --hard origin/main')
    if result.returncode != 0:
        print_error(
            f'error! error return code: {result.returncode}. error msg: 如上所示\n请确认:(1)是否配置能访问国外网站的代理！\n(2)请确认{depot_tools_path}是否是正确的depot_tools的git仓库')
    return result


def download_cef(CHROMIUM_BUILD):
    result = run_one_cmd(f'git clone https://bitbucket.org/chromiumembedded/cef.git -b {CHROMIUM_BUILD}')
    if result.returncode != 0:
        print_error(
            f'error! error return code: {result.returncode}. error msg: 如上所示\n请确认:(1)请确认是否配置能访问国外网站的代理！')
    return result


def update_cef(cef_path, CHROMIUM_BUILD):
    result = run_one_cmd(
        f'cd {cef_path} && git reset --hard HEAD && git clean -fxd && git fetch origin {CHROMIUM_BUILD} && git reset --hard origin/{CHROMIUM_BUILD}')
    if result.returncode != 0:
        print_error(
            f'error! error return code: {result.returncode}. error msg: 如上所示\n请确认:(1)是否配置能访问国外网站的代理！\n(2)请确认{cef_path}是否是正确的cef的git仓库')
    return result


def set_gclient():
    result = run_one_cmd(f'cd chromium && gclient config --name src https://chromium.googlesource.com/chromium/src.git --cache-dir {target_download_path}/chromium_cache_dir')
    if result.returncode != 0:
        print_error(
            f'error! error return code: {result.returncode}. error msg: 如上所示\n请确认:(1)是否配置能访问国外网站的代理！')
    return result


def check_gclient(gclient_path):
    with open(gclient_path, 'r') as f:
        data = f.read()
        if 'https://chromium.googlesource.com/chromium/src.git' not in data or 'cache_dir' not in data:
            print_error(
                f'error! 请检查本地chromium/.gclient是否正确指向https://chromium.googlesource.com/chromium/src.git，若否请手动删除.gclient文件')
            sys.exit(1)


def download_chromium(i, CHROMIUM_MAJOR, CHROMIUM_BUILD, CHROMIUM_PATCH):
    print_blue(f'尝试第{i + 1}/8次下载chromium代码中，受限制于代码仓库过大以及网络原因，可能需同步多次')
    result = run_one_cmd(
        f'cd chromium && gclient sync --revision src@refs/tags/{CHROMIUM_MAJOR}.0.{CHROMIUM_BUILD}.{CHROMIUM_PATCH}')
    if result.returncode != 0:
        return result
    
    # 尝试解决空文件夹错误
    run_one_cmd(f'cd chromium && gclient revert')

    result = run_one_cmd(
        f'cd chromium && gclient sync --revision src@refs/tags/{CHROMIUM_MAJOR}.0.{CHROMIUM_BUILD}.{CHROMIUM_PATCH} --reset -D --force')
    if result.returncode != 0:
        print_error(
            f'error! error return code: {result.returncode}. error msg: 第{i + 1}/8次同步chromium失败')
    return result


def update_chromium(i, CHROMIUM_MAJOR, CHROMIUM_BUILD, CHROMIUM_PATCH):
    print_blue(f'尝试第{i + 1}/8次更新chromium代码中，受限制于代码仓库过大以及网络原因，可能需同步多次')
    result = run_one_cmd(
        f'cd chromium && gclient recurse -- git reset --hard HEAD && gclient recurse --git clean -ffd')
    if result.returncode != 0:
        print_error(
            f'error! error return code: {result.returncode}. error msg: 第{i + 1}/8次同步chromium失败')
    result = download_chromium(i, CHROMIUM_MAJOR, CHROMIUM_BUILD, CHROMIUM_PATCH)
    return result


def download_repository_for_upload(CHROMIUM_MAJOR):
    result = run_one_cmd(
        f'git clone https://xxx/xxx.git -b xxx')
    if result.returncode != 0:
        print_error(
            f'unknown error! error return code: {result.returncode}. error msg: 如上所示')
    return result


def update_repository_for_upload(CHROMIUM_MAJOR):
    result = run_one_cmd(
        f'cd xxx && git reset --hard HEAD && git clean -ffxd && git fetch origin xxx && git reset --hard origin/xxx')
    if result.returncode != 0:
        print_error(
            f'unknown error! error return code: {result.returncode}. error msg: 如上所示')
    return result


def check_returncode(run_one_cmd_result):
    if run_one_cmd_result.returncode != 0:
        ERROR('命令执行失败，程序退出')
        sys.exit(-1)

target_download_path = os.path.join(os.sep.join(__file__.split(os.sep)[:-3]), 'chromium_origin')
if not os.path.exists(target_download_path):
    os.mkdir(target_download_path)
depot_tools_path = os.path.join(target_download_path, 'depot_tools')
os.environ['PATH'] = f'{depot_tools_path}:{os.environ["PATH"]}'
os.chdir(target_download_path)