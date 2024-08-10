#!/usr/bin/env python3

import argparse
import shutil

'''
Copy a file from output/ to src/ directory for ohos_nweb_hap build case.
'''

def copyFile(source, target, dummy):
  shutil.copyfile(source, target)
  print("copy %s to %s" % (source, target))

  with open(dummy, 'w') as dummy_file:
    dummy_file.write("\n")

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('--source', required=True,
                      help='Source file.')
  parser.add_argument('--target', required=True,
                      help='Target file.')
  parser.add_argument('--dummy', required=True,
                      help='A dummy output file for work arround.')

  options = parser.parse_args()

  if options.source is None:
    parser.error('--source option must be specified')
  if options.target is None:
    parser.error('--target option must be specified')
  if options.dummy is None:
    parser.error('--dummy option must be specified')
  copyFile(options.source, options.target, options.dummy)

if __name__ == '__main__':
  main()
