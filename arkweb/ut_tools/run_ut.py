#!/usr/bin/env python3
#
# Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
#
""" Run unittests and print results.
"""

import argparse
import codecs
import glob
import json
import os
import re
import shutil
import subprocess
import sys

from datetime import datetime
from enum import Enum

# Global Defines
SRC_DIR = os.path.realpath(
    os.path.join(os.path.dirname(__file__), ".."))
# out/xxx/UT_REPORT which stores all the ut results
UT_REPORT_DIR = 'UT_REPORT'
# out/xxx/UT_REPORT/coverage wich stores all the coverage results
COVERAGE_DIR = 'coverage'
EXIT_IF_FAILED = True
IS_FAILED = False

TIMESTAMP = datetime.now().strftime("%Y%m%d%H%M%S")

# Linux Test Result Type 1 Example:
# [  FAILED  ] Eee.Fff (219 ms)
# [1327/1327] Eee.Fff (219 ms)
# 3 tests failed:
#    Aaa.Bbb
#    Ccc.Ddd
#    Eee.Fff
# 1 test crashed:
#    Ggg.Hhh
# 3 tests timed out :
#    Iii.Jjj
#Tests took 52 seconds.
_EXTRACT_LINUX_TEST_RESULT1_REGEX = re.compile(
    r'\[[0-9]+/(?P<total_cases>[0-9]+)\].*\s+'
    r'(?:[0-9]+ tests? (?:failed|crashed)|'
    r'SUCCESS: all tests passed).*(\s.*)+'
    r'Tests took (?P<total_time>[0-9]+) seconds',
    flags=re.DOTALL)

_EXTRACT_LINUX_TEST_RESULT1_FAILED_REGEX = re.compile(
    r'(?P<failed_cases>[0-9]+) tests? failed:')

_EXTRACT_LINUX_TEST_RESULT1_CRASHED_REGEX = re.compile(
    r'(?P<crashed_cases>[0-9]+) tests? crashed:')

_EXTRACT_LINUX_TEST_RESULT1_TIMEOUT_REGEX = re.compile(
    r'(?P<timed_out_cases>[0-9]+) tests? timed out:')

# Linux Test Result Type 2 Example:
# [==========] 144 tests from 22 test suites ran. (5492 ms total)
# [  PASSED  ] 142 tests.
# [  SKIPPED ] 1 test, listed below:
# [  SKIPPED ] Aaa.Bbb
# [  FAILED  ] 1 test, listed below:
# [  FAILED  ] Aaa.Bbb
_EXTRACT_LINUX_TEST_RESULT2_REGEX = re.compile(
    r'\[={10}\]\s*(?P<total_cases>[0-9]+)\s*tests? from.*test suites ran.*'
    r'\((?P<total_time>[0-9]+) ms total\)')

_EXTRACT_LINUX_TEST_RESULT2_FAILED_REGEX = re.compile(
    r'\[\s+FAILED\s+\]\s+(?P<failed_cases>[0-9]+) test?, listed below:')

# Android Test Result Example:
#  <tfoot>
#    <tr>
#        <td class="suite-table-summary-column-0 center">
#            <a onclick="showTestsOfOneSuiteOnlyWithNewState(&#34;TOTAL&#34;)">TOTAL</a>
#          </td><td class="suite-table-summary-column-1 center">
#            <b>1165</b>
#          </td><td class="suite-table-summary-column-2 center success">
#            <b>0</b>
#          </td><td class="suite-table-summary-column-3 center">
#            <b>1165</b>
#          </td><td class="suite-table-summary-column-4 center">
#            <b>1618</b>
#          </td>
#      </tr>
#  </tfoot>
_EXTRACT_ANDROID_TEST_RESULT_PASSED_REGEX = re.compile(
    r'suite-table-summary-column-1.*\s'
    r'.*<b>(?P<passed_cases>[0-9]+)</b>')

_EXTRACT_ANDROID_TEST_RESULT_FAILED_REGEX = re.compile(
    r'suite-table-summary-column-2.*\s'
    r'.*<b>(?P<failed_cases>[0-9]+)</b>')

_EXTRACT_ANDROID_TEST_RESULT_TOTAL_REGEX = re.compile(
    r'suite-table-summary-column-3.*\s'
    r'.*<b>(?P<total_cases>[0-9]+)</b>')

_EXTRACT_ANDROID_TEST_RESULT_TIME_REGEX = re.compile(
    r'suite-table-summary-column-4.*\s'
    r'.*<b>(?P<total_time>[0-9]+)</b>')


class RunningMode(Enum):
  all = 'all'
  changes = 'changes'
  specific = 'specific'


class TestTarget(object):

  def __init__(self, test_suite_name: str, test_target_path: str,
               gtest_filter: str, test_filter_path: str,
               device_flags_file: str):
    """Constructs a TestTarget object.

    Args:
      _test_suite_name: string, eg. "android_webview_unittests"
      _test_target_path: string, eg. "//base:base_unittests"
      _gtest_filter: string, eg. "-MinidumpWriterTest.DeletedBinary;XX.X"
      _test_filter_path: file path string, eg. "huawei/webview/test/a.filter"
      _device_flags_file: cmdline flags file, eg. "huawei/webview/test/a.flags"
    """
    self._test_suite_name = test_suite_name.strip().lower()
    self._test_target_path = test_target_path.strip().lower()
    # gtest_filter is case sensitive.
    self._gtest_filter = gtest_filter.strip()
    self._test_filter_path = test_filter_path.strip().lower()
    self._device_flags_file = device_flags_file.strip().lower()


class TestResult(object):

  def __init__(self, test_suite_name: str, failed_cases: str, total_cases: str,
               total_time: str):
    """Constructs a TestResult object.

    Args:
      _test_suite_name: string, eg. "android_webview_unittests"
      _test_target_path: string, eg. "//base:base_unittests"
      _test_filter: file path string, eg. "huawei/webview/test/a.filter"
      _device_flags_file: cmdline flags file, eg. "huawei/webview/test/a.flags"
    """
    assert test_suite_name != ""

    if failed_cases == None:
      failed_cases = '0'
    if total_cases == None:
      total_cases = '0'
    if total_time == None:
      total_time = '0'

    self._test_suite_name = test_suite_name.strip().lower()
    self._failed_cases = failed_cases
    self._total_cases = total_cases
    self._passed_cases = str(int(total_cases) - int(failed_cases))
    self._total_time = total_time
    self._max_field_len = max(len(self._total_cases), len(self._total_time))

  def __lt__(self, other):
    return self._max_field_len < other._max_field_len


def PrintRed(text):
  print("\033[31m{}\033[0m".format(text))


def PrintGreen(text):
  print("\033[32m{}\033[0m".format(text))


def GetLatestFolderWithPrefix(directory: str, prefix: str):
  all_folders = glob.glob(os.path.join(directory, prefix + "*"))
  latest_folder_name = max(all_folders, key=os.path.getctime, default=None)
  return latest_folder_name

def RemoveJsonComments(code):
    # remove single-line comment
    code = re.sub(r'[^"//.*"]//.*','', code,flags=re.MULTILINE)
    # remove multiple-line comment
    code = re.sub(r'/\*.*?\*/','', code,flags=re.DOTALL)
    return code

def GetAllTestTargetsFromConfig(config: str):
  test_targets = {}
  with codecs.open(config, encoding='utf-8') as f:
    config_json = json.loads(RemoveJsonComments(f.read()))
    for target in config_json:
      test_suite_name = target['test_suite']

      test_target_path = target['test_target']

      gtest_filter = ""
      if 'gtest_filter' in target:
        gtest_filter = target['gtest_filter']

      test_filter_path = ""
      if 'test_filter_file' in target:
        test_filter_path = target['test_filter_file']

      device_flags_file = ""
      if 'device_flags_file' in target:
        device_flags_file = target['device_flags_file']

      if test_suite_name in test_targets:
        print("Error! The test_suite_name '%s' is repeated in '%s'" %
              (test_suite_name, config))
        sys.exit(1)
      else:
        test_target = TestTarget(test_suite_name, test_target_path,
                                 gtest_filter, test_filter_path,
                                 device_flags_file)
        test_targets[test_suite_name] = test_target

  return test_targets


# Get changed files from git not untracked
# eg. files under newly added dir
def GetGitUntrackedFileList():
  command = ['git', 'ls-files', '--others', '--exclude-standard']
  result = subprocess.run(command, capture_output=True, text=True, cwd=SRC_DIR)
  untracked_file_list = result.stdout.strip().split('\n')
  return untracked_file_list


# Get changed files from git not unstaged
def GetGitUnstagedFileList():
  command = ['git', 'diff', '--name-only', '--relative']
  result = subprocess.run(command, capture_output=True, text=True, cwd=SRC_DIR)
  unstaged_file_list = result.stdout.strip().split('\n')
  return unstaged_file_list


# Get changed files from git staged
def GetGitStagedFileList():
  command = ['git', 'diff', '--name-only', '--cached', '--relative']
  result = subprocess.run(command, capture_output=True, text=True, cwd=SRC_DIR)
  staged_file_list = result.stdout.strip().split('\n')
  return staged_file_list


def GetGitRemoteBranch():
  command = ['git', 'rev-parse', '--abbrev-ref', '--symbolic-full-name', '@{u}']
  result = subprocess.run(command, capture_output=True, text=True, cwd=SRC_DIR)
  remote_branch = result.stdout.strip()
  return remote_branch


# Get changed files from git local committed
def GetGitCommittedFileList():
  remote_branch = GetGitRemoteBranch()
  command = [
      'git', 'log', f'{remote_branch}..HEAD', '--name-only',
      '--pretty=format:""', '--relative'
  ]
  result = subprocess.run(command, capture_output=True, text=True, cwd=SRC_DIR)
  committed_file_list = result.stdout.strip().split('\n')
  return committed_file_list


def GetLocalChangedFileList():
  changed_file_list = []

  untracked_file_list = GetGitUntrackedFileList()
  changed_file_list += untracked_file_list

  unstaged_file_list = GetGitUntrackedFileList()
  changed_file_list += unstaged_file_list

  staged_file_list = GetGitStagedFileList()
  changed_file_list += staged_file_list

  committed_file_list = GetGitCommittedFileList()
  changed_file_list += committed_file_list

  # Remove empty or repeat file
  changed_file_list = list(set(changed_file_list))
  if '' in changed_file_list:
    changed_file_list.remove('')
  if '""' in changed_file_list:
    changed_file_list.remove('""')

  return changed_file_list


def WriteGnAnalyzeInputJson(test_targets: dict, changed_file_list: list,
                            output_dir: str):
  gn_analyze_input_json_template = '{\n  "files":%s,\n  "test_targets":%s\n}'

  changed_file_list_str = "[\n"
  for f in changed_file_list:
    if len(changed_file_list_str) > 2:
      changed_file_list_str += ',\n'
    changed_file_list_str += "    \"//"
    changed_file_list_str += f
    changed_file_list_str += "\""
  changed_file_list_str += "\n  ]"

  test_targets_str = "[\n"
  for k, v in test_targets.items():
    if len(test_targets_str) > 2:
      test_targets_str += ',\n'
    test_targets_str += "    \""
    test_targets_str += v._test_target_path
    test_targets_str += "\""
  test_targets_str += "\n  ]"

  gn_analyze_input_json_path = os.path.join(SRC_DIR, output_dir, UT_REPORT_DIR,
                                            "input.json")
  with codecs.open(gn_analyze_input_json_path, 'w', encoding='utf-8') as f:
    f.write(gn_analyze_input_json_template %
            (changed_file_list_str, test_targets_str))
  return gn_analyze_input_json_path


def ReadGnAnalyzeOutputJson(gn_analyze_output_json_path: str):
  with codecs.open(gn_analyze_output_json_path, encoding='utf-8') as f:
    gn_analyze_output = json.load(f)
  return gn_analyze_output


def ExecGnAnalyze(gn_analyze_input_json: str, gn_analyze_output_json: str,
                  output_dir: str):
  command = [
      'gn', 'analyze', f'{output_dir}', f'{gn_analyze_input_json}',
      f'{gn_analyze_output_json}'
  ]
  subprocess.run(command, cwd=SRC_DIR)


def GetTestTargetsByChangedList(test_targets: dict, changed_file_list: list,
                                output_dir: str):
  gn_analyze_input_json = WriteGnAnalyzeInputJson(test_targets,
                                                  changed_file_list, output_dir)
  gn_analyze_output_json = os.path.join(SRC_DIR, output_dir, UT_REPORT_DIR,
                                        "output.json")
  ExecGnAnalyze(gn_analyze_input_json, gn_analyze_output_json, output_dir)
  gn_analyze_output = ReadGnAnalyzeOutputJson(gn_analyze_output_json)

  updated_test_targets = {}
  for test_target_path in gn_analyze_output['test_targets']:
    paths = test_target_path.rsplit(':', 1)
    if len(paths) > 1:
      test_suite_name = paths[1]
      updated_test_targets[test_suite_name] = test_targets[test_suite_name]
    else:
      print('Error! Invalid test target path :%s' % test_target_path)
      sys.exit(1)

  return updated_test_targets


def RunSpecificTest(test_suite_name: str, test_filter: str,
                    test_launcher_filter_file: str, device_flags_file: str,
                    output_dir: str, ip_port: str):
  command = [f'./run_{test_suite_name}']
  if test_filter != "":
    command += [f'--gtest_filter={test_filter}']
    command += ['--single-process-tests']
  if test_launcher_filter_file != "" and os.path.exists(
      test_launcher_filter_file):
    command += [f'--test-launcher-filter-file={test_launcher_filter_file}']
  if device_flags_file != "" and os.path.exists(device_flags_file):
    command += ['--device-flags-file', f'{device_flags_file}']
  if ip_port is not None:
    command += [f'--ip-port={ip_port}']
  command += ['--fast-local-dev']

  print("RunSpecificTest(%s):%s" % (test_suite_name, command))
  run_test_script_dir = os.path.join(SRC_DIR, output_dir, "bin")
  subprocess.run(command, cwd=run_test_script_dir)


def RunTestForLinux(test_suite_name: str, gtest_filter: str,
                    test_filter_file: str, device_flags_file: str,
                    output_dir: str, log_dir: str):
  command = [f'./run_{test_suite_name}']
  if gtest_filter != "":
    command += [f'--gtest_filter={gtest_filter}']
  if test_filter_file != "" and os.path.exists(test_filter_file):
    command += [f'--test-launcher-filter-file={test_filter_file}']
  if device_flags_file != "" and os.path.exists(device_flags_file):
    command += [f'--device-flags-file={device_flags_file}']
  command += ['--fast-local-dev', '--test-launcher-retry-limit=0']

  print("RunTestForLinux(%s):%s" % (test_suite_name, command))
  run_test_script_dir = os.path.join(SRC_DIR, output_dir, "bin")
  log_file = os.path.join(log_dir, test_suite_name + '.txt')

  with codecs.open(log_file, 'w', encoding='utf-8') as f:
    # Use subprocess.PIPE to print and write the logfile at the same time.
    process = subprocess.Popen(command,
                               stdout=subprocess.PIPE,
                               text=True,
                               cwd=run_test_script_dir)
    for line in process.stdout:
      print(line, end='')
      f.write(line)

    # Wait for the subprocess finished
    return_code = process.wait()
    if return_code != 0:
      PrintRed(test_suite_name + ' failed, return ' + str(return_code))
      if EXIT_IF_FAILED:
        PrintRed('Test exits because the ' + test_suite_name + ' failed.')
        sys.exit(1)


def RunTestWithCoverage(test_targets: dict, output_dir: str):
  command = ['python3', 'tools/code_coverage/coverage.py']
  for test_suite_name, target in test_targets.items():
    command += [f'{test_suite_name}']

  coverage_result_dir = os.path.join(output_dir, UT_REPORT_DIR, COVERAGE_DIR)
  command += ['-b', f'{output_dir}', '-o', f'{coverage_result_dir}']

  for test_suite_name, target in test_targets.items():
    test_suite_script = os.path.join(output_dir, test_suite_name)
    sub_command_str = test_suite_script
    if target._test_filter_path != "":
      sub_command_str += \
          ' --test-launcher-filter-file ' + target._test_filter_path
    if target._device_flags_file != "":
      sub_command_str += \
          ' --device-flags-file ' + target._device_flags_file

    command += ['-c', f'{sub_command_str}']

  print("RunTestWithCoverage:%s" % (command))
  subprocess.run(command, cwd=SRC_DIR)


def RunTestForOpenharmony(test_suite_name: str, gtest_filter: str,
                      test_filter_file: str, device_flags_file: str,
                      output_dir: str, log_dir: str, ip_port: str):
  command = [f'./run_{test_suite_name}']
  if gtest_filter != "":
    command += [f'--gtest_filter={gtest_filter}']
  if test_filter_file != "" and os.path.exists(test_filter_file):
    command += [f'--test-launcher-filter-file={test_filter_file}']
  if device_flags_file != "" and os.path.exists(device_flags_file):
    command += [f'--device-flags-file={device_flags_file}']
  command += ['--fast-local-dev']
  if ip_port is not None:
    command += [f'--ip-port={ip_port}']
  print(f"=============================================================for test---{test_suite_name}\n")
  print("RunTestForOpenharmony(%s):%s" % (test_suite_name, command))
  run_test_script_dir = os.path.join(SRC_DIR, output_dir, "bin")
  log_file = os.path.join(log_dir, test_suite_name + '.txt')
  with codecs.open(log_file, 'w', encoding='utf-8') as f:
    # Use subprocess.PIPE to print and write the logfile at the same time.
    process = subprocess.Popen(command,
                               stdout=subprocess.PIPE,
                               text=True,
                               cwd=run_test_script_dir)
    for line in process.stdout:
      print(line, end='')
      f.write(line)

    # Wait for the subprocess finished
    return_code = process.wait()
    if return_code != 0:
      PrintRed(test_suite_name + ' failed, return ' + str(return_code))
      if EXIT_IF_FAILED:
        PrintRed('Test exits because the ' + test_suite_name + ' failed.')
        sys.exit(1)

  # test_suite_result_dir = GetLatestFolderWithPrefix(
  #     os.path.join(SRC_DIR, output_dir), "TEST_RESULTS_")
  # if test_suite_result_dir == None:
  #   PrintRed('Failed to find the TEST_RESULTS_* directory.')
  #   if EXIT_IF_FAILED:
  #     sys.exit(1)

  # shutil.copytree(os.path.join(SRC_DIR, output_dir, test_suite_result_dir),
  #                 os.path.join(log_dir, test_suite_name))


def CollectTestResultsForAndroid(test_targets: dict, log_dir: str):
  test_results = []

  accumulated_total = 0
  accumulated_failed = 0
  accumulated_time = 0
  max_suite_name_len = 0
  for test_suite_name, target in test_targets.items():
    report_file = os.path.join(log_dir, test_suite_name,
                               'test_results_presentation',
                               'test_results_presentation.html')
    print("report_file:", report_file)
    with codecs.open(report_file, 'r', encoding='utf-8') as f:
      content = f.read()

      passed_cases = str(0)
      passed_match = re.search(_EXTRACT_ANDROID_TEST_RESULT_PASSED_REGEX,
                               content)
      if passed_match:
        passed_cases = passed_match.group('passed_cases')

      failed_cases = str(0)
      failed_match = re.search(_EXTRACT_ANDROID_TEST_RESULT_FAILED_REGEX,
                               content)
      if failed_match:
        failed_cases = failed_match.group('failed_cases')

      total_cases = str(0)
      total_match = re.search(_EXTRACT_ANDROID_TEST_RESULT_TOTAL_REGEX, content)
      if total_match:
        total_cases = total_match.group('total_cases')

      total_time = str(0)
      total_time_match = re.search(_EXTRACT_ANDROID_TEST_RESULT_TIME_REGEX,
                                   content)
      if total_time_match:
        total_time = total_time_match.group('total_time')
        # Unit: ms to s
        total_time = str(round(int(total_time) / 1000))

      test_results.append(
          TestResult(test_suite_name, failed_cases, total_cases, total_time))
      accumulated_total += int(total_cases)
      accumulated_failed += int(failed_cases)
      accumulated_time += int(total_time)
      max_suite_name_len = max(max_suite_name_len, len(test_suite_name))

  assert len(test_results) > 0, 'Error! No test results finded.'
  assert accumulated_total >= accumulated_failed, 'Error! Bad Boy...'
  assert max_suite_name_len > 0, 'Error! Invalid test suite name length.'

  print("CollectTestResultsForAndroid finished.")
  return (test_results, accumulated_total, accumulated_failed, accumulated_time,
          max_suite_name_len)

def CollectTestResultsForOpenHarmony(test_targets: dict, log_dir: str):
  RE_TOTAL_CASES = re.compile(r'\[(\d+)\/(\d+)\]')
  RE_TOTAL_TIME = re.compile(r'Tests took (\d+) seconds')
  RE_FAILED_CASES = re.compile(r'(?P<failed_cases>[0-9]+) tests? failed:')
  RE_CRASHES_CASES = re.compile(r'(?P<crashed_cases>[0-9]+) tests? crashed:')
  RE_TIMEOUT_CASES = re.compile(r'(?P<timed_out_cases>[0-9]+) tests? timed out:')
  test_results = []

  accumulated_total = 0
  accumulated_failed = 0
  accumulated_time = 0
  max_suite_name_len = 0
  for test_suite_name, target in test_targets.items():
    log_file = os.path.join(log_dir, test_suite_name + '.txt')
    with codecs.open(log_file, 'r', encoding='utf-8') as f:
      content = f.read()
      total_cases_res = re.search(RE_TOTAL_CASES, content)
      if total_cases_res:
        total_cases = total_cases_res.group(2)
        total_time_match = re.search(RE_TOTAL_TIME, content)
        if total_time_match:
            total_time = total_time_match.group(1)

        failed_cases = str(0)
        failed_match = re.search(RE_FAILED_CASES,
                                  content)
        if failed_match:
            failed_cases = failed_match.group('failed_cases')

        crashed_cases = str(0)
        crashed_match = re.search(RE_CRASHES_CASES,
                                  content)
        if crashed_match:
            crashed_cases = crashed_match.group('crashed_cases')

        timed_out_cases = str(0)
        timed_out_match = re.search(RE_TIMEOUT_CASES,
                                    content)
        if timed_out_match:
            timed_out_cases = timed_out_match.group('timed_out_cases')

        test_results.append(
            TestResult(
                test_suite_name,
                str(
                    int(failed_cases) + int(crashed_cases) +
                    int(timed_out_cases)), total_cases, total_time))
        accumulated_total += int(total_cases)
        accumulated_failed += int(failed_cases) + int(crashed_cases) +\
                              int(timed_out_cases)
        accumulated_time += int(total_time)
        max_suite_name_len = max(max_suite_name_len, len(test_suite_name))
      else:
        test_result2 = re.search(_EXTRACT_LINUX_TEST_RESULT2_REGEX, content)
        if test_result2:
          total_cases = test_result2.group('total_cases')
          total_time = test_result2.group('total_time')
          # Unit: ms to s
          total_time = str(round(int(total_time) / 1000))

          failed_cases = str(0)
          failed_match = re.search(_EXTRACT_LINUX_TEST_RESULT2_FAILED_REGEX,
                                   content)
          if failed_match:
            failed_cases = failed_match.group('failed_cases')

          test_results.append(
              TestResult(test_suite_name, failed_cases, total_cases,
                         total_time))
          accumulated_total += int(total_cases)
          accumulated_failed += int(failed_cases)
          accumulated_time += int(total_time)
          max_suite_name_len = max(max_suite_name_len, len(test_suite_name))

  assert len(test_results) > 0, 'Error! No test results finded.'
  assert accumulated_total >= accumulated_failed, 'Error! Bad Boy...'
  assert max_suite_name_len > 0, 'Error! Invalid test suite name length.'

  return (test_results, accumulated_total, accumulated_failed, accumulated_time,
          max_suite_name_len)

def CollectTestResultsForLinux(test_targets: dict, log_dir: str):
  print("=====================================CollectTestResultsForLinux")
  print(log_dir)
  print(test_targets)
  test_results = []

  accumulated_total = 0
  accumulated_failed = 0
  accumulated_time = 0
  max_suite_name_len = 0
  for test_suite_name, target in test_targets.items():
    print(test_suite_name)
    print(target)
    log_file = os.path.join(log_dir, test_suite_name + '.txt')
    with codecs.open(log_file, 'r', encoding='utf-8') as f:
      content = f.read()
      test_result1 = re.search(_EXTRACT_LINUX_TEST_RESULT1_REGEX, content)
      if test_result1:
        total_cases = test_result1.group('total_cases')
        total_time = test_result1.group('total_time')

        failed_cases = str(0)
        failed_match = re.search(_EXTRACT_LINUX_TEST_RESULT1_FAILED_REGEX,
                                 content)
        if failed_match:
          failed_cases = failed_match.group('failed_cases')

        crashed_cases = str(0)
        crashed_match = re.search(_EXTRACT_LINUX_TEST_RESULT1_CRASHED_REGEX,
                                  content)
        if crashed_match:
          crashed_cases = crashed_match.group('crashed_cases')

        timed_out_cases = str(0)
        timed_out_match = re.search(_EXTRACT_LINUX_TEST_RESULT1_TIMEOUT_REGEX,
                                    content)
        if timed_out_match:
          timed_out_cases = timed_out_match.group('timed_out_cases')

        test_results.append(
            TestResult(
                test_suite_name,
                str(
                    int(failed_cases) + int(crashed_cases) +
                    int(timed_out_cases)), total_cases, total_time))
        accumulated_total += int(total_cases)
        accumulated_failed += int(failed_cases) + int(crashed_cases) +\
                              int(timed_out_cases)
        accumulated_time += int(total_time)
        max_suite_name_len = max(max_suite_name_len, len(test_suite_name))
      else:
        test_result2 = re.search(_EXTRACT_LINUX_TEST_RESULT2_REGEX, content)
        if test_result2:
          total_cases = test_result2.group('total_cases')
          total_time = test_result2.group('total_time')
          # Unit: ms to s
          total_time = str(round(int(total_time) / 1000))

          failed_cases = str(0)
          failed_match = re.search(_EXTRACT_LINUX_TEST_RESULT2_FAILED_REGEX,
                                   content)
          if failed_match:
            failed_cases = failed_match.group('failed_cases')

          test_results.append(
              TestResult(test_suite_name, failed_cases, total_cases,
                         total_time))
          accumulated_total += int(total_cases)
          accumulated_failed += int(failed_cases)
          accumulated_time += int(total_time)
          max_suite_name_len = max(max_suite_name_len, len(test_suite_name))

  assert len(test_results) > 0, 'Error! No test results finded.'
  assert accumulated_total >= accumulated_failed, 'Error! Bad Boy...'
  assert max_suite_name_len > 0, 'Error! Invalid test suite name length.'

  return (test_results, accumulated_total, accumulated_failed, accumulated_time,
          max_suite_name_len)


def PrintSummary(test_results: list, accumulated_total: str,
                 accumulated_failed: str, accumulated_time: str,
                 max_suite_name_len: int, log_dir: str):
  max_field_len = max(max(test_results)._max_field_len, len('failed'))
  max_suite_name_len = max(max_suite_name_len, len('unit_test_name(s)'))

  # :> set the layout on the right
  _SUMMARY_FORAMT = "| {:>{field_len}} | {:>{field_len}} | {:>{field_len}} "\
                    "| {:>{field_len}} | {:>{suite_name_len}} |"

  summary_file = os.path.join(log_dir, 'summary.txt')
  with codecs.open(summary_file, 'w', encoding='utf-8') as summary:
    print('Summary:')

    title = _SUMMARY_FORAMT.format('pass',
                                   'failed',
                                   'total',
                                   'time',
                                   'unit_test_name(s)',
                                   field_len=max_field_len,
                                   suite_name_len=max_suite_name_len)
    print(title)
    summary.write(title + '\n')

    title_split = '-' * max_field_len
    title_suite_name_split = '-' * max_suite_name_len
    title_split_line = _SUMMARY_FORAMT.format(title_split,
                                              title_split,
                                              title_split,
                                              title_split,
                                              title_suite_name_split,
                                              field_len=max_field_len,
                                              suite_name_len=max_suite_name_len)
    print(title_split_line)
    summary.write(title_split_line + '\n')

    for result in test_results:
      line = _SUMMARY_FORAMT.format(result._passed_cases,
                                    result._failed_cases,
                                    result._total_cases,
                                    result._total_time,
                                    result._test_suite_name,
                                    field_len=max_field_len,
                                    suite_name_len=max_suite_name_len)
      print(line)
      summary.write(line + '\n')

    accumulated_line = _SUMMARY_FORAMT.format(str(accumulated_total -
                                                  accumulated_failed),
                                              accumulated_failed,
                                              accumulated_total,
                                              accumulated_time,
                                              'total',
                                              field_len=max_field_len,
                                              suite_name_len=max_suite_name_len)
    print(accumulated_line)
    summary.write(accumulated_line + '\n')

  print('You can find the test results in ' + log_dir)
  if accumulated_failed > 0:
    PrintRed('Failed ' + str(accumulated_failed) + ' tests.')
    global IS_FAILED
    IS_FAILED = True
    IS_FAILED = True
  else:
    PrintGreen('All tests passed.')


def WriteSummaryHtml(test_results: list, accumulated_total: str,
                     accumulated_failed: str, accumulated_time: str,
                     log_dir: str):
  summary_file = os.path.join(log_dir, 'summary.html')
  with codecs.open(summary_file, 'w', encoding='utf-8') as summary:
    html_header = '''
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <style>
        table {
            display: table;
            border-collapse: separate;
            box-sizing: border-box;
            text-indent: initial;
            border-spacing: 2px;
            border-color: grey;
        }
        table a {
            text-decoration: underline
        }
        table.reference {
            border-collapse: collapse;
            width: 100%;
            margin-bottom: 4px;
            margin-top: 4px;
            font-family:"Verdana";
        }
        table.reference tr:nth-child(odd) {
            background-color: #f6f4f0
        }

        table.reference tr:nth-child(even) {
            background-color: #fff
        }
        table.reference th {
            color: #292421;
            background-color: #d1ba74;
            border: 1px solid #555;
            font-size: 12px;
            padding: 3px;
            vertical-align: top;
            text-align: left
        }
        table.reference th a:link,table.reference th a:visited {
            color: #fff
        }
        table.reference th a:active,table.reference th a:hover {
            color: #ee872a
        }
        tr td:first-child {
            min-width: 25px
        }
        table.reference td {
            line-height: 2em;
            min-width: 24px;
            border: 1px solid #d4d4d4;
            padding: 5px;
            padding-top: 7px;
            padding-bottom: 7px;
            vertical-align: top
        }
    </style>
  </head>
  <body>'''
    summary.write(html_header + '\n')

    table_title = '''
    <table class="reference">
      <tbody>
        <tr>
          <th>test name</th>
          <th>number of success</th>
          <th>number of failed</th>
          <th>all tests</th>
          <th>elapsed time(s)</th>
        </tr>'''
    summary.write(table_title + '\n')

    _FAILED_BACKGROUND_COLOR = "#FE4365"
    _SUCCESS_BACKGROUND_COLOR = "#407434"
    accumulated_success = str(int(accumulated_total) - int(accumulated_failed))
    bg_color = _FAILED_BACKGROUND_COLOR if int(
        accumulated_failed) > 0 else _SUCCESS_BACKGROUND_COLOR
    total_summary = f'''
        <tr>
          <td>Total</td>
          <td>{accumulated_success}</td>
          <td style="background-color: {bg_color}">{accumulated_failed}</td>
          <td>{accumulated_total}</td>
          <td>{accumulated_time}</td>
        </tr>'''

    summary.write(total_summary + '\n')

    for result in test_results:
      bg_color = _FAILED_BACKGROUND_COLOR if int(
          result._failed_cases) > 0 else _SUCCESS_BACKGROUND_COLOR
      html_link = os.path.join(result._test_suite_name,
                               'test_results_presentation',
                               'test_results_presentation.html')
      # logcat_link = os.path.join(result._test_suite_name, 'logcat')
      # rawlog_link = os.path.join(result._test_suite_name, 'raw_logs')
      # logcat_link = rawlog_link if 'junit' in result._test_suite_name else logcat_link

      one_result = f'''
        <tr>
          <td>{result._test_suite_name}</td>
          <td>{result._passed_cases}</td>
          <td style="background-color: {bg_color}">{result._failed_cases}</td>
          <td>{result._total_cases}</td>
          <td>{result._total_time}</td>
        </tr>'''

      summary.write(one_result + '\n')

    html_end = '''
      </tbody>
    </table>
  </body>
</html>'''
    summary.write(html_end)
    print(summary_file + ' Generated.')


def PrintSummaryForAndroid(test_targets: dict, log_dir: str):
  (test_results, accumulated_total, accumulated_failed, accumulated_time,
   max_suite_name_len) = CollectTestResultsForAndroid(test_targets, log_dir)

  PrintSummary(test_results, accumulated_total, accumulated_failed,
               accumulated_time, max_suite_name_len, log_dir)
  WriteSummaryHtml(test_results, accumulated_total, accumulated_failed,
                   accumulated_time, log_dir)


def PrintSummaryForLinux(test_targets: dict, log_dir: str):
  (test_results, accumulated_total, accumulated_failed, accumulated_time,
   max_suite_name_len) = CollectTestResultsForLinux(test_targets, log_dir)

  PrintSummary(test_results, accumulated_total, accumulated_failed,
               accumulated_time, max_suite_name_len, log_dir)
  WriteSummaryHtml(test_results, accumulated_total, accumulated_failed,
                   accumulated_time, log_dir)

def PrintSummaryForOpenHarmony(test_targets: dict, log_dir: str):
  (test_results, accumulated_total, accumulated_failed, accumulated_time,
   max_suite_name_len) = CollectTestResultsForOpenHarmony(test_targets, log_dir)

  PrintSummary(test_results, accumulated_total, accumulated_failed,
               accumulated_time, max_suite_name_len, log_dir)
  WriteSummaryHtml(test_results, accumulated_total, accumulated_failed,
                   accumulated_time, log_dir) 


def SendEmail(log_dir: str, email_config: str):
  EMAIL_PY = os.path.join(os.path.realpath(os.path.dirname(__file__)),
                          'send_email.py')
  command = [
      sys.executable, f'{EMAIL_PY}', '--result_dir', f'{log_dir}', '--config',
      f'{email_config}'
  ]
  print(command)
  process = subprocess.Popen(command, cwd=SRC_DIR)

  # Wait until command finished.
  process.communicate()
  return_code = process.returncode
  if return_code == 0:
    print("SendEmail finished.")
  else:
    print("SendEmail failed with return_code: %d" % return_code)
    sys.exit(1)


def RunAllTests(test_targets: dict, output_dir: str, enable_coverage: bool,
                test_for_openharmony: bool, ip_port: str, email_config: str):
  if enable_coverage:
    RunTestWithCoverage(test_targets, output_dir)
    return

  log_dir = os.path.join(SRC_DIR, output_dir, UT_REPORT_DIR, TIMESTAMP)
  if not os.path.exists(log_dir):
    print("Create directory: %s" % log_dir)
    os.makedirs(log_dir)

  for test_suite_name, target in test_targets.items():
    test_filter_file = os.path.join(SRC_DIR, 'ut_tools',target._test_filter_path)
    # print('filter directory: %s' % test_filter_file)
    if not os.path.isfile(test_filter_file) or not os.path.exists(
        test_filter_file):
      test_filter_file = ""
    device_flags_file = os.path.join(SRC_DIR, 'ut_tools',target._device_flags_file)
    # print('device flag directory: %s' % device_flags_file)
    if not os.path.isfile(device_flags_file) or not os.path.exists(
        device_flags_file):
      device_flags_file = ""

    if test_for_openharmony:
      RunTestForOpenharmony(test_suite_name, target._gtest_filter, test_filter_file,
                        device_flags_file, output_dir, log_dir, ip_port)
    else:
      RunTestForLinux(test_suite_name, target._gtest_filter, test_filter_file,
                      device_flags_file, output_dir, log_dir)

  if test_for_openharmony:
    PrintSummaryForOpenHarmony(test_targets, log_dir)
  else:
    PrintSummaryForLinux(test_targets, log_dir)

  if os.path.isfile(email_config):
    SendEmail(log_dir, email_config)

  if IS_FAILED:
    sys.exit(1)


def EnsureUtReportDirExist(output_dir: str):
  ut_report_dir = os.path.join(SRC_DIR, output_dir, UT_REPORT_DIR)
  if (not os.path.exists(ut_report_dir)):
    os.mkdir(ut_report_dir)
  return ut_report_dir


def EnsureCoverageResultsDirExist(output_dir: str):
  EnsureUtReportDirExist(output_dir)
  coverage_dir = os.path.join(SRC_DIR, output_dir, UT_REPORT_DIR, COVERAGE_DIR)
  if (not os.path.exists(coverage_dir)):
    os.mkdir(coverage_dir)
  return


def BuildArgumentParser():
  parser = argparse.ArgumentParser(
      description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument('--mode',
                      choices=RunningMode,
                      dest='mode',
                      help='Specifies the mode in which ut runs.',
                      required=True,
                      type=RunningMode)
  parser.add_argument('--ip-port',
                      dest='ip_port',
                      default = None,
                      help='Specify the ip:port for the remote connected device.')
  parser.add_argument('--out',
                      dest='output_dir',
                      help='The path to the build directory, '
                      'eg. --out out/default',
                      default="",
                      required=True,
                      type=os.path.normpath)
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
                      type=os.path.realpath)
  parser.add_argument(
      '--email',
      dest='email_config',
      help='A json configure file like:'
      '{\n'
      '  "server_base_url":"http://xx.xxx.xxx.xxx:8080/results",\n'
      '  "server_base_path":"/usr1/ut_reports/results",\n'
      '  "summary_file":"summary.html",\n'
      '  "subject":"[UT Results Notification]",\n'
      '  "sender":"*****@***.com",\n'
      '  "receivers":"abcd@***.com,efg@***.com,hij@***.com",\n'
      '  "auth_username":"xxxxxxxx",\n'
      '  "auth_password":"********",\n'
      '  "smtp_server":"smtp.huawei.com",\n'
      '  "smtp_server_port":"587"\n'
      '}',
      default="",
      type=os.path.realpath)
  parser.add_argument('--coverage',
                      action='store_true',
                      dest='enable_coverage',
                      help='Generate coverage report or not')
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

  # Those arguments are supported from build/ohos/test_runner.py
  parser.add_argument('--test-suite',
                      dest='test_suite',
                      help='test suite name to run, '
                      'eg. android_webview_unittests',
                      default="")
  parser.add_argument('--test-filter',
                      dest='test_filter',
                      help='googletest-style filter string, '
                      'eg. PasswordAutofillAgentTest.SuggestLatestCredentials',
                      default="")
  parser.add_argument('--test-launcher-filter-file',
                      dest='test_filter_file',
                      help='Path to file that contains googletest-style filter'
                      ' strings.'
                      'See also //testing/buildbot/filters/README.md.',
                      default="",
                      type=os.path.realpath)
  parser.add_argument('--device-flags-file',
                      dest='device_flags_file',
                      help='The relative filepath to a file containing '
                      'command-line flags to set on the device',
                      default="")
  return parser


def main():
  parser = BuildArgumentParser()
  args = parser.parse_args()
  mode = args.mode
  config = args.config
  output_dir = args.output_dir
  test_suite = args.test_suite
  test_filter = args.test_filter
  test_filter_file = args.test_filter_file
  device_flags_file = args.device_flags_file
  test_for_openharmony = args.test_for_openharmony
  ip_port = args.ip_port
  email_config = args.email_config
  global EXIT_IF_FAILED
  if args.continue_if_failed:
    EXIT_IF_FAILED = False

  assert output_dir.startswith("out"), \
      "Error! --out need a standard build directory which start with 'out'."
  assert os.path.exists(os.path.join(SRC_DIR, output_dir)) and \
      os.path.isdir(os.path.join(SRC_DIR, output_dir)), \
      "Error! invalid --out value:" + output_dir
  if not os.path.exists(config) or not os.path.isfile(config):
    config = ""
  if not os.path.isfile(test_filter_file) or not os.path.exists(
      test_filter_file):
    test_filter_file = ""
  if not os.path.isfile(device_flags_file) or not os.path.exists(
      device_flags_file):
    device_flags_file = ""

  EnsureUtReportDirExist(output_dir)

  if mode == RunningMode.all or mode == RunningMode.changes:
    assert config != "" and os.path.exists(config) and os.path.isfile(config), \
        "Error! " + args.config + " dose not exist or is not a valid file."
    assert test_suite == "" and test_filter == "" and test_filter_file == "" \
        and device_flags_file == "", \
        "Error! mode{all, changes} should only work with --config, " \
        "--test_suite,--test-filter,--test-launcher-filter-file,"\
        "--device-flags-file can only worked with mode{specific}."
  elif mode == RunningMode.specific:
    assert config == "", \
        "Error! mode{specific} not support running with a config profile."
    assert not args.enable_coverage, \
        "Error! --coverage is supported only in mode{all, changes}."

  begin_time = datetime.now()
  print("Begin Time:%s" % begin_time)

  # Find test targets by mode
  if mode == RunningMode.all or mode == RunningMode.changes:
    test_targets = {}
    print("============================= print args.config ")
    print(args.config)
    test_targets = GetAllTestTargetsFromConfig(args.config)

    # Calculating the target affected by locally modified files
    if mode == RunningMode.changes:
      changed_file_list = GetLocalChangedFileList()
      test_targets = GetTestTargetsByChangedList(test_targets,
                                                 changed_file_list, output_dir)
    print("--------------------------------------------------")
    print("Prepare to run those test suites :")
    for test_suite_name, v in test_targets.items():
      print(test_suite_name)
    print("--------------------------------------------------")

    # Running tests
    RunAllTests(test_targets, output_dir, args.enable_coverage,
                test_for_openharmony, ip_port, email_config)
  elif mode == RunningMode.specific:
    # Get the test suite from the command line
    print("--------------------------------------------------")
    print("Prepare to run test suites :%s" % test_suite)
    print("--------------------------------------------------")
    RunSpecificTest(test_suite, test_filter, test_filter_file,
                    device_flags_file, output_dir, ip_port)

  end_time = datetime.now()
  print("End Time:%s, total duration:%ss." % (end_time, end_time - begin_time))

  return 0


if '__main__' == __name__:
  sys.exit(main())
