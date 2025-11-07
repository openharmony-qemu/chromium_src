#!/usr/bin/env python3
import json
import os
import glob
from collections import defaultdict, Counter

def analyze_all_json_files():
    content_dir = "/home/sujiahao/code/arkweb_141/src/claude_out/report/content/"
    json_files = glob.glob(os.path.join(content_dir, "*.json"))

    total_files = 0
    successful_resolutions = 0
    failed_resolutions = 0
    total_hunks = 0
    resolved_hunks = 0
    unresolved_hunks = 0

    risk_level_counts = Counter()
    conflict_type_counts = Counter()
    function_module_counts = Counter()
    resolution_status_counts = Counter()

    high_risk_files = []
    medium_risk_files = []
    low_risk_files = []

    detailed_files = []

    for json_file in json_files:
        try:
            with open(json_file, 'r', encoding='utf-8') as f:
                data = json.load(f)

            total_files += 1

            # 提取基本信息
            file_path = data.get('file_path', 'Unknown')
            summary = data.get('summary', {})

            total_hunks += summary.get('total_hunks', 0)
            resolved_hunks += summary.get('resolved_hunks', 0)
            unresolved_hunks += summary.get('unresolved_hunks', 0)

            # 分析hunks
            hunks = data.get('hunks_analysis', [])
            file_risk_levels = []

            for hunk in hunks:
                risk_level = hunk.get('risk_level', '未知')
                resolution_status = hunk.get('resolution_status', '未知')
                conflict_cause = hunk.get('conflict_cause', '')

                file_risk_levels.append(risk_level)
                risk_level_counts[risk_level] += 1
                resolution_status_counts[resolution_status] += 1

                # 分析冲突类型
                if 'OpenHarmony' in conflict_cause or 'OHOS' in conflict_cause:
                    conflict_type_counts['OpenHarmony平台适配'] += 1
                elif 'Chromium' in conflict_cause or 'upgrade' in conflict_cause:
                    conflict_type_counts['版本升级冲突'] += 1
                elif 'dependency' in conflict_cause:
                    conflict_type_counts['依赖冲突'] += 1
                else:
                    conflict_type_counts['其他冲突'] += 1

            # 分析功能模块
            if 'app' in file_path:
                function_module_counts['应用层'] += 1
            elif 'browser' in file_path:
                function_module_counts['浏览器核心'] += 1
            elif 'renderer' in file_path:
                function_module_counts['渲染器'] += 1
            elif 'gpu' in file_path:
                function_module_counts['GPU处理'] += 1
            elif 'common' in file_path:
                function_module_counts['公共组件'] += 1
            elif 'public' in file_path:
                function_module_counts['公共接口'] += 1
            else:
                function_module_counts['其他模块'] += 1

            # 判断文件解决状态
            if summary.get('unresolved_hunks', 0) == 0 and summary.get('total_hunks', 0) > 0:
                successful_resolutions += 1
            elif summary.get('total_hunks', 0) > 0:
                failed_resolutions += 1

            # 确定文件风险等级（取最高风险）
            file_risk = '低风险'
            if '高风险' in file_risk_levels:
                file_risk = '高风险'
                high_risk_files.append({
                    'file_path': file_path,
                    'risk_level': file_risk,
                    'total_hunks': summary.get('total_hunks', 0),
                    'resolved_hunks': summary.get('resolved_hunks', 0),
                    'unresolved_hunks': summary.get('unresolved_hunks', 0)
                })
            elif '中风险' in file_risk_levels:
                file_risk = '中风险'
                medium_risk_files.append({
                    'file_path': file_path,
                    'risk_level': file_risk,
                    'total_hunks': summary.get('total_hunks', 0),
                    'resolved_hunks': summary.get('resolved_hunks', 0),
                    'unresolved_hunks': summary.get('unresolved_hunks', 0)
                })
            else:
                low_risk_files.append({
                    'file_path': file_path,
                    'risk_level': file_risk,
                    'total_hunks': summary.get('total_hunks', 0),
                    'resolved_hunks': summary.get('resolved_hunks', 0),
                    'unresolved_hunks': summary.get('unresolved_hunks', 0)
                })

            detailed_files.append({
                'file_path': file_path,
                'total_hunks': summary.get('total_hunks', 0),
                'resolved_hunks': summary.get('resolved_hunks', 0),
                'unresolved_hunks': summary.get('unresolved_hunks', 0),
                'risk_level': file_risk,
                'resolution_status': '完全解决' if summary.get('unresolved_hunks', 0) == 0 else '部分解决'
            })

        except Exception as e:
            print(f"Error processing {json_file}: {e}")
            continue

    return {
        'total_files': total_files,
        'successful_resolutions': successful_resolutions,
        'failed_resolutions': failed_resolutions,
        'total_hunks': total_hunks,
        'resolved_hunks': resolved_hunks,
        'unresolved_hunks': unresolved_hunks,
        'risk_level_counts': dict(risk_level_counts),
        'conflict_type_counts': dict(conflict_type_counts),
        'function_module_counts': dict(function_module_counts),
        'resolution_status_counts': dict(resolution_status_counts),
        'high_risk_files': sorted(high_risk_files, key=lambda x: x['unresolved_hunks'], reverse=True),
        'medium_risk_files': sorted(medium_risk_files, key=lambda x: x['unresolved_hunks'], reverse=True),
        'low_risk_files': sorted(low_risk_files, key=lambda x: x['unresolved_hunks'], reverse=True),
        'detailed_files': sorted(detailed_files, key=lambda x: (x['risk_level'], x['unresolved_hunks']), reverse=True)
    }

if __name__ == "__main__":
    analysis_result = analyze_all_json_files()
    print(json.dumps(analysis_result, ensure_ascii=False, indent=2))