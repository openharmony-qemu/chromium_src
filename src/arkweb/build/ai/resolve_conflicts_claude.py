import subprocess
import os
import sys
import time
from pathlib import Path
import shlex
import argparse

# --- Configuration (请根据您的环境修改以下路径) ---

# 1. 搜索目录 (SEARCH_DIR): 脚本将在此目录下递归查找所有 .rej 文件。
SCRIPT_DIR = Path(__file__).resolve().parent
SEARCH_DIR = SCRIPT_DIR.parent.parent.parent / "cc"
SEARCH_DIR = SEARCH_DIR.resolve()

# 2. CLI 工作目录根 (CLI_CWD_ROOT): 
CLI_CWD_ROOT = SCRIPT_DIR.parent.parent.parent.resolve()

# 3. 基础路径 (BASE_PATH): 
BASE_PATH = SCRIPT_DIR.parent.parent.parent.parent.parent.resolve()

# 4. Claude Prompt TOML 文件路径
RESOLVE_REJ_TOML_PATH = SCRIPT_DIR.parent.parent.parent / ".claude" / "commands" / "arkweb" / "resolve_rej.toml"
REPORT_TOML_PATH = SCRIPT_DIR.parent.parent.parent / ".claude" / "commands" / "arkweb" / "generate_summary_report.toml"

# 每次调用之间的延迟（秒）
DELAY_SECONDS = 5

# Claude CLI 允许的工具集 - 确保包含Write权限
ALLOWED_TOOLS = "Read(**),Edit(**),Write(**),Bash(**),WebFetch,Search(**),Glob(**),MCP"

# --- TOML 读取函数 ---
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
    """从指定的 TOML 文件中加载 prompt 模板。"""
    if not toml_path.is_file():
        print(f"❌ 错误: Prompt TOML 文件未找到: {toml_path}")
        sys.exit(1)
    try:
        with open(toml_path, 'rb') as f:
            data = tomllib.load(f)
        prompt = data.get('prompt', '')
        if not prompt:
            print(f"❌ 错误: 在 {toml_path} 中未找到顶层的 'prompt' 字段。")
            sys.exit(1)
        return prompt
    except Exception as e:
        print(f"❌ 错误: 解析 TOML 文件 {toml_path} 时发生错误: {e}")
        sys.exit(1)

def call_claude_cli(rej_file_path: Path, cli_cwd_root: Path, base_path: Path, search_dir: Path, prompt_template: str, model: str = 'qwen-plus') -> bool:
    """调用 Claude CLI 命令来解决单个 .rej 冲突。"""
    try:
        # 1. 检查目标文件是否存在
        target_file_path = rej_file_path.with_suffix('')
        if not target_file_path.exists():
            print(f"    [SKIP] 目标文件 {target_file_path.name} 不存在，跳过处理。")
            return False

        # 2. 计算相对路径
        relative_rej_path = rej_file_path.relative_to(cli_cwd_root)
        relative_target_path = target_file_path.relative_to(cli_cwd_root)

        # 3. 获取搜索目录名
        search_dir_name = search_dir.name

        # 4. 格式化 prompt 模板
        try:
            prompt_content = prompt_template.format(
                args1=str(base_path),
                args2=str(relative_target_path),
                args3=str(relative_rej_path),
                args4=search_dir_name
            )
        except KeyError as e:
            print(f"    [❌] Prompt 模板格式化失败，缺少占位符: {e}")
            return False

        # 5. 构建命令行参数
        command_list = [
            'claude',
            '-p', prompt_content,
            '--model', model,
            '--allowedTools', ALLOWED_TOOLS,
            '-d'
        ]

        # 6. 打印命令
        command_str_for_print = shlex.join(command_list)
        print(f"    [CMD] 正在尝试解决: {relative_rej_path}")
        # print(f"    [CMD] 命令: {command_str_for_print}")

        # 7. 运行子进程
        result = subprocess.run(
            command_list,
            cwd=cli_cwd_root, 
            capture_output=True,
            check=False,
            text=True,
            encoding='utf-8',
            timeout=600
        )

        if result.returncode == 0:
            print(f"    [✅] 成功解决并应用补丁 (Exit Code 0)。")
            return True
        else:
            print(f"    [❌] 失败 (Exit Code {result.returncode})。请人工检查 {relative_rej_path}")
            if result.stderr.strip():
                print(f"        Stderr (前200字符):\n{result.stderr.strip()[:200]}...")
            return False

    except FileNotFoundError:
        print("    [❌] 致命错误: 'claude' 命令未找到。")
        return False
    except Exception as e:
        print(f"    [❌] 发生未知错误: {e}")
        return False

def call_claude_report_cli(search_dir: Path, cli_cwd_root: Path, base_path: Path, prompt_template: str, model: str = 'qwen-plus') -> bool:
    """调用 Claude CLI 命令生成汇总报告。"""
    try:
        search_dir_name = search_dir.name

        prompt_content = prompt_template.format(
            args1=base_path,
            args2=search_dir_name
        )

        command_list = [
            'claude',
            '-p', prompt_content,
            '--model', model,
            '--allowedTools', ALLOWED_TOOLS,
            '-d'
        ]

        command_str_for_print = shlex.join(command_list)
        print(f"--- 📝 启动总报告生成 ---")
        # print(f"  [CMD] 正在调用: {command_str_for_print}")
        
        result = subprocess.run(
            command_list,
            cwd=cli_cwd_root, 
            capture_output=True,
            check=False,
            text=True,
            encoding='utf-8',
            timeout=1200
        )

        if result.returncode == 0:
            print(f"  [✅] 成功完成报告生成 (Exit Code 0)。")
            return True
        else:
            print(f"  [❌] 报告生成失败 (Exit Code {result.returncode})。")
            if result.stderr.strip():
                print(f"    Stderr (前500字符):\n{result.stderr.strip()[:500]}...")
            return False

    except Exception as e:
        print(f"  [❌] 发生未知错误: {e}")
        return False

def main():
    """主执行逻辑：查找文件并循环调用 CLI，并在最后生成报告。"""
    # 添加命令行参数解析
    parser = argparse.ArgumentParser(description='AI 冲突解决脚本')
    parser.add_argument('--model', type=str, default='GLM-4.6', help='指定使用的 Claude 模型 (默认: qwen-plus)')
    args = parser.parse_args()
    model = args.model

    if not SEARCH_DIR.is_dir():
        print(f"❌ 错误: 搜索路径 '{SEARCH_DIR}' 不是一个有效的目录。")
        sys.exit(1)

    # --- 加载 Prompt 模板 ---
    print("--- 📋 加载 Prompt 模板 ---")
    resolve_prompt_template = load_prompt_from_toml(RESOLVE_REJ_TOML_PATH)
    report_prompt_template = load_prompt_from_toml(REPORT_TOML_PATH)
    print("✅ Prompt 模板加载成功。")

    print(f"\n--- 🚀 AI 冲突解决开始 🚀 ---")
    print(f"脚本位置: {SCRIPT_DIR}")
    print(f"搜索目录: {SEARCH_DIR}")
    print(f"CLI 相对路径基准: {CLI_CWD_ROOT}")
    print(f"基础路径: {BASE_PATH}")
    print(f"使用模型: {model}")

    # 查找所有 .rej 文件，并排序
    all_rej_files = sorted(list(SEARCH_DIR.glob('**/*.rej')))
    
    if not all_rej_files:
        print("\nℹ️ 未找到任何 .rej 文件，脚本结束。")
        call_claude_report_cli(SEARCH_DIR, CLI_CWD_ROOT, BASE_PATH, report_prompt_template, model)
        sys.exit(0)

    total_count = len(all_rej_files)
    print(f"\nℹ️ 找到 {total_count} 个 .rej 文件，开始处理...")
    
    success_count = 0
    failure_count = 0
    
    for i, rej_file in enumerate(all_rej_files):
        print(f"\n--- 处理文件 {i + 1}/{total_count} ---")
        
        try:
            if call_claude_cli(rej_file, CLI_CWD_ROOT, BASE_PATH, SEARCH_DIR, resolve_prompt_template, model):
                success_count += 1
            else:
                failure_count += 1
        except KeyboardInterrupt:
            print("\n🛑 用户中断。")
            break
        
        if i < total_count - 1:
            print(f"    [INFO] 等待 {DELAY_SECONDS} 秒...")
            time.sleep(DELAY_SECONDS) 

    print("\n--- 🏁 冲突解决摘要 🏁 ---")
    print(f"总共尝试解决文件: {total_count}")
    print(f"✅ 成功解决数量: {success_count}")
    print(f"❌ 待人工复核数量: {failure_count}")
    
    # --- 验证.rej文件完整性 ---
    print("\n--- 🔍 验证.rej文件完整性 ---")
    remaining_rej_files = list(SEARCH_DIR.glob('**/*.rej'))
    print(f"✅ 保留 {len(remaining_rej_files)} 个.rej文件，保持完整性")
    
    # --- 生成汇总报告 ---
    call_claude_report_cli(SEARCH_DIR, CLI_CWD_ROOT, BASE_PATH, report_prompt_template, model)


if __name__ == "__main__":
    main()