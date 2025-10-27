import os
import sys

import hdc_runner
import host_path
import device_dependencies
import argparse


def ProcessCommonOptions(args):
    """Processes and handles all common options."""
    if args.output_directory:
        host_path.SetOutputDirectory(args.output_directory)
    if args.device_path:
        host_path.SetDevicePath(args.device_path)
    else:
        host_path.SetDevicePath("/data/app/el1/bundle/public/com.ohos.nweb/libs/arm64/")
    if args.ip_port:
        host_path.SetIpPort(args.ip_port)


def BuildArgumentParser():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument(
        '--output-directory',
        dest='output_directory', type=os.path.realpath,
        help='Path to the directory in which build files are'
             ' located (must include build type).')
    parser.add_argument(
        '--device-path',
        dest='device_path',
        help="Device path to store the test executable"
             " and deps.")
    parser.add_argument(
        '--ip-port',
        dest='ip_port',
        help="ip:port combination for hdc forwarding"
             " need to set up windows hdc first.")
    parser.add_argument(
        '--runtime-deps-path',
        dest='runtime_deps_path', type=os.path.realpath,
        help='Runtime data dependency file from GN.')
    parser.add_argument(
        '-s', '--suite',
        dest='suite_name', required=True,
        help='Executable name of the test suite to run.')
    parser.add_argument(
        '--test-launcher-filter-file',
        dest='filter_path',
        help='Filter file path of the test suite to run.')
    parser.add_argument(
        '--gtest_filter',
        dest='gtest_filter',
        help='Google Gtest styled filter string.'
             'eg. FooTest.*:BarTest.*-FooTest.Bar:BarTest.Foo'
             'Runs everything in test suite FooTest except FooTest.Bar'
             ' and everything in test suite BarTest except BarTest.Foo')

    parser.add_argument('--device-flags-file',
        dest='device_flags_file',
        help='The relative filepath to a file containing '
        'command-line flags to set on the device')
    return parser


def ResolveDeviceFlag(device_flags_file):
    flag_list = []
    with open(device_flags_file,'r') as file:
        for line in file:
            flag_list.append(line.strip())
    return flag_list        



def RunTest(args):
    ProcessCommonOptions(args)
    # SIGTERM handler?
    print(args)
    device_flag = []
    if not hdc_runner.IsDeviceAvailable():
        raise Exception("No available device.")
    deps_file = host_path.GetOutDirectory()+ '/'+ args.suite_name + '.runtime_deps'
    print("RunTest deps_file: " + deps_file)
    executable_file = host_path.GetOutDirectory()+ '/'+ args.suite_name
    need_to_push = device_dependencies.GetDataDependencies(deps_file)
    print(need_to_push)
    if need_to_push != []:
        hdc_runner.PushDepsFiles(need_to_push)
    hdc_runner.PushBinaryFile(executable_file)
    if args.filter_path:
        hdc_runner.PushBinaryFile(args.filter_path)
    if args.device_flags_file:
        device_flag = ResolveDeviceFlag(args.device_flags_file)
    print("Rebooting Now.", flush = True)
    hdc_runner.Sync()
    hdc_runner.Reboot()
    print("Reboot Finished.", flush = True)
    print("Start running UT.", flush = True)
    hdc_runner.RunFile(args.suite_name, args.filter_path, args.gtest_filter, device_flag)
    print('runtest done')



def main():
    parser = BuildArgumentParser()
    args, unknown = parser.parse_known_args()
    # print(unknown)
    RunTest(args)
    print("done")


if __name__ == '__main__':
    sys.exit(main())
