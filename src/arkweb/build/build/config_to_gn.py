#!/usr/bin/env python
#
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""hw build config to gn file
"""

import fnmatch
import optparse
import os
import sys
import json
import re
import copy
import getopt
import hashlib

gtemplate = """declare_args() {\n
%s
}
ohos_nweb_ex_defines_main = %s
ohos_nweb_ex_defines_blink_core = %s
ohos_nweb_ex_defines_other = %s
ohos_nweb_ex_build_defines = %s
"""

glineSep = "\n*******************************************************\n"

gKeyVariables = 'variables'
class BuildConfigException(Exception):
  def __init__(self, msg):
    self.msg = msg
  def __str__(self):
    return self.msg

def generateGnConfig(itemEnable, itemDisable, gnOutputDest, targetName):
  global gtemplate
  gnOutputFile = open(gnOutputDest, 'w')

  strItemGn = ''
  listItemDefineMain = []
  listItemDefineBlink = []
  listItemDefineOther = []
  listBuildDefines = []

  for item in itemEnable:
    if item.get('effect').find('gn') >= 0:
      strItemGn = strItemGn + item.get('name') + '=true\n'
    if item.get('effect').find('main') >= 0:
      listItemDefineMain.append(item.get('name'))
    if item.get('effect').find('blink') >= 0:
      listItemDefineBlink.append(item.get('name'))
    if item.get('effect').find('other') >= 0:
      listItemDefineOther.append(item.get('name'))
    listBuildDefines.append(item.get('name').lower())


  for item in itemDisable:
    if item.get('effect').find('gn') >= 0:
      strItemGn = strItemGn + item.get('name') + '=false\n'
  strItemGn = strItemGn.lower()

  contentMain = gtemplate%(strItemGn, json.dumps(listItemDefineMain), json.dumps(listItemDefineBlink), json.dumps(listItemDefineOther), json.dumps(listBuildDefines))
  print(glineSep + "final build_config:\n" + contentMain + glineSep)

  gnOutputFile.write(contentMain)
  gnOutputFile.close()
  pass

def hasVariables(item):
  return isinstance(item, dict) and item.has_key(gKeyVariables)

def runWithVariables(baseFuncName, item):
  if not hasVariables(item):
    return None

  itemExtFuncName = '%s_%s' % (baseFuncName, item.get('name'))
  if itemExtFuncName not in globals():
    return None

  return eval(itemExtFuncName)(item.get('name'), item.get(gKeyVariables))

def WriteIfChange(outFile, contentMain):
  oldContent = ""
  if os.path.isfile(outFile):
    outputFile = open(outFile, 'r')
    oldContent = outputFile.read()
    outputFile.close()

  if hashlib.md5(contentMain.strip()).hexdigest() != hashlib.md5(oldContent.strip()).hexdigest():
    print("regenerate " + outFile + "\n")
    outputFile = open(outFile, 'w')
    outputFile.write(contentMain)
    outputFile.close()
  pass

def getItemFromFile(configFile):
  content = open(configFile, 'r').read().strip()
  #splitItem = eval("[" + content + "]")
  from collections import OrderedDict
  if content.endswith(','):
    content = content[:-1]
  splitItem = json.loads("[" + content + "]", object_pairs_hook=OrderedDict)
  return splitItem

def isInConfigItems(configItems, newItem):
  indice = 0
  for item in configItems:
    if item['name'] == newItem['name']:
      return True, indice
    indice = indice + 1
  return False, -1

def mergeAndSetAllItems(allConfigItems, newConfigItems):
  for newItem in newConfigItems:
    isExist, indice = isInConfigItems(allConfigItems, newItem)
    if isExist == False:
      newItem['default'] = "false"
      allConfigItems.append(newItem)
  return allConfigItems

def delConfigItems(allConfigItems, delItem):
  for item in allConfigItems:
    isExist, indice = isInConfigItems(allConfigItems, delItem)
    if isExist == True:
      del allConfigItems[indice]
      return allConfigItems
  return allConfigItems

def doCheck(newConfigItems, file):
  itemsCopy=copy.deepcopy(newConfigItems)
  itemsCopy.sort(key = lambda x:x["name"])
  for i in range(len(newConfigItems)):
    if "dependence" not in newConfigItems[i]:
      print("\n\033[1;31mError:Plese check config.json!!!!!!!!! key: " + newConfigItems[i].get('name') + " have add {dependence} value [file is] " + file +"\033[0m \n")
      return False
    if (newConfigItems[i].get('name') != itemsCopy[i].get('name')):
      print("\n\033[1;31mError:Plese check config.json is with right Alphabet sort !!!!!!!!! key is: " + newConfigItems[i].get('name') + " [right key is->] "
+ itemsCopy[i].get('name') + " [file is] " + file +"\033[0m \n")
      return False
  return True

def getAllConfigItems(configDirectory):
  allConfigItems = []
  files= os.listdir(configDirectory)
  # do not support recursively
  for file in files:
    newConfigItems = getItemFromFile(configDirectory + '/' + file)
    if not doCheck(newConfigItems, file):
      return "error"
    allConfigItems = mergeAndSetAllItems(allConfigItems, newConfigItems)
  return allConfigItems

def main(argv):
  configFile = ''
  gnOutputDest = ''
  templateOutputPath = ''
  targetName = ''
  dynamicConfigs = {}
  opts, args = getopt.getopt(argv, "i:o:d:t:D:", ["input=", "output=", "template-dir=", "target_name=", "dynamic-config="])
  for opt, arg in opts:
    if opt in ("-i", "--input"):
      configFile = arg
    elif opt in ("-o", "--output"):
      gnOutputDest = arg
    elif opt in ("-d", "--template-dir"):
      templateOutputPath = arg
    # elif opt in ("-t", "--target_name"):
    #   targetName = arg
    elif opt in ("-D", "--dynamic-config"):
      kv = arg.split('=')
      if len(kv) >= 2:
        dynamicConfigs[kv[0]] = kv[1]

  # get directory path from configFile
  if (len(configFile) > 0):
    configDirectory = os.path.dirname(configFile)
  else:
    configDirectory = os.path.split(os.path.realpath(__file__))[0] + '/config/'
  print("configDirectory:%s" % configDirectory)

  # scan and merge all jsons under configDirectory, set all of items' default value to 'false'
  allConfigItems = getAllConfigItems(configDirectory)
  allConfigDisableItems = copy.deepcopy(allConfigItems) ;

  # override allConfigsItems
  overrideItems = []
  if len(configFile) > 0:
    overrideItems = getItemFromFile(configFile)

  # construct itemEnable and itemDisable
  itemEnable = []
  itemDisable = []

  for item in overrideItems:
    if len(item) < 5:
      print("json.config format error")
      return 1
    name = item['name']
    if name in dynamicConfigs and dynamicConfigs[name] != item['default']:
      item['default'] = dynamicConfigs[name]

    if item.get('default') == 'true':
      print(item.get('name') + " :" + item.get('effect') + ": [enable]")
      itemEnable.append(item)
      allConfigDisableItems = delConfigItems(allConfigDisableItems, item)

  itemDisable = allConfigDisableItems

  generateGnConfig(itemEnable, itemDisable, gnOutputDest, targetName)
  return 0

if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))
