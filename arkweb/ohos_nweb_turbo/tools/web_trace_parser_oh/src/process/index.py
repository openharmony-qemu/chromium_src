# 获取当前web render进程的运行状态
from src.tools.trace_handler import get_trace_line_time
from src.process.interface.init import (
    RenderProcessSleepingMap,
    RenderProcessSleepingUnit
)
from src.process.interface.render_process_statistic import (
    RenderProcessSleepingStatistic,
    RenderProcessSleepingStatisticMap
)
from src.tools.excel_writer import write_excel_list
from src.network.interface.init import SubResourceInfo


# 主要包含 Runnable、Running、Sleeping三个状态
# Runnable开始 / Sleeping结束：
# 关键字1：sched_wakeup
# 关键字2：pid=render_process_id
def is_sleeping_end(line: str, render_process_id: str) -> bool:
    if line.find('sched_wakeup') != -1 and line.find('pid=' + render_process_id) != -1:
        return True
    return False


# Runnable结束 / Running开始：
# 关键字1：sched_switch
# 关键字2：render_process_name-render_process_id 开头
# 关键字3：next_pid=27209
# def is_switch_to_running(line) -> bool:


# Running结束 / Sleeping开始
# 关键字1：sched_switch
# 关键字2：prev_pid=render_process_id
def is_sleeping_start(line: str, render_process_id: str) -> bool:
    if line.find('sched_switch') != -1 and line.find('prev_pid=' + render_process_id) != -1:
        return True
    return False


def get_render_process_sleeping_time_list(
        lines: str,
        render_process_id: str,
        start_time: float,
        end_time: float
) -> list[RenderProcessSleepingUnit]:
    sleeping_time_list = []
    process_sleep_unit: RenderProcessSleepingUnit = RenderProcessSleepingUnit()
    for line in lines:
        if line.find("sched_switch") == -1 and line.find('sched_wakeup') == -1:
            continue
        time = get_trace_line_time(line)
        if time == 0:
            continue
        if float(time) < start_time:
            continue
        if float(time) > end_time:
            sleeping_time_list.append(process_sleep_unit)
            break

        if is_sleeping_start(line, render_process_id):
            process_sleep_unit.sleeping_start = time
            continue
        if is_sleeping_end(line, render_process_id):
            process_sleep_unit.sleeping_end = time

            if time and process_sleep_unit.sleeping_start:
                process_sleep_unit.during_time = (time - process_sleep_unit.sleeping_start) * 1000

            sleeping_time_list.append(process_sleep_unit)
            process_sleep_unit = RenderProcessSleepingUnit()
    return sleeping_time_list


def matches_sleep_time_sub_resource(
        sleep_time_list: list[RenderProcessSleepingUnit],
        sub_resource_info_map: dict[str, SubResourceInfo]
) -> list[RenderProcessSleepingUnit]:
    for url, info in sub_resource_info_map.items():
        if not info.request_resource_time or not info.receive_response_time:
            continue

        for sleep_unit in sleep_time_list:
            if not sleep_unit.sleeping_start or not sleep_unit.sleeping_end:
                continue

            if sleep_unit.sleeping_start <= info.net_receive_response_time <= sleep_unit.sleeping_end:
                sleep_unit.matched_resources.append(url)
                break

    return sleep_time_list


def render_process_sleeping_and_matched_sub_resource_statistic(
        sleep_time_list_matched_sub_resource: list[RenderProcessSleepingUnit]
) -> RenderProcessSleepingStatistic:
    render_process_sleeping_statistic = RenderProcessSleepingStatistic()
    max_sleeping_time = 0
    max_sleeping_matched_sub_resource_num = 0
    for info in sleep_time_list_matched_sub_resource:
        render_process_sleeping_statistic.total_sleeping_time += info.during_time
        matched_sub_resource_num = len(info.matched_resources)
        if matched_sub_resource_num > 0:
            render_process_sleeping_statistic.matched_sub_resource_num += matched_sub_resource_num
        if info.during_time > max_sleeping_time:
            max_sleeping_time = info.during_time
            max_sleeping_matched_sub_resource_num = matched_sub_resource_num
    render_process_sleeping_statistic.max_sleeping_during_time = max_sleeping_time
    render_process_sleeping_statistic.max_sleeping_matched_resources = max_sleeping_matched_sub_resource_num

    return render_process_sleeping_statistic


def render_process_sleeping_info_write_to_excel(
        path: str,
        sheet_name: str,
        lines: list[str],
        render_process_id: str,
        start_time: float,
        end_time: float,
        # sub_resource_info_map: dict[str, SubResourceInfo]
) -> RenderProcessSleepingStatistic:

    print("\n开始解析render进程空闲时间-----------------------")
    sleeping_time_list = get_render_process_sleeping_time_list(lines, render_process_id, start_time, end_time)

    # print("开始匹配render进程空闲时间可能相关的子资源...")
    # sleep_time_list_matched_sub_resource = matches_sleep_time_sub_resource(sleeping_time_list, sub_resource_info_map)

    # print(f'sleep_time_list_matched_sub_resource: {sleep_time_list_matched_sub_resource}')
    print(f"开始将render进程空闲时间相关信息写入 excel sheet: {sheet_name}...")
    write_excel_list(
        path,
        sheet_name,
        RenderProcessSleepingMap,
        sleeping_time_list
    )

    # print("开始统计render进程空闲时间与相关子资源")
    render_process_sleeping_statistic = render_process_sleeping_and_matched_sub_resource_statistic(
        sleeping_time_list
    )

    print("render进程空闲时间解析完毕--------------------------")
    return render_process_sleeping_statistic
