#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2023 Huawei Device Co., Ltd.
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
import shutil
import sys


def main():
    """
    Copies NDK files by dynamically transforming their paths based on
    predefined rules. It receives the project's root directory as a
    command-line argument to construct absolute paths, making it
    independent of the execution environment.
    """
    if len(sys.argv) < 2:
        print("Error: Project root directory not provided.", file=sys.stderr)
        return 1

    project_root = sys.argv[1]

    base_src_path = os.path.join(project_root, "arkweb/ohos_adapter_ndk/stub")
    base_dest_path = os.path.join(project_root, "ohos_sdk/openharmony/native/sysroot/usr")

    if not os.path.isdir(base_src_path):
        print(f"Error: Source directory not found at '{base_src_path}'", file=sys.stderr)
        return 1

    copied_count = 0
    print("Applying path transformation rules to copy NDK files...")
    for root, _, files in os.walk(base_src_path):
        for filename in files:
            source_file = os.path.join(root, filename)
            relative_path_to_stub = os.path.relpath(source_file, base_src_path)

            dest_relative_path = ""

            # Rule 1: Handle header files by restructuring the path
            if os.sep + 'include' + os.sep in source_file:
                parts = relative_path_to_stub.split(os.sep + 'include' + os.sep, 1)
                module_path = parts[0]
                header_path = parts[1]
                dest_relative_path = os.path.join('include', module_path, header_path)

            # Rule 2: Handle library files by stripping the module path
            elif os.sep + 'lib' + os.sep in source_file:
                parts = relative_path_to_stub.split(os.sep + 'lib' + os.sep, 1)
                lib_path = os.path.join('lib', parts[1])
                dest_relative_path = lib_path

            else:
                print(f"Warning: No specific rule for '{relative_path_to_stub}'. Skipping.", file=sys.stderr)
                continue

            destination_file = os.path.join(base_dest_path, dest_relative_path)

            try:
                dest_dir = os.path.dirname(destination_file)
                os.makedirs(dest_dir, exist_ok=True)
                shutil.copy2(source_file, destination_file)
                copied_count += 1
            except OSError as e:
                print(f"Error copying {source_file} to {destination_file}: {e}", file=sys.stderr)
                return 1

    print(f"\nSuccessfully copied {copied_count} files based on path rules.")
    return 0


if __name__ == '__main__':
    sys.exit(main())