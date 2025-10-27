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

def make_capi_static_funcs(funcs, indent, defined_names, translate_map):
  new_list = []
  old_list = make_file_base.get_func_name_list(funcs)

  result = ''
  for func in funcs:
    if func.get_retval().get_type().is_result_string():
      result += indent + '// The resulting string must be freed by calling cef_string_userfree_free().\n'

    suffix = ''
    new_list = make_file_base.get_func_name_count(func.get_capi_name(), old_list, new_list)
    if new_list.count(func.get_capi_name()) != 0:
      suffix = str(new_list.count(func.get_capi_name()))

    result += '\n' + indent + 'ARK_WEB_EXPORT ' + func.get_capi_proto(defined_names, suffix) + ';\n'

  return result

# ------------------------------------------------------------------------------------------------------#

def make_capi_class_funcs(funcs, indent, defined_names, translate_map):
  new_list = []
  old_list = make_file_base.get_func_name_list(funcs)
  
  result = ''
  for func in funcs:
    result += '\n'
    if func.get_retval().get_type().is_result_string():
      result += indent + '// The resulting string must be freed by calling cef_string_userfree_free().\n'

    suffix = ''
    new_list = make_file_base.get_func_name_count(func.get_capi_name(), old_list, new_list)
    if new_list.count(func.get_capi_name()) != 0:
      suffix = str(new_list.count(func.get_capi_name()))
    elif file_parser.check_func_name_is_key_work(func.get_capi_name()):
      suffix = '0'

    parts = func.get_capi_parts()
    result += indent+parts['retval']+' (ARK_WEB_CALLBACK *'+parts['name'] + suffix + \
              ')('+', '.join(parts['args'])+');\n'

  return result

# ------------------------------------------------------------------------------------------------------#

def make_capi_include_file(clses, header, file_name, all_declares):
  result = ''
  
  # identify all includes and forward declarations
  internal_includes = set([])
  translated_includes = set([])
  for cls in clses:
    includes = cls.get_includes()
    for include in includes:
      translated_includes.add(include)

    declares = cls.get_forward_declares()
    for declare in declares:
      declare_cls = header.get_class(declare)
      if declare_cls is None:
        raise Exception('Unknown class: %s' % declare)

      all_declares.add(declare_cls.get_capi_name())

    funcs = cls.get_virtual_funcs()
    for func in funcs:
      raw_type = func.get_retval().get_raw_type()
      if raw_type.find("<" + cls.get_name() + ">") != -1:
        all_declares.add(cls.get_capi_name())
        break

  # output translated includes
  flag = True
  if len(translated_includes) > 0:
    sorted_includes = sorted(translated_includes)
    for include in sorted_includes:
      if include == 'base/include/ark_web_base_ref_counted' and flag:
        flag = False
        result += '#include "base/capi/ark_web_base_ref_counted_capi.h"\n'
      elif include == 'base/include/ark_web_types' or include == 'ohos_nweb/include/ark_web_value' \
          or include == 'ohos_nweb/include/ark_web_message' or include == 'ohos_nweb/include/ark_web_nweb_structs' \
          or include == 'ohos_adapter/include/ark_web_adapter_structs' or include.endswith('_vector'):
        result += '#include "' + include + '.h"\n'
      else:
        if include.startswith('ohos_nweb/include/') and flag:
          flag = False
          result += '#include "base/capi/ark_web_base_ref_counted_capi.h"\n'
        result += '#include "' + include.replace('/include/', '/capi/') + '_capi.h"\n'
  else:
    result += '#include "base/capi/ark_web_base_ref_counted_capi.h"\n'

  # output internal includes
  if len(internal_includes) > 0:
    sorted_includes = sorted(internal_includes)
    for include in sorted_includes:
      result += '#include "include/' + include + '.h"\n'  

  return result

# ------------------------------------------------------------------------------------------------------#

def make_capi_class_body(clses, header, file_name, all_declares):
  result = ''

  # output forward declarations
  if len(all_declares) > 0:
    sorted_declares = sorted(all_declares)
    for declare in sorted_declares:
      result += '\n' + \
                'typedef struct _' + declare + ' ' + declare + ';\n'

  # structure names that have already been defined
  defined_names = header.get_defined_structs()

  # map of strings that will be changed in C++ comments
  translate_map = header.get_capi_translations()

  # output classes
  for cls in clses:
    # virtual functions are inside the structure
    capi_name = cls.get_capi_name()
    result += '\n' + \
              'typedef struct _' + capi_name + ' {\n' + \
              '  /**\n' + \
              '   * @brief Base structure.\n' + \
              '   */\n' + \
              '  ' + cls.get_parent_capi_name() + ' base;\n'
    funcs = cls.get_virtual_funcs()
    result += make_capi_class_funcs(funcs, '  ', defined_names, translate_map)
    result += '} ' + capi_name + ';\n'

    defined_names.append(capi_name)

    # static functions become global
    funcs = cls.get_static_funcs()
    if len(funcs) > 0:
      result += make_capi_static_funcs(funcs, '', defined_names, translate_map)

  # output global functions
  funcs = header.get_funcs(file_name)
  if len(funcs) > 0:
    result += make_capi_static_funcs(funcs, '', defined_names, translate_map) + '\n'

  return result

# ------------------------------------------------------------------------------------------------------#

def make_capi_header_file(header, dir_path, dir_name, file_name):
  # header string
  content = make_file_base.get_copyright() 

  content += \
"""
#ifndef $GUARD$
#define $GUARD$
#pragma once
"""
  content += '\n'

  clses = header.get_classes(file_name)
  
  all_declares = set([])
  content += make_capi_include_file(clses, header, file_name, all_declares)

  content += \
"""
#ifdef __cplusplus
extern "C" {
#endif
"""

  content += make_capi_class_body(clses, header, file_name, all_declares)

  # footer string
  content += \
"""
#ifdef __cplusplus
}
#endif

#endif // $GUARD$
"""

  # add the guard string
  guard = file_name.replace('/', '_').replace('.', '_capi_').upper() + '_'
  content = content.replace('$GUARD$', guard)

  absolute_dir = os.path.join(os.path.join(dir_path, dir_name), 'capi')
  absolute_path = os.path.join(absolute_dir, file_name.replace('.', '_capi.'))

  return (content, absolute_path)

