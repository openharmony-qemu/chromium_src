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

import subprocess
import argparse
import sys
import os
import time
import util
import config


def check_repo_installed():
    try:
        subprocess.run(["repo", "--version"], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False


# 如果repo init执行失败，请配置http代理
def repo_init(manifest_url, branch, workspace, model = "default.xml"):
    util.INFO(f"Initializing repo with {manifest_url} [{branch}] [{model}]")
    try:
        cmd = ["repo", "init", "-u", manifest_url, "-b", branch, "-m", model, "--repo-branch=stable_py3"]
        print(cmd)
        subprocess.run(
            cmd,
            cwd=workspace,
            check=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT
        )
    except subprocess.CalledProcessError as e:
        util.ERROR(f"Repo init failed: {e.output}")
        sys.exit(1)


def repo_sync(workspace, jobs=4, retries=3):
    for attempt in range(retries):
        util.INFO(f"\nStarting repo sync (attempt {attempt+1}/{retries})...")
        assert util.run_one_cmd(f'cd {workspace} && repo forall -c "git reset --hard; git clean -fxd"')[2] == 0
        try:
            process = subprocess.Popen(
                ["repo", "sync", "-d", "-j", str(jobs), "--no-tags"], # , "--force-sync"
                cwd=workspace,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT
            )

            # Real-time output processing
            while True:
                output = process.stdout.readline()
                if output == '' and process.poll() is not None:
                    break
                if output:
                    util.INFO(output.strip())
            
            if process.returncode != 0:
                raise subprocess.CalledProcessError(process.returncode, "repo sync")
            
            util.INFO("Sync completed successfully")
            return
        except subprocess.CalledProcessError as e:
            util.ERROR(f"\nSync failed with error code {e.returncode}")
            if attempt < retries - 1:
                util.ERROR("Retrying after 30 seconds...")
                time.sleep(30)
            else:
                util.ERROR("Maximum retries reached. Exiting.")
                sys.exit(1)


def auto_sync_repo(workspace, branch, manifest, model, jobs=4):
    # Create workspace if not exists
    os.makedirs(workspace, exist_ok=True)

    # Initialize repo
    if not os.path.exists(os.path.join(workspace, ".repo")):
        repo_init(manifest, branch, workspace, model)
    else:
        util.INFO("Existing repo workspace detected")
    
    # Start sync
    repo_sync(workspace, jobs=jobs)


def git_init(url, branch, workspace):
    try:
        subprocess.run(
            ["git", "clone", url, "-b", branch, workspace],
            check=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT
        )
    except subprocess.CalledProcessError as e:
        util.ERROR(f"git init failed: {e.output}")
        sys.exit(1)


def git_sync(workspace, branch, retries=3):
    # reset and clean
    cmd = f"cd {workspace} && git reset --hard && git clean -fxd && git checkout {branch}"
    stderr, result = util.exec_and_stderr_stdout(cmd, True)
    util.ERROR(stderr)

    for attempt in range(retries):
        try:
            process = subprocess.run(
                ["git", "pull"],
                cwd=workspace,
                text=True,
            )
            util.INFO("Sync completed successfully")
            return
        except subprocess.CalledProcessError as e:
            util.ERROR(f"\nSync failed with error code {e.returncode}")
            if attempt < retries - 1:
                util.ERROR("Retrying after 30 seconds...")
                time.sleep(30)
            else:
                util.ERROR("Maximum retries reached. Exiting.")
                sys.exit(1)


def auto_sync_git(workspace, branch, url):
    # Create workspace if not exists
    os.makedirs(workspace, exist_ok=True)

    # Initialize repo
    if not os.path.exists(os.path.join(workspace, ".git")):
        git_init(url, branch, workspace)
    else:
        util.INFO("Existing git workspace detected")
    
    # Start sync
    git_sync(workspace, branch)


def parse_and_sync(code_path, code_config):
    try:
        repository_type, branch, url, model = code_config
        auto_sync_repo(code_path, branch, url, model)
    except ValueError:
        repository_type, branch, url = code_config
        auto_sync_git(code_path, branch, url)
    util.INFO(f"同步{code_path.split('/')[-1]}成功")


def sync_local_data():
    util.INFO("开始下载代码数据, 时间较长，请稍等...")
    parse_and_sync(config.source_left_path, config.remote_source_left_path)
    parse_and_sync(config.source_right_path, config.remote_source_right_path)
    parse_and_sync(config.source_right_for_apply_path, config.remote_source_right_path)
    parse_and_sync(config.target_path, config.remote_target_path)


def download_local_data():
    sync_local_data()
    return True


if __name__ == '__main__':
    download_local_data()