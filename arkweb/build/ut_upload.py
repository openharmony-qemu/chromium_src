#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# Copyright (c) 2022 Huawei Device Co., Ltd.
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
#

import os
import shutil
import argparse


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('ut')
    args = parser.parse_args()
    print("start archiving")
    start_line = 0
    end_line = 0
    lines = []
    with open('arkweb/build/unittests/BUILD.gn', 'r', encoding='utf-8') as file:
        lines = file.readlines()
        for index, line in enumerate(lines):
            if args.ut == 'allut':
                if "oh_base_unittests" in line:
                    start_line = index
                    break
            if args.ut == 'coreut':
                if "oh_core_unittests" in line:
                    start_line = index
                    break
    for j in range(len(lines) - start_line):
        if "}" in lines[start_line + j]:
            end_line = start_line + j
            break

    deps_start = start_line
    deps_end = start_line

    for k in range(end_line - start_line):
        if "deps = [" in lines[start_line + k]:
            deps_start = start_line + k
            break

    for h in range(end_line - deps_start):
        if "]" in lines[deps_start + h]:
            deps_end = deps_start + h
            break

    deps_list = []
    for h in range(deps_end - deps_start - 1):
        deps_list.append(lines[deps_start + h + 1])

    out_dir = "out/musl_64/"
    saved_dir = out_dir + "ut/"
    unstripped = out_dir + "exe.unstripped/"
    if not os.path.exists(saved_dir):
        os.makedirs(saved_dir)
    if not os.path.exists(saved_dir + out_dir):
        os.makedirs(saved_dir + out_dir)
    file_array = []
    for deps in deps_list:
        utpath = deps[deps.find(":")+1:deps.rfind("\"")]
        dep_path = utpath + ".runtime_deps"
        copy_file(out_dir + utpath, saved_dir + out_dir + utpath)
        if "GERRIT_CHANGE_URL" not in os.environ:
            copy_file(unstripped + utpath, saved_dir + unstripped + utpath)
        with open(out_dir + dep_path, 'r', encoding='utf-8') as out_file:
            for file_path in out_file.readlines():
                file_path = file_path.replace("\n", "").replace("lib.unstripped/", "").replace("exe.unstripped/", "")
                if file_path not in file_array:
                    file_array.append(file_path)
                    if file_path.startswith("../../"):
                        file_path = file_path.replace("../../", "")
                        try:
                            copy_file(file_path, saved_dir + file_path)
                        except Exception as e:
                            print("permission denied")
                    else:
                        file_path = file_path.replace("./", "")
                        copy_file(out_dir + file_path, saved_dir + out_dir + file_path)
    global count
    print("Path：" + saved_dir + "，Quantity：" + str(count))

count = 0


def copy_file(source_path, target_path):
    folder_path = target_path[0:target_path.rfind("/")]
    if not os.path.exists(folder_path):
        os.makedirs(folder_path)
    try:
        shutil.copy(source_path, target_path)
        global count
        count += 1
    except FileNotFoundError:
        print("FileNotFoundError: " + source_path)
    except IsADirectoryError:
        copy_folder(source_path, target_path)


def copy_folder(source_folder, destination_folder):
    if not os.path.exists(destination_folder):
        os.makedirs(destination_folder)

    for item in os.listdir(source_folder):
        source = os.path.join(source_folder, item)
        destination = os.path.join(destination_folder, item)

        if os.path.isdir(source):
            try:
                copy_folder(source, destination)
            except Exception as e:
                print("permission denied")
        else:
            try:
                shutil.copy(source, destination)
                global count
                count += 1
            except Exception as e:
                print("permission denied")

if __name__ == "__main__":
    main()