import datetime
import queue
import re
import subprocess
import threading
import time
from utils import ReportTbl, ReadConfigure, SafeGetVal, FindGroupbyID, FindGroupbyEMail

"""
# 安装 openpyxl
> pip3 install --trusted-host mirrors.huaweicloud.com -i https://mirrors.huaweicloud.com/repository/pypi/simple openpyxl openpyxl
# 执行分析脚本
> python src/cef/ohos_cef_ext/tools/stat_diff.py
# 导出文件为 当前目录下的 report.xlsx
"""

## 常量
exclude = 'src/third_party/ohos_ndk' # 排除 ohos_ndk
file_exts = ['cc', 'cpp', 'h']
rows_head = ('文件路径', '领域', 'PL', '责任人', '修改行数', '与基线不同行数', '提交id', '提交日期', '提交消息')
ignore_list = [
    'Created-by:',
    'Author-id:',
    'MR-id:',
    'Commit-by:',
    'Merged-by:',
    'E2E-issues:',
    'Description:',
    'Change-Id:',
    'Signed-off-by:', 
    'TicketNo:', 
    'Team:', 
    'Feature or Bugfix:', 
    'Binary Source:', 
    'PrivateCode(Yes/No):']

def produceProc(git_tool, task_queue,local,dir):
    # 分析两个目录 并根据单文件分析线程数设置结束信号
    for i in range(0, len(local)):
        git_tool.diffDir(task_queue, local[i], dir[i])

# 分析线程
def childProc(git_tool, task_queue, idx, dir_report):
    while True:
        item = task_queue.get()
        if item is None:  # 通过放入None来表示结束信号
            print(f"{idx}:子线程退出")
            break
        print(f"{idx}:子线程分析项目: {item}")

        (change_count, dir_a, dir_b, xdir, file_name) = item
        fname_arr = git_tool.getFulName(dir_a, xdir, file_name)  # 获取完整路径
        # print(f'fname_arr:{fname_arr}')
        for fname in fname_arr:
            record = None
            if len(fname) == 0 : 
                record = (file_name, "", '', 'unknow', -1, change_count)  # 单项分析结果：已删除的
            else:
                diff_file_name = fname.replace(dir_a,dir_b)
                command = ['diff', fname, diff_file_name] # 利用diff比较，排查除未变更的文件
                result = str(subprocess.run(command, capture_output=True, text=True).stdout)
                if len(result) != 0:
                    record = git_tool.getResult(fname, change_count)  # 单项分析结果
            
            if record != None:
                dir_report.append(record)  # 加入单项分析结果
                # print(f"{idx}:finished: {record}")

class GitTool:
    def __init__(self):
        self.conf = ReadConfigure()


    def codeSync(self, repo, branch, git_path_dir, commitid='', cherry_pick=''):
        """
         git下载代码到指定目录
         > git_tool.codeSync('https://szv-open.codehub.huawei.com/innersource/shanhai/wutong/oh-chromium/chromium_cef.git', 'chromium_132.0.6789.0', '../chromium_cef')
        """
        command = ['git', '-C', git_path_dir, 'branch']
        result = str(subprocess.run(command, capture_output=True, text=True).stdout)
        if branch == result.replace('*', '').strip():
            print("代码已同步")
        else:
            command = ['git', 'clone', repo, '-b', branch, git_path_dir]
            result = str(subprocess.run(command, capture_output=True, text=True).stdout)

        if len(commitid):
            command = ['git', '-C', git_path_dir, 'reset', '--hard', commitid]
            result = str(subprocess.run(command, capture_output=True, text=True).stdout)

        if len(cherry_pick):
            arr = cherry_pick.split(',')
            for i in range(0, len(arr)):
                command = ['git', '-C', git_path_dir, 'cherry-pick', arr[i]]
                result = str(subprocess.run(command, capture_output=True, text=True).stdout)
        print("SUCC: codeSync")

    def diffDir(self, file_stats, dir_a, dir_b):
        """
         比较指定目录下文件
         > git_tool.diffDir('src/cef/libcef', '../chromium_cef/libcef')
        """
        command = ['git', 'diff', '--stat', dir_b, dir_a]
        result = str(subprocess.run(command, capture_output=True, text=True).stdout)
        for line in result.splitlines():
            line = line.strip()
            match = re.match(r'(.*)\|\s+(\d+)\s+([+-]+)', line)
            if match:
                file_name = match.group(1)
                file_name = file_name.replace('}', '').replace('=> /dev/null', '').strip().split('/')
                xdir = ''
                if len(file_name) >2:
                    xdir = file_name[-2]
                file_name = file_name[-1]  # 仅获取文件名 不包含目录
                change_count = int(match.group(2))
                if len(file_name.split('.')) > 1 and file_name.split('.')[1] in file_exts:
                    file_stats.put((change_count, dir_a, dir_b, xdir, file_name))

    def getFulName(self, dir_a, xdir, fname):
        """
         获取完整路径
        > a.getFulName('src/cef/libcef', 'mojom', 'BUILD.gn')
         'src/cef/libcef/common/mojom/BUILD.gn'
        """
        find_command = ['find', dir_a, '-name', fname]
        file_name = str(subprocess.run(find_command, capture_output=True, text=True).stdout).strip()
        arr = file_name.split('\n')
        arr = [x for x in arr if exclude not in x]
        if len(xdir):
            arr = [x for x in arr if len(x.split('/')) < 2 or x.split('/')[-2] == xdir]
        return arr

    def getResult(self, file_name, in_change_count):
        """
         分析单个文件修改历史记录
         > git_tool.getResult('src/cef/libcef/common/mojom/BUILD.gn', 0)
         [文件路径 责任人 修改行数 总修改行数 与基线不同行数 提交id 提交日期 提交消息]
        """
        git_command = ['git', 'log', '--stat', file_name]
        log_result = str(subprocess.run(git_command, capture_output=True, text=True).stdout)
        # log_result += '\n'
        author_stats = [] # 单笔修改改动的信息列表
        for record in log_result.split('\ncommit'):
            lines = record.splitlines()
            if len(lines) > 6:
                match = re.match(r'(\S+)\s+\|\s+(\d+)\s+([+-]+)', lines[-2].strip())
                if match:
                    change_count = int(match.group(2))
                    commitid = lines[0].replace('commit ', '').strip()
                    # 责任人，pl，所属部门
                    auth = lines[1].replace('Author: ', '').strip()
                    pl = ''
                    group = ''
                    keys = re.findall(r'[a-z]{1,2}[0-9]{7,8}', auth) # 提取工号
                    if len(keys):
                        auth,pl,group = FindGroupbyID(keys[0])
                    else :
                        auth,pl,group = FindGroupbyEMail(auth)

                    date = lines[2].replace('Date:  ', '').replace('+0800', '').strip()
                    t = datetime.datetime.strptime(date, "%a %b %d %H:%M:%S %Y")
                    # 转成本地时间的字符串
                    date = t.strftime("%Y-%m-%d %H:%M:%S")
                    # 提交信息过滤
                    msg_arr = lines[3:-2]
                    for item in ignore_list :
                        msg_arr = [x for x in msg_arr if item not in x]
                    msg = '\n'.join(msg_arr).strip()
                    if '解耦' not in msg and auth != '30007565':
                        author_stats.append((change_count, auth, pl, group, commitid, date, msg)) # 单笔修改改动的信息
                else:
                    print('75->', lines[-2].strip())
                    print('75->', lines[-3].strip())
                    print('=============')
                    print('75->', record)  # 部分修改不包含 +- 行数信息 比如merge进来的更改
                    print('=============')

        # 移除第一笔提交FindGroupbyID
        if len(author_stats) > 1:
            author_stats = author_stats[:-1]

        if len(author_stats) > 0:
            author_stats.sort(reverse=True)
            change_count, auth, pl,group, commitid, date, msg = author_stats[0]
            # print(f'-> {file_name} {commitid}')
            return file_name, group, pl, auth, change_count, in_change_count, commitid, date, msg
        return file_name, '', '', '', -1, -1

    def exec(self):
        THREADNUM = 5

        # 自动生成报告
        tbl = ReportTbl()

        time.time()
        BEGIN_TIME = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) #开始时间
        summ_data = [('', ''),
                ('', '自动生成报告'),
                ('', '')]

        for item in self.conf['COMPARE_DIR']:
            # 需分析的文件
            dir_report = []
            task_queue = queue.Queue()

            name = SafeGetVal(item, "name")
            repo = SafeGetVal(item, "repo")
            branch = SafeGetVal(item, "branch")
            commitid = SafeGetVal(item, "commitid")
            cherry_pick = SafeGetVal(item, "cherry-pick")
            targetdir = SafeGetVal(item, "targetdir")

            local = SafeGetVal(item, "local").split(',')
            dir = SafeGetVal(item, "dir").split(',')
            if len(local) != len(dir):
                print(f'项目{name} 配置错误 COMPARE_DIR.local 与 COMPARE_DIR.dir 中的目录个数不一致')
                continue

            # 源码同步
            self.codeSync(repo, branch, targetdir, commitid, cherry_pick)

            # 1 个生产者线程
            produce_thread = threading.Thread(target=produceProc, args=(self, task_queue,local,dir))
            produce_thread.start()

            # THREADNUM 个消费者线程
            thread_arr = []
            for i in range(0, THREADNUM):
                consumer_thread = threading.Thread(target=childProc, args=(self, task_queue, i, dir_report))
                consumer_thread.start()
                thread_arr.append(consumer_thread)

            # 等待分析线程结束
            produce_thread.join()            
            for i in range(0, THREADNUM):
                task_queue.put(None)  # 结束信号
            for consumer_thread in thread_arr:
                consumer_thread.join()

            report = dir_report
            report.sort(key=lambda tup: tup[5], reverse=True)
            all_line_cnt = 0
            for item in report:
                all_line_cnt += int(item[5])

            report.insert(0, rows_head)
            tbl.addSheet(report, name)  # 添加sheet
            print('项目报告生成成功：', name)

            summ_data.append(('项目：', name))
            summ_data.append(('目录', ','.join(local)))
            summ_data.append(('累计修改行数', all_line_cnt))
            summ_data.append(('比较目录', ','.join(dir)))
            summ_data.append(('比较源码', repo))
            summ_data.append(('分支', branch))
            summ_data.append(('提交号', commitid))
            summ_data.append(rows_head)
            arr = report[1:4]
            for record in arr:
                summ_data.append(record)
            summ_data.append(('', ''))

        time.time()
        END_TIME =time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) #结束时间
        summ_data.append(('开始时间：', BEGIN_TIME, '完成时间：', END_TIME))
        tbl.addFirstSheet(summ_data, '总览')  # 首页sheet
        tbl.save('report.xlsx')  # 写入报告


def test():
    git_tool = GitTool()
    print(git_tool.getFulName('src/third_party', 'range.join.iterator', 'types.h'))

def autoExec():
    git_tool = GitTool()
    git_tool.exec()

if __name__ == '__main__':
    test_flag = False
    if test_flag:
        test()
    else:
        autoExec()
