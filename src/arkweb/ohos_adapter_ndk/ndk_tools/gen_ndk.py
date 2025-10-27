import json
import os
import re
import argparse

StubStart = """
"""
WrapStart = """#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "arkweb/ohos_adapter_ndk/ndk_version.h"
"""
StubExample = """__attribute__((weak, visibility("default"), noinline)) void %s()
{
}
"""
WrapExample = """__attribute__((visibility("default"), noinline)) %s
{
  if (CheckTargetApiVersion(%s)) {
    abort();
  }
  return %s;
}
"""
Gni = """
wrapper_ldflags = [
%s
]

stub_sources = [
%s
]

wrapper_sources = [
%s
]
"""


class ApiParse:
    def __init__(self, json_file, version_limit):
        with open(json_file, 'r') as f_ndk:
            ndks = json.load(f_ndk)
        for name, data in ndks.items():
            ndks[name]["apis"] = {}
            for version, apis in data['apiList'].items():
                ndks[name]["apis"][version] = []
                if version <= version_limit:
                    continue
                for api in apis:
                    try:
                        ndks[name]["apis"][version].append(re.findall(r"(.*)\s+([a-zA-Z_\d]+)\((.*)\)", api)[0])
                    except:
                        raise f"The API format does not meet the specifications, api is : {api}"
            ndks[name]["includes"] = [f'#include "{include}"' for include in data["includes"]]
        self.ndks = ndks

    def generate(self, output_path, absolute_path):
        stub_sources = []
        wrapper_sources = []
        ld_flags = ['  "-Wl,--no-as-needed",']
        os.makedirs(output_path, exist_ok=True)
        for name, data in self.ndks.items():
            stub_file = f"{output_path}/{name}_stub.c"
            stub_sources.append(f'  "{absolute_path}/{name}_stub.c",')
            wrapper_file = f"{output_path}/{name}_wrapper.c"
            wrapper_sources.append(f'  "{absolute_path}/{name}_wrapper.c",')
            api_stubs = []
            api_wrappers = []
            api_wrapper_externs = []
            for version, apis in data["apis"].items():
                for type_, name, params in apis:
                    api_stubs.append(StubExample % (name))
                    wrapper_api = f"{type_} __wrap_{name}({params})"
                    new_params = ", ".join([arg.strip().split(" ")[-1].replace("*", "") for arg in params.split(",")])
                    real_api = f"__real_{name}({new_params})"
                    api_wrappers.append(WrapExample % (wrapper_api, version, real_api))
                    api_wrapper_externs.append(f"extern {type_} __real_{name}({params});")
                    ld_flags.append(f'  "-Wl,-wrap,{name}",')
            with open(stub_file, 'w') as f:
                f.write(StubStart + "\n")
                f.write("\n".join(api_stubs))
            with open(wrapper_file, 'w') as f:
                f.write(WrapStart + "\n")
                f.write("\n".join(data["includes"]) + "\n\n")
                f.write("\n".join(api_wrappers))
        with open(f"{output_path}/adapter_ndk_config.gni", 'w') as f:
            f.write(Gni % ("\n".join(ld_flags), "\n".join(stub_sources), "\n".join(wrapper_sources)))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=f"generate ndk adapter template.\n")
    parser.add_argument("-i", "--input", type=str, help="api json file", default="")
    parser.add_argument("-o", "--output", type=str, help="output path")
    parser.add_argument("-a", "--absolute", type=str, help="output absolute path")
    parser.add_argument("-v", "--version", type=str, help="api version")
    args = parser.parse_args()
    if args.input:
        parse_result = ApiParse(args.input, args.version)
        parse_result.generate(args.output, args.absolute)
