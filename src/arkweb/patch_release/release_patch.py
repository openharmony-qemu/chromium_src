#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import argparse
import os
import re
import subprocess
import sys

NEW_FILE = re.compile('^diff --git')
INDEX_LINE = re.compile('(^index [0-9a-fA-F]+\.\.[0-9a-fA-F]+) [0-7]+$')
MODE_LINE = re.compile('^old|new mode [0-7]+$')
BIN_FILE = re.compile('^Binary files')

EXCLUDE_DIRS = [
    'src/ohos_ndk*', 'src/ohos_sdk', 'src/ohos_nweb_*',
    'src/ohos_browser_shell', 'src/huawei', 'src/third_party/node',
    '*/.github/*'
]


def _bytes_to_string(str_bytes):
  if sys.version_info.major == 2:
    return str(str_bytes)
  return str_bytes.decode('utf-8')


def MakePatch():
  arg_parser = argparse.ArgumentParser()
  arg_parser.add_argument('-r',
                          '--range',
                          required=True,
                          help='Commit range, format: A..B')
  arg_parser.add_argument('--exclude-dirs',
                          required=False,
                          default=EXCLUDE_DIRS,
                          action='append',
                          help='Exclude those directories, default: %s' %
                          EXCLUDE_DIRS)
  arg_parser.add_argument('--include-file-mode-change',
                          required=False,
                          default=False,
                          action='store_true',
                          help='Include file mode changes in final patch file.')
  arg_parser.add_argument('output', help='Output patch filename')
  opts = arg_parser.parse_args()

  git_find_top_dir = 'git rev-parse --show-toplevel'
  proc = subprocess.Popen(git_find_top_dir,
                          shell=True,
                          stderr=subprocess.PIPE,
                          stdout=subprocess.PIPE)
  proc.wait()
  if proc.returncode != 0:
    print("ERROR: Failed to find a valid git repo:")
    print(proc.stderr.readline())
    return 4
  git_top_dir = _bytes_to_string(proc.stdout.readline()).strip()
  os.chdir(git_top_dir)

  git_diff_detect_cmd = 'git diff --no-color --quiet --exit-code "%s"' % opts.range
  proc = subprocess.Popen(git_diff_detect_cmd,
                          shell=True,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE)
  proc.wait()
  diff_detect_rc = proc.returncode
  if diff_detect_rc == 0:
    print("WARN: No difference found!")
    return 1
  if diff_detect_rc != 1:
    print("ERROR: Execute `git diff' command failed(%d), command output:" %
          diff_detect_rc)
    print(proc.stderr.readline())
    return 3

  git_diff_cmd = 'git diff --no-color --binary "%s" -- src' % opts.range
  for exclude in opts.exclude_dirs:
    git_diff_cmd = "%s ':!%s'" % (git_diff_cmd, exclude)
  proc = subprocess.Popen(git_diff_cmd,
                          shell=True,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE)
  with open(opts.output, 'w') as out:
    _buffer = []
    _last_file_line = ''
    _binary_file = False
    while True:
      line_bytes = proc.stdout.readline()
      if not line_bytes:
        break
      line = _bytes_to_string(line_bytes)
      if opts.include_file_mode_change:
        out.write(line)
        continue

      if NEW_FILE.match(line):
        _write_buffer_to_patch(out, _buffer, _last_file_line, _binary_file)
        _last_file_line = line
        _buffer = []
        _binary_file = False

      if BIN_FILE.match(line):
        _binary_file = True

      if MODE_LINE.match(line):
        continue

      _buffer.append(line)

    _write_buffer_to_patch(out, _buffer, _last_file_line, _binary_file)

  return 0


def _write_buffer_to_patch(out, changes, filename, is_binary):
  if len(changes) <= 1 or len(filename) <= 0:
    return

  if not is_binary:
    for _idx in range(len(changes)):
      line = changes[_idx]
      index_line = INDEX_LINE.match(line)
      if index_line and len(index_line.groups()) == 1:
        changes[_idx] = index_line.groups()[0] + '\n'

  _gitignore_file = False
  _files = filename.split()
  for _name in _files:
    if _name.strip().endswith('.gitignore.backup'):
      _gitignore_file = True
      break
  if not _gitignore_file:
    out.writelines(changes)


if __name__ == '__main__':
  sys.exit(MakePatch())
