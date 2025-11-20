#!/usr/bin/env python
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

#!/usr/bin/env python3
import os
import subprocess
import json
import argparse
import stat
from concurrent.futures import ThreadPoolExecutor, as_completed


def find_metadata_files(search_root):
    try:
        result = subprocess.run(
            ["find", search_root, "-name", "*.build_metadata"],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        return [line.strip() for line in result.stdout.splitlines() if line.strip()]
    except subprocess.CalledProcessError as e:
        print(f"Error finding metadata files: {e}")
        return []

 
def extract_labels_from_file(file_path):
    mojom_labels = []
    buildflag_header_labels = []
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
            try:
                data = json.loads(content)
                if isinstance(data, dict):
                    for key, value in data.items():
                        if key == "label":
                            if isinstance(value, str) and value.startswith("//") and "mojom" in value:
                                mojom_labels.append(value)
                            elif isinstance(value, list):
                                for item in value:
                                    if isinstance(item, str) and item.startswith("//") and "mojom" in item:
                                        mojom_labels.append(item)
                    if isinstance(data, dict) and data.get("template_type") == "buildflag_header":
                        label = data.get("label")
                        if label and isinstance(label, str) and label.startswith("//"):
                            buildflag_header_labels.append(label)
            except json.JSONDecodeError as e:
                for line in content.splitlines():
                    line = line.strip()
                    if line.startswith("//") and "mojom" in line:
                        mojom_labels.append(line)
                    elif line.startswith("//") and "buildflag_header" in line:
                        buildflag_header_labels.append(line)
    except FileNotFoundError as e:
        print(f"Error reading {file_path}: {e}")
    return mojom_labels, buildflag_header_labels


def collect_all_labels(metadata_files, max_worker=8):
    all_mojom_labels = set()
    all_buildflag_header_labels = set()
    with ThreadPoolExecutor(max_workers=max_worker) as executor:
        future_to_file = {executor.submit(extract_labels_from_file, path): path for path in metadata_files}
        for future in as_completed(future_to_file):
            mojom_labels, buildflag_header_labels = future.result()
            all_mojom_labels.update(mojom_labels)
            all_buildflag_header_labels.update(buildflag_header_labels)
    return sorted(all_mojom_labels), sorted(all_buildflag_header_labels)


def write_gni_file(output_path, mojom_labels, buildflag_header_labels):
    flags = os.O_WRONLY | os.O_CREAT | os.O_TRUNC
    modes = stat.S_IWUSR | stat.S_IRUSR
    with os.fdopen(os.open(output_path, flags, modes), 'w', encoding='utf-8') as f:
        f.write("mojom_targets = [\n")
        for label in mojom_labels:
            f.write(f"  \"{label}\",\n")
        f.write("]\n\n")

        f.write("buildflag_header_targets = [\n")
        for label in buildflag_header_labels:
            f.write(f"  \"{label}\",\n")
        f.write("]\n")


def main():
    parser = argparse.ArgumentParser(description='Collect target labels from metadata files.')
    parser.add_argument('--search-root', required=True, help='Root directory to search for metadata files.')
    parser.add_argument('--output', required=True, help='Path to the output GN file.')
    parser.add_argument('--threads', type=int, default=8, help='Maximum number of worker threads.')
    args = parser.parse_args()
    
    metadata_files = find_metadata_files(args.search_root)
    print(f"Found {len(metadata_files)} metadata files.")
    
    mojom_labels, buildflag_header_labels = collect_all_labels(metadata_files, args.threads)
    print(f"Found {len(mojom_labels)} Mojom labels and {len(buildflag_header_labels)} buildflag_header labels.")
    
    write_gni_file(args.output, mojom_labels, buildflag_header_labels)
    print(f"GN file written to {args.output}")

if __name__ == "__main__":
    main()
    
