#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A script to find test files in the Chromium source tree and generate gtest filters for them.

[MODIFIED] The script performs the following steps:
1. Accepts a .txt file as input, where each line contains a .cc or .cpp file name.
2. Iterates through each file name in the .txt file:
    a. [NEW] Skips the file if it is already a test file (e.g., _unittest.cc).
    b. Searches the 'src' directory for the corresponding test file.
    c. Calls the "tools/make_gtest_filter.py" script to generate the --gtest-filter argument.
3. Merges all generated --gtest-filter patterns into a single string.
4. Saves the combined --gtest-filter argument string to the "gtest_filter.txt" file.
"""

import sys
import subprocess
import argparse
import os
from pathlib import Path
from typing import List, Tuple

class TestFinder:
    """
    Finds test files in the source tree based on Chromium naming conventions.
    """
    def __init__(self, search_root: Path, suffixes: List[str]):
        if not search_root.is_dir():
            raise FileNotFoundError(f"Search root directory not found: {search_root}")
        self.search_root = search_root
        self.suffixes = suffixes

    def find_test_file(self, base_name: str) -> Path:
        """
        Recursively searches for the first test file matching the base name and suffix list.
        """
        print(f"    > Searching for test file for {base_name}...")
        for suffix in self.suffixes:
            pattern = f"**/{base_name}{suffix}"
            try:
                found_file = next(self.search_root.rglob(pattern), None)
                if found_file:
                    return found_file.resolve()
            except Exception as e:
                print(f"WARNING: Error searching for {pattern}: {e}", file=sys.stderr)
                continue

        raise FileNotFoundError(
            f"Test file for '{base_name}' not found in {self.search_root}.\n"
            f"Attempted suffixes: {self.suffixes}"
        )

class GTestFilterGenerator:
    """
    Runs the make_gtest_filter.py script and parses its output.
    """
    def __init__(self, script_path: Path, src_root: Path, build_dir_name: str):
        if not script_path.is_file():
            raise FileNotFoundError(f"gtest filter script not found: {script_path}")
        self.script_path = str(script_path)
        self.src_root = src_root
        self.build_dir_name = build_dir_name

    def get_filter(self, test_file_path: Path) -> Tuple[str, str]:
        """
        Runs the script and returns (test_suite, gtest_filter).
        """
        try:
            relative_test_path = test_file_path.relative_to(self.src_root)
        except ValueError:
            raise ValueError(
                f"Test file {test_file_path} is not inside src_root {self.src_root}."
            )

        cmd = [
            sys.executable,
            str(self.script_path),
            str(relative_test_path)
        ]

        env = os.environ.copy()
        env['CHROMIUM_OUT_DIR'] = self.build_dir_name

        print(f"    > Running command: CHROMIUM_OUT_DIR={self.build_dir_name} {' '.join(cmd)}")

        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                check=True,
                encoding='utf-8',
                cwd=self.src_root,
                env=env
            )
        except subprocess.CalledProcessError as e:
            print(f"STDERR: {e.stderr}", file=sys.stderr)
            raise RuntimeError(
                f"make_gtest_filter.py failed (exit code {e.returncode}):\n"
                f"Command: CHROMIUM_OUT_DIR={self.build_dir_name} {' '.join(cmd)}\n"
                f"STDOUT: {e.stdout}\nSTDERR: {e.stderr}"
            ) from e

        output = result.stdout.strip()
        if not output:
            raise ValueError(f"make_gtest_filter.py produced no output for {relative_test_path}")

        lines = output.splitlines()

        test_suite = ""
        gtest_filter = ""

        # Format A: Success
        if len(lines) >= 2 and lines[1].strip().startswith('--gtest_filter='):
            test_suite = lines[0].strip()
            gtest_filter = lines[1].strip()

        # Format B: Fallback (Test suite not found)
        elif len(lines) == 1 and ':' in lines[0]:
            print("    > WARNING: make_gtest_filter.py could not locate the Test Suite.", file=sys.stderr)
            print(f"    >          Please check if CHROMIUM_OUT_DIR='{self.build_dir_name}' is correct and built.", file=sys.stderr)
            print("    >          Using raw test list as filter.", file=sys.stderr)

            test_suite = "UNKNOWN (Could not locate from build graph)"
            gtest_filter = f"--gtest_filter={lines[0].strip()}"
        
        # Format C: Other unexpected case
        else:
            raise ValueError(
                f"Unexpected output format from make_gtest_filter.py:\n{output}"
            )

        return test_suite, gtest_filter


class FilterFileWriter:
    """
    [MODIFIED] Writes the gtest filter to a text file.
    """
    def save(self, output_path: Path, gtest_filter: str):
        """
        [MODIFIED] Saves the gtest filter string to the specified output file.

        Args:
            output_path (Path): The path to the target .txt file.
            gtest_filter (str): The gtest filter string.

        Raises:
            RuntimeError: If file writing fails.
        """
        try:
            with output_path.open('w', encoding='utf-8') as f:
                # Write only the gtest_filter string
                f.write(f"{gtest_filter}\n")
        except IOError as e:
            raise RuntimeError(f"Unable to write to output file {output_path}: {e}") from e

def main():
    """
    Main orchestration function.
    """
    parser = argparse.ArgumentParser(
        description="Finds Chromium test files and generates gtest filters for them.",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument(
        "input_file",
        type=Path,
        help="Path to the .txt file containing a list of .cc/.cpp files (e.g., my_files.txt, one per line)"
    )
    parser.add_argument(
        "--src_root",
        type=Path,
        default="chromium/src",
        help="Path to the chromium/src directory (default: ./chromium/src)"
    )
    parser.add_argument(
        "--build_dir",
        type=str,
        default="out/Default",
        help="Relative path to the GN build directory (e.g., 'out/Default' or 'out/musl_64') (default: out/Default)"
    )
    args = parser.parse_args()

    # --- Configuration ---
    TEST_SUFFIXES = [
        "_unittest.cc",
        "_unittests.cc",
        "_browsest.cc",
        "_test.cc",
        "_unittest.cpp",
        "_unittests.cpp",
        "_browsest.cpp",
        "_test.cpp"
    ]
    FILTER_SCRIPT_NAME = "tools/make_gtest_filter.py"
    OUTPUT_FILE_NAME = "gtest_filter.txt"
    # --- End Configuration ---

    all_filter_patterns = []

    try:
        # 1. Set up paths and variables
        src_root = args.src_root.resolve()
        input_file_path = args.input_file.resolve()

        output_path = Path(OUTPUT_FILE_NAME).resolve()
        filter_script_path = src_root / FILTER_SCRIPT_NAME

        build_dir_path = src_root / args.build_dir
        if not build_dir_path.is_dir():
            print(f"WARNING: The specified build directory --build_dir '{build_dir_path}' does not exist.", file=sys.stderr)
            print("    'make_gtest_filter.py' might not be able to find the Test Suite.", file=sys.stderr)

        finder = TestFinder(src_root, TEST_SUFFIXES)
        generator = GTestFilterGenerator(filter_script_path, src_root, args.build_dir)

        # 2. Read input file
        if not input_file_path.is_file():
            raise FileNotFoundError(f"Input file not found: {input_file_path}")

        with input_file_path.open('r', encoding='utf-8') as f:
            source_files = [line.strip() for line in f if line.strip() and not line.startswith('#')]

        if not source_files:
            print("Input file is empty or contains only comments. No action taken.")
            sys.exit(0)
        
        print(f"Step 1: Found {len(source_files)} files. Starting processing...")

        # 3. Iterate through each file
        for source_file_name in source_files:
            print(f"\n--- Processing: {source_file_name} ---")

            # Check if the file itself is a test file (the new feature)
            if any(source_file_name.endswith(suffix) for suffix in TEST_SUFFIXES):
                print(f"    > WARNING: {source_file_name} appears to already be a test file, skipping.", file=sys.stderr)
                continue # Skip to the next file in the list

            try:
                base_name = Path(source_file_name).stem
                if not base_name:
                    raise ValueError("Invalid file name.")

                # 3a. Find the test file
                print(f"    > Searching for test file for '{base_name}'...")
                test_file_path = finder.find_test_file(base_name)
                print(f"    > Found: {test_file_path.relative_to(src_root)}")

                # 3b. Generate Filter
                print(f"    > Running gtest filter script...")
                _test_suite, gtest_filter = generator.get_filter(test_file_path)
                print(f"    > Success: {gtest_filter}")

                # 3c. Extract and collect filter pattern
                if gtest_filter.startswith('--gtest_filter=') and len(gtest_filter) > 15:
                    pattern = gtest_filter[15:] # Gets "Suite.Test:Suite.Test2"
                    all_filter_patterns.append(pattern)
                else:
                    print(f"    > WARNING: Invalid filter format generated for {source_file_name}, skipped: {gtest_filter}", file=sys.stderr)

            except (FileNotFoundError, ValueError, RuntimeError) as e:
                # Single file processing failed, print warning and continue
                print(f" WARNING: Failed to process {source_file_name}, skipped: {e}", file=sys.stderr)

        # 4. Merge and save to file
        print("\n--- All files processed ---")

        if not all_filter_patterns:
            raise RuntimeError("Failed to generate gtest filters for any file.")

        combined_patterns = ":".join(all_filter_patterns)
        final_gtest_filter = f"--gtest_filter={combined_patterns}"

        print(f"Step 2: Merging {len(all_filter_patterns)} filters to {output_path}...")
        writer = FilterFileWriter()
        writer.save(output_path, final_gtest_filter)

        print(f"    > SUCCESS! Combined Filter:\n    > {final_gtest_filter}")
        print("\nALL DONE.")
    
    except (FileNotFoundError, ValueError, RuntimeError, KeyboardInterrupt) as e:
        if isinstance(e, KeyboardInterrupt):
            print("\nOperation interrupted by user.", file=sys.stderr)
            sys.exit(130)
        print(f"\nERROR: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()