#!/bin/bash
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

set -e

if [[ "$1" == "-clean" ]]; then
  folder_names=("target" "data/source_left" "data/source_right" "data/source_right_for_apply")
  for folder_name in "${folder_names[@]}"
  do
    if [ -d "$folder_name" ]; then
      if [ -n "$(ls -A "$folder_name" 2>/dev/null)" ]; then
        echo rm -r "$folder_name"
        rm -r "$folder_name"
      else
        echo rm -d "$folder_name"
        rm -d "$folder_name"
      fi
    fi
  done
  exit 0
fi


SCRIPT_PATH=$(realpath "$0")
CHECK_DIR=$(dirname $(dirname $(dirname $(dirname "$SCRIPT_PATH"))))
echo "$CHECK_DIR"
if [ -d "$CHECK_DIR/.repo" ]; then
  echo "此脚本不能在repo仓库下执行，请移出repo仓库"
  exit 1
fi


folder_names=("data" "result/src")
for folder_name in "${folder_names[@]}"
do
  if [ ! -d "$folder_name" ]; then
    echo mkdir -p "$folder_name"
    mkdir -p "$folder_name"
  fi
done


folder_name="target"
if [ ! -d "$folder_name" ]; then
    echo python3 src/download.py
    python3 src/download.py
    if [ $? -ne 0 ]; then
        echo "执行失败，脚本终止。"
        exit 1
    fi
fi

cmds=("python3 src/download.py" "python3 src/difference.py" "python3 src/applypatch.py")
for cmd in "${cmds[@]}"
do
    echo $cmd
    $cmd
    if [ $? -ne 0 ]; then
        echo "$cmd 执行失败，脚本终止。"
        exit 1
    fi
done