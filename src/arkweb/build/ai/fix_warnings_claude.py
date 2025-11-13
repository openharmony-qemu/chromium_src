import subprocess
import sys
import time
from pathlib import Path
import argparse
import csv
from collections import defaultdict

# --- Configuration (请根据您的环境修改以下路径) ---

SCRIPT_DIR = Path(__file__).resolve().parent

# 运行 claude CLI 的工作目录根
CLI_CWD_ROOT = SCRIPT_DIR.parent.parent.parent.resolve()

# 项目基础路径（用于拼接相对文件路径）
BASE_PATH = SCRIPT_DIR.parent.parent.parent.parent.parent.resolve()

# Prompt Markdown 文件路径
FIX_WARNINGS_PROMPT_PATH = SCRIPT_DIR.parent.parent.parent / ".claude" / "commands" / "arkweb" / "fix_build_warning.md"
REPORT_PROMPT_PATH = SCRIPT_DIR.parent.parent.parent / ".claude" / "commands" / "arkweb" / "generate_warning_summary_report.md"

# 默认CSV报告路径
DEFAULT_CSV_PATH = SCRIPT_DIR / "warnings_report.csv"

# 每次调用之间的延迟（秒）
DELAY_SECONDS = 5

# Claude CLI 允许的工具集
ALLOWED_TOOLS = "Read(**),Edit(**),Write(**),Bash(**),WebFetch,Search(**),Glob(**),MCP"

def load_prompt_from_md(md_path: Path) -> str:
    """读取 Markdown Prompt 模板全文."""
    if not md_path.is_file():
        print(f"ERROR: Prompt file not found: {md_path}")
        sys.exit(1)
    try:
        content = md_path.read_text(encoding="utf-8")
        if not content.strip():
            print(f"ERROR: Prompt file is empty: {md_path}")
            sys.exit(1)
        return content
    except Exception as e:
        print(f"ERROR: Failed to read prompt file: {md_path} -> {e}")
        sys.exit(1)

def parse_warnings_csv(csv_path: Path) -> list:
    warnings = []
    if not csv_path.is_file():
        print(f"ERROR: CSV file not found: {csv_path}")
        sys.exit(1)
    try:
        with open(csv_path, 'r', encoding='utf-8-sig') as file:
            reader = csv.DictReader(file)
            for row in reader:
                warnings.append({
                    'file_path': row['文件路径'].strip(),
                    'warning_type': row['警告类型'].strip(),
                    'warning_desc': row['警告的说明'].strip(),
                    'suggestion': row['修复建议'].strip(),
                    'line_numbers': row['行号'].strip(),
                    'count': int(row['出现次数'])
                })
    except Exception as e:
        print(f"ERROR: Failed to parse CSV file: {e}")
        sys.exit(1)
    return warnings

def group_warnings_by_file(warnings: list) -> dict:
    file_warnings = defaultdict(list)
    for warning in warnings:
        file_warnings[warning['file_path']].append(warning)
    return dict(file_warnings)

def call_claude_cli(file_path: str, warnings: list, cli_cwd_root: Path, base_path: Path,
                    prompt_template: str, model: str = 'glm-4.6') -> bool:
    try:
        warnings_summary_lines = []
        for w in warnings:
            warnings_summary_lines.append(f"类型: {w['warning_type']}")
            warnings_summary_lines.append(f"描述: {w['warning_desc']}")
            warnings_summary_lines.append(f"行号: {w['line_numbers']}")
            warnings_summary_lines.append(f"建议: {w['suggestion']}")
            warnings_summary_lines.append("---")
        warnings_text = "\n".join(warnings_summary_lines)

        try:
            prompt_content = prompt_template.format(
                args1=str(base_path),
                args2=file_path,
                args3=warnings_text
            )
        except KeyError as e:
            print(f"    [FAIL] Missing placeholder in prompt template: {e}")
            return False

        command_list = [
            'claude',
            '-p', prompt_content,
            '--model', model,
            '--allowedTools', ALLOWED_TOOLS,
            '-d'
        ]
        print(f"    [CMD] 修复文件: {file_path}")

        result = subprocess.run(
            command_list,
            cwd=cli_cwd_root,
            capture_output=True,
            text=True,
            encoding='utf-8',
            timeout=600
        )
        if result.returncode == 0:
            print("    [PASS] Fix successful")
            return True
        else:
            print(f"    [FAIL] Fix failed (Exit {result.returncode})")
            if result.stderr.strip():
                print(f"        Stderr:\n{result.stderr.strip()[:200]}...")
            return False
    except FileNotFoundError:
        print("    [FAIL] 'claude' command not found")
        return False
    except Exception as e:
        print(f"    [FAIL] Unknown error: {e}")
        return False

def call_claude_report_cli(csv_path: Path, results: dict, cli_cwd_root: Path,
                           base_path: Path, prompt_template: str, model: str = 'glm-4.6') -> bool:
    try:
        lines = [
            "处理结果摘要:",
            f"总文件数: {results['total_files']}",
            f"成功修复: {results['success_count']}",
            f"修复失败: {results['failure_count']}",
            f"跳过处理: {results['skip_count']}",
            ""
        ]
        if results['success_files']:
            lines.append("Successfully fixed files:")
            lines += [f"  [PASS] {p}" for p in results['success_files']]
            lines.append("")
        if results['failure_files']:
            lines.append("Failed to fix files:")
            lines += [f"  [FAIL] {p}" for p in results['failure_files']]
            lines.append("")
        if results['skip_files']:
            lines.append("Skipped files:")
            lines += [f"  [SKIP] {p}" for p in results['skip_files']]
        results_text = "\n".join(lines)

        prompt_content = prompt_template.format(
            args1=base_path,
            args2=str(csv_path),
            args3=results_text
        )

        command_list = [
            'claude',
            '-p', prompt_content,
            '--model', model,
            '--allowedTools', ALLOWED_TOOLS,
            '-d'
        ]
        print("--- Generating summary report ---")
        result = subprocess.run(
            command_list,
            cwd=cli_cwd_root,
            capture_output=True,
            text=True,
            encoding='utf-8',
            timeout=1200
        )
        if result.returncode == 0:
            report_output = result.stdout.strip()
            if not report_output:
                print("  [WARN] Report content is empty")
            else:
                print("  [PASS] Report content:")
                print("--------------------------------------------------")
                print(report_output)
                print("--------------------------------------------------")
            return True
        else:
            print(f"  [FAIL] Report generation failed (Exit {result.returncode})")
            if result.stderr.strip():
                print(f"    Stderr:\n{result.stderr.strip()[:500]}...")
            return False
    except Exception as e:
        print(f"  [FAIL] Unknown error: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description='AI 警告修复脚本')
    parser.add_argument('--csv', type=str, help='警告CSV路径')
    parser.add_argument('--model', type=str, default='glm-4.6', help='模型名称')
    args = parser.parse_args()
    model = args.model
    csv_path = Path(args.csv) if args.csv else DEFAULT_CSV_PATH
    if not csv_path.is_file():
        print(f"ERROR: CSV file does not exist: {csv_path}")
        sys.exit(1)

    print("--- Loading prompts (Markdown) ---")
    fix_prompt_template = load_prompt_from_md(FIX_WARNINGS_PROMPT_PATH)
    report_prompt_template = load_prompt_from_md(REPORT_PROMPT_PATH)
    print("[PASS] Prompts loaded successfully")

    print(f"\n--- Starting fix process ---")
    print(f"Script: {SCRIPT_DIR}")
    print(f"CSV: {csv_path}")
    print(f"BASE: {BASE_PATH}")
    print(f"Model: {model}")

    print("\n--- Parsing CSV ---")
    warnings = parse_warnings_csv(csv_path)
    print(f"[PASS] Found {len(warnings)} warning records")

    file_warnings = group_warnings_by_file(warnings)
    print(f"[PASS] Affecting {len(file_warnings)} files")
    if not file_warnings:
        print("[INFO] No warnings found, exiting.")
        sys.exit(0)

    total_files = len(file_warnings)
    success_count = failure_count = skip_count = 0
    success_files, failure_files, skip_files = [], [], []

    for i, file_path in enumerate(sorted(file_warnings.keys())):
        print(f"\n--- 处理 {i+1}/{total_files} ---")
        print(f"文件: {file_path}")
        warnings_in_file = file_warnings[file_path]
        print(f"包含 {len(warnings_in_file)} 条警告")

        if 'chromium_ext' not in file_path:
            print(f"    [SKIP] Skipping non-chromium_ext related file.")
            skip_count += 1
            skip_files.append(file_path)
            continue
        try:
            ok = call_claude_cli(file_path, warnings_in_file, CLI_CWD_ROOT, BASE_PATH,
                                 fix_prompt_template, model)
            if ok:
                success_count += 1
                success_files.append(file_path)
            else:
                failure_count += 1
                failure_files.append(file_path)
        except KeyboardInterrupt:
            print("\n[STOP] Process interrupted by user.")
            break
        if i < total_files - 1:
            print(f"    等待 {DELAY_SECONDS}s...")
            time.sleep(DELAY_SECONDS)

    results = {
        'total_files': total_files,
        'success_count': success_count,
        'failure_count': failure_count,
        'skip_count': skip_count,
        'success_files': success_files,
        'failure_files': failure_files,
        'skip_files': skip_files
    }

    print("\n--- Summary ---")
    print(f"Total files: {total_files}  Success: {success_count}  Failed: {failure_count}  Skipped: {skip_count}")

    call_claude_report_cli(csv_path, results, CLI_CWD_ROOT, BASE_PATH, report_prompt_template, model)

if __name__ == "__main__":
    main()