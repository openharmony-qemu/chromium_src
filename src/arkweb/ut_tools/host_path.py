import os
import logging

LLVM_BIN_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__),
                                 os.pardir, os.pardir, 'third_party', 'llvm-build', 'Release+Asserts', 'bin'))

LLVM_COV_PATH = os.path.join(LLVM_BIN_DIR, 'llvm-cov')
LLVM_PROFDATA_PATH = os.path.join(LLVM_BIN_DIR, 'llvm-profdata')

DIR_SOURCE_ROOT = os.environ.get(
    'CHECKOUT_SOURCE_ROOT',
    os.path.abspath(os.path.join(os.path.dirname(__file__),
                                 os.pardir, os.pardir)))


def SetOutputDirectory(output_directory):
    """Set the Chromium output directory.

      This must be called early by scripts that rely on GetOutDirectory() or
      CheckOutputDirectory(). Typically by providing an --output-dir or
      --chromium-output-dir option.
      """
    os.environ['CHROMIUM_OUTPUT_DIR'] = os.path.abspath(output_directory)


# The message that is printed when the Chromium output directory cannot
# be found. Note that CHROMIUM_OUT_DIR and BUILDTYPE are not mentioned
# intentionally to encourage the use of CHROMIUM_OUTPUT_DIR instead.
_MISSING_OUTPUT_DIR_MESSAGE = '\
The Chromium output directory could not be found.'


def GetOutDirectory():
    """Returns the Chromium build output directory.

      NOTE: This is determined in the following way:
        - From a previous call to SetOutputDirectory()
        - Otherwise, from the CHROMIUM_OUTPUT_DIR env variable, if it is defined.
        - Otherwise, from the current Chromium source directory, and a previous
          call to SetBuildType() or the BUILDTYPE env variable, in combination
          with the optional CHROMIUM_OUT_DIR env variable.
      """
    if 'CHROMIUM_OUTPUT_DIR' in os.environ:
        return os.path.abspath(os.path.join(
            DIR_SOURCE_ROOT, os.environ.get('CHROMIUM_OUTPUT_DIR')))

    raise EnvironmentError(_MISSING_OUTPUT_DIR_MESSAGE)


def SetDevicePath(device_path):
    """Set the directory where you want to push all the unittests executables
        and all the dependency files
      """
    os.environ['DEVICE_PATH'] = device_path


_MISSING_DEVICE_PATH_MESSAGE = '\
The Device Path directory could not be found.'


def GetDevicePath():
    if 'DEVICE_PATH' in os.environ:
        return os.environ.get('DEVICE_PATH')
    raise EnvironmentError(_MISSING_DEVICE_PATH_MESSAGE)


def SetIpPort(ip_port):
    os.environ['HDC_IP_PORT'] = ip_port


def GetIpPort():
    if 'HDC_IP_PORT' in os.environ:
        return os.environ.get('HDC_IP_PORT')
    return None
