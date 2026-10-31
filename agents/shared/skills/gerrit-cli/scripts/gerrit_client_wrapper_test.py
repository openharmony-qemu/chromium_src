#!/usr/bin/env vpython3
# Copyright 2026 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Unit tests for gerrit_client_wrapper.py."""
# pylint: disable=protected-access

import contextlib
import io
import os
from pathlib import Path
import unittest
from unittest import mock

from pyfakefs import fake_filesystem_unittest

import gerrit_client_wrapper


class GerritClientWrapperTest(fake_filesystem_unittest.TestCase):
    def setUp(self):
        self.setUpPyfakefs()
        self.which_patcher = mock.patch(
            'gerrit_client_wrapper.shutil.which', return_value=None
        )
        self.which_patcher.start()
        self.addCleanup(self.which_patcher.stop)
        self.call_patcher = mock.patch(
            'gerrit_client_wrapper.subprocess.call', return_value=0
        )
        self.call_mock = self.call_patcher.start()
        self.addCleanup(self.call_patcher.stop)

    def test_find_on_path_finds_python_script(self):
        directory = Path.cwd() / 'depot_tools'
        client = directory / 'gerrit_client.py'
        self.fs.create_file(client)

        with mock.patch.dict(os.environ, {'PATH': str(directory)}):
            self.assertEqual(
                str(client),
                gerrit_client_wrapper._find_on_path('gerrit_client.py'),
            )

    def test_main_uses_resolved_client_and_vpython_on_posix(self):
        client = Path.cwd() / 'depot_tools' / 'gerrit_client.py'
        self.fs.create_file(client)

        with (
            mock.patch.dict(os.environ, {'PATH': str(client.parent)}),
            mock.patch('gerrit_client_wrapper.sys.platform', 'linux'),
            mock.patch('sys.argv', ['wrapper.py', 'changes', '--help']),
        ):
            self.assertEqual(0, gerrit_client_wrapper.main())

        self.call_mock.assert_called_once_with(
            [
                'vpython3',
                str(client),
                'changes',
                '--help',
            ]
        )

    def test_main_bypasses_batch_launcher_on_windows(self):
        depot_tools = Path.cwd() / 'depot_tools'
        client = depot_tools / 'gerrit_client.py'
        executable = depot_tools / '.cipd_bin' / 'vpython3.exe'
        self.fs.create_file(client)
        self.fs.create_file(executable)
        arguments = ['changes', '--query', 'status:open & whoami']

        with (
            mock.patch.dict(os.environ, {'PATH': str(depot_tools)}),
            mock.patch('gerrit_client_wrapper.sys.platform', 'win32'),
            mock.patch('sys.argv', ['wrapper.py'] + arguments),
        ):
            self.assertEqual(0, gerrit_client_wrapper.main())

        self.call_mock.assert_called_once_with(
            [str(executable), str(client)] + arguments
        )

    def test_main_does_not_fall_back_to_batch_launcher(self):
        depot_tools = Path.cwd() / 'depot_tools'
        self.fs.create_file(depot_tools / 'gerrit_client.py')
        stdout = io.StringIO()

        with (
            mock.patch.dict(os.environ, {'PATH': str(depot_tools)}),
            mock.patch('gerrit_client_wrapper.sys.platform', 'win32'),
            contextlib.redirect_stdout(stdout),
        ):
            self.assertEqual(1, gerrit_client_wrapper.main())

        self.call_mock.assert_not_called()
        self.assertEqual(
            'vpython3 executable not found. '
            'Is depot_tools available and added to PATH?\n',
            stdout.getvalue(),
        )


if __name__ == '__main__':
    unittest.main()
