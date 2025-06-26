#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
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
#

import sys
import os
import json


script_path = os.path.abspath(__file__)
script_dir = os.path.dirname(script_path)
prepopulated_engines_path = os.path.join(script_dir, "..", "..", "..", "components",\
    "search_engines", "prepopulated_engines.json")


def remove_line_comments(input_path, output_path):
    fdo = os.open(output_path, os.O_WRONLY | os.O_CREAT, mode=0o644)
    with open(input_path, 'r', encoding='utf-8') as infile, \
        os.fdopen(fdo, 'w', encoding='utf-8') as outfile:
        for line in infile:
            stripped_line = line.strip()
            if not stripped_line or not stripped_line.startswith('//'):
                outfile.write(line)


def traverse_and_modify(obj):
    if isinstance(obj, dict):
        for key, value in obj.items():
            if "_url" in key and isinstance(value, str):
                obj[key] = "https://***/"
            if "_url" in key and isinstance(value, list):
                obj[key] = ["https://***/"]
            else:
                traverse_and_modify(value)
    elif isinstance(obj, list):
        for item in obj:
            traverse_and_modify(item)


def main():
    dst_path = os.path.join(script_dir, "prepopulated_engines.json")
    remove_line_comments(prepopulated_engines_path, dst_path)
    with open(dst_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    traverse_and_modify(data)
    fdo = os.open(dst_path, os.O_WRONLY | os.O_CREAT | os.O_TRUNC, mode=0o644)
    with os.fdopen(fdo, 'w', encoding='utf-8') as ofile:
        json.dump(data, ofile, indent=4, ensure_ascii=False)


if __name__ == '__main__':
    sys.exit(main())
