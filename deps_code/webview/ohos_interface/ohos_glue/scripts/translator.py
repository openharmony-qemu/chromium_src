#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Huawei Device Co., Ltd.
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

from __future__ import absolute_import

import os
import sys
import file_parser
import system_util
import make_capi_header
import make_cpptoc_impl
import make_ctocpp_impl
import make_cpptoc_header
import make_ctocpp_header

# pylint:disable=huawei-redefined-outer-name

# cannot be loaded as a module
if __name__ != "__main__":
  sys.stderr.write('This file cannot be loaded as a module!')
  sys.exit()

root_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

module_name = ''
if len(sys.argv) - 1 > 0:
  module_name = sys.argv[1]

if len(sys.argv) - 2 > 0:
  dir_names = (sys.argv[2], '')
else:
  dir_names = ('ohos_adapter', 'ohos_nweb', '')

# Track the number of files that were written.
file_count = 0

# ------------------------------------------------------------------------------------------------------#

def update_file(contents, file_path):
  if contents[-1:] != "\n":
    # Add newline at end of file.
    contents += "\n"

  file_dir = os.path.split(file_path)[0]
  if not os.path.isdir(file_dir):
    system_util.make_dirs(file_dir)

  system_util.write_file(file_path, contents)
  sys.stdout.write('Finish to write file ' + os.path.basename(file_path) + '...\n')

  global file_count
  file_count += 1

# ------------------------------------------------------------------------------------------------------#

def is_same_side(header, class_name):
  cls = header.get_class(class_name)
  if cls is None:
    raise Exception('Class does not exist: ' + class_name)

  if cls.is_webview_side():
     return module_name == "webview"

  return module_name == "webcore"

# ------------------------------------------------------------------------------------------------------#

def update_capi_file(header, dir_name, work_dir):
  dir_path = os.path.join(os.path.join(work_dir, dir_name), 'capi')

  # build the list of file name to make
  file_names = sorted(header.get_file_names())
  if len(file_names) != 0:
    sys.stdout.write('In C API header directory ' + dir_path + '...\n')

  for file_name in file_names:
    sys.stdout.write('Generating ' + file_name + ' C API header...\n')
    update_file(*make_capi_header.make_capi_header_file(header, work_dir, dir_name, file_name))

# ------------------------------------------------------------------------------------------------------#

def update_cpptoc_file(header, dir_name, work_dir):
  dir_path = os.path.join(os.path.join(work_dir, dir_name), 'cpptoc')

  # build the list of class name to make
  class_names = sorted(header.get_class_names())
  if len(class_names) != 0:
    sys.stdout.write('In CppToC directory ' + dir_path + '...\n')

  for class_name in class_names:
    if len(module_name) != 0 and not is_same_side(header, class_name):
      continue
		
    sys.stdout.write('Generating ' + class_name + 'CppToC class header...\n')
    update_file(*make_cpptoc_header.make_cpptoc_header_file(header, work_dir, dir_name, class_name))

    sys.stdout.write('Generating ' + class_name + 'CppToC class implementation...\n')
    update_file(*make_cpptoc_impl.make_cpptoc_impl_file(header, work_dir, dir_name, class_name))

# ------------------------------------------------------------------------------------------------------#

def update_ctocpp_file(header, dir_name, work_dir):
  dir_path = os.path.join(os.path.join(work_dir, dir_name), 'ctocpp')

  # build the list of class name to make
  class_names = sorted(header.get_class_names())
  if len(class_names) != 0:
    sys.stdout.write('In CToCpp directory ' + dir_path + '...\n')

  for class_name in class_names:
    if len(module_name) != 0 and is_same_side(header, class_name):
      continue

    sys.stdout.write('Generating ' + class_name + 'CToCpp class header...\n')
    update_file(*make_ctocpp_header.make_ctocpp_header_file(header, work_dir, dir_name, class_name))

    sys.stdout.write('Generating ' + class_name + 'CToCpp class implementation...\n')
    update_file(*make_ctocpp_impl.make_ctocpp_impl_file(header, work_dir, dir_name, class_name))

# ------------------------------------------------------------------------------------------------------#

def translate_dir(dir_name):
  global file_count
  file_count = 0

  # make sure the header directory exists
  include_file_dir = os.path.join(os.path.join(root_dir, dir_name), 'include')
  if not system_util.path_exists(include_file_dir):
    sys.stderr.write('Directory ' + include_file_dir + ' does not exist.\n')
    return

  # create the header object
  sys.stdout.write('Parsing C++ headers from ' + include_file_dir + '...\n')
  header = file_parser.obj_header()

  # add include files to be processed
  header.set_root_directory(include_file_dir)
  header.add_directory(include_file_dir)

  # output the C API header file
  update_capi_file(header, dir_name, root_dir)

  # output the CppToC class file
  update_cpptoc_file(header, dir_name, root_dir)

  # output the CToCpp class file
  update_ctocpp_file(header, dir_name, root_dir)

  sys.stdout.write('Done - Wrote ' + str(file_count) + ' files.\n')

# ------------------------------------------------------------------------------------------------------#

for dir_name in dir_names:
  if len(dir_name) > 0:
    translate_dir(dir_name)
