#!/usr/bin/python3
# -*- coding:utf-8 -*-

import argparse
import codecs
import json
import os
import shutil
import smtplib
import subprocess
import sys

from email.mime.text import MIMEText
from email.header import Header
from socket import socket
from bs4 import BeautifulSoup

def GetFailed(email_config: dict, result_dir_basename: str):
  server_base_path = email_config['server_base_path']
  summary_file_name = email_config['summary_file']
  html = os.path.join(server_base_path, result_dir_basename,
                              summary_file_name)
  assert os.path.isfile(html), \
      "Error: " + summary_file + " does not exist or not invalid file."
  htmlfile = open(html, 'r', encoding='utf-8')
  soup = BeautifulSoup(htmlfile, 'html.parser')
  table = soup.find('table')
  rows = table.find_all('tr')
  header_elements = rows[0].find_all('th')
  failed = 0;
  for index, element in enumerate(header_elements):
      if element.text == "number of failed":
        failed = index
        break
  elements = [rows[1].find_all('td')[failed] for row in (rows[1:])]
  return sum(int(element.text) for element in elements)


def InjectServerBaseUrlToLink(email_config: dict, summary_file: str):
  server_base_url = email_config['server_base_url'].lower()
  server_base_url = server_base_url if server_base_url.endswith(
      '/') else server_base_url + '/'

  command = [
      'sed', '-i', '-E', r's/(<a\s+.*href)="([^h][^t][^t][^p].*)"/\1="' +
      server_base_url.replace('/', '\/') + r'\2"/g', 'summary.html'
  ]
  print(command)

  result = subprocess.run(command, capture_output=True, text=True)
  if result.returncode == 0:
    print("Inject server base url into html:", server_base_url)
  else:
    print("Error: Inject server base url into html failed.")
    sys.exit(1)


def GetSummaryHtmlFromEmailConfig(email_config: dict, result_dir_basename: str):
  # server_base_path = email_config['server_base_path']
  summary_file_name = email_config['summary_file']
  summary_file = os.path.join(result_dir_basename,
                              summary_file_name)
  assert os.path.isfile(summary_file), \
      "Error: " + summary_file + " does not exist or not invalid file."

  # InjectServerBaseUrlToLink(email_config, summary_file)

  content = ""
  with codecs.open(summary_file, 'r', encoding='utf-8') as f:
    content = f.read()

  return content


def SendEmail(email_config: dict, result_dir_basename: str):
  assert result_dir_basename != ""

  mail_msg = GetSummaryHtmlFromEmailConfig(email_config, result_dir_basename)
  message = MIMEText(mail_msg, 'html', 'utf-8')

  sender = email_config['sender']
  message['From'] = Header(sender, 'utf-8')

  receivers = email_config['receivers']
  message['To'] = Header(receivers, 'utf-8')

  subject = email_config['subject']
  failed = GetFailed(email_config, result_dir_basename)
  message['Subject'] = Header(subject + " failed cases: %d" % failed, 'utf-8')

  try:
    server = smtplib.SMTP(email_config['smtp_server'],
                          email_config['smtp_server_port'])
    server.starttls()
    server.login(email_config['auth_username'], email_config['auth_password'])
    server.sendmail(sender, receivers.split(','), message.as_string())
    print("sendmail done.")
  except socket.error as e:
    print("Error: Cannot connect to smtp_server")
  except smtplib.SMTPException:
    print("Error: Cannot sendmail by SMTPException")
  except:
    print("Unknown error:", sys.exc_info()[0])
  finally:
    if server != None:
      server.close()


def ParseEmailConfig(config: str):
  email_config = {}
  with codecs.open(config, encoding='utf-8') as f:
    email_config = json.load(f)

  return email_config


def CopyResultToServerBasePath(result_dir: str, server_base_path: str):
  shutil.copytree(result_dir,
                  os.path.join(server_base_path, os.path.basename(result_dir)))
  print('Copy %s to %s finished.' % (result_dir, server_base_path))


def BuildArgumentParser():
  parser = argparse.ArgumentParser(
      description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument(
      '--config',
      dest='email_config',
      help='A json configure file like:'
      '{\n'
      '  "server_base_url":"http://xx.xxx.xxx.xxx:8080/results",\n'
      '  "server_base_path":"/usr1/ut_reports/results",\n'
      '  "summary_file":"summary.html",\n'
      '  "subject":"[UT Results Notification]",\n'
      '  "sender":"*****@***.com",\n'
      '  "receivers":"abcd@***.com,efg@***.com,hij@***.com",\n'
      '  "auth_username":"xxxxxxxx",\n'
      '  "auth_password":"********",\n'
      '  "smtp_server":"smtp.huawei.com",\n'
      '  "smtp_server_port":"587"\n'
      '}\n',
      required=True,
      type=os.path.realpath)
  parser.add_argument('--result_dir',
                      dest='result_dir',
                      help='The path to test results like: '
                      '  /xx/xxx/out/test/UT_REPORT/20230814170644',
                      required=True,
                      type=os.path.realpath)
  return parser


def main():
  parser = BuildArgumentParser()
  args, unknown_args = parser.parse_known_args()
  result_dir = args.result_dir
  email_config = ParseEmailConfig(args.email_config)

  assert os.path.isdir(result_dir), \
         "Error: " + result_dir + " is not exist or invalid directory."
  # for future usage when we have a public server
  # assert os.path.isdir(email_config['server_base_path']), \
  #        "Error: The <server_base_path> in config is not exist, or you have no"\
  #        "permission to access the path " + email_config['server_base_path']

  # CopyResultToServerBasePath(result_dir, email_config['server_base_path'])
  SendEmail(email_config, result_dir)


if __name__ == "__main__":
  sys.exit(main())
