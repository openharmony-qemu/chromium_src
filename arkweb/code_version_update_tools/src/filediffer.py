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
import util

import config


class FileDiffer:
    def __init__(self, absolute_path, project_name, filename, status, paths):
        self.absolute_path = absolute_path #
        self.project_name = project_name # base
        self.filename = filename # BUILD.gn
        self.status = status # [M] [A] [D]
        self.paths = paths # ['/xxx/xxx', '/xxx/xxx']
        self.is_binary = self.is_binary_file() # False
        self.sub_path = self.get_sub_path() # win
        self.diff_filename = self.get_diff_filename() # [M][win]-BUILD.gn.diff
        self.save_filename = self.get_save_filename() # result/base/[M][win]-BUILD.gn.diff
        self.left_file = self.get_left_filename() #
        self.right_file = self.get_right_filename() #
        self.is_ignored_file = self.is_ignored_file() # False
        self.is_legal = self.get_legal() # True
        self.diff_line = self.get_diff_line() # 5
        self.relative_filename = self.get_relative_filename() # win/BUILD.gn
        self.merge_status = False
        self.merge_faild_line = 0
        self.link = None # https://xxx/xxx/diffs?filePath=xxx/xxx.xx
        self.rej_link = None
        self.count_total_hunk = 0
        self.count_fail_hunk = 0
    
    def set_link(self, url):
        self.link = url

    def set_rej_link(self, rej_url):
        self.rej_link = rej_url
    
    def set_merge_status(self, status):
        self.merge_status = status
    
    def get_diff_line(self):
        is_print = False
        if self.is_binary:
            return 0
        elif self.status == "[M]":
            command = "diff -Nau --no-dereference --ignore-all-space --strip-trailing-cr " + self.left_file + " " + self.right_file + " | grep -E '^[+]' | wc -l "
            return int(util.exec_and_output(command, is_print)) - 1 # 删除首行路径标识数据
        elif self.status == "[A]":
            command = "wc -l " + self.right_file + " | awk '{print $1}'"
            return int(util.exec_and_output(command, is_print))
        elif self.status == "[D]":
            command = "wc -l " + self.left_file + " | awk '{print $1}'"
            return int(util.exec_and_output(command, is_print))
        else:
            return -1

    def get_legal(self):
        return True

    def is_binary_file(self):
        file = self.absolute_path + self.filename
        if os.path.isdir(file):
            return True
        if not os.path.exists(file):
            return True
        if file.split(".")[-1] == "ko":
            return True
        with open(file, 'r') as f:
            try:
                f.read(1024)
            except UnicodeDecodeError:
                return True
            else:
                return False
    
    def get_sub_path(self):
        parts = self.absolute_path.split('/')
        try:
            # 查找基目录第一次出现的位置
            index = parts.index(self.project_name)
        except ValueError:
            return ""
        relative_parts = parts[index + 1:-1]
        sub_path = '/'.join(relative_parts)
        return sub_path
    
    def get_diff_filename(self):
        diff_filename = self.status + "[" + self.sub_path.replace("/", "_") + "]-" + self.filename + ".diff"
        return diff_filename
    
    def get_save_filename(self):
        save_filename = os.path.join("result", "src", self.project_name, self.diff_filename)
        return save_filename
    
    def get_left_filename(self):
        if len(self.paths) < 2:
            return ''
        left_file = os.path.join(self.paths[0], self.sub_path, self.filename)
        return left_file
    
    def get_right_filename(self):
        if len(self.paths) < 2:
            return ''
        right_file = os.path.join(self.paths[1], self.sub_path, self.filename)
        return right_file
    
    def get_relative_filename(self):
        relative_filename = os.path.join(self.sub_path, self.filename)
        return relative_filename
    
    # TODO:后续增加后缀以过滤
    def is_ignored_file(self):
        for ignored_path in config.ignored_path:
            if ignored_path in self.absolute_path:
                return True
        return False
