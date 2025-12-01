#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2025 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
IDL生成和实现代码映射器
1. 收集idl列表；
2. 找到每个IDL文件在out下的的生成代码
3. 找到每个IDL文件在src/third_party/blink下的实现代码
"""

import sys
import os
import json
import re
import importlib
import argparse

from pathlib import Path
from collections import defaultdict
from typing import Set, List, Dict

parser = argparse.ArgumentParser()
parser.add_argument("-s", "--source", help="代码根目录")
parser.add_argument("-t", "--target", help="源码编译的形态")
parser.add_argument("-o", "--out", help="结果生成目录")
args = parser.parse_args()
sys.path.append(os.path.join(args.source, "src/tools/idl_parser"))

from idl_parser import IDLParser, ParseFile
from idl_lexer import IDLLexer
from idl_node import IDLNode


def is_test_idl_file(file_path: str) -> bool:
    """判断是否为测试相关的IDL文件"""
    path_lower = file_path.lower()

    # 测试相关的关键词
    test_keywords = [
        '/test/',
        '/testing/',
        'test_',
        '_test.',
    ]

    # 检查路径中是否包含测试关键词
    for keyword in test_keywords:
        if keyword in path_lower:
            return True

    # 检查文件名是否以test开头
    filename = os.path.basename(file_path).lower()
    if filename.startswith('test'):
        return True

    return False


def collect_all_idl_files(root_dir: str) -> Set[str]:
    """收集所有IDL文件，排除测试相关文件"""
    print(f"🔍 开始收集IDL文件，根目录: {root_dir}")

    idl_files = set()
    excluded_files = set()

    # 遍历所有目录
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            if file.endswith('.idl'):
                full_path = os.path.join(root, file)

                # 规范化路径
                normalized_path = os.path.normpath(full_path)

                # 检查是否为测试相关文件
                if is_test_idl_file(normalized_path):
                    excluded_files.add(normalized_path)
                else:
                    idl_files.add(normalized_path)

    print(f"📊 收集结果:")
    print(f"  找到的IDL文件总数: {len(idl_files) + len(excluded_files)}")
    print(f"  有效的IDL文件: {len(idl_files)}")
    print(f"  排除的测试文件: {len(excluded_files)}")

    return idl_files


class ConsolidatedIDLProcessor:
    def __init__(self, out_dir):
        self.out_dir = out_dir
        self.lexer = IDLLexer()
        self.parser = IDLParser(self.lexer)
        self.failed_files = []

        # 初始化GN生成文件存储
        self.generated_files = {
            'interface': defaultdict(list),
            'dictionary': defaultdict(list),
            'enumeration': defaultdict(list),
            'callback_function': defaultdict(list),
            'callback_interface': defaultdict(list),
            'namespace': defaultdict(list),
            'observable_array': defaultdict(list),
            'sync_iterator': defaultdict(list),
            'async_iterator': defaultdict(list),
            'typedef': defaultdict(list),
            'union': defaultdict(list)
        }

        self.includes_map = defaultdict(set)
        self.mixin_to_targets = defaultdict(set)

    # ========== IDL解析功能 (来自complete_idl_parser.py) ==========
    def extract_implemented_as_from_idl(self, idl_path: str):
        """全面地从IDL文件中提取ImplementedAs信息，支持所有语法结构"""
        implemented_as_map = {}
        try:
            with open(idl_path, 'r', encoding='utf-8') as f:
                content = f.read()

            # 1. 接口、命名空间、字典、回调级别 (包括partial)
            pattern1 = r'\[[^\]]*ImplementedAs\s*=\s*([^\s,\]]+)[^\]]*\]\s*(?:partial\s+)?(interface|namespace|callback|dictionary)\s+(\w+)'

            # 2. Mixin级别
            pattern2 = r'\[[^\]]*ImplementedAs\s*=\s*([^\s,\]]+)[^\]]*\]\s*(?:partial\s+)?(interface\s+mixin|namespace\s+mixin)\s+(\w+)'

            # 3. 枚举级别
            pattern3 = r'\[[^\]]*ImplementedAs\s*=\s*([^\s,\]]+)[^\]]*\]\s*(?:partial\s+)?enum\s+(\w+)'

            # 4. 属性级别
            pattern4 = r'\[[^\]]*ImplementedAs\s*=\s*([^\s,\]]+)[^\]]*\]\s*(readonly\s+)?attribute\s+[^;]+\s+(\w+)\s*;'

            # 5. 方法级别 (暂时不实现，因为复杂度较高且相对少见)

            patterns_to_process = [
                (pattern1, None, lambda m: m.group(3)),  # entity_type在group2中
                (pattern2, None, lambda m: m.group(3)),  # entity_type在group2中
                (pattern3, 'enum', lambda m: m.group(2)),  # entity_type是'enum'
                (pattern4, 'attribute', lambda m: m.group(3))   # entity_type是'attribute', name在group3
            ]

            for pattern, entity_type, extract_name in patterns_to_process:
                matches = re.finditer(pattern, content, re.MULTILINE | re.IGNORECASE)

                for match in matches:
                    implemented_as = match.group(1).strip().strip('"\'')

                    if entity_type is None:
                        # 对于pattern1和pattern2，entity_type在group中
                        actual_entity_type = match.group(2).lower().replace(' ', '')
                        if actual_entity_type == 'interfacemixin':
                            actual_entity_type = 'interface_mixin'
                        elif actual_entity_type == 'namespacemixin':
                            actual_entity_type = 'namespace_mixin'
                        entity_name = match.group(3)
                    else:
                        actual_entity_type = entity_type
                        entity_name = extract_name(match)

                    key = f"{actual_entity_type}:{entity_name}"
                    implemented_as_map[key] = implemented_as

        except Exception as e:
            print(f"⚠️ 读取IDL文件 {idl_path} 时出错: {e}")

        return implemented_as_map

    def extract_typedef_from_idl(self, idl_path: str):
        """从IDL文件中提取typedef的完整定义"""
        typedef_map = {}
        try:
            with open(idl_path, 'r', encoding='utf-8') as f:
                content = f.read()
            content = re.sub(r'//.*', '', content)
            content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
            typedef_pattern = r'typedef\s+([^;]+)\s+([^;]+);'
            matches = re.finditer(typedef_pattern, content, re.MULTILINE)
            for match in matches:
                type_def = match.group(1).strip()
                alias_name = match.group(2).strip()
                typedef_map[alias_name] = type_def
        except Exception as e:
            print(f"⚠️ 提取typedef时出错 {idl_path}: {e}")
        return typedef_map

    def parse_idl_file(self, file_path):
        """解析单个IDL文件，完整功能+ImplementedAs提取"""
        try:
            if not os.path.exists(file_path):
                error_msg = f"文件不存在: {file_path}"
                self.failed_files.append({'file': file_path, 'error': error_msg})
                return self._create_empty_result(file_path, error_msg)

            implemented_as_map = self.extract_implemented_as_from_idl(file_path)
            typedef_definitions = self.extract_typedef_from_idl(file_path)

            ast = ParseFile(self.parser, file_path)
            if not ast:
                error_msg = f"解析失败，AST为空"
                self.failed_files.append({'file': file_path, 'error': error_msg})
                return self._create_empty_result(file_path, error_msg)

            result = {
                "idl_path": file_path,
                "interface": [],
                "dictionary": [],
                "element": [],
                "callback": [],
                "namespace": [],
                "typedef": [],
                "includes_info": {}
            }

            self._extract_definitions(ast, result, implemented_as_map, typedef_definitions)
            return result

        except Exception as e:
            error_msg = f"解析错误: {str(e)}"
            self.failed_files.append({'file': file_path, 'error': error_msg})
            return self._create_empty_result(file_path, error_msg)

    # ========== 文件映射功能 (来自perfect_file_mapper.py) ==========
    def load_gn_generated_files(self):
        """加载GN文件中定义的实际生成文件"""
        print("🔍 加载GN文件中定义的实际生成文件...")
        gn_files = {
            'core': os.path.join(args.source, "src/third_party/blink/renderer/bindings/generated_in_core.gni"),
            'modules': os.path.join(args.source, "src/third_party/blink/renderer/bindings/generated_in_modules.gni"),
            'extensions_chromeos': os.path.join(args.source, "src/third_party/blink/renderer/bindings/generated_in_extensions_chromeos.gni"),
            'extensions_webview': os.path.join(args.source, "src/third_party/blink/renderer/bindings/generated_in_extensions_webview.gni"),
        }

        for component, gn_file_path in gn_files.items():
            if os.path.exists(gn_file_path):
                self._parse_gn_file_enhanced(gn_file_path, component)

        total_files = sum(len(files) for files_dict in self.generated_files.values() for files in files_dict.values())
        print(f"✅ 从GN文件加载了 {total_files} 个生成文件定义")

    def parse_includes_statements(self, idl_data):
        """解析所有IDL文件中的includes语句"""
        print("🔍 解析IDL文件中的includes语句...")
        includes_pattern = r'^([A-Za-z_][A-Za-z0-9_]*)\s+includes\s+([A-Za-z_][A-Za-z0-9_]*);'

        for idl_file in idl_data:
            idl_path = idl_file['idl_path']
            try:
                with open(idl_path, 'r', encoding='utf-8') as f:
                    content = f.read()
            except Exception as e:
                print(f"Warning: 无法读取 {idl_path}: {e}")
                continue

            for line_num, line in enumerate(content.split('\n'), 1):
                line = line.strip()
                if not line or line.startswith('//') or line.startswith('/*'):
                    continue

                match = re.match(includes_pattern, line)
                if match:
                    target_interface = match.group(1)
                    mixin_interface = match.group(2)
                    self.includes_map[target_interface].add(mixin_interface)
                    self.mixin_to_targets[mixin_interface].add(target_interface)

        print(f"✅ 找到 {len(self.includes_map)} 个目标接口包含混入接口")
        print(f"✅ 找到 {len(self.mixin_to_targets)} 个混入接口被使用")

    def find_gen_files_for_entity(self, entity_name, entity_type):
        """基于GN定义为实体查找生成文件"""
        def normalize_name(name):
            return re.sub(r'[^a-zA-Z0-9]', '', name).lower()

        normalized_entity = normalize_name(entity_name)
        search_types = [entity_type]

        if entity_type == 'callback':
            search_types = ['callback_interface', 'callback_function']

        for search_type in search_types:
            if normalized_entity in self.generated_files.get(search_type, {}):
                files = self.generated_files[search_type][normalized_entity]
                return self._get_all_files(files)

            entity_files = self.generated_files.get(search_type, {})
            for key, files in entity_files.items():
                normalized_key = normalize_name(key)
                if normalized_entity == normalized_key:
                    return self._get_all_files(files)

            for key, files in entity_files.items():
                normalized_key = normalize_name(key)
                if (normalized_entity in normalized_key or normalized_key in normalized_entity):
                    return self._get_all_files(files)

        return []

    def map_idl_to_generated_files(self, idl_data):
        """将IDL映射到生成文件"""
        mapped_idl_data = []
        print("🔗 开始IDL到生成文件映射...")

        for idx, idl_file in enumerate(idl_data):
            if idx % 200 == 0:
                print(f"处理进度: {idx+1}/{len(idl_data)} ({(idx+1)/len(idl_data)*100:.1f}%)")

            mapped_file = {
                'idl_path': idl_file['idl_path'],
                'interface': [],
                'dictionary': [],
                'element': [],
                'callback': [],
                'namespace': [],
                'typedef': [],
                'includes_info': {}
            }

            # 处理接口
            for interface in idl_file.get('interface', []):
                interface_name = interface['interface_name']
                gen_files = self.find_gen_files_for_entity(interface_name, 'interface')
                if not gen_files:
                    gen_files = self.find_gen_files_for_entity(interface_name, 'callback')

                enhanced_interface = dict(interface)
                enhanced_interface['generated_files'] = gen_files
                mapped_file['interface'].append(enhanced_interface)

            # 处理其他实体类型
            for entity_type in ['dictionary', 'element', 'callback', 'namespace', 'typedef']:
                for entity in idl_file.get(entity_type, []):
                    entity_name = entity.get(f'{entity_type}_name', '') or entity.get('callback_name', '') or entity.get('namespace_name', '') or entity.get('value', '')
                    if not entity_name:
                        continue

                    search_type = entity_type
                    if entity_type == 'element':
                        search_type = 'enumeration'

                    gen_files = self.find_gen_files_for_entity(entity_name, search_type)
                    enhanced_entity = dict(entity)
                    enhanced_entity['generated_files'] = gen_files
                    mapped_file[entity_type].append(enhanced_entity)

            mapped_idl_data.append(mapped_file)

        return mapped_idl_data

    # ========== 主处理流程 ==========
    def process_all_files(self, file_list_path, output_dir):
        """处理所有IDL文件并映射到生成文件"""
        # 读取文件列表
        with open(file_list_path, 'r') as f:
            idl_files = [line.strip() for line in f if line.strip()]

        total_files = len(idl_files)
        idl_results = []

        print(f"🚀 开始处理 {total_files} 个IDL文件...")
        print("阶段1: IDL解析")

        # 阶段1: 解析所有IDL文件
        for i, file_path in enumerate(idl_files, 1):
            try:
                result = self.parse_idl_file(file_path)
                if 'parse_error' not in result:
                    idl_results.append(result)

                if i % 50 == 0:
                    print(f"已解析 {i}/{total_files} 个文件")

            except Exception as e:
                print(f"处理文件 {file_path} 时出错: {e}")

        print(f"✅ 成功解析 {len(idl_results)} 个IDL文件")

        # 阶段2: 加载GN生成文件定义
        print("\n阶段2: 加载GN生成文件定义")
        self.load_gn_generated_files()

        # 阶段3: 解析includes语句
        print("\n阶段3: 解析includes语句")
        self.parse_includes_statements(idl_results)

        # 阶段4: 映射到生成文件
        print("\n阶段4: 映射到生成文件")
        mapped_data = self.map_idl_to_generated_files(idl_results)

        # 生成统计报告
        total_idl_files = len(mapped_data)
        files_with_gen_files = 0
        total_generated_files = 0

        for idl_file in mapped_data:
            has_gen_files = False

            # 检查接口
            for interface in idl_file.get('interface', []):
                gen_files = interface.get('generated_files', [])
                if gen_files:
                    has_gen_files = True
                    total_generated_files += len(gen_files)

            # 检查其他实体类型
            for entity_type in ['dictionary', 'element', 'callback', 'namespace', 'typedef']:
                for entity in idl_file.get(entity_type, []):
                    gen_files = entity.get('generated_files', [])
                    if gen_files:
                        has_gen_files = True
                        total_generated_files += len(gen_files)

            if has_gen_files:
                files_with_gen_files += 1

        report = {
            'total_idl_files': total_idl_files,
            'files_with_generated_files': files_with_gen_files,
            'success_rate': f"{files_with_gen_files/total_idl_files*100:.1f}%",
            'total_generated_files': total_generated_files,
            'failed_files': len(self.failed_files),
            'source': 'consolidated_idl_parser_and_mapper'
        }

        report_file = os.path.join(output_dir, "idl_to_gen_report.json")
        with open(report_file, 'w', encoding='utf-8') as f:
            json.dump(report, f, ensure_ascii=False, indent=2)

        print(f"\n📊 处理统计:")
        print(f"  📁 处理的IDL文件: {report['total_idl_files']}")
        print(f"  📄 找到生成文件的IDL: {report['files_with_generated_files']} ({report['success_rate']})")
        print(f"  📋 总生成文件数: {report['total_generated_files']}")
        print(f"  ❌ 解析失败文件: {report['failed_files']}")

        return mapped_data

    def _extract_definitions(self, node, result, implemented_as_map, typedef_definitions):
        """从AST中提取IDL定义，添加ImplementedAs支持"""
        if not isinstance(node, IDLNode):
            return
        node_type = node.GetClass()

        if node_type == 'Interface':
            interface_info = self._extract_interface(node, implemented_as_map)
            if interface_info:
                result["interface"].append(interface_info)
        elif node_type == 'Dictionary':
            dict_info = self._extract_dictionary(node, implemented_as_map)
            if dict_info:
                result["dictionary"].append(dict_info)
        elif node_type == 'Enum':
            enum_info = self._extract_enum(node, implemented_as_map)
            if enum_info:
                result["element"].append(enum_info)
        elif node_type == 'Callback':
            callback_info = self._extract_callback(node, implemented_as_map)
            if callback_info:
                result["callback"].append(callback_info)
        elif node_type == 'Namespace':
            namespace_info = self._extract_namespace(node, implemented_as_map)
            if namespace_info:
                result["namespace"].append(namespace_info)
        elif node_type == 'Typedef':
            typedef_info = self._extract_typedef(node, implemented_as_map, typedef_definitions)
            if typedef_info:
                result["typedef"].append(typedef_info)

        children = node.GetChildren()
        for child in children:
            self._extract_definitions(child, result, implemented_as_map, typedef_definitions)

    def _extract_interface(self, node, implemented_as_map):
        """提取接口信息，添加ImplementedAs支持"""
        interface_name = node.GetName()
        if not interface_name:
            return None
        interface_info = {
            "interface_name": interface_name,
            "method": [],
            "attribute": [],
            "constant": []
        }
        implemented_as = implemented_as_map.get(f"interface:{interface_name}")
        if implemented_as:
            interface_info["ImplementedAs"] = implemented_as

        children = node.GetChildren()
        for child in children:
            child_type = child.GetClass()
            child_name = child.GetName()
            if child_type == 'Operation':
                method_name = self._get_operation_name(child)
                if method_name and method_name not in interface_info["method"]:
                    interface_info["method"].append(method_name)
            elif child_type == 'Attribute':
                # 提取属性信息，包括类型和ImplementedAs
                attr_info = self._extract_attribute_info(child, implemented_as_map, interface_name)
                if attr_info:
                    interface_info["attribute"].append(attr_info)
            elif child_type == 'Const':
                const_name = child_name
                if const_name and const_name not in interface_info["constant"]:
                    interface_info["constant"].append(const_name)
        return interface_info

    def _extract_attribute_info(self, attribute_node, implemented_as_map, interface_name):
        """提取属性信息，包括名称、类型和ImplementedAs"""
        attr_name = attribute_node.GetName()
        if not attr_name:
            return None

        # 获取属性的基本信息
        attr_info = {
            "name": attr_name,
            "type": None,
            "readonly": attribute_node.GetProperty('READONLY') is True,
            "static": attribute_node.GetProperty('STATIC') is True
        }

        # 检查属性级别的ImplementedAs
        attribute_implemented_as = implemented_as_map.get(f"attribute:{attr_name}")
        if attribute_implemented_as:
            attr_info["ImplementedAs"] = attribute_implemented_as

        # 获取类型信息
        children = attribute_node.GetChildren()
        for child in children:
            if isinstance(child, IDLNode) and child.GetClass() == 'Type':
                type_children = child.GetChildren()
                for type_child in type_children:
                    if isinstance(type_child, IDLNode) and type_child.GetClass() == 'Typeref':
                        type_name = type_child.GetName()
                        attr_info["type"] = type_name
                        break
                break

        # 如果没有找到类型，使用字符串表示
        if not attr_info["type"]:
            attr_info["type"] = "unknown"

        return attr_info

    def _extract_dictionary(self, node, implemented_as_map):
        """提取字典信息，添加ImplementedAs支持"""
        dict_name = node.GetName()
        if not dict_name:
            return None
        dict_info = {
            "dictionary_name": dict_name,
            "value": []
        }
        implemented_as = implemented_as_map.get(f"dictionary:{dict_name}")
        if implemented_as:
            dict_info["ImplementedAs"] = implemented_as

        children = node.GetChildren()
        for child in children:
            if child.GetClass() == 'Key':
                field_name = child.GetName()
                if field_name and field_name not in dict_info["value"]:
                    dict_info["value"].append(field_name)
        return dict_info

    def _extract_enum(self, node, implemented_as_map):
        """提取枚举信息，添加ImplementedAs支持"""
        enum_name = node.GetName()
        if not enum_name:
            return None
        enum_info = {
            "element_name": enum_name,
            "value": []
        }
        implemented_as = implemented_as_map.get(f"enum:{enum_name}")
        if implemented_as:
            enum_info["ImplementedAs"] = implemented_as

        children = node.GetChildren()
        for child in children:
            if child.GetClass() == 'EnumItem':
                enum_value = child.GetName()
                if enum_value and enum_value not in enum_info["value"]:
                    enum_info["value"].append(enum_value)
        return enum_info

    def _extract_callback(self, node, implemented_as_map):
        """提取回调信息，添加ImplementedAs支持"""
        callback_name = node.GetName()
        if not callback_name:
            return None
        callback_info = {
            "callback_name": callback_name
        }
        implemented_as = implemented_as_map.get(f"callback:{callback_name}")
        if implemented_as:
            callback_info["ImplementedAs"] = implemented_as
        return callback_info

    def _extract_namespace(self, node, implemented_as_map):
        """提取命名空间信息，添加ImplementedAs支持"""
        namespace_name = node.GetName()
        if not namespace_name:
            return None
        namespace_info = {
            "namespace_name": namespace_name,
            "method": [],
            "attribute": []
        }
        implemented_as = implemented_as_map.get(f"namespace:{namespace_name}")
        if implemented_as:
            namespace_info["ImplementedAs"] = implemented_as

        children = node.GetChildren()
        for child in children:
            child_type = child.GetClass()
            child_name = child.GetName()
            if child_type == 'Operation':
                method_name = self._get_operation_name(child)
                if method_name and method_name not in namespace_info["method"]:
                    namespace_info["method"].append(method_name)
            elif child_type == 'Attribute':
                # 提取属性信息，包括类型
                attr_info = self._extract_attribute_info(child)
                if attr_info:
                    namespace_info["attribute"].append(attr_info)
        return namespace_info

    def _extract_typedef(self, node, implemented_as_map, typedef_definitions):
        """提取类型定义信息，添加ImplementedAs支持"""
        typedef_name = node.GetName()
        if not typedef_name:
            return None
        typedef_info = {
            "name": typedef_definitions.get(typedef_name, ""),
            "value": typedef_name
        }
        implemented_as = implemented_as_map.get(f"typedef:{typedef_name}")
        if implemented_as:
            typedef_info["ImplementedAs"] = implemented_as
        return typedef_info

    def _get_operation_name(self, operation_node):
        """获取操作（方法）的名称"""
        op_name = operation_node.GetName()
        properties = operation_node._properties
        if properties.get('GETTER'):
            return 'getter'
        elif properties.get('SETTER'):
            return 'setter'
        elif properties.get('DELETER'):
            return 'deleter'
        elif op_name and op_name != 'None':
            return op_name
        return None

    def _create_empty_result(self, file_path, error_msg):
        """创建空结果"""
        return {
            "idl_path": file_path,
            "interface": [],
            "dictionary": [],
            "element": [],
            "callback": [],
            "namespace": [],
            "typedef": [],
            "includes_info": {},
            "parse_error": error_msg
        }

    def _parse_gn_file_enhanced(self, gn_file_path, component):
        """增强的GN文件解析，处理条件编译块"""
        try:
            with open(gn_file_path, 'r', encoding='utf-8') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {gn_file_path}: {e}")
            return
        self._parse_gn_file_enhanced_logic(content, component)

    def _parse_gn_file_enhanced_logic(self, content, component):
        """增强的GN文件解析方法，可以处理条件编译块"""
        file_type_patterns = {
            'interface': f'generated_interface_sources_in_{component}',
            'dictionary': f'generated_dictionary_sources_in_{component}',
            'enumeration': f'generated_enumeration_sources_in_{component}',
            'callback_function': f'generated_callback_function_sources_in_{component}',
            'callback_interface': f'generated_callback_interface_sources_in_{component}',
            'namespace': f'generated_namespace_sources_in_{component}',
            'observable_array': f'generated_observable_array_sources_in_{component}',
            'sync_iterator': f'generated_sync_iterator_sources_in_{component}',
            'async_iterator': f'generated_async_iterator_sources_in_{component}',
            'typedef': f'generated_typedef_sources_in_{component}',
            'union': f'generated_union_sources_in_{component}',
        }

        lines = content.split('\n')
        current_file_type = None
        in_list = False
        current_list_content = []

        for line in lines:
            line = line.strip()
            for file_type, pattern in file_type_patterns.items():
                if line.startswith(pattern) and ('=' in line or '+=' in line):
                    current_file_type = file_type
                    if '[' in line:
                        in_list = True
                        list_start = line.find('[') + 1
                        remaining_content = line[list_start:].strip()
                        if remaining_content and ']' in remaining_content:
                            list_end = remaining_content.find(']')
                            current_list_content.append(remaining_content[:list_end])
                            self._process_file_list(current_list_content, current_file_type, component)
                            current_list_content = []
                            in_list = False
                            current_file_type = None
                        elif remaining_content:
                            current_list_content.append(remaining_content)
                    break

            if in_list and current_file_type:
                if ']' in line:
                    list_end = line.find(']')
                    if list_end > 0:
                        current_list_content.append(line[:list_end])
                    else:
                        current_list_content.append(line)
                    self._process_file_list(current_list_content, current_file_type, component)
                    current_list_content = []
                    in_list = False
                    current_file_type = None
                elif line and not line.startswith('//') and not line.startswith('#'):
                    current_list_content.append(line)

    def _process_file_list(self, list_content, file_type, component):
        """处理文件列表内容，提取文件路径"""
        full_content = '\n'.join(list_content)
        file_pattern = r'\"([^\"]*v8_[^\"]*\.(?:cc|h))\"'
        matches = re.findall(file_pattern, full_content)
        full_path_pre = os.path.join(args.source, "src/out", args.target)
        for match in matches:
            if match.startswith('$root_gen_dir/'):
                relative_path = match[14:]
                full_path = f"{full_path_pre}/gen/{relative_path}"
            else:
                full_path = f"{full_path_pre}/gen/third_party/blink/renderer/bindings/{match}"

            if os.path.exists(full_path):
                entity_name = self._extract_entity_name_from_file_path(full_path)
                if entity_name:
                    self.generated_files[file_type][entity_name].append(full_path)

    def _extract_entity_name_from_file_path(self, file_path):
        """从文件路径中提取实体名称"""
        file_name = os.path.basename(file_path)
        if file_name.startswith('v8_'):
            entity_name = file_name[3:]
            if entity_name.endswith('.cc'):
                entity_name = entity_name[:-3]
            elif entity_name.endswith('.h'):
                entity_name = entity_name[:-2]
            return entity_name
        return None

    def _get_all_files(self, files):
        """获取所有文件，不区分头文件和源文件"""
        return sorted(list(files))


# mapper impl
class ImplementationMapper:
    def __init__(self, mapped_gen_data):
        self.blink_root = os.path.join(args.source, "src/third_party/blink/renderer")
        self.consolidated_data = None
        self.generated_file_cache = {}  # 缓存生成文件内容
        self.impl_file_cache = {}       # 缓存实现文件内容

        # 加载consolidated数据
        self.consolidated_data = mapped_gen_data

    def map_idl_to_implementations(self):
        """将IDL映射到实现文件"""
        print("\n阶段5: 映射到实现文件")
        print("🔍 开始映射IDL到实现文件...")

        results = []
        processed_count = 0

        for idl_data in self.consolidated_data:
            if processed_count % 200 == 0:
                print(f"处理进度: {processed_count+1}/{len(self.consolidated_data)} ({(processed_count+1)/len(self.consolidated_data)*100:.1f}%)")

            processed_count += 1

            # 创建结果对象，保持原有格式
            result = {
                'idl_path': idl_data['idl_path'],
                'interface': [],
                'dictionary': [],
                'element': [],
                'callback': [],
                'namespace': [],
                'typedef': [],
                'includes_info': {}
            }

            # 处理接口
            for interface in idl_data.get('interface', []):
                impl_info = self._find_implementation_for_interface(idl_data['idl_path'], interface)

                enhanced_interface = dict(interface)
                enhanced_interface['implementation_info'] = impl_info

                result['interface'].append(enhanced_interface)

            # 处理其他实体类型
            for entity_type in ['dictionary', 'element', 'callback', 'namespace', 'typedef']:
                for entity in idl_data.get(entity_type, []):
                    impl_info = self._find_implementation_for_other_entity(
                        idl_data['idl_path'], entity, entity_type
                    )

                    enhanced_entity = dict(entity)
                    enhanced_entity['implementation_info'] = impl_info

                    result[entity_type].append(enhanced_entity)

            # 保持原有的generated_files字段
            for entity_type in ['interface', 'dictionary', 'element', 'callback', 'namespace', 'typedef']:
                for entity in idl_data.get(entity_type, []):
                    # 在result中查找对应的entity并添加generated_files
                    for result_entity in result[entity_type]:
                        # 简单匹配策略：匹配名称
                        entity_name = entity.get(f'{entity_type}_name', '') or entity.get('callback_name', '') or entity.get('namespace_name', '') or entity.get('value', '')
                        result_entity_name = result_entity.get(f'{entity_type}_name', '') or result_entity.get('callback_name', '') or result_entity.get('namespace_name', '') or result_entity.get('value', '')

                        if entity_name == result_entity_name:
                            if 'generated_files' in entity:
                                result_entity['generated_files'] = entity['generated_files']
                            break

            results.append(result)

        print(f"✅ 处理完成！")
        return results

    def generate_report(self, mapped_data):
        """生成报告"""
        total_idl_files = len(mapped_data)
        files_with_impl = 0
        files_with_gen = 0
        total_impl_files = 0
        total_gen_files = 0

        for data in mapped_data:
            # 统计实现文件
            has_impl_files = False
            for entity_type in ['interface', 'dictionary', 'element', 'callback', 'namespace', 'typedef']:
                for entity in data.get(entity_type, []):
                    impl_info = entity.get('implementation_info', {})
                    impl_files = impl_info.get('files', [])
                    if impl_files:
                        has_impl_files = True
                        total_impl_files += len(impl_files)

            if has_impl_files:
                files_with_impl += 1

            # 统计生成文件
            has_gen_files = False
            for entity_type in ['interface', 'dictionary', 'element', 'callback', 'namespace', 'typedef']:
                for entity in data.get(entity_type, []):
                    if entity.get('generated_files'):
                        has_gen_files = True
                        total_gen_files += len(entity.get('generated_files', []))

            if has_gen_files:
                files_with_gen += 1

        report = {
            'total_idl_files': total_idl_files,
            'files_with_implementation': files_with_impl,
            'files_with_generated_files': files_with_gen,
            'implementation_success_rate': f"{files_with_impl/total_idl_files*100:.1f}%",
            'generated_success_rate': f"{files_with_gen/total_idl_files*100:.1f}%",
            'total_implementation_files': total_impl_files,
            'total_generated_files': total_gen_files
        }

        return report

    def _extract_implemented_as_from_idl_enhanced(self, idl_path):
        """全面版ImplementedAs提取，支持所有语法结构"""
        implemented_as_map = {}
        try:
            with open(idl_path, 'r', encoding='utf-8') as f:
                content = f.read()

            # 1. 接口、命名空间、字典、回调级别 (包括partial)
            pattern1 = r'\[[^\]]*ImplementedAs\s*=\s*([^\s,\]]+)[^\]]*\]\s*(?:partial\s+)?(interface|namespace|callback|dictionary)\s+(\w+)'

            # 2. Mixin级别
            pattern2 = r'\[[^\]]*ImplementedAs\s*=\s*([^\s,\]]+)[^\]]*\]\s*(?:partial\s+)?(interface\s+mixin|namespace\s+mixin)\s+(\w+)'

            # 3. 枚举级别
            pattern3 = r'\[[^\]]*ImplementedAs\s*=\s*([^\s,\]]+)[^\]]*\]\s*(?:partial\s+)?enum\s+(\w+)'

            # 4. 属性级别
            # 匹配 [ImplementedAs=XXX] readonly attribute Type name;
            #      [ImplementedAs=XXX] attribute Type name;
            # 使用更精确的匹配，排除attribute关键字，直接匹配属性名
            pattern4 = r'\[[^\]]*ImplementedAs\s*=\s*([^\s,\]]+)[^\]]*\]\s*(readonly\s+)?attribute\s+[^;]+\s+(\w+)\s*;'

            # 5. 方法级别 (暂时不实现，因为比较复杂且相对少见)

            patterns_to_process = [
                (pattern1, None, lambda m: m.group(3)),  # entity_type在group2中
                (pattern2, None, lambda m: m.group(3)),  # entity_type在group2中
                (pattern3, 'enum', lambda m: m.group(2)),  # entity_type是'enum'
                (pattern4, 'attribute', lambda m: m.group(3))   # entity_type是'attribute', name在group3
            ]

            # 暂时跳过方法级别，因为复杂度较高
            # (pattern5, 'method', lambda m: m.group(2))   # entity_type是'method'

            for pattern, entity_type, extract_name in patterns_to_process:
                matches = re.finditer(pattern, content, re.MULTILINE | re.IGNORECASE)

                for match in matches:
                    implemented_as = match.group(1).strip().strip('"\'')

                    if entity_type is None:
                        # 对于pattern1和pattern2，entity_type在group中
                        actual_entity_type = match.group(2).lower().replace(' ', '')
                        if actual_entity_type == 'interfacemixin':
                            actual_entity_type = 'interface_mixin'
                        elif actual_entity_type == 'namespacemixin':
                            actual_entity_type = 'namespace_mixin'
                        entity_name = match.group(3)
                    else:
                        actual_entity_type = entity_type
                        entity_name = extract_name(match)

                    key = f"{actual_entity_type}:{entity_name}"
                    implemented_as_map[key] = implemented_as

        except Exception as e:
            print(f"⚠️ 读取IDL文件 {idl_path} 时出错: {e}")

        return implemented_as_map

    def _parse_generated_file(self, gen_file_path):
        """解析生成文件，提取IDL接口/方法/属性对应的C++函数"""
        if gen_file_path in self.generated_file_cache:
            return self.generated_file_cache[gen_file_path]

        try:
            with open(gen_file_path, 'r', encoding='utf-8') as f:
                content = f.read()
        except Exception as e:
            print(f"⚠️ 无法读取生成文件 {gen_file_path}: {e}")
            self.generated_file_cache[gen_file_path] = {}
            return {}

        # 提取blink_receiver->后面的函数名
        pattern = r'blink_receiver->\s*([A-Za-z_][A-Za-z0-9_]*)'
        matches = re.findall(pattern, content)

        # 提取接口名和类名
        class_pattern = r'class\s+([A-Za-z_][A-Za-z0-9_]*)\s*:'
        class_matches = re.findall(class_pattern, content)

        interface_pattern = r'V8[A-Z][a-zA-Z0-9_]*'
        interface_matches = [m for m in re.findall(interface_pattern, content) if m != 'V8']

        result = {
            'receiver_functions': set(matches),
            'classes': set(class_matches),
            'interfaces': set(interface_matches)
        }

        self.generated_file_cache[gen_file_path] = result
        return result

    def _find_impl_files_in_directory(self, idl_dir_path, interface_name):
        """在IDL文件所在目录和扩展目录寻找实现文件"""
        impl_files = {
            'headers': [],
            'sources': []
        }

        # 搜索目录列表：原始目录 + 扩展目录
        search_dirs = []
        if os.path.exists(idl_dir_path):
            search_dirs.append(idl_dir_path)

        # 添加扩展目录
        extension_base = os.path.join(args.source, "src/arkweb/chromium_ext/third_party/blink/renderer")
        if '/third_party/blink/renderer/' in idl_dir_path:
            relative_path = idl_dir_path.split('/third_party/blink/renderer/')[1]
            extension_dir = os.path.join(extension_base, relative_path)
            if os.path.exists(extension_dir):
                search_dirs.append(extension_dir)

        # 寻找相同/类似名称的文件
        base_name = interface_name.lower()

        # 可能的文件名模式
        possible_names = [
            f"{base_name}.h",
            f"{base_name}.cc",
            f"{base_name}.cpp",
            f"{interface_name}.h",
            f"{interface_name}.cc",
            f"{interface_name}.cpp"
        ]

        # 处理特殊的命名模式（如 ArkWeb -> ark_web.h, DOMAI -> ai.cc）
        if base_name == "ai" or interface_name == "DOMAI":
            possible_names.extend(["ai.h", "ai.cc", "ai.cpp"])

        # 处理ArkWeb特殊情况
        if interface_name == "ArkWeb":
            possible_names.extend(["ark_web.h", "ark_web.cc"])
        elif base_name == "arkweb":
            possible_names.extend(["ark_web.h", "ark_web.cc"])

        # 在所有搜索目录中查找文件
        for search_dir in search_dirs:
            for file_path in os.listdir(search_dir):
                if file_path in possible_names:
                    full_path = os.path.join(search_dir, file_path)
                    # 避免重复添加
                    if full_path not in impl_files['headers'] + impl_files['sources']:
                        if file_path.endswith(('.h', '.hpp')):
                            impl_files['headers'].append(full_path)
                        elif file_path.endswith(('.cc', '.cpp')):
                            impl_files['sources'].append(full_path)

        return impl_files

    def _parse_impl_file(self, impl_file_path):
        """解析实现文件，提取接口/类/函数定义"""
        if impl_file_path in self.impl_file_cache:
            return self.impl_file_cache[impl_file_path]

        try:
            with open(impl_file_path, 'r', encoding='utf-8') as f:
                content = f.read()
        except Exception as e:
            print(f"⚠️ 无法读取实现文件 {impl_file_path}: {e}")
            self.impl_file_cache[impl_file_path] = {}
            return {}

        # 提取类定义（主要在.h文件中）
        class_pattern = r'class\s+([A-Za-z_][A-Za-z0-9_]*)\s*[:{]'
        class_pattern2 = r'class\s+(?:[A-Za-z_][A-Za-z0-9_]*\s+)*([A-Za-z_][A-Za-z0-9_]*)\s+final\s*[:{]'
        class_matches = re.findall(class_pattern, content)
        if impl_file_path.endswith('ark_web.h'):
            class_matches = re.findall(class_pattern2, content)

        # 提取C++成员函数定义 ClassName::MethodName 格式（主要在.cc文件中）
        method_pattern = r'([A-Za-z_][A-Za-z0-9_]*)::([A-Za-z_][A-Za-z0-9_]*)\s*\([^)]*\)\s*(?:const)?\s*[{;]'
        method_matches = re.findall(method_pattern, content)

        # 提取构造函数和析构函数
        constructor_pattern = r'([A-Za-z_][A-Za-z0-9_]*)::([A-Za-z_][A-Za-z0-9_]*)\s*\([^)]*\)\s*[:{;]'
        constructor_matches = re.findall(constructor_pattern, content)

        # 提取普通函数定义（非成员函数）
        function_pattern = r'([A-Za-z_][A-Za-z0-9_<>*&\s]+(?:\s*[&*])?)\s+([A-Za-z_][A-Za-z0-9_]*)\s*\([^)]*\)\s*(?:const)?\s*[{;]'
        # 过滤掉已经匹配的成员函数
        all_function_matches = re.findall(function_pattern, content)
        standalone_functions = []
        for func_match in all_function_matches:
            func_name = func_match[1]
            # 检查是否是成员函数（如果前面有类名::就是成员函数）
            if not re.search(r'\b' + func_name + r'\s*\([^)]*\)\s*[{;]', content.replace(func_match[0], '')):
                standalone_functions.append(func_name)

        # 提取属性对应的getter/setter方法（更通用的模式）
        getter_pattern = r'([A-Za-z_][A-Za-z0-9_]*)\s*\([^)]*\)\s*(?:const\s*)?[{;]'
        setter_pattern = r'(?:void\s+)?(?:Set|set)([A-Za-z_][A-Za-z0-9_]*)\s*\([^)]*\)\s*[{;]'

        getter_matches = re.findall(getter_pattern, content)
        setter_matches = re.findall(setter_pattern, content)

        # 收集所有的类名（包括从成员函数中提取的类名）
        all_classes = set(class_matches)
        for class_name, method_name in method_matches:
            all_classes.add(class_name)
        for class_name, method_name in constructor_matches:
            all_classes.add(class_name)

        # 收集所有的函数名
        all_functions = set(standalone_functions)
        for class_name, method_name in method_matches:
            all_functions.add(method_name)
        for class_name, method_name in constructor_matches:
            all_functions.add(method_name)

        result = {
            'classes': all_classes,
            'functions': all_functions,
            'getters': set(getter_matches),
            'setters': set(setter_matches)
        }

        self.impl_file_cache[impl_file_path] = result
        return result

    def _search_all_impl_files_in_dir(self, idl_dir_path, interface_name, methods, attributes, implemented_as=None, entity_type='interface'):
        """在目录中搜索所有.h/.cc文件，寻找匹配的接口/类/函数"""
        if not os.path.exists(idl_dir_path):
            return {
                'headers': [],
                'sources': [],
                'matched_classes': set(),
                'matched_functions': set()
            }

        result = {
            'headers': [],
            'sources': [],
            'matched_classes': set(),
            'matched_functions': set()
        }

        # 构建搜索的关键词集合 - ImplementedAs有最高优先级
        search_keywords = set()

        # 优先使用ImplementedAs字段的值作为主要搜索关键词
        if implemented_as:
            search_keywords.add(implemented_as)
            # 为ImplementedAs添加常见的变体
            search_keywords.add(implemented_as.lower())
            search_keywords.add(implemented_as.upper())
            # 如果ImplementedAs以特定后缀结尾，添加不带后缀的版本
            for suffix in ['Impl', 'External', 'Internal', 'Base']:
                if implemented_as.endswith(suffix):
                    base_name = implemented_as[:-len(suffix)]
                    search_keywords.add(base_name)
                    search_keywords.add(base_name.lower())

        # 添加原始接口名称，但优先级较低
        if interface_name:
            search_keywords.add(interface_name)
            search_keywords.add(interface_name.lower())
            search_keywords.add(interface_name[0].upper() + interface_name[1:] if interface_name else '')

        # 从方法名中提取关键词
        for method in methods:
            # 转换为可能的C++方法名（驼峰转下划线等）
            method_variants = [
                method,
                method.lower(),
                method[0].upper() + method[1:],  # 首字母大写
                self._camel_to_snake(method),
                f"Get{method}",      # getter模式
                f"Set{method}",      # setter模式
                f"{method}ForBinding"
            ]
            search_keywords.update(method_variants)

        # 从属性名中提取关键词
        for attr in attributes:
            if isinstance(attr, dict):
                attr_name = attr.get('name', '')
            else:
                attr_name = str(attr)

            if attr_name:
                attr_variants = [
                    attr_name,
                    attr_name.lower(),
                    attr_name[0].upper() + attr_name[1:],
                    self._camel_to_snake(attr_name),
                    f"Get{attr_name}",
                    f"Set{attr_name}",
                    f"{attr_name}ForBinding"
                ]
                search_keywords.update(attr_variants)

        # 搜索目录中的所有实现文件
        for file_name in os.listdir(idl_dir_path):
            if file_name.endswith(('.h', '.hpp', '.cc', '.cpp')):
                file_path = os.path.join(idl_dir_path, file_name)
                file_data = self._parse_impl_file(file_path)

                has_match = False

                # 检查类名匹配
                for keyword in search_keywords:
                    if keyword in file_data['classes']:
                        result['matched_classes'].add(keyword)
                        has_match = True

                # 检查函数名匹配
                for keyword in search_keywords:
                    if keyword in file_data['functions']:
                        result['matched_functions'].add(keyword)
                        has_match = True

                # 检查getter/setter匹配
                for keyword in search_keywords:
                    if keyword in file_data['getters'] or keyword in file_data['setters']:
                        result['matched_functions'].add(keyword)
                        has_match = True

                # 如果有匹配，添加到结果中
                if has_match:
                    if file_name.endswith(('.h', '.hpp')):
                        result['headers'].append(file_path)
                    elif file_name.endswith(('.cc', '.cpp')):
                        result['sources'].append(file_path)

        return result

    def _camel_to_snake(self, name):
        """驼峰命名转下划线命名"""
        s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
        return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

    def _find_implementation_for_interface(self, idl_file_path, interface_info):
        """为接口寻找实现文件"""
        interface_name = interface_info.get('interface_name', '')
        implemented_as = interface_info.get('ImplementedAs', '')
        methods = interface_info.get('method', [])
        attributes = interface_info.get('attribute', [])

        # 使用增强的ImplementedAs提取来补充原始数据
        enhanced_implemented_as_map = self._extract_implemented_as_from_idl_enhanced(idl_file_path)
        enhanced_implemented_as = enhanced_implemented_as_map.get(f"interface:{interface_name}")

        # 如果增强提取找到了ImplementedAs，优先使用它
        if enhanced_implemented_as and not implemented_as:
            implemented_as = enhanced_implemented_as

        # 获取IDL文件所在目录
        idl_dir = os.path.dirname(idl_file_path)

        # 添加 ImplementedAs 作为搜索关键词
        search_keywords = set([interface_name])
        if implemented_as:
            search_keywords.add(implemented_as)

        # 首先尝试寻找文件名相同/相似的实现文件
        impl_files = self._find_impl_files_in_directory(idl_dir, implemented_as or interface_name)

        # 如果找到了文件，验证其中是否包含匹配的接口/类/函数
        if impl_files['headers'] or impl_files['sources']:
            all_files = impl_files['headers'] + impl_files['sources']
            file_data = self._parse_impl_file(all_files[0]) if all_files else {}

            # 检查是否找到了匹配的类名
            matched_classes = []
            for keyword in search_keywords:
                if keyword in file_data['classes']:
                    matched_classes.append(keyword)

            if matched_classes:
                return {
                    'files': all_files,
                    'matched_classes': matched_classes,
                    'matched_functions': [],  # 这里可以进一步分析函数匹配
                    'search_method': 'filename_match'
                }

        # 如果文件名匹配失败，搜索目录中的所有文件寻找匹配的函数/类
        search_result = self._search_all_impl_files_in_dir(
            idl_dir, interface_name, methods, attributes, implemented_as
        )

        if search_result['headers'] or search_result['sources']:
            return {
                'files': search_result['headers'] + search_result['sources'],
                'matched_classes': list(search_result['matched_classes']),
                'matched_functions': list(search_result['matched_functions']),
                'search_method': 'content_match'
            }

        return {
            'files': [],
            'matched_classes': [],
            'matched_functions': [],
            'search_method': 'not_found'
        }

    def _find_implementation_for_other_entity(self, idl_file_path, entity_info, entity_type):
        """为其他实体（字典、枚举、命名空间等）寻找实现文件"""
        entity_name = entity_info.get(f'{entity_type}_name', '')

        # 特殊处理namespace
        if entity_type == 'namespace':
            return self._find_implementation_for_namespace(idl_file_path, entity_info)

        # 获取IDL文件所在目录
        idl_dir = os.path.dirname(idl_file_path)

        # 首先尝试寻找文件名相同/相似的实现文件
        impl_files = self._find_impl_files_in_directory(idl_dir, entity_name)

        return {
            'files': impl_files['headers'] + impl_files['sources'],
            'search_method': 'filename_match' if impl_files['headers'] or impl_files['sources'] else 'not_found'
        }

    def _find_implementation_for_namespace(self, idl_file_path, namespace_info):
        """为namespace寻找实现文件"""
        namespace_name = namespace_info.get('namespace_name', '')
        implemented_as = namespace_info.get('ImplementedAs', '')
        methods = namespace_info.get('method', [])
        attributes = namespace_info.get('attribute', [])

        # 使用增强的ImplementedAs提取来补充原始数据
        enhanced_implemented_as_map = self._extract_implemented_as_from_idl_enhanced(idl_file_path)
        enhanced_implemented_as = enhanced_implemented_as_map.get(f"namespace:{namespace_name}")

        # 如果增强提取找到了ImplementedAs，优先使用它
        if enhanced_implemented_as and not implemented_as:
            implemented_as = enhanced_implemented_as

        # 获取IDL文件所在目录
        idl_dir = os.path.dirname(idl_file_path)

        # 添加 ImplementedAs 作为搜索关键词
        search_keywords = set([namespace_name])
        if implemented_as:
            search_keywords.add(implemented_as)

        # 首先尝试寻找文件名相同/相似的实现文件
        impl_files = self._find_impl_files_in_directory(idl_dir, implemented_as or namespace_name)

        # 如果找到了文件，验证其中是否包含匹配的类/函数
        if impl_files['headers'] or impl_files['sources']:
            all_files = impl_files['headers'] + impl_files['sources']
            file_data = self._parse_impl_file(all_files[0]) if all_files else {}

            # 检查是否找到了匹配的类名（IDL namespace在C++中通常实现为class）
            matched_classes = []
            for keyword in search_keywords:
                if keyword in file_data['classes']:
                    matched_classes.append(keyword)

            if matched_classes:
                return {
                    'files': all_files,
                    'matched_classes': matched_classes,
                    'matched_functions': [],  # 这里可以进一步分析函数匹配
                    'search_method': 'filename_match'
                }

        # 如果文件名匹配失败，搜索目录中的所有文件寻找匹配的类/函数
        search_result = self._search_all_impl_files_in_dir(
            idl_dir, namespace_name, methods, attributes, implemented_as, entity_type='namespace'
        )

        if search_result['headers'] or search_result['sources']:
            return {
                'files': search_result['headers'] + search_result['sources'],
                'matched_classes': list(search_result['matched_classes']),
                'matched_functions': list(search_result['matched_functions']),
                'search_method': 'content_match'
            }

        return {
            'files': [],
            'matched_classes': [],
            'matched_functions': [],
            'search_method': 'not_found'
        }


def main():
    """主函数"""
    print("🚀 开始直接遍历文件系统收集IDL文件")
    current_dir = os.getcwd()
    print("📁 工作目录:", current_dir)

    # 配置路径
    arkweb_root = args.source
    if not arkweb_root:
        print(f"❌ 需要输入arkweb源码路径.")
        return

    blink_dir = os.path.join(arkweb_root, "src/third_party/blink/renderer")
    output_dir = args.out
    if not output_dir:
        output_dir = os.path.join(current_dir, "result")

    # 直接遍历收集所有IDL文件
    print(f"\n🚀 开始收集IDL文件...")
    if not os.path.exists(blink_dir):
        print(f"❌ 目录不存在: {blink_dir}")
        return

    direct_idl_files = collect_all_idl_files(blink_dir)
    os.makedirs(output_dir, exist_ok=True)

    # 保存直接遍历收集的IDL文件列表
    idl_list_output_file = os.path.join(output_dir, "all_idl_files.txt")
    with open(idl_list_output_file, 'w', encoding='utf-8') as f:
        for file_path in sorted(direct_idl_files):
            f.write(f"{file_path}\n")
    print(f"💾idl列表结果已保存到: {idl_list_output_file}")

    print("🚀 开始IDL实现代码映射...")
    processor = ConsolidatedIDLProcessor(os.path.join(arkweb_root, "src/out", args.target, "gen/third_party/blink/renderer"))

    # 处理IDL文件并映射到生成文件
    mapped_idl_gen_data = processor.process_all_files(idl_list_output_file, output_dir)

    # 创建映射器
    mapper = ImplementationMapper(mapped_idl_gen_data)

    # 执行映射
    mapped_data = mapper.map_idl_to_implementations()

    # 保存结果
    output_file = os.path.join(output_dir, "idl_to_gen_and_impl_files.json")
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(mapped_data, f, ensure_ascii=False, indent=2)

    print(f"💾 映射结果已保存到: {output_file}")

    # 生成报告
    report = mapper.generate_report(mapped_data)
    report_file = os.path.join(output_dir, "idl_to_impl_report.json")
    with open(report_file, 'w', encoding='utf-8') as f:
        json.dump(report, f, ensure_ascii=False, indent=2)

    print(f"📋 映射报告已保存到: {report_file}")

    # 打印统计结果
    print(f"\n📊 映射统计:")
    print(f"  📁 处理的IDL文件: {report['total_idl_files']}")
    print(f"  🔧 找到实现文件的IDL: {report['files_with_implementation']} ({report['implementation_success_rate']})")
    print(f"  🏗️ 找到生成文件的IDL: {report['files_with_generated_files']} ({report['generated_success_rate']})")
    print(f"  📋 总实现文件数: {report['total_implementation_files']}")
    print(f"  📋 总生成文件数: {report['total_generated_files']}")

    print(f"\n✅ IDL实现代码映射完成！")

if __name__ == "__main__":
    main()