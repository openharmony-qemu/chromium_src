# Copyright 2022 The Chromium Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""
Filters clang args to make them suitable for libclang.

Rust involves several libclang-based tools that parse C++.
We pass such tools our complete {{cflags}}, but a few of the
arguments aren't appropriate for libclang (for example those
which load plugins).

This function filters them out.
"""


def filter_clang_args(clangargs):

  def do_filter(args):
    i = 0
    while i < len(args):
      # 过滤掉指向 profdata 文件的路径指令
      if args[i].startswith('-fprofile-use='):
        pass 
      # 过滤掉 PGO 屏蔽警告指令
      elif args[i] in [
          '-Wno-profile-instr-unprofiled',
          '-Wno-profile-instr-out-of-date',
          '-Wno-backend-plugin'
      ]:
        pass
      # Intercept plugin arguments
      elif args[i] == '-Xclang':
        i += 1
        if args[i] == '-add-plugin':
          pass
        elif args[i].startswith('-plugin-arg'):
          i += 2
      else:
        yield args[i]
      i += 1

  return list(do_filter(clangargs))
