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
import re
import sys
import pickle

import util
import config
from filediffer import FileDiffer


def compare_folder(dir1, dir2, for_src):
    """
    The following result list is returned:
    1. Files path1/filename and path2/filename1 differ
    2. Only in path1: filename
    """
    is_print = True
    if for_src:
        cmd = "diff -aq --no-dereference --ignore-all-space --strip-trailing-cr -x \".git*\" " + dir1 + " " + dir2
    else:
        cmd = "diff -arq --no-dereference --ignore-all-space --strip-trailing-cr -x \".git*\" " + dir1 + " " + dir2
    result = util.exec_and_output(cmd, is_print)
    return result.split("\n")


def get_file_extension(file_path):
    if '.' in file_path:
        ext = file_path[file_path.rfind('.'):]
        return ext
    else:
        return None


def get_list_differ(dir1, dir2, for_src):
    list_file_differ = []
    re_modify = f"Files {dir1}/(.+?) and"
    re_add_del = r"Only in (.+?): (.*)"

    diff_lines = compare_folder(dir1, dir2, for_src)
    for line in diff_lines:
        # Initializing Variables
        filename = "unknown"
        if for_src:
            project_name = '../src'
        else:
            project_name = dir1.split("/")[-1]
        absolute_path = f"{dir1}/"
        status = "unknown"
        paths = [dir1, dir2]

        if line == "":
            continue
        if line.find("Files ") != -1:
            tmp = re.findall(re_modify, line)
            filename = tmp[0].split("/")[-1]
            absolute_path += tmp[0].split(filename)[0]
            status = "[M]"
        elif line.find("Only") != -1:
            tmp = re.findall(re_add_del, line)
            path = tmp[0][0]
            filename = tmp[0][1]
            absolute_path = f"{path}/"
            status = "[A]" if path.find(dir1) == -1 else "[D]"
        else:
            util.ERROR(f"Unknown differ type. line:{line}")
        
        extension = get_file_extension(filename)
        if extension in set(config.interest_file_type) and not any(substring in absolute_path for substring in config.ignored_path):
            list_file_differ.append(FileDiffer(absolute_path, project_name, filename, status, paths))
    return list_file_differ


def compare_file_and_save(folder_differ_map):
    """
    差异diff写入至文件
    """
    for (cur_dir, list_file_differ) in folder_differ_map.items():
        for file_differ_obj in list_file_differ:
            left_file = file_differ_obj.left_file
            right_file = file_differ_obj.right_file
            save_filename = file_differ_obj.save_filename
            util.mkdir_data_folder(f"result/src/{file_differ_obj.project_name}")
            if file_differ_obj.is_binary:
                command = f"echo \"二进制文件，不显示差异\" > {save_filename}"
            elif file_differ_obj.status == "[M]":
                command = f"diff -Nau --no-dereference --ignore-all-space --strip-trailing-cr {left_file} {right_file} > {save_filename}"
            else:
                command = f"echo \"新增/删除的文件\" > {save_filename}"
            is_print = True
            util.exec_and_output(command, is_print)


def handle_folder_diff(path1, path2):
    folder_differ_map = {}
    dir_list = util.get_work_dir_list(path1)
    print(dir_list)
    for cur_dir in dir_list:
        if cur_dir not in config.test_interest_path:
            continue
        list_file_differ = get_list_differ(f"{path1}/{cur_dir}", f"{path2}/{cur_dir}", False)
        folder_differ_map[cur_dir] = list_file_differ
    return folder_differ_map


def dump_differ_map(folder_differ_map):
    dump_filename = os.path.join(config.root, 'result', 'folder_differ_map.pkl')
    f = open(dump_filename, "wb")
    pickle.dump(folder_differ_map, f)
    f.close()


def difference_main(path1, path2):
    folder_differ_map = handle_folder_diff(path1, path2)

    list_file_differ_for_src = get_list_differ(path1, path2, True)
    folder_differ_map['../src'] = list_file_differ_for_src

    compare_file_and_save(folder_differ_map)
    dump_differ_map(folder_differ_map)


if __name__ == '__main__':
    util.INFO("==========difference running===========")
    left_path = os.path.join(config.source_left_path, "src")
    right_path = os.path.join(config.source_right_path, "src")
    util.INFO(f"PATH1: {left_path}, path2: {right_path}")
    difference_main(right_path, left_path)
