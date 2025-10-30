import subprocess
import os
import sys
import time
from pathlib import Path

# --- Configuration (请根据您的环境修改以下两个路径) ---

# 1. 搜索目录 (SEARCH_DIR): 脚本将在此目录下递归查找所有 .rej 文件。
#    ⚠️ 请务必修改为您希望搜索 .rej 文件的根目录路径。
current_working_dir = Path.cwd()
SEARCH_DIR = current_working_dir / "google_apis"
SEARCH_DIR = SEARCH_DIR.resolve()

# 2. CLI 工作目录根 (CLI_CWD_ROOT): 
#    - 这是用于计算给 'gemini -p' 的相对路径的基准。
#    - 这也是 'gemini' 子进程的执行目录 (cwd)。
#    - 关键: 确保 SEARCH_DIR 或其父目录是 CLI_CWD_ROOT 的子目录，以正确计算相对路径。
CLI_CWD_ROOT = Path(os.getcwd()).resolve()

# 每次调用之间的延迟（秒），用于避免 API 速率限制
DELAY_SECONDS = 5

# CLI 命令 Prompt (用于单个文件解冲突)
RESOLVE_CONFLICT_PROMPT = "/arkweb:resolve_rej"
# CLI 命令 Prompt (用于报告生成)
REPORT_PROMPT = "/arkweb:generate_summary_report"


def call_gemini_cli(rej_file_path: Path, cli_cwd_root: Path) -> bool:
    """
    调用 Gemini CLI 命令来解决单个 .rej 冲突。
    :param rej_file_path: .rej 文件的完整路径。
    :param cli_cwd_root: CLI 命令的基准工作目录。
    :return: 成功（退出码 0）返回 True，否则返回 False。
    """
    try:
        # 1. 检查目标文件（去掉 .rej 后缀的文件）是否存在
        target_file_path = rej_file_path.with_suffix('')
        if not target_file_path.exists():
            print(f"    [SKIP] 目标文件 {target_file_path.name} 不存在，跳过处理。")
            return False

        # 2. 计算相对于 CLI_CWD_ROOT 的相对路径
        relative_path = rej_file_path.relative_to(cli_cwd_root)
        relative_path_str = str(relative_path)
        
        # 3. 构造完整的命令行 Prompt
        # 格式必须是: /arkweb:resolve_rej <相对路径>
        prompt_content = f"{RESOLVE_CONFLICT_PROMPT} {relative_path_str}"
        
        # 4. 构建命令行参数
        command = [
            'gemini',
            '-p', prompt_content,
            '-d', # 开启调试模式，提供更多上下文
            '--approval-mode=auto_edit', # 自动应用修改
        ]

        print(f"    [CMD] 正在尝试解决: {relative_path_str}")

        # 5. 运行子进程，关键是设置 cwd
        result = subprocess.run(
            command,
            cwd=cli_cwd_root, 
            capture_output=True,
            check=False, # 允许非零退出码
            text=True,
            encoding='utf-8',
            timeout=300 # 5 分钟超时时间
        )

        if result.returncode == 0:
            print(f"    [✅] 成功解决并应用补丁 (Exit Code 0)。")
            return True
        else:
            print(f"    [❌] 失败 (Exit Code {result.returncode})。请人工检查 {relative_path_str}")
            # 简要打印错误输出
            if result.stderr.strip():
                print(f"        Stderr (前200字符):\n{result.stderr.strip()[:200]}...")
            return False

    except FileNotFoundError:
        print("    [❌] 致命错误: 'gemini' 命令未找到。请确保 Gemini CLI 已安装并配置在 PATH 中。")
        return False
    except ValueError as ve:
        print(f"    [❌] 路径计算错误 (ValueError): {ve}")
        print(f"    ❗ 检查 .rej 文件是否在 CLI 工作目录 '{cli_cwd_root}' 之下。")
        return False
    except Exception as e:
        print(f"    [❌] 发生未知错误: {e}")
        return False


def call_gemini_report_cli(search_dir: Path, cli_cwd_root: Path) -> bool:
    """
    调用 Gemini CLI 命令，传递 SEARCH_DIR 的相对路径，让 CLI 负责文件遍历和报告生成。
    
    :param search_dir: 包含 .rej.json 文件的目录的绝对路径。
    :param cli_cwd_root: CLI 命令的基准工作目录 (cwd)。
    :return: 成功（退出码 0）返回 True，否则返回 False。
    """
    
    # 1. 计算相对于 CLI_CWD_ROOT 的相对路径，这是传递给 CLI 的参数
    try:
        relative_dir = search_dir.relative_to(cli_cwd_root)
        relative_dir_str = str(relative_dir)
    except ValueError as ve:
        print(f"❌ 路径计算错误 (ValueError): {ve}")
        print(f"❗ 检查搜索目录 '{search_dir}' 是否在 CLI 工作目录 '{cli_cwd_root}' 之下。")
        return False
    
    report_filename_base = search_dir.name  # 例如: google_apis
    report_filename = f"{report_filename_base}_allrej.md"
    # 最终输出路径相对于 CLI_CWD_ROOT (例如: google_apis/google_apis_allrej.md)
    relative_output_path = relative_dir / report_filename

    # 2. 构造完整的命令行 Prompt
    # 格式: /arkweb:generate_summary_report <相对路径>
    prompt_content = f"{REPORT_PROMPT} {relative_dir_str}"
    
    # 3. 构建命令行参数 (不再需要 -o，因为 TOML 假定 CLI 内部处理输出路径)
    command = [
        'gemini',
        '-p', prompt_content,
        '-d', # 开启调试模式，提供更多上下文
        '--approval-mode=auto_edit', # 自动应用修改（这里仅为保持风格一致）
    ]

    print(f"--- 📝 启动总报告生成 ---")
    print(f"  [CMD] 正在调用: gemini -p '{prompt_content}' ...")
    
    # 4. 运行子进程
    try:
        result = subprocess.run(
            command,
            cwd=cli_cwd_root, 
            capture_output=True,
            check=False, # 允许非零退出码
            text=True,
            encoding='utf-8',
            timeout=600 # 10 分钟超时时间
        )

        # 5. 检查结果 (注意：TOML 中报告路径是 SEARCH_DIR/SEARCH_DIR_allrej.md)
        # 报告文件应该生成在 SEARCH_DIR 内部
        report_filename = f"{search_dir.name}/{search_dir.name}_allrej.md"
        
        if result.returncode == 0:
            print(f"  [✅] 成功完成报告生成 (Exit Code 0)。")
            print(f"  请检查文件: {os.path.join(cli_cwd_root.as_posix(), report_filename)}")
            return True
        else:
            print(f"  [❌] 报告生成失败 (Exit Code {result.returncode})。")
            if result.stderr.strip():
                print(f"    Stderr (前500字符):\n{result.stderr.strip()[:500]}...")
            return False

    except FileNotFoundError:
        print("  [❌] 致命错误: 'gemini' 命令未找到。请确保 Gemini CLI 已安装并配置在 PATH 中。")
        return False
    except Exception as e:
        print(f"  [❌] 发生未知错误: {e}")
        return False


def main():
    """主执行逻辑：查找文件并循环调用 CLI，并在最后生成报告。"""
    if not SEARCH_DIR.is_dir():
        print(f"❌ 错误: 搜索路径 '{SEARCH_DIR}' 不是一个有效的目录。")
        sys.exit(1)
        
    print(f"--- 🚀 AI 冲突解决开始 🚀 ---")
    print(f"搜索目录: {SEARCH_DIR.name}/")
    print(f"CLI 相对路径基准 (cwd): {CLI_CWD_ROOT}")

    # 查找所有 .rej 文件，并排序
    all_rej_files = sorted(list(SEARCH_DIR.glob('**/*.rej')))
    
    if not all_rej_files:
        print("\nℹ️ 未找到任何 .rej 文件，脚本结束。")
        # 即使没有 .rej 文件，也尝试生成报告，以防之前有残留的 .rej.json
        call_gemini_report_cli(SEARCH_DIR, CLI_CWD_ROOT) 
        sys.exit(0)

    total_count = len(all_rej_files)
    print(f"\nℹ️ 找到 {total_count} 个 .rej 文件，开始处理...")
    
    success_count = 0
    failure_count = 0
    
    for i, rej_file in enumerate(all_rej_files):
        print(f"\n--- 处理文件 {i + 1}/{total_count} ---")
        
        try:
            if call_gemini_cli(rej_file, CLI_CWD_ROOT):
                success_count += 1
            else:
                failure_count += 1
        except KeyboardInterrupt: 
            print("\n🛑 用户中断。")
            break
        
        # 每次调用后等待，除非是最后一个文件
        if i < total_count - 1:
            print(f"    [INFO] 等待 {DELAY_SECONDS} 秒...")
            time.sleep(DELAY_SECONDS) 

    print("\n--- 🏁 冲突解决摘要 🏁 ---")
    print(f"总共尝试解决文件: {total_count}")
    print(f"✅ 成功解决数量: {success_count}")
    print(f"❌ 待人工复核数量: {failure_count}")
    print("请手动检查剩余未被删除的 .rej 文件。")
    
    # --- 最终逻辑: 生成汇总报告 ---
    # 在所有文件处理完成后调用简化的报告生成函数
    call_gemini_report_cli(SEARCH_DIR, CLI_CWD_ROOT)


if __name__ == "__main__":
    main()
