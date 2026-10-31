#!/usr/bin/env vpython3
# Copyright 2026 The Chromium Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Tests for history.py."""

# pylint: disable=protected-access

import datetime
from pathlib import Path
import unittest
from unittest import mock

from pyfakefs import fake_filesystem_unittest

import history


class HistoryTest(fake_filesystem_unittest.TestCase):
    """Tests CL description history updates."""

    def setUp(self):
        self.setUpPyfakefs()
        self.history_path = Path('/checkout') / 'history.md'
        self.today = datetime.date(2026, 8, 28)

    def test_record_tag_creates_history(self):
        history.record_tag(
            self.history_path,
            '[masonry]',
            'masonry, grid lanes',
            self.today,
        )

        tags, bugs = history._load_history(self.history_path)
        self.assertEqual(
            tags,
            [['masonry, grid lanes', '[masonry]', '2026-08-28', '1']],
        )
        self.assertEqual(bugs, [])

    def test_record_tag_updates_matching_entry(self):
        history.record_tag(
            self.history_path, '[masonry]', 'masonry', self.today
        )
        history.record_tag(
            self.history_path,
            '[masonry]',
            'masonry, grid lanes',
            self.today,
        )

        tags, _ = history._load_history(self.history_path)
        self.assertEqual(
            tags,
            [['masonry, grid lanes', '[masonry]', '2026-08-28', '2']],
        )

    def test_record_public_and_internal_bugs(self):
        history.record_bug(
            self.history_path,
            'public',
            '123456',
            'masonry',
            'General feature work',
            self.today,
        )
        history.record_bug(
            self.history_path,
            'internal',
            '789012',
            'gap decorations',
            'General feature work',
            self.today,
        )

        _, bugs = history._load_history(self.history_path)
        self.assertEqual(
            bugs,
            [
                [
                    'masonry',
                    '123456',
                    'General feature work',
                    '2026-08-28',
                    '1',
                ],
                [
                    'gap decorations',
                    'b:789012',
                    'General feature work',
                    '2026-08-28',
                    '1',
                ],
            ],
        )

    def test_tag_and_bug_confirmations_are_independent(self):
        history.record_tag(
            self.history_path, '[masonry]', 'masonry', self.today
        )
        history.record_bug(
            self.history_path,
            'public',
            '123456',
            'masonry',
            'General feature work',
            self.today,
        )
        history.record_tag(
            self.history_path, '[masonry]', 'grid lanes', self.today
        )

        tags, bugs = history._load_history(self.history_path)
        self.assertEqual(tags[0][-1], '2')
        self.assertEqual(bugs[0][-1], '1')

    def test_rejects_prefixed_bug_id(self):
        with self.assertRaisesRegex(history.HistoryError, 'only digits'):
            history.record_bug(
                self.history_path,
                'internal',
                'b:123456',
                'masonry',
                'General feature work',
                self.today,
            )

    def test_rejects_invalid_markdown_cell(self):
        with self.assertRaisesRegex(history.HistoryError, 'pipes or newlines'):
            history.record_tag(
                self.history_path,
                '[masonry]',
                'masonry | grid lanes',
                self.today,
            )

    @mock.patch('history._run_git')
    def test_find_repository_root_prefers_superproject(self, run_git):
        run_git.return_value = mock.Mock(
            returncode=0, stdout='/workspace/chromium/src\n'
        )

        root = history.find_repository_root('/workspace/chromium/src/agents')

        self.assertEqual(root, Path('/workspace/chromium/src'))
        run_git.assert_called_once_with(
            ['rev-parse', '--show-superproject-working-tree'],
            Path('/workspace/chromium/src/agents'),
        )

    @mock.patch('history._run_git')
    @mock.patch('history.find_repository_root')
    def test_get_history_path_requires_ignored_target(self, find_root, run_git):
        find_root.return_value = Path('/workspace/chromium/src')
        run_git.return_value = mock.Mock(returncode=1)

        with self.assertRaisesRegex(
            history.HistoryError, 'unignored history path'
        ):
            history._get_history_path()


if __name__ == '__main__':
    unittest.main()
