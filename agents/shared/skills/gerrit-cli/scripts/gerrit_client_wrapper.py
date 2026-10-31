#!/usr/bin/env vpython3
# Copyright 2026 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Helper wrapper script to run gerrit_client.py from PATH."""

import os
from pathlib import Path
import shutil
import subprocess
import sys


def _find_on_path(name):
    path = shutil.which(name)
    if path:
        return path

    # Python scripts are not executable extensions on Windows by default, but
    # vpython3 can still execute a script found in a PATH directory.
    for directory in os.environ.get('PATH', '').split(os.pathsep):
        directory = directory.strip('"')
        if not directory:
            continue
        candidate = Path(directory) / name
        if candidate.is_file():
            return str(candidate)
    return None


def _find_vpython(client):
    if sys.platform != 'win32':
        return 'vpython3'

    # vpython3.bat runs through cmd.exe, which can reinterpret Gerrit query
    # characters such as '>' as shell operators. Call the executable directly.
    depot_tools_dir = Path(client).resolve().parent
    return str(depot_tools_dir / '.cipd_bin' / 'vpython3.exe')


def main():
    client = _find_on_path('gerrit_client.py')
    if not client:
        print(
            'gerrit_client.py not found. '
            'Is depot_tools available and added to PATH?'
        )
        return 1

    vpython = _find_vpython(client)
    if not Path(vpython).is_file() and sys.platform == 'win32':
        print(
            'vpython3 executable not found. '
            'Is depot_tools available and added to PATH?'
        )
        return 1

    return subprocess.call([vpython, client] + sys.argv[1:])


if __name__ == '__main__':
    sys.exit(main())
