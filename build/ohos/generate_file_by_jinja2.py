#!/usr/bin/env python3

import io
import argparse
import os
import sys

jinja2_path = os.path.normpath(
    os.path.join(
        os.path.abspath(__file__), *[os.path.pardir] * 3 + ['third_party']))
nom_path = os.path.normpath(
    os.path.join(os.path.abspath(__file__),
        *[os.path.pardir] * 3 + ['tools/json_comment_eater']))
action_helper_path = os.path.normpath(
    os.path.join(os.path.abspath(__file__), *[os.path.pardir] * 2))

sys.path.insert(0, jinja2_path)
sys.path.insert(0, nom_path)
sys.path.insert(0, action_helper_path)

import jinja2
from json_comment_eater import Nom
import action_helpers

'''
The gn variable can be injected into the template using jinja2 to generate a file.
'''

def processJinjaTemplate(input_file, output_file, context):
  (template_path, template_name) = os.path.split(input_file)
  env = jinja2.Environment(
      loader=jinja2.FileSystemLoader(template_path), trim_blocks=True)
  template = env.get_template(template_name)
  rendered = template.render(context)
  rendered_without_comments = Nom(rendered)

  with io.open(output_file, 'w', encoding='utf-8') as out_file:
    out_file.write(rendered_without_comments)

def _ParseVariables(variables_arg, error_func):
  variables = {}
  for v in action_helpers.parse_gn_list(variables_arg):
    if '=' not in v:
      error_func('--variables argument must contain "=": ' + v)
    name, _, value = v.partition('=')
    variables[name] = value
  return variables

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('--input_template', required=True,
                      help='Input template file which need to be processed.')
  parser.add_argument('--output_file', required=True,
                      help='Output file to place generated.')
  parser.add_argument('--variables',
                      help='Variables to be made available in '
                      'the template processing environment, '
                      '(e.g. --variables "abc=123")', default='')

  options = parser.parse_args()
  if options.input_template is None:
    parser.error('--input_template option must be specified')
  if options.output_file is None:
    parser.error('--output_file option must be specified')

  variables = _ParseVariables(options.variables, parser.error)

  processJinjaTemplate(options.input_template, options.output_file, context=variables)

if __name__ == '__main__':
  main()
