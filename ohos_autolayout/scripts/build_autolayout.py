#!/usr/bin/env python3
"""Builds the ohos_autolayout bundle via npm and stages outputs for GN."""

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
BUILD_DIR = REPO_ROOT / "build"
if str(BUILD_DIR) not in sys.path:
    sys.path.insert(0, str(BUILD_DIR))

import action_helpers  # pylint: disable=wrong-import-position

IGNORE_NAMES = {"node_modules", "dist", "__pycache__"}
IGNORE_SUFFIXES = {".pyc"}


def run_cmd(cmd, cwd, env=None):
    print("[build_autolayout]", " ".join(cmd))
    subprocess.check_call(cmd, cwd=cwd, env=env)


def ensure_empty_dir(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path)
    path.mkdir(parents=True, exist_ok=True)


def copy_project(src: Path, dst: Path) -> None:
    ignore = shutil.ignore_patterns("node_modules", "dist", "*.pyc", "__pycache__")
    shutil.copytree(src, dst, ignore=ignore)


def iter_source_files(src: Path):
    for path in src.rglob('*'):
        if path.is_dir():
            continue
        relative = path.relative_to(src)
        if any(part in IGNORE_NAMES for part in relative.parts):
            continue
        if path.suffix in IGNORE_SUFFIXES:
            continue
        yield path


def main(argv=None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-dir", required=True)
    parser.add_argument("--work-dir", required=True)
    parser.add_argument("--output-dir", required=True)
    parser.add_argument("--npm", default="npm")
    parser.add_argument("--mode", choices=["release", "dev"], default="release")
    parser.add_argument("--depfile", default="")
    args = parser.parse_args(argv)

    source_dir = Path(args.source_dir).resolve()
    work_dir = Path(args.work_dir).resolve()
    output_dir = Path(args.output_dir).resolve()

    if os.path.basename(args.npm) == args.npm:
        npm_path = shutil.which(args.npm)
    else:
        npm_path = Path(args.npm)
    if not npm_path:
        raise FileNotFoundError(f"Unable to locate npm executable '{args.npm}'")
    if isinstance(npm_path, Path):
        npm_path = str(npm_path.resolve())

    ensure_empty_dir(work_dir)
    copy_project(source_dir, work_dir)

    env = os.environ.copy()
    if args.mode == "release":
        env.setdefault("NODE_ENV", "production")
        npm_script = "release"
        output_file_name = "autolayout.min.js"
    else:
        env.setdefault("NODE_ENV", "development")
        npm_script = "dev"
        output_file_name = "autolayout.js"

    run_cmd([npm_path, "install"], cwd=str(work_dir), env=env)
    run_cmd([npm_path, "run", npm_script], cwd=str(work_dir), env=env)

    bundle_dir = work_dir / "dist" / "sdk" / "webview"
    bundle = bundle_dir / output_file_name
    if not bundle.is_file():
        raise FileNotFoundError(f"Expected bundle not found: {bundle}")

    output_dir.mkdir(parents=True, exist_ok=True)
    output_path = output_dir / output_file_name
    shutil.copy2(bundle, output_path)

    sourcemap = bundle.with_suffix(bundle.suffix + ".map")
    if sourcemap.is_file():
        shutil.copy2(sourcemap, output_dir / sourcemap.name)

    if args.depfile:
        deps = [str(path.resolve()) for path in iter_source_files(source_dir)]
        action_helpers.write_depfile(args.depfile, str(output_path.resolve()), deps)

    return 0


if __name__ == "__main__":
    sys.exit(main())
