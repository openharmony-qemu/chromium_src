import subprocess
import os
import sys
import time
from pathlib import Path
import shlex # 导入 shlex 模块，用于正确地引用命令行参数

# --- Configuration (请根据您的环境修改以下路径) ---

# 1. 搜索目录 (SEARCH_DIR): 脚本将在此目录下递归查找所有 .rej 文件。
#    ⚠️ 请务必修改为您希望搜索 .rej 文件的根目录路径。
current_working_dir = Path.cwd()
SEARCH_DIR = current_working_dir / "base"
SEARCH_DIR = SEARCH_DIR.resolve()

# 2. CLI 工作目录根 (CLI_CWD_ROOT): 
#    - 这是用于计算给 'claude' 的相对路径的基准。
#    - 这也是 'claude' 子进程的执行目录。
#    - 关键: 确保 SEARCH_DIR 或其父目录是 CLI_CWD_ROOT 的子目录，以正确计算相对路径。
CLI_CWD_ROOT = Path(os.getcwd()).resolve()

# 3. Claude Prompt TOML 文件路径 (相对于脚本位置)
#    脚本将根据自身位置自动计算这些文件的完整路径。
#    假设脚本位于 /home/sujiahao/code/arkweb_141/chromium_src/src
#    TOML 文件位于 .claude/commands/arkweb/ 目录下。
SCRIPT_DIR = Path(__file__).resolve().parent
RESOLVE_REJ_TOML_PATH = SCRIPT_DIR / ".claude" / "commands" / "arkweb" / "resolve_rej.toml"
REPORT_TOML_PATH = SCRIPT_DIR / ".claude" / "commands" / "arkweb" / "generate_summary_report.toml"

# 每次调用之间的延迟（秒），用于避免 API 速率限制
DELAY_SECONDS = 5

# Claude CLI 允许的工具集
ALLOWED_TOOLS = "Read(**),Edit(**),Write(**),Bash(**),WebFetch,Search(**),Glob(**),MCP"

# --- TOML 读取函数 ---
# Python 3.11+ 内置 tomllib，旧版本需要安装 toml
try:
    import tomllib
except ImportError:
    try:
        import tomli as tomllib
    except ImportError:
        print("❌ 致命错误: 需要安装 'tomli' 库来解析 TOML 文件 (Python < 3.11)。")
        print("   请运行: pip install tomli")
        sys.exit(1)

def load_prompt_from_toml(toml_path: Path) -> str:
    """
    从指定的 TOML 文件中加载 prompt 模板。
    修改为直接从顶层读取 'prompt' 字段。
    """
    if not toml_path.is_file():
        print(f"❌ 错误: Prompt TOML 文件未找到: {toml_path}")
        print(f"   请检查脚本位置和 TOML 文件路径是否正确。")
        sys.exit(1)
    try:
        with open(toml_path, 'rb') as f: # tomllib 需要二进制模式读取
            data = tomllib.load(f)
        # 提取 prompt 字段 (直接从顶层获取)
        prompt = data.get('prompt', '')
        if not prompt:
            print(f"❌ 错误: 在 {toml_path} 中未找到顶层的 'prompt' 字段。")
            sys.exit(1)
        return prompt
    except Exception as e:
        print(f"❌ 错误: 解析 TOML 文件 {toml_path} 时发生错误: {e}")
        sys.exit(1)


def call_claude_cli(rej_file_path: Path, cli_cwd_root: Path, prompt_template: str) -> bool:
    """
    调用 Claude CLI 命令来解决单个 .rej 冲突。
    :param rej_file_path: .rej 文件的完整路径。
    :param cli_cwd_root: CLI 命令的基准工作目录。
    :param prompt_template: 从 TOML 文件中读取的 prompt 模板。
    :return: 成功（退出码 0）返回 True，否则返回 False。
    """
    try:
        # 1. 检查目标文件（去掉 .rej 后缀的文件）是否存在
        target_file_path = rej_file_path.with_suffix('')
        if not target_file_path.exists():
            print(f"    [SKIP] 目标文件 {target_file_path.name} 不存在，跳过处理。")
            return False

        # 2. 计算相对于 CLI_CWD_ROOT 的相对路径
        relative_rej_path = rej_file_path.relative_to(cli_cwd_root)
        relative_target_path = target_file_path.relative_to(cli_cwd_root)
        
        # 3. 使用实际路径格式化 prompt 模板
        #    根据新的 TOML，占位符是 {args1} 和 {args2}
        try:
            # TOML 中的 {{args1}} 会被读取为 '{args1}'，所以这里用 'args1' 作为键
            prompt_content = prompt_template.format(
                args1=str(relative_rej_path),
                args2=str(relative_target_path)
            )
        except KeyError as e:
            print(f"    [❌] Prompt 模板格式化失败，缺少占位符: {e}")
            print(f"    ❗ 请检查 {RESOLVE_REJ_TOML_PATH.name} 中的 prompt 是否包含 {{args1}} 和 {{args2}}。")
            return False

        # 4. 构建命令行参数 (列表形式，用于 subprocess.run)
        command_list = [
            'claude',
            '-p', prompt_content,
            '--model', 'GLM-4.6',
            '--allowedTools', ALLOWED_TOOLS,
        ]

        # 5. 构建用于打印的命令字符串 (使用 shlex.join，会自动添加引号)
        #    这会生成类似您期望的格式: claude ... -p '...' --allowedTools "..."
        command_str_for_print = shlex.join(command_list)

        print(f"    [CMD] 正在尝试解决: {relative_rej_path}")
        print(f"    [CMD] 命令: {command_str_for_print}")

        # 6. 运行子进程，关键是设置 cwd (仍然使用列表形式的 command_list)
        result = subprocess.run(
            command_list,
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
            print(f"    [❌] 失败 (Exit Code {result.returncode})。请人工检查 {relative_rej_path}")
            # 简要打印错误输出
            if result.stderr.strip():
                print(f"        Stderr (前200字符):\n{result.stderr.strip()[:200]}...")
            return False

    except FileNotFoundError:
        print("    [❌] 致命错误: 'claude' 命令未找到。请确保 Claude CLI 已安装并配置在 PATH 中。")
        return False
    except ValueError as ve:
        print(f"    [❌] 路径计算错误: {ve}")
        print(f"    ❗ 检查 .rej 文件是否在 CLI 工作目录 '{cli_cwd_root}' 之下。")
        return False
    except Exception as e:
        print(f"    [❌] 发生未知错误: {e}")
        return False


def call_claude_report_cli(search_dir: Path, cli_cwd_root: Path, prompt_template: str) -> bool:
    """
    调用 Claude CLI 命令，传递 SEARCH_DIR 的相对路径，让 CLI 负责文件遍历和报告生成。
    
    :param search_dir: 包含 .rej.json 文件的目录的绝对路径。
    :param cli_cwd_root: CLI 命令的基准工作目录。
    :param prompt_template: 从 TOML 文件中读取的 prompt 模板。
    :return: 成功（退出码 0）返回 True，否则返回 False。
    """
    
    # 1. 计算相对于 CLI_CWD_ROOT 的相对路径，这是传递给 CLI 的参数
    try:
        relative_dir_str = str(search_dir.relative_to(cli_cwd_root))
    except ValueError as ve:
        print(f"❌ 路径计算错误: {ve}")
        print(f"❗ 检查搜索目录 '{search_dir}' 是否在 CLI 工作目录 '{cli_cwd_root}' 之下。")
        return False
    
    # 2. 使用实际路径格式化 prompt 模板
    #    根据新的 TOML，占位符是 {{args}}
    try:
        # TOML 中的 {{args}} 会被读取为 '{args}'，所以这里用 'args' 作为键
        prompt_content = prompt_template.format(args=relative_dir_str)
    except KeyError as e:
        print(f"  [❌] 报告 Prompt 模板格式化失败，缺少占位符: {e}")
        print(f"  ❗ 请检查 {REPORT_TOML_PATH.name} 中的 prompt 是否包含 {{args}}。")
        return False
    
    # 3. 构建命令行参数 (列表形式，用于 subprocess.run)
    command_list = [
        'claude',
        '-p', prompt_content,
        '--model', 'GLM-4.6',
        '--allowedTools', ALLOWED_TOOLS,
    ]

    # 4. 构建用于打印的命令字符串
    command_str_for_print = shlex.join(command_list)

    print(f"--- 📝 启动总报告生成 ---")
    print(f"  [CMD] 正在调用: {command_str_for_print}")
    
    # 5. 运行子进程 (仍然使用列表形式的 command_list)
    try:
        result = subprocess.run(
            command_list,
            cwd=cli_cwd_root, 
            capture_output=True,
            check=False, # 允许非零退出码
            text=True,
            encoding='utf-8',
            timeout=600 # 10 分钟超时时间
        )

        # 6. 检查结果
        # Claude CLI 的报告路径由 prompt 内部逻辑决定，这里我们只检查退出码
        if result.returncode == 0:
            print(f"  [✅] 成功完成报告生成 (Exit Code 0)。")
            print(f"  请检查 Claude CLI 输出或 prompt 中指定的报告文件位置。")
            return True
        else:
            print(f"  [❌] 报告生成失败 (Exit Code {result.returncode})。")
            if result.stderr.strip():
                print(f"    Stderr (前500字符):\n{result.stderr.strip()[:500]}...")
            return False

    except FileNotFoundError:
        print("  [❌] 致命错误: 'claude' 命令未找到。请确保 Claude CLI 已安装并配置在 PATH 中。")
        return False
    except Exception as e:
        print(f"  [❌] 发生未知错误: {e}")
        return False


def main():
    """主执行逻辑：查找文件并循环调用 CLI，并在最后生成报告。"""
    if not SEARCH_DIR.is_dir():
        print(f"❌ 错误: 搜索路径 '{SEARCH_DIR}' 不是一个有效的目录。")
        sys.exit(1)
        
    # --- 加载 Prompt 模板 ---
    print("--- 📋 加载 Prompt 模板 ---")
    resolve_prompt_template = load_prompt_from_toml(RESOLVE_REJ_TOML_PATH)
    report_prompt_template = load_prompt_from_toml(REPORT_TOML_PATH)
    print("✅ Prompt 模板加载成功。")

    print(f"\n--- 🚀 AI 冲突解决开始 🚀 ---")
    print(f"搜索目录: {SEARCH_DIR.name}/")
    print(f"CLI 相对路径基准: {CLI_CWD_ROOT}")

    # 查找所有 .rej 文件，并排序
    all_rej_files = sorted(list(SEARCH_DIR.glob('**/*.rej')))
    
    if not all_rej_files:
        print("\nℹ️ 未找到任何 .rej 文件，脚本结束。")
        # 即使没有 .rej 文件，也尝试生成报告
        call_claude_report_cli(SEARCH_DIR, CLI_CWD_ROOT, report_prompt_template) 
        sys.exit(0)

    total_count = len(all_rej_files)
    print(f"\nℹ️ 找到 {total_count} 个 .rej 文件，开始处理...")
    
    success_count = 0
    failure_count = 0
    
    for i, rej_file in enumerate(all_rej_files):
        print(f"\n--- 处理文件 {i + 1}/{total_count} ---")
        
        try:
            if call_claude_cli(rej_file, CLI_CWD_ROOT, resolve_prompt_template):
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
    # 在所有文件处理完成后调用报告生成函数
    call_claude_report_cli(SEARCH_DIR, CLI_CWD_ROOT, report_prompt_template)


if __name__ == "__main__":
    main()
