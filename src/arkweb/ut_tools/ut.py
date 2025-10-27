#!/usr/bin/env python3
#
# Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
#
""" The entry of compiling and build public unittests.
"""

import argparse
import codecs
import re
import os
import sys
import json
import subprocess
import shutil
from datetime import datetime
from enum import Enum

# Global Defines
SRC_DIR = os.path.realpath(
    os.path.join(os.path.dirname(__file__), ".."))
OUT_DIR = ""

class RunningMode(Enum):
  all = 'all'
  changes = 'changes'
  specific = 'specific'

# Assumed the run_ut.py located at the same directory with this script
RUN_UT_PY = os.path.join(os.path.realpath(os.path.dirname(__file__)),
                         'run_ut.py')


def BuildArgumentParser():
  parser = argparse.ArgumentParser(
      description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)

  parser.add_argument('--gn-args',
                      dest='gn_args',
                      help='Custom gn args string',
                      default="")
  parser.add_argument('--out',
                      dest='output_directory',
                      help='Custom output directory name under <src>/out/',
                      default="test")
  parser.add_argument('--nextbuild',
                      action='store_true',
                      dest='use_nextbuild',
                      help='Use nexbuild in build process or not')
  parser.add_argument('--compile-only',
                      action='store_true',
                      dest='compile_only',
                      help='Compiled only, will not running tests')
  parser.add_argument('--coverage',
                      action='store_true',
                      dest='enable_coverage',
                      help='Enable coverage in tests')
  parser.add_argument('--openharmony',
                      action='store_true',
                      dest='test_for_openharmony',
                      help='Running unittests on linux is the default choice, '
                      'you can add argument --openharmony to force running tests on'
                      'openharmony, but openharmony tests depends on devices.')
  parser.add_argument('--continue-if-failed',
                      action='store_true',
                      dest='continue_if_failed',
                      help='Continue running next suite if current failed.')
  parser.add_argument('--mode',
                      choices=RunningMode,
                      dest='mode',
                      help='Specifies the mode in which ut runs.',
                      required=True,
                      type=RunningMode)
  parser.add_argument('--config',
                      dest='config',
                      help='A json file which including unit test target lists'
                      ' and its filter, json like '
                      '[\n'
                      '  {\n'
                      '    "test_suite":"base_unittests",\n'
                      '    "test_target":"//base:base_unittests",\n'
                      '    "test_filter":"huawei/webview/test/base_unittests.fi'
                      'lter",\n'
                      '    "device_flags_file":"huawei/webview/test/test.flags"'
                      '\n'
                      '  }\n'
                      ']',
                      default="",
                      required=True,
                      type=os.path.realpath)
  return parser


def EnsureOutDirExist(output_directory: str):
  assert len(output_directory) > 0, \
      "Error! Invalid output directory:" + output_directory

  global OUT_DIR
  OUT_DIR = output_directory

  out_dir = os.path.join(SRC_DIR, OUT_DIR)
  if (not os.path.exists(out_dir)):
    os.makedirs(out_dir)
  return


def InitGnProject(enable_coverage: bool, test_for_openharmony: bool, gn_args: str,
                  use_nextbuild: bool, out_dir: str):
  if test_for_openharmony:
    gn_args += ' target_os = "ohos"'\
                ' is_debug = false'\
                ' is_official_build = true'\
                ' is_component_build = false'\
                ' is_chrome_branded = false'\
                ' use_official_google_api_keys = false'\
                ' use_ozone = true'\
                ' use_aura = true'\
                ' ozone_auto_platforms = false'\
                ' ozone_platform = "headless"'\
                ' ozone_platform_headless = true'\
                ' enable_extensions = true'\
                ' ffmpeg_branding = "Chrome"'\
                ' use_kerberos = false'\
                ' use_bundled_fontconfig = true'\
                ' enable_resource_allowlist_generation = false'\
                ' clang_use_chrome_plugins = false'\
                ' enable_message_center = true'\
                ' safe_browsing_mode = 0'\
                ' use_custom_libcxx = false'\
                ' use_sysroot = false'\
                ' gpu_switch = "on"'\
                ' proprietary_codecs = true'\
                ' media_use_ffmpeg = true'\
                ' enable_ohos_nweb_hap = true'\
                ' target_cpu = "arm64"'\
                ' use_musl = true'\
                ' use_ohos_sdk_sysroot = false'\
                ' product_name = "all"'\
                ' cc_wrapper = "ccache"'\
                ' clang_use_chrome_plugins = false'\
                ' linux_use_bundled_binutils = false'\
                ' build_chromium_with_ohos_src = false'\
                ' is_asan = false'\
                ' symbol_level = 1'\
                ' use_goma = false'\

    gn_args += ' ohos_nweb_ex_config_name = "//{}/ohos_nweb_ex_config.gn"'.format(out_dir)   
  else:
    gn_args += ' is_component_build=false'\
               ' dcheck_always_on=true'\
               ' is_debug=false'\
               ' enable_widevine=true'\
               ' clang_use_chrome_plugins=false'\
               ' media_use_ffmpeg=false'

  if use_nextbuild:
    gn_args += ' cc_wrapper=""'

  if enable_coverage:
    gn_args += ' use_clang_coverage=true'\

  command = ['gn', 'gen', f'{OUT_DIR}']
  if len(gn_args) > 0:
    command += [f'--args={gn_args}']

  print(command)
  result = subprocess.run(command, cwd=SRC_DIR)
  if result.returncode != 0:
    print("subprocess failed with exit code: ", result.returncode)
    sys.exit(1)


def resolve_autoninja():
  if shutil.which('autoninja') is None:
    return os.path.join(SRC_DIR, 'third_party', 'depot_tools', 'autoninja')
  return 'autoninja'


def BuildProject(use_nextbuild: bool, targets: list):
  for target in targets:
    print(f'----------------building {target} begin----------------\n')
    command = []
    if use_nextbuild:
      next_build_bin_path = shutil.which('NextBuild')
      if len(next_build_bin_path) > 0:
        command += [f'{next_build_bin_path}', '--cache']
      else:
        print("Warning! NextBuild does not exist in your machine.")

    autoninja = resolve_autoninja()
    command += [f'{autoninja}', '-C', f'{OUT_DIR}', target ]

    print(command)
    print(SRC_DIR)
    result = subprocess.run(command, cwd=SRC_DIR, timeout=60000)
    if result.returncode != 0:
      print("subprocess failed with exit code: ", result.returncode)
      sys.exit(1)
    print(f'----------------building {target} end----------------\n')

def RunTests(enable_coverage: bool, test_for_openharmony: bool,
             continue_if_failed: bool, rest_args):
  command = ['python3', f'{RUN_UT_PY}', '--out', f'{OUT_DIR}']

  if continue_if_failed:
    command += ['--continue-if-failed']

  if test_for_openharmony:
    command += ['--openharmony']

  if enable_coverage:
    command += ['--coverage']

  command += rest_args

  print(command)
  process = subprocess.Popen(command, cwd=SRC_DIR)

  # Wait until command finished.
  process.communicate()
  return_code = process.returncode
  if return_code == 0:
    print("RunTests finished.")
  else:
    print("RunTests failed with return_code: %d" % return_code)
    sys.exit(1)

def RemoveJsonComments(code):
    # remove single-line comment
    code = re.sub(r'[^"//.*"]//.*','', code,flags=re.MULTILINE)
    # remove multiple-line comment
    code = re.sub(r'/\*.*?\*/','', code,flags=re.DOTALL)
    return code

def GetAllTestTargetsFromConfig(config: str):
  test_targets = []
  with codecs.open(config, encoding='utf-8') as f:
    json_without_comments = RemoveJsonComments(f.read())
    config_json = json.loads(json_without_comments)
    for target in config_json:
      test_target_path = target['test_target']
      test_suite_name = test_target_path.replace('//','',1)
      if test_suite_name in test_targets:
        print("Error! The test_suite_name '%s' is repeated in '%s'" %
              (test_suite_name, config))
        sys.exit(1)
      else:
        test_targets.append(test_suite_name)

  return test_targets

def main(args):
  parser = BuildArgumentParser()
  known_options, rest_args = parser.parse_known_args(args)

  use_nextbuild = known_options.use_nextbuild
  compile_only = known_options.compile_only
  enable_coverage = known_options.enable_coverage
  test_for_openharmony = known_options.test_for_openharmony
  continue_if_failed = known_options.continue_if_failed
  output_directory = known_options.output_directory
  gn_args = known_options.gn_args

  EnsureOutDirExist(output_directory)

  begin_time = datetime.now()
  print("UT Begin Time:%s" % begin_time)

  # ReConfigured the project by `gn gen <output> --args='...'`
  InitGnProject(enable_coverage, test_for_openharmony, gn_args, use_nextbuild, output_directory)


  test_targets = []
  test_targets = GetAllTestTargetsFromConfig(known_options.config)
  print("--------------------------------------------------")
  print("Prepare to build those test suites :")
  for test_suite_name in test_targets:
    print(test_suite_name)
  print("--------------------------------------------------")

  # Build
  BuildProject(use_nextbuild, test_targets)
  print("-------------------------------------------------- build end")

  # RunTest
  if not compile_only:
    print("-------------------------------------------------- before run")
    rest_args += ['--mode']
    rest_args += [known_options.mode.value]
    if known_options.mode != RunningMode.specific:
      rest_args += ['--config']
      rest_args += [known_options.config]
    print("-------------------------------------------------- before run 111 ")
    RunTests(enable_coverage, test_for_openharmony, continue_if_failed, rest_args)


  end_time = datetime.now()
  print("UT End Time:%s, total duration:%ss." %
        (end_time, end_time - begin_time))

  return 0


if '__main__' == __name__:
  sys.exit(main(sys.argv[1:]))
