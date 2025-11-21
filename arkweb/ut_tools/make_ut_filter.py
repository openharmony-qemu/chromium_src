#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A script to find test files in the Chromium source tree and generate gtest filters for them.
[NO BUILD DIR DEPENDENCY VERSION]

Steps:
1. Accepts a .txt file as input.
2. Finds the corresponding test file (via Name or Content Reference).
3. Parses the test file to generate gtest_filter patterns.
4. Saves result to gtest_filter.txt.

FAIL-SAFE: Catch all errors, ensure output file is generated (even if empty).
"""

import sys
import subprocess
import argparse
import os
from pathlib import Path
from typing import List, Tuple, Optional


class TestFinder:
    """
    Finds test files using Naming Convention (fast) and Content Reference (smart fallback).
    """
    def __init__(self, search_root: Path, suffixes: List[str]):
        self.search_root = search_root
        self.suffixes = suffixes

    def find_test_file(self, source_file_name: str) -> Optional[Path]:
        if not self.search_root.is_dir():
            print(f"WARNING: Search root directory not found: {self.search_root}", file=sys.stderr)
            return None

        base_name = Path(source_file_name).stem
        
        # Strategy 1: Name Match
        print(f"    > [Strategy 1] Searching by name for {base_name}...")
        try:
            for suffix in self.suffixes:
                found_file = next(self.search_root.rglob(f"**/{base_name}{suffix}"), None)
                if found_file:
                    return found_file.resolve()
        except Exception:
            pass

        # Strategy 2: Content Fallback
        print(f"    > [Strategy 2] Name match failed. Trying content reference search...")
        return self.find_fallback_by_content(source_file_name)

    def find_fallback_by_content(self, source_file_name: str) -> Optional[Path]:
        try:
            source_path_obj = next(self.search_root.rglob(f"**/{source_file_name}"), None)
            if not source_path_obj:
                return None

            source_dir = source_path_obj.parent
            header_name = source_path_obj.with_suffix(".h").name
            
            candidates = list(source_dir.glob("*.cc")) + list(source_dir.glob("*.cpp"))
            for subdir in ["tests", "test", "unittests", "unit_tests"]:
                sub_path = source_dir / subdir
                if sub_path.is_dir():
                    candidates.extend(sub_path.glob("*.cc"))
                    candidates.extend(sub_path.glob("*.cpp"))

            for candidate in candidates:
                if not any(str(candidate).endswith(s) for s in self.suffixes): continue
                if candidate.name == source_path_obj.name: continue

                try:
                    content = candidate.read_text(encoding='utf-8', errors='ignore')
                    if f'"{header_name}"' in content or f'"/{header_name}"' in content:
                        print(f"    > [Fallback] Found reference in {candidate.name}!")
                        return candidate.resolve()
                except Exception:
                    continue
        except Exception:
            pass
        return None


class GTestFilterGenerator:
    """
    Runs make_gtest_filter.py WITHOUT requiring a build directory.
    It relies purely on text parsing of the test file.
    """
    def __init__(self, script_path: Path, src_root: Path):
        self.script_path = script_path
        self.src_root = src_root

    def get_filter(self, test_file_path: Path) -> Tuple[Optional[str], Optional[str]]:
        if not self.script_path.is_file():
            print(f"WARNING: script not found: {self.script_path}", file=sys.stderr)
            return None, None

        try:
            try:
                relative_test_path = test_file_path.relative_to(self.src_root)
            except ValueError:
                return None, None

            # We do NOT pass CHROMIUM_OUT_DIR here.
            # The script will run in "offline" mode (parsing only).
            cmd = [sys.executable, str(self.script_path), str(relative_test_path)]

            result = subprocess.run(
                cmd, capture_output=True, text=True, check=True, encoding='utf-8', cwd=self.src_root
            )

            output = result.stdout.strip()
            if not output:
                return None, None

            lines = output.splitlines()

            # Logic to handle output when Build Dir is missing:
            # The script usually just outputs the raw filter patterns if it can't find the suite.

            # Case A: Standard Output (Suite + Filter) - rare without build dir
            if len(lines) >= 2 and lines[1].strip().startswith('--gtest_filter='):
                return lines[0].strip(), lines[1].strip()

            # Case B:Raw Filter Output (Just the pattern or --gtest_filter=...)
            elif len(lines) >= 1:
                raw_line = lines[0].strip()
                # Sometimes it outputs just "Foo.Bar", sometimes "--gtest_filter=Foo.Bar"
                if raw_line.startswith('--gtest_filter='):
                    return "UNKNOWN_SUITE", raw_line
                else:
                    # Assume it returned just the pattern "Test.Case"
                    return "UNKNOWN_SUITE", f"--gtest_filter={raw_line}"

            return None, None

        except Exception:
            return None, None


class FilterFileWriter:
    def save(self, output_path: Path, content:str):
        try:
            with output_path.open('w', encoding='utf-8') as f:
                f.write(f"{content}\n")
        except IOError:
            pass

    def save_empty(self, output_path: Path):
        try:
            with output_path.open('w', encoding='utf-8') as f:
                f.write("")
        except IOError:
            pass


def main():
    parser = argparse.ArgumentParser(description="Generate gtest filters (No Build Dir required).")
    parser.add_argument("input_file", type=Path, help="Path to the .txt file")
    parser.add_argument("--src_root", type=Path, default="chromium/src", help="Path to chromium/src")
    # --build_dir argument is REMOVED

    args = parser.parse_args()
    OUTPUT_FILE_NAME = "gtest_filter.txt"
    output_path = Path(OUTPUT_FILE_NAME).resolve()
    writer = FilterFileWriter()

    try:
        TEST_SUFFIXES = [
            "_unittest.cc", "_unittests.cc", "_browsest.cc", "_test.cc",
            "_unittest.cpp", "_unittests.cpp", "_browsest.cpp", "_test.cpp",
            "test.cc"
        ]
        FILTER_SCRIPT_NAME = "tools/make_gtest_filter.py"

        all_filter_patterns = []
        src_root = args.src_root.resolve()
        input_file_path = args.input_file.resolve()
        filter_script_path = src_root / FILTER_SCRIPT_NAME

        if not src_root.is_dir() or not input_file_path.is_file():
            writer.save_empty(output_path)
            return

        finder = TestFinder(src_root, TEST_SUFFIXES)
        #No longer passing build_dir here
        generator = GTestFilterGenerator(filter_script_path, src_root)

        try:
            with input_file_path.open('r', encoding='utf-8') as f:
                source_files = [line.strip() for line in f if line.strip() and not line.startswith('#')]
        except Exception:
            writer.save_empty(output_path)
            return

        if not source_files:
            writer.save_empty(output_path)
            return

        print(f"Found {len(source_files)} files. Processing...")

        for source_file_name in source_files:
            print(f"\n--- Processing: {source_file_name} ---")

            if any(source_file_name.endswith(suffix) for suffix in TEST_SUFFIXES):
                # Treat as test file directly if input is already a test
                if not Path(source_file_name).is_absolute():
                    test_file_path = src_root / source_file_name
                else:
                    test_file_path = Path(source_file_name)
                # If file doesn't exist, try to find it
                if not test_file_path.exists():
                    test_file_path = finder.find_test_file(source_file_name)
            else:
                test_file_path = finder.find_test_file(source_file_name)

            if not test_file_path:
                print(f"    > SKIPPING: Not found.")
                continue

            print(f"    > Found: {test_file_path.relative_to(src_root)}")

            _test_suite, gtest_filter = generator.get_filter(test_file_path)

            if not gtest_filter:
                print(f"    > SKIPPING: Generator failed.")
                continue

            print(f"    > Generated: {gtest_filter}")

            # Robust parsing of the output string
            prefix = "--gtest_filter="
            if prefix in gtest_filter:
                # Extract everything after the prefix
                clean_pattern = gtest_filter.split(prefix)[1].strip()
                if clean_pattern:
                    all_filter_patterns.append(clean_pattern)
            else:
                # If output was just "Suite.Test", use it directly
                if gtest_filter and not " " in gtest_filter:
                    all_filter_patterns.append(gtest_filter)

        print("\n--- Processing Complete ---")

        if not all_filter_patterns:
            print("WARNING: No filters generated.")
            writer.save_empty(output_path)
            return

        combined = ":".join(all_filter_patterns)
        final_output = f"--gtest_filter={combined}"

        writer.save(output_path, final_output)
        print(f"    > Saved to {OUTPUT_FILE_NAME}")

    except Exception as e:
        print(f"CRITICAL ERROR: {e}", file=sys.stderr)
        writer.save_empty(output_path)
        sys.exit(0)

if __name__ == "__main__":
    main()