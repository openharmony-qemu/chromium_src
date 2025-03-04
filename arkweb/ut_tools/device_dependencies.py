import os
import re

import host_path

_INCLUSIONS = [
    re.compile(r'.*\.so'), # the so files that needed by the unittests
    re.compile(r'icudtl.dat')
    # add more file types here if needed in the future
]


def _FilterDataDeps(abs_host_files):
    inclusions = _INCLUSIONS
    return [p for p in abs_host_files if any(r.match(p) for r in inclusions) and
            not '/' in os.path.relpath(p)]
    # only retrieve files in current directory, not any subdirectory.


def GetDataDependencies(runtime_deps_path):
    """Returns a list of device data dependencies.

      Args:
        runtime_deps_path: A str path to the .runtime_deps file.
      Returns:
        A list of (host_path, device_path) tuples.
    """
    if not runtime_deps_path:
        print("No runtime_deps_path.")
        return []

    with open(runtime_deps_path, 'r') as runtime_deps_file:
        rel_host_files = [l.strip() for l in runtime_deps_file if l]
    result =_FilterDataDeps(rel_host_files)

    output_directory = host_path.GetOutDirectory()
    abs_host_files = [
        os.path.abspath(os.path.join(output_directory, r))
        for r in result]
    return abs_host_files
