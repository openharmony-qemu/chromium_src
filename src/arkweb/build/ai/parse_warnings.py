import re
import csv
import argparse
from collections import defaultdict
from pathlib import Path

def load_suggestion_config(config_path: Path):
    if not config_path.is_file():
        return {}
    try:
        try:
            import tomllib
        except ImportError:
            import tomli as tomllib
        with open(config_path, 'rb') as f:
            data = tomllib.load(f)
        return data
    except Exception:
        print(f"加载建议配置文件时出错: {Exception}")
        return {}

SUGGESTION_CONF = load_suggestion_config(
    Path(__file__).parent / 'warning_suggestions.toml'
)

def get_fix_suggestion(warning_type, warning_desc):
    """根据警告类型和描述生成修复建议"""
    if warning_type in SUGGESTION_CONF:
        suggestion = SUGGESTION_CONF[warning_type].get('suggestion')
        if suggestion:
            return suggestion
    default_suggestion = SUGGESTION_CONF.get('general', {}).get('suggestion', '无修复建议，请参考告警说明进行修复。')
    return default_suggestion

def process_log_to_csv(log_file, output_csv_file):
    """
    从原始日志文件读取、解析、合并警告，并生成最终的CSV报告。
    """
    try:
        with open(log_file, 'r', encoding='utf-8') as file:
            lines = file.readlines()
    except FileNotFoundError:
        print(f"错误：找不到日志文件 {log_file}")
        return
    except Exception as e:
        print(f"读取文件时出错：{e}")
        return

    # key: (警告类型, 警告说明), value: {'count': 次数, 'locations': {文件: {行号集合}}, 'suggestion': 建议}
    merged_warnings = defaultdict(lambda: {'count': 0, 'locations': defaultdict(set)})

    # 正则表达式，用于从单行中直接提取所有信息
    warning_line_re = re.compile(r'^(?P<file>.+?):(?P<line>\d+):\d+:\s+warning:\s+(?P<desc>.+?)\s+\[(?P<type>-W.+?)\]$')
    
    total_warnings_found = 0

    for line in lines:
        match = warning_line_re.match(line.strip())
        if not match:
            continue
        
        total_warnings_found += 1
        
        # 从匹配中提取信息
        file_path = match.group('file')
        line_no = match.group('line')
        warning_desc = match.group('desc').strip()
        warning_type = match.group('type').strip()

        # 修改文件路径，去掉开头的../../, 改为src/
        if file_path.startswith('../../'):
            file_path = 'src/' + file_path[6:]
        
        # 使用(类型, 描述, 文件路径)作为合并的键
        key = (warning_type, warning_desc, file_path)
        
        # 累加计数并记录位置
        merged_warnings[key]['count'] += 1
        merged_warnings[key]['locations'][file_path].add(line_no)
        
        # 仅在第一次遇到此键时存储建议
        if 'suggestion' not in merged_warnings[key]:
             merged_warnings[key]['suggestion'] = get_fix_suggestion(warning_type, warning_desc)

    # 按出现次数对合并后的警告进行排序
    sorted_warnings = sorted(merged_warnings.items(), key=lambda item: item[1]['count'], reverse=True)

    # 将合并后的数据转换为用于写入CSV的列表格式
    # 每个警告类型和文件组合占一行
    output_data = []
    row_index = 1

    for (warning_type, warning_desc, file_path), data in sorted_warnings:
        suggestion = data.get('suggestion', 'N/A')
        file_count = data['count']  # 该文件在日志中的出现次数

        # 获取该文件的所有行号
        line_nums = data['locations'][file_path]
        sorted_lines = sorted(list(line_nums), key=int)
        lines_str = ', '.join(sorted_lines)

        output_data.append([
            row_index,
            warning_type,
            warning_desc,
            suggestion,
            file_path,
            lines_str,
            file_count
        ])
        row_index += 1

    # 写入CSV文件
    try:
        with open(output_csv_file, 'w', newline='', encoding='utf-8-sig') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(['序号', '警告类型', '警告的说明', '修复建议', '文件路径', '行号', '出现次数'])
            writer.writerows(output_data)
        
        print(f"处理完成。共发现 {total_warnings_found} 个警告，生成 {len(output_data)} 行记录。")
        print(f"CSV报告已保存到 {output_csv_file}")
    except Exception as e:
        print(f"写入CSV文件时出错: {e}")


if __name__ == "__main__":
     # 使用示例: python parse_warning.py /path/to/build.log --output warnings_report.csv
    parser = argparse.ArgumentParser(description='从编译日志中筛选、合并告警信息，并生成CSV报告。')
    parser.add_argument('log_file', type=str, help='原始编译日志文件路径 (例如 build.log)')
    parser.add_argument('--output', type=str, default='warnings_report.csv', 
                        help='输出的CSV报告文件路径 (默认: warnings_report.csv)')
    
    args = parser.parse_args()
    
    process_log_to_csv(args.log_file, args.output)