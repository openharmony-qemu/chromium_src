#!/usr/bin/env python3

import argparse
import sys
import tarfile


def BuildTarball(output, files):
  _files_to_write = []
  for _file_pair in files:
    _pair = _file_pair.split(':')
    if len(_pair) == 2:
      _files_to_write.append((_pair[0], _pair[1]))
    elif len(_pair) == 1:
      _files_to_write.append((_pair[0], ))
    else:
      print("ERROR: Invalid file to be added to the tarball: %s" % _file_pair)
      return 1

  with tarfile.open(output, 'w:gz') as tarball:
    for _file_pair in _files_to_write:
      if len(_file_pair) == 2:
        tarball.add(_file_pair[0], _file_pair[1])
      else:
        tarball.add(_file_pair[0])
  return 0


def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('--output',
                      default='',
                      help='Path to the output tarball',
                      required=True)
  parser.add_argument('--files',
                      default=[],
                      action='append',
                      help='Files to be added to the tarball',
                      required=True)
  options = parser.parse_args(sys.argv[1:])

  return BuildTarball(options.output, options.files)


if __name__ == '__main__':
  sys.exit(main())
