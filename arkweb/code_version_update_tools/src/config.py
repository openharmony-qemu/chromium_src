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

root = os.sep.join(__file__.split(os.sep)[:-2])
source_left_path = os.path.join(root, 'data', 'source_left')
source_right_path = os.path.join(root, 'data', 'source_right')
source_right_for_apply_path = os.path.join(root, 'data', 'source_right_for_apply')
target_path = os.path.join(root, 'target')

ignored_path = ['cef/include/capi', 'cef/libcef_dll']
interest_file_type = ['.cc', '.h', '.cpp', '.hpp', '.gn', '.gni', '.grdp', '.grd', '.mojom', '.proto', '.fbs', '.yaml', '.json5', '.gypi']

# 自定义参数:
test_interest_path = [
    'cef', 'media', 'ppapi', 'gin', 'crypto', 'gpu', 'components', 'ui', 'extensions',
    'google_apis', 'fuchsia_web', 'pdf', 'buildtools', 'chromeos', 'sandbox', 'testing',
    'chromecast', 'rlz', 'device', 'headless', 'chrome', 'native_client_sdk', 'sql', 'ipc',
    'build_overides', 'LICENSE.chromium_os', 'mojo', 'docs', 'apps', 'android_webview', 'ios', 'storage',
    'content', 'ash', 'base', 'url', 'net', 'reomting', 'third_party', 'printing', 'services', 'infra', 'cc',
    'build', 'v8', 'skia', 'dbus'
]

# xxx与xxx的对比
remote_source_left_path = ('repo', '132_trunk', 'https://gitcode.com/openharmony-tpc/manifest.git', 'developer.xml')
remote_source_right_path = ('git', 'chromium_baseline', 'https://gitcode.com/openharmony-tpc/chromium_src.git')
remote_target_path = ('git', 'xxx', 'https://xxx/xxx.git')

# chromium_origin_xxx所使用的版本号，谷歌版本号格式：MAJOR.MINOR.BUILD.PATCH。如：https://chromium.googlesource.com/chromium/src/+refs
MAJOR = 138
BUILD = 7204
PATCH = 35