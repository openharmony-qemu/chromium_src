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
import datetime

from chromium_origin_utils import MAJOR, BUILD, run_one_cmd, \
download_depot_tools, update_depot_tools, download_cef, update_cef, \
set_gclient, check_gclient, download_chromium, update_chromium, \
download_repository_for_upload, update_repository_for_upload, target_download_path, print_error, check_returncode

if sys.version_info.major != 3:
    sys.stderr.write('Python3 is required!')
    sys.exit(1)
if platform.system().lower() != 'linux':
    sys.stderr.write(f'This is for linux! not for {platform.system()}')
    sys.exit(1)

now_time_str = datetime.datetime.now().isoformat().replace(':', '-')

# ******************************
# 下载xxx目标仓库
# ******************************
repository_for_upload_path = os.path.join(target_download_path, 'xxx')
if not os.path.exists(repository_for_upload_path):
    result = download_repository_for_upload(MAJOR)
else:
    result = update_repository_for_upload(MAJOR)
check_returncode(result)

result = run_one_cmd(f'cd xxx && git checkout -b chromium_{MAJOR}_{BUILD}_{now_time_str} && git branch -u origin/xxx')
check_returncode(result)

# ******************************
# copy cef to chromium/cef
# ******************************
if not os.path.exists(os.path.join(target_download_path, 'chromium', 'src', 'cef')):
    print(f'检测到xxx/chromium/cef已存在，请确认之前是否已更改过{target_download_path}/chromium，建议重新执行download程序重置chromium代码')
    sys.exit(-1)

result = run_one_cmd('cp -a cef chromium/src/cef')
check_returncode(result)
result = run_one_cmd('cd chromium/src/cef && ./tools/patch_updater.sh')
check_returncode(result)

# ******************************
# 复制chromium代码到repository_for_upload
# ******************************
result = run_one_cmd('rsync -a --exclude="*/.git/" --delete --delete-excluded chromium/ xxx/chromium/')
check_returncode(result)
result = run_one_cmd(f'cd xxx && git add . && git commit -m "chromium_{MAJOR}_{BUILD} {now_time_str}"')
check_returncode(result)
result = run_one_cmd(f'cd xxx && find chromium -type f -size +50M | xargs git lfs track && git add . && git commit --amend --no-edit && git review -y')
check_returncode(result)
