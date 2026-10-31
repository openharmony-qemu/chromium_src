#!/usr/bin/env python3
# Copyright 2026 The Chromium Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Records user-confirmed CL description tags and bugs."""

import argparse
from contextlib import contextmanager
import datetime
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile

try:
    import fcntl
except ImportError:
    fcntl = None


_HISTORY_PATH = Path('.agents') / 'cl-description' / 'history.md'
_TAG_HEADER = '| Signals | Tag | Last confirmed | Confirmations |'
_TAG_SEPARATOR = '|---|---|---|---|'
_BUG_HEADER = (
    '| Signals | Bug | Applies when | Last confirmed | Confirmations |'
)
_BUG_SEPARATOR = '|---|---|---|---|---|'
_TAG_PATTERN = re.compile(r'^\[[A-Za-z0-9][A-Za-z0-9 ._+:/-]*\]$')
_BUG_ID_PATTERN = re.compile(r'^[0-9]+$')
_TAG_COLUMN_COUNT = 4
_BUG_COLUMN_COUNT = 5
_MAX_TAG_LENGTH = 64
_MAX_SIGNALS_LENGTH = 200
_MAX_APPLIES_WHEN_LENGTH = 160


class HistoryError(Exception):
    """Raised when the history cannot be read or updated safely."""


def _validate_cell(value, name, max_length):
    value = value.strip()
    if not value:
        raise HistoryError(f'{name} cannot be empty')
    if len(value) > max_length:
        raise HistoryError(f'{name} must not exceed {max_length} characters')
    if any(character in value for character in '|\r\n'):
        raise HistoryError(f'{name} cannot contain pipes or newlines')
    return value


def _parse_row(line, column_count, table_name):
    if not line.startswith('|') or not line.endswith('|'):
        raise HistoryError(f'Invalid row in the {table_name} table: {line}')
    cells = [cell.strip() for cell in line[1:-1].split('|')]
    if len(cells) != column_count:
        raise HistoryError(f'Invalid row in the {table_name} table: {line}')
    return cells


def _parse_table(lines, heading, header, separator, column_count):
    try:
        heading_index = lines.index(heading)
    except ValueError as error:
        raise HistoryError(f'Missing {heading} table') from error

    index = heading_index + 1
    while index < len(lines) and not lines[index]:
        index += 1
    if index >= len(lines) or lines[index] != header:
        raise HistoryError(f'Invalid header for the {heading} table')
    index += 1
    if index >= len(lines) or lines[index] != separator:
        raise HistoryError(f'Invalid separator for the {heading} table')

    rows = []
    for line in lines[index + 1 :]:
        if line.startswith('## '):
            break
        if not line:
            continue
        rows.append(_parse_row(line, column_count, heading))
    return rows


def _load_history(path):
    if not path.exists():
        return [], []

    lines = path.read_text(encoding='utf-8').splitlines()
    tags = _parse_table(
        lines,
        '## Tags',
        _TAG_HEADER,
        _TAG_SEPARATOR,
        _TAG_COLUMN_COUNT,
    )
    bugs = _parse_table(
        lines,
        '## Bugs',
        _BUG_HEADER,
        _BUG_SEPARATOR,
        _BUG_COLUMN_COUNT,
    )
    return tags, bugs


def _render_history(tags, bugs):
    lines = [
        '# CL Description Selection History',
        '',
        '## Tags',
        '',
        _TAG_HEADER,
        _TAG_SEPARATOR,
    ]
    lines.extend(f"| {' | '.join(row)} |" for row in tags)
    lines.extend(['', '## Bugs', '', _BUG_HEADER, _BUG_SEPARATOR])
    lines.extend(f"| {' | '.join(row)} |" for row in bugs)
    return '\n'.join(lines) + '\n'


def _write_history(path, tags, bugs):
    path.parent.mkdir(parents=True, exist_ok=True)
    file_descriptor, temporary_path = tempfile.mkstemp(
        dir=path.parent, prefix='.history.', text=True
    )
    temporary_path = Path(temporary_path)
    try:
        with os.fdopen(file_descriptor, 'w', encoding='utf-8') as file:
            file.write(_render_history(tags, bugs))
        temporary_path.replace(path)
    except Exception:
        try:
            temporary_path.unlink()
        except FileNotFoundError:
            pass
        raise


@contextmanager
def _lock_history(path):
    path.parent.mkdir(parents=True, exist_ok=True)
    lock_path = path.with_name(f'{path.name}.lock')
    with lock_path.open('a', encoding='utf-8') as lock_file:
        if fcntl is not None:
            try:
                fcntl.flock(lock_file.fileno(), fcntl.LOCK_EX)
            except AttributeError:
                pass
            except OSError as error:
                raise HistoryError(
                    f'failed to lock history file: {lock_path}'
                ) from error
        yield


def _confirmation_count(row, table_name):
    try:
        count = int(row[-1])
    except ValueError as error:
        raise HistoryError(
            f'Invalid confirmation count in the {table_name} table'
        ) from error
    if count < 1:
        raise HistoryError(
            f'Invalid confirmation count in the {table_name} table'
        )
    return count


def record_tag(path, tag, signals, today=None):
    """Records a confirmed component tag."""
    tag = _validate_cell(tag, 'tag', _MAX_TAG_LENGTH)
    if not _TAG_PATTERN.fullmatch(tag):
        raise HistoryError('tag must be enclosed in square brackets')
    signals = _validate_cell(signals, 'signals', _MAX_SIGNALS_LENGTH)
    confirmed = (today or datetime.date.today()).isoformat()
    with _lock_history(path):
        tags, bugs = _load_history(path)

        for index, row in enumerate(tags):
            if row[1] == tag:
                count = _confirmation_count(row, 'Tags') + 1
                tags[index] = [signals, tag, confirmed, str(count)]
                break
        else:
            tags.append([signals, tag, confirmed, '1'])

        _write_history(path, tags, bugs)


def record_bug(path, bug_type, bug_id, signals, applies_when, today=None):
    """Records a confirmed public or internal bug."""
    if bug_type not in ('public', 'internal'):
        raise HistoryError('bug type must be public or internal')
    bug_id = bug_id.strip()
    if not _BUG_ID_PATTERN.fullmatch(bug_id):
        raise HistoryError('bug ID must contain only digits')
    bug = bug_id if bug_type == 'public' else f'b:{bug_id}'
    signals = _validate_cell(signals, 'signals', _MAX_SIGNALS_LENGTH)
    applies_when = _validate_cell(
        applies_when, 'applies when', _MAX_APPLIES_WHEN_LENGTH
    )
    confirmed = (today or datetime.date.today()).isoformat()
    with _lock_history(path):
        tags, bugs = _load_history(path)

        for index, row in enumerate(bugs):
            if row[1] == bug:
                count = _confirmation_count(row, 'Bugs') + 1
                bugs[index] = [
                    signals,
                    bug,
                    applies_when,
                    confirmed,
                    str(count),
                ]
                break
        else:
            bugs.append([signals, bug, applies_when, confirmed, '1'])

        _write_history(path, tags, bugs)


def _run_git(arguments, cwd):
    return subprocess.run(
        ['git', *arguments],
        cwd=cwd,
        capture_output=True,
        text=True,
        check=False,
    )


def find_repository_root(cwd=None):
    """Finds the outer repository root for the current checkout."""
    cwd = Path(cwd or Path.cwd())
    result = _run_git(['rev-parse', '--show-superproject-working-tree'], cwd)
    if result.returncode == 0 and result.stdout.strip():
        return Path(result.stdout.strip())

    result = _run_git(['rev-parse', '--show-toplevel'], cwd)
    if result.returncode == 0 and result.stdout.strip():
        return Path(result.stdout.strip())
    return None


def _get_history_path():
    repository_root = find_repository_root()
    if repository_root is None:
        raise HistoryError('cannot determine the Chromium repository root')

    result = _run_git(
        [
            '-C',
            str(repository_root),
            'check-ignore',
            '--quiet',
            '--',
            str(_HISTORY_PATH),
        ],
        repository_root,
    )
    if result.returncode != 0:
        raise HistoryError(
            f'refusing to write unignored history path: {_HISTORY_PATH}'
        )
    return repository_root / _HISTORY_PATH


def _create_argument_parser():
    parser = argparse.ArgumentParser(
        description='Record confirmed CL description tags and bugs.'
    )
    subparsers = parser.add_subparsers(dest='command', required=True)

    tag_parser = subparsers.add_parser(
        'record-tag', help='Create or update a confirmed component tag.'
    )
    tag_parser.add_argument(
        '--tag', required=True, help='Bracketed component tag.'
    )
    tag_parser.add_argument(
        '--signals',
        required=True,
        help='Terms and paths that identify matching changes.',
    )

    bug_parser = subparsers.add_parser(
        'record-bug', help='Create or update a confirmed bug reference.'
    )
    bug_parser.add_argument(
        '--type',
        choices=('public', 'internal'),
        required=True,
        help='Tracker type used to normalize the stored reference.',
    )
    bug_parser.add_argument(
        '--id', required=True, help='Numeric bug ID without a prefix.'
    )
    bug_parser.add_argument(
        '--signals',
        required=True,
        help='Terms and paths that identify matching changes.',
    )
    bug_parser.add_argument(
        '--applies-when',
        required=True,
        help='Condition under which the bug should be suggested.',
    )
    return parser


def main(argv=None):
    parser = _create_argument_parser()
    arguments = parser.parse_args(argv)
    try:
        history_path = _get_history_path()
        if arguments.command == 'record-tag':
            record_tag(history_path, arguments.tag, arguments.signals)
        else:
            record_bug(
                history_path,
                arguments.type,
                arguments.id,
                arguments.signals,
                arguments.applies_when,
            )
    except (HistoryError, OSError) as error:
        print(f'error: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
