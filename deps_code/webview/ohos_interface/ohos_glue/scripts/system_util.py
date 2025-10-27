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

from io import open
from glob import iglob

import os
import datetime
import fnmatch
import shutil
import sys
import time

# ------------------------------------------------------------------------------------------------------#

def get_year():
  return str(datetime.datetime.now().year)

# ------------------------------------------------------------------------------------------------------#

def read_file(name, normalize=True):
  with open(name, 'r', encoding='utf-8') as f:
    data = f.read()
    if normalize:
      data = data.replace("\r\n", "\n")
    return data

# ------------------------------------------------------------------------------------------------------#

def write_file(name, data):
  with open(name, 'w', encoding='utf-8') as f:
    if sys.version_info.major == 2:
      f.write(data.decode('utf-8'))
    else:
      f.write(data)

# ------------------------------------------------------------------------------------------------------#
def make_dirs(name, quiet=True):
  if not path_exists(name):
    if not quiet:
      sys.stdout.write('creating ' + name + ' directory.\n')
    os.makedirs(name)

# ------------------------------------------------------------------------------------------------------#
def path_exists(name):
  return os.path.exists(name)

# ------------------------------------------------------------------------------------------------------#

def backup_file(name, quiet=True):
  bak_name = name + '.' + time.strftime('%Y-%m-%d-%H-%M-%S')
  shutil.move(name, bak_name)
  if not quiet:
    sys.stdout.write('moving ' + name + ' file.\n')

# ------------------------------------------------------------------------------------------------------#

def get_files(search_glob):
  recursive_glob = '**' + os.path.sep
  if recursive_glob in search_glob:
    if sys.version_info >= (3, 5):
      result = iglob(search_glob, recursive=True)
    else:
      result = get_files_recursive(*search_glob.split(recursive_glob))
  else:
    result = iglob(search_glob)

  return sorted(result)

# ------------------------------------------------------------------------------------------------------#

def get_files_recursive(directory, pattern):
  for root, dirs, files in os.walk(directory):
    for basename in files:
      if fnmatch.fnmatch(basename, pattern):
        filename = os.path.join(root, basename)
        yield filename

