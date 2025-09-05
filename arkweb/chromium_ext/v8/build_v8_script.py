#!/usr/bin/env python
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
import re

# Switch to V8 directory
try:
    os.chdir("../../v8")
except FileNotFoundError as e:
    print(f"Error: Directory not found - {e}")
    exit(1)
except PermissionError as e:
    print(f"Error: Permission denied - {e}")
    exit(1)

# use git show get include/v8-version.h change
try:
    result = subprocess.run(
        ["git", "show", "HEAD:include/v8-version.h"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=True,
        text=True
    )
    content = result.stdout
except subprocess.CalledProcessError as e:
    print(f"Error: Git command failed - {e.stderr}")
    print(f"Failed to retrieve the content of the include/v8-version.h file! Error: {e.stderr}")
    exit(1)

# Extract the V8_INNER_VERSION macro definition
version_pattern = re.compile(r'#define\s+V8_INNER_VERSION\s+(\d+)')
match = version_pattern.search(content)
if not match:
    print("FAILED: V8_INNER_VERSION definition not found in the file!")
    exit(1)
current_version = int(match.group(1))

# Retrieve the version number of the last commit (assuming it is HEAD~1)
try:
    prev_result = subprocess.run(
        ["git", "show", "HEAD~1:include/v8-version.h"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=True,
        text=True
    )
    prev_content = prev_result.stdout
except subprocess.CalledProcessError as e:
    print("FAILED: Unable to retrieve the content of the last submitted v8-version.h file!")
    exit(1)
prev_match = version_pattern.search(prev_content)
if not prev_match:
    print("FAILED: V8_INNER_VERSION definition not found in the submission!")
    exit(1)
prev_version = int(prev_match.group(1))

# Check if +1
if current_version != prev_version + 1:
    print(f"FAILE: The current version number {current_version} must be exactly one greater than the previous version {prev_version}")
    exit(1)
else:
    print(f"SUCCESS: The current version number {current_version} is the previous version {prev_version} +1!")
