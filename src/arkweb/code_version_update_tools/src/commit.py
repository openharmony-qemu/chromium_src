#!/usr/bin/env python3
# -*- coding: utf-8 -*-

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

import os
import re
import sys
import pickle
import datetime
import shutil
import copy

import util
import config
from itertools import islice

from openpyxl import Workbook
from openpyxl.styles import Font, Alignment, colors
from openpyxl.utils import get_column_letter

font_link = Font(color="0000FF", underline="single")  # 蓝色文字，单线下划线


def get_folder_patched_map():
    dump_filename = os.path.join(config.root, 'result', f"folder_patched_map_{pkl_suffix}.pkl")
    with open(dump_filename, "rb") as f:
        folder_differ_map = pickle.load(f)
        return folder_differ_map
    return {}


def do_commit(folder_differ_map, work_path, branch):
    for (dir_name, file_differ_list) in folder_differ_map.items():
        if dir_name == ".gitkeep":
            continue
        
        # 按仓库路径（cwd）分组文件
        cwd_files = {}
        for file_differ_obj in file_differ_list:
            if file_differ_obj.is_binary or file_differ_obj.is_ignored_file:
                continue
            if not file_differ_obj.merge_status:
                continue
            
            # 计算仓库路径（cwd）和文件相对路径
            repo_path = os.path.join(work_path, file_differ_obj.project_name)
            file_relpath = os.path.join(file_differ_obj.relative_filename)
            if file_relpath.startswith('/'):
                file_relpath = '.' + file_relpath
            
            # 按cwd分组存储文件路径
            if repo_path not in cwd_files:
                cwd_files[repo_path] = []
            cwd_files[repo_path].append(file_relpath)
        
        # 对每个仓库批量添加文件（每100个一批）
        for cwd, files in cwd_files.items():
            files_iter = iter(files)
            commit_part_cnt = 1
            while batch := list(islice(files_iter, 100)):  # Python 3.8+ 海象运算符
                batch = [i for i in batch if os.path.exists(os.path.join(cwd, i))]
                cmd = f"git -C {cwd} add -f {' '.join(batch)}"
                util.exec_and_stderr_stdout(cmd, is_print=True)
                msg = f'commit {dir_name}_{commit_part_cnt}'
                cmd = f"cd {cwd} && git commit -m '{msg}'"
                util.exec_and_stderr_stdout(cmd, is_print=True)
                commit_part_cnt = commit_part_cnt + 1

                git_path = os.path.join(config.root, 'xxx')
                cmd = f"cd {cwd} && {git_path} xxx"
                stderr, result = util.exec_and_stderr_stdout(cmd, is_print=True)

                util.ERROR(stderr)
                util.INFO(result)

                if len(result.splitlines()) > 7:
                    pattern_url = r'http[s]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\\(\\),]|(?:%[0-9a-fA-F][0-9a-fA-F]))+/[0-9]+'
                    pattern_commitid = r'([0-9a-fA-F]{40})\s+->'
                    re_result = re.findall(pattern_url, result)
                    if len(re_result) < 1:
                        continue
                    mr_url = re_result[0]
                    mr_commitid = re.findall(pattern_commitid, result)[0]
                    for file_differ_obj in file_differ_list:
                        file_relpath = os.path.join(file_differ_obj.relative_filename)
                        if file_relpath.startswith('/'):
                            file_relpath = '.' + file_relpath
                        if file_relpath not in batch:
                            continue
                        url_filePath = os.path.normpath(f'{work_path_perfix}/{file_differ_obj.project_name + "/" + file_differ_obj.relative_filename}')
                        url = f'{mr_url}/diffs?commit_id={mr_commitid}&filePath={url_filePath}'
                        file_differ_obj.set_link(url)
        rej_folder_patched_map = folder_differ_map


def do_rej_commit(folder_patched_map, work_path):
    for (dir_name, file_differ_list) in folder_patched_map.items():
        if dir_name == ".gitkeep":
            continue
        
        path2obj_dict = {}

        cwd_files = {}
        for file_differ_obj in file_differ_list:
            if file_differ_obj.is_binary or file_differ_obj.is_ignored_file:
                continue
            link = file_differ_obj.link
            if link:
                continue
            
            # 计算仓库路径（cwd）和文件相对路径
            repo_path = os.path.join(work_path, file_differ_obj.project_name)
            file_relpath = os.path.join(file_differ_obj.relative_filename)
            if file_relpath.startswith('/'):
                file_relpath = '.' + file_relpath
            path2obj_dict[file_relpath] = file_differ_obj
            file_relpath = file_relpath + ".rej"

            # 按cwd分组存储文件路径
            if repo_path not in cwd_files:
                cwd_files[repo_path] = []
            cwd_files[repo_path].append(file_relpath)
        
        # 对每个仓库批量添加文件（每100个一批）
        for cwd, files in cwd_files.items():
            files_iter = iter(files)
            commit_part_cnt = 1
            while rej_batch := list(islice(files_iter, 50)):  # Python 3.8+ 海象运算符
                rej_batch_rej = copy.deepcopy(rej_batch)
                for rej_item_index, rej_item in enumerate(rej_batch_rej):
                    if not os.path.exists(os.path.join(cwd, rej_item)):
                        rej_batch_rej[rej_item_index] = rej_item.replace('.rej', '.diff')
                        file_relpath_item = rej_item.replace('.rej', '')
                        try:
                            shutil.copy(path2obj_dict[file_relpath_item].save_filename, os.path.join(cwd, rej_batch_rej[rej_item_index]))
                        except KeyError:
                            util.ERROR(f'KeyError in copy: {path2obj_dict[file_relpath_item].save_filename}')
                            continue
                        except OSError:
                            util.ERROR(f'OSError in copy: {path2obj_dict[file_relpath_item].save_filename}')
                            continue
                cmd = f"git -C {cwd} add -f {' '.join(rej_batch_rej)}"
                util.exec_and_stderr_stdout(cmd, is_print=True)

                rej_batch_origin = [i.replace('.rej', '') for i in rej_batch]
                rej_batch_origin = [i for i in rej_batch_origin if os.path.exists(os.path.join(cwd, i))]
                cmd = f"git -C {cwd} add -f {' '.join(rej_batch_origin)}"
                util.exec_and_stderr_stdout(cmd, is_print=True)
                
                msg = f'commit {dir_name}_{commit_part_cnt}'
                cmd = f"cd {cwd} && git commit -m '{msg}'"
                util.exec_and_stderr_stdout(cmd, is_print=True)
                commit_part_cnt = commit_part_cnt + 1

                git_path = os.path.join(config.root, 'xxx')
                cmd = f"cd {cwd} && {git_path} xxx"
                stderr, result = util.exec_and_stderr_stdout(cmd, is_print=True)

                util.ERROR(stderr)
                util.INFO(result)

                if len(result.splitlines()) > 7:
                    pattern_url = r'http[s]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\\(\\),]|(?:%[0-9a-fA-F][0-9a-fA-F]))+/[0-9]+'
                    pattern_commitid = r'([0-9a-fA-F]{40})\s+->'
                    re_result = re.findall(pattern_url, result)
                    if len(re_result) < 1:
                        continue
                    mr_url = re_result[0]
                    mr_commitid = re.findall(pattern_commitid, result)[0]
                    for file_differ_obj in file_differ_list:
                        file_relpath = os.path.join(file_differ_obj.relative_filename)
                        if file_relpath.startswith('/'):
                            file_relpath = '.' + file_relpath
                        if file_relpath not in rej_batch_origin:
                            continue
                        url_filePath = os.path.normpath(f'{work_path_perfix}/{file_differ_obj.project_name + "/" + file_differ_obj.relative_filename}')
                        rej_url = f'{mr_url}/diffs?commit_id={mr_commitid}&filePath={url_filePath}'
                        file_differ_obj.set_rej_link(rej_url)
                    for rej_file_differ_obj in file_differ_list:
                        rej_file_relpath = os.path.join(rej_file_differ_obj.relative_filename)
                        if rej_file_relpath.startswith('/'):
                            rej_file_relpath = '.' + rej_file_relpath
                        rej_file_relpath = rej_file_relpath + ".rej"
                        if rej_file_relpath not in rej_batch_rej and rej_file_relpath.replace('.rej', '.diff') not in rej_batch_rej:
                            continue
                        if rej_file_relpath in rej_batch_rej:
                            rej_file_name = rej_file_differ_obj.project_name + "/" + rej_file_differ_obj.relative_filename + ".rej"
                        elif rej_file_relpath.replace('.rej', '.diff') in rej_batch_rej:
                            rej_file_name = rej_file_differ_obj.project_name + "/" + rej_file_differ_obj.relative_filename + ".diff"
                        url_filePath = os.path.normpath(f'{work_path_perfix}/{rej_file_name}')
                        rej_url = f'{mr_url}/diffs?commit_id={mr_commitid}&filePath={url_filePath}'
                        rej_file_differ_obj.set_rej_link(rej_url)
                        rej_link_map[os.path.normpath("src/" + rej_file_name)] = rej_url


def export_all_diff(folder_differ_map_source_right, folder_differ_map_target):
    # 创建 Workbook
    wb = Workbook()
    ws = wb.active
    # 写入标题
    headers = ["文件名", "总行数", "失败行数", "总Hunk数", "失败Hunk数", "是否成功应用", "类型", "链接", "rej链接"]
    ws.append(headers)

    # 设置标题样式
    for col in range(1, len(headers) + 1):
        cell = ws.cell(row=1, column=col)
        cell.font = Font(bold=True)
        cell.alignment = Alignment(horizontal="center")
    
    # tmp map for filename to file_diff_obj
    tmp_dict = {}
    for (dir_name, file_differ_list) in folder_differ_map_source_right.items():
        for i, file_differ_obj in enumerate(file_differ_list):
            filename = "src/" + file_differ_obj.project_name + "/" + file_differ_obj.relative_filename
            filename = filename.replace("//", "/")
            tmp_dict[filename] = file_differ_obj
    
    for (dir_name, file_differ_list) in folder_differ_map_target.items():
        for i, file_differ_obj in enumerate(file_differ_list):
            filename = "src/" + file_differ_obj.project_name + "/" + file_differ_obj.relative_filename
            filename = filename.replace("//", "/")
            merge_status = file_differ_obj.merge_status
            link = tmp_dict[filename].link
            status = file_differ_obj.status
            if file_differ_obj.is_ignored_file:
                status = "[IGNORED]ignored"
            rej_file = os.path.normpath(f"{filename}.rej")
            rej_link = rej_link_map.get(rej_file, "")

            if not rej_link:
                rej_link = rej_link_map.get(rej_file.replace('.rej', '.diff'), "")
            
            ws.append([os.path.normpath(filename), int(file_differ_obj.diff_line), int(file_differ_obj.merge_faild_line), int(file_differ_obj.count_total_hunk), int(file_differ_obj.count_fail_hunk), merge_status, status])
            row_link_cnt = ws.max_row # 直接获取当前最大行号

            if link: # 设置超链接样式
                linkcell = ws.cell(row=row_link_cnt, column=8, value=link)
                linkcell.hyperlink = link
                linkcell.font = font_link
            else:
                util.WARNING(f"link null, status:{status}, file:{filename}")
            
            if rej_link: # 设置超链接样式
                print(rej_file)
                print(rej_link)
                rejlinkcell = ws.cell(row=row_link_cnt, column=9, value=rej_link)
                rejlinkcell.hyperlink = rej_link
                rejlinkcell.font = font_link

    current_time = datetime.datetime.now()
    formatted_time = current_time.strftime("%Y-%m-%d-%H-%M-%S")
    wb.save(f"result/mrcommit_report_{formatted_time}.xlsx")


def main(work_path, branch):
    util.INFO("===========commit running=============")
    patched_map = get_folder_patched_map()
    do_commit(patched_map, work_path, branch)
    do_rej_commit(patched_map, work_path)
    util.INFO("-----------------------------------")
    util.INFO("")
    return patched_map


if __name__ == '__main__':
    # 确认是否先执行过apply patch
    assert util.run_one_cmd(f'cd {config.source_right_for_apply_path} && git status --short')[0] != '', \
        '检测到未执行apply patch或已经执行过了commit，再次执行commit.py前请再次执行一遍applypatch.py'
    assert util.run_one_cmd(f'cd {config.target_path} && git status --short')[0] != '', \
        '检测到未执行apply patch或已经执行过了commit，再次执行commit.py前请再次执行一遍applypatch.py'
    
    # *******************************************************************
    # source_right和target的顺序不能修改，影响rej_link_map的执行结果
    # *******************************************************************
    # source_right
    type_source_right, branch_source_right, url_source_right = config.remote_source_right_path
    work_path_source_right = os.path.join(config.source_right_for_apply_path, 'src')
    pkl_suffix = 'source_right_for_apply'
    work_path_perfix = 'src'
    rej_link_map = {}
    patched_map_source_right = main(work_path_source_right, branch_source_right)

    # target
    type_target, branch_target, url_target = config.remote_target_path
    work_path_target = os.path.join(config.target_path, 'chromium', 'src')
    pkl_suffix = 'target'
    work_path_perfix = os.path.join('chromium', 'src')
    rej_link_map = {}
    patched_map_target = main(work_path_target, branch_target)

    export_all_diff(patched_map_source_right, patched_map_target)
