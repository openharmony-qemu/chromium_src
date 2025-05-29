import json
import subprocess
import re
import sys
import argparse


class ApiParse:
    def __init__(self, json_file):
        with open(json_file, 'r') as f_ndk:
            ndks = json.load(f_ndk)
        self.api_names = set()
        for name, data in ndks.items():
            for version, apis in data['apiList'].items():
                for api in apis:
                    try:
                        self.api_names.add(re.findall(r"(.*)\s+\*{0,2}([a-zA-Z_\d]+)\((.*)\)", api)[0][1])
                    except:
                        raise f"The API format does not meet the specifications, api is : {api}"

    def check(self):
        result = subprocess.run('nm -CD libarkweb_engine.so  | grep OH_ | grep " U "',
                                shell=True, capture_output=True, text=True, check=True)
        ndk_symbols = set([symbol.strip()[2:]for symbol in result.stdout.strip().split("\n")])
        check_result = "\n".join(list(ndk_symbols - self.api_names))
        if check_result:
            raise ValueError(f'The following NDK interfaces are not defined in the JSON file:\n{check_result}')


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=f"check ndk adapter template.\n")
    parser.add_argument("-i", "--input", type=str, help="api json file", default="")
    args = parser.parse_args()
    parse_result = ApiParse(args.input)
    parse_result.check()
