#!/usr/bin/env python3
# coding=utf-8
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

import argparse
import os
import stat
import sys

_CWD = os.getcwd()
_OLD_SCHEME = "chrome"
_NEW_SCHEME = "arkweb"


def _replace_scheme_in_file(src_path, dest_path, old_scheme, new_scheme):
  old_scheme_string = old_scheme + "://"
  new_scheme_string = new_scheme + "://"
  try:
    os.makedirs(os.path.dirname(dest_path), exist_ok=True)

    with open(src_path, 'r', encoding='utf-8', errors='ignore') as f_in:
      content = f_in.read()
    new_content = content.replace(old_scheme_string, new_scheme_string)

    flags = os.O_WRONLY | os.O_CREAT
    mode = stat.S_IWUSR | stat.S_IRUSR
    with os.fdopen(os.open(dest_path, flags, mode), 'w') as f_out:
      f_out.write(new_content)

    return True
  except Exception as e:
    sys.stderr.write(f"Error processing {src_path}: {e}\n")
    return False


def main(argv):
  parser = argparse.ArgumentParser()
  parser.add_argument('--in_folder', required=True)
  parser.add_argument('--out_folder', required=True)
  parser.add_argument('--in_files', nargs='*', required=True)

  args = parser.parse_args(argv)
  out_path = os.path.normpath(
      os.path.join(_CWD, args.out_folder).replace('\\', '/'))
  in_path = os.path.normpath(
      os.path.join(_CWD, args.in_folder).replace('\\', '/'))

  success_count = 0
  failure_count = 0
  for input_file in args.in_files:
    input_filepath = os.path.join(in_path, input_file)
    output_filepath = os.path.join(out_path, input_file)

    if _replace_scheme_in_file(input_filepath, output_filepath, _OLD_SCHEME,
                               _NEW_SCHEME):
      success_count += 1
    else:
      failure_count += 1

  if failure_count > 0:
    sys.exit(1)


if __name__ == "__main__":
  main(sys.argv[1:])
