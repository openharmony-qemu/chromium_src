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
import file_parser
import make_file_base

# pylint:disable=huawei-redefined-outer-name

# ------------------------------------------------------------------------------------------------------#

def make_cpptoc_header_file(header, dir_path, dir_name, class_name):
  cls = header.get_class(class_name)
  if cls is None:
    raise Exception('Class does not exist: ' + class_name)

  content = make_file_base.get_copyright()
  content += '\n'

  content += \
"""
#ifndef $GUARD$
#define $GUARD$
#pragma once
"""
  content += '\n'

  result = make_file_base.make_include_file(cls, 'CppToC', header, dir_name, class_name)
  content += result['content']
  content += '\n'

  content += 'namespace OHOS::ArkWeb {\n\n'

  content += '// Wrap a C++ class with a C structure.\n'
  content += '// This class may be instantiated and accessed DLL-side only.\n'
  content += make_file_base.make_class_define(cls, 'CppToC', result['base_name'], class_name, '')
  content += '\n'

  content += '} // OHOS::ArkWeb\n\n'
  
  content += \
"""
#endif // $GUARD$
"""

  # add the guard string
  def_name = make_file_base.make_def_file(cls, class_name)
  guard = def_name.upper() + '_CPPTOC_H_'
  content = content.replace('$GUARD$', guard)

  absolute_dir = os.path.join(os.path.join(dir_path, dir_name), 'cpptoc')
  absolute_path = os.path.join(absolute_dir, file_parser.get_capi_name(class_name, False) + '_cpptoc.h')

  return (content, absolute_path)
