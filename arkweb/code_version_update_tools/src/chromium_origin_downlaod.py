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
from config import MAJOR, BUILD, PATCH
from chromium_origin_utils import run_one_cmd, \
download_depot_tools, update_depot_tools, download_cef, update_cef, \
set_gclient, check_gclient, download_chromium, update_chromium, \
download_repository_for_upload, update_repository_for_upload, target_download_path, check_returncode

if sys.version_info.major != 3:
    sys.stderr.write('Python3 is required!')
    sys.exit(1)
if platform.system().lower() != 'linux':
    sys.stderr.write(f'This is for linux! not for {platform.system()}')
    sys.exit(1)

# ******************************
# 下载/更新本地 depot_tools
# ******************************
depot_tools_path = os.path.join(target_download_path, 'depot_tools')
for i in range(3):
    if not os.path.exists(depot_tools_path):
        result = download_depot_tools()
        if result.returncode == 0:
            break
    else:
        result = update_depot_tools(depot_tools_path)
        if result.returncode == 0:
            break
check_returncode(result)


# ******************************
# 下载/更新本地 cef
# ******************************
cef_path = os.path.join(target_download_path, 'cef')
for i in range(3):
    if not os.path.exists(cef_path):
        result = download_cef(BUILD)
        if result.returncode == 0:
            break
    else:
        result = update_cef(cef_path, BUILD)
        if result.returncode == 0:
            break
check_returncode(result)


# ******************************
# 创建chromium文件夹
# ******************************
chromium_path = os.path.join(target_download_path, 'chromium')
if not os.path.exists(chromium_path):
    result = run_one_cmd('mkdir chromium')
check_returncode(result)


# *******************************************
# 创建/验证 "chromium/.gclient" 是否配置正确
# *******************************************
gclient_path = os.path.join(target_download_path, 'chromium', '.gclient')
for i in range(3):
    if not os.path.exists(gclient_path):
        result = set_gclient()
        if result.returncode == 0:
            break
    else:
        check_gclient(gclient_path)
check_returncode(result)


# ******************************
# 下载chromium代码
# ******************************
for i in range(8):
    if (not os.path.exists(os.path.join(target_download_path, 'chromium', '.gcs_entries'))) or (not os.path.exists(os.path.join(target_download_path, 'chromium', '.gclient_entries'))):
        result = download_chromium(i, MAJOR, BUILD, PATCH)
        if result.returncode == 0:
            break
    else:
        result = update_chromium(i, MAJOR, BUILD, PATCH)
        if result.returncode == 0:
            break
check_returncode(result)

