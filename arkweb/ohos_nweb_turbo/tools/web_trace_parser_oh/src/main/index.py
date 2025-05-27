from src.main.interface.basic_trace_info import BasicTraceInfo
from src.main.interface.main_process_trace_info_statistic import (
    MainProcessTraceStatistic
)
from src.main.interface.statistic_unit import StatisticUnit
from src.main.interface.main_process_init import MainProcessInitInfo
from src.tools.trace_handler import (
    get_trace_line_time,
    get_url_by_trace_line,
    get_function_time,
    calculate_single_task_time,
    get_id_by_trace_line
)

from src.network.interface.network_timing_info import NetworkTimingInfo
from src.main.key_trace_point import (
    render_key_function,
    step_begin_main_resource,
    step_end_TouchUp2NWeb,
    step_end_MainResource,
    step_end_CommitMainResource,
    step_create_nweb,
    step_EvaluateScript,
    step_v8_callFunction,
    step_dom_content_loaded_event,
    step_swap_buffers,
    step_begin_subresource_net,
    step_connect_start,
    step_connect_end,
    request_time_step,
    response_time_step,
    response_end_step,
    step_end_subresource_net,
    useragent_changed,
    first_paint,
    first_contentful_paint
)
from src.tools.excel_writer import write_excel_xls_append, write_excel_map


# 主要加载流程
# 获取主要加载流程中的关键trace信息
def get_basic_trace_info(
        lines: list[str],
        start_step_condition_list: list[str],
        end_step: str,
        file_name: str
) -> BasicTraceInfo:
    basic_trace_info = BasicTraceInfo()
    basic_trace_info.case_name = file_name
    for line in lines:
        # 获取开始时间点
        if all(condition in line for condition in start_step_condition_list):
            basic_trace_info.start_time = get_trace_line_time(line)

        # 获取结束时间点
        if line.find(end_step) != -1:
            basic_trace_info.end_time = get_trace_line_time(line)

        # 获取render进程id和进程名
        if (
                basic_trace_info.render_process_name == "" and
                basic_trace_info.render_process_id == "" and
                any(key_function in line for key_function in render_key_function)
        ):
            process_name_id: str = line.split('(')[0]
            process_name_id_list: list[str] = process_name_id.strip().split("-")

            length: int = len(process_name_id_list)
            # 如果只有两项 则说明包名比较规范，数组里只有进程名和进程id
            if length == 2:
                basic_trace_info.render_process_id = process_name_id_list[1]
                basic_trace_info.render_process_name = process_name_id_list[0]
            # 如果大于两项，则说明包名不规范，使用特殊手段获取进程id与进程名称
            else:
                basic_trace_info.render_process_id = process_name_id_list[length - 1]
                basic_trace_info.render_process_name = process_name_id.replace('-' + basic_trace_info.render_process_id, '')

        url = get_url_by_trace_line(line)
        if (
                any(key_step in line for key_step in step_begin_main_resource) and
                url != "" and
                url != "about:blank"
        ):
            basic_trace_info.main_url = get_url_by_trace_line(line)

        # commit结束
        if (
                any(key_step in line for key_step in step_end_CommitMainResource)
        ):
            basic_trace_info.commit_end = get_trace_line_time(line)

        # commit后首帧
        if line.find(step_swap_buffers) != -1:
            time = get_trace_line_time(line)
            if (
                    basic_trace_info.first_swap_buffer_after_commit == 0 and
                    0 < basic_trace_info.commit_end < time
            ):
                basic_trace_info.first_swap_buffer_after_commit = time
            basic_trace_info.last_swap_buffer_time = time

        # fp
        if line.find(first_paint) != -1:
            basic_trace_info.first_paint = get_trace_line_time(line)

        # fcp
        if line.find(first_contentful_paint) != -1:
            basic_trace_info.first_contentful_paint = get_trace_line_time(line)

        # if (
        #         basic_trace_info.end_time and
        #         basic_trace_info.main_url and
        #         basic_trace_info.render_process_id and
        #         basic_trace_info.render_process_name
        # ):
        #     break

    if basic_trace_info.end_time == 0:
        print("未找到结束时间，用最后一个swapBuffer时间替代")
        basic_trace_info.end_time = basic_trace_info.last_swap_buffer_time

    return basic_trace_info


def get_main_process_init_info(lines, basic_trace_info: BasicTraceInfo) -> MainProcessInitInfo:
    main_process_init_info = MainProcessInitInfo()
    main_url = basic_trace_info.main_url
    for line in lines:
        # 响应
        if (
                main_process_init_info.create_nweb_start == 0 and
                any(condition in line for condition in step_end_TouchUp2NWeb)
        ):
            main_process_init_info.create_nweb_start = get_trace_line_time(line)

        # 主资源开始请求
        if (
                main_process_init_info.main_resource_start == 0 and
                any(key_step in line for key_step in step_begin_main_resource)
        ):
            main_process_init_info.main_resource_start = get_trace_line_time(line)
            print(f"main_resource_start: {get_trace_line_time(line)}")
            print(f"main_resource_start line: {line}")

        if not main_url:
            continue
        # 主资源网络线程开始请求
        if (
                main_process_init_info.main_resource_net_start == 0 and
                line.find(main_url) != -1 and
                line.find(step_begin_subresource_net) != -1
        ):
            main_process_init_info.main_resource_net_start = get_trace_line_time(line)
            print(f"main_resource_net_start : {main_process_init_info.main_resource_net_start}")

        # 主资源网络线程开始连接
        if (
                main_process_init_info.main_resource_connect_start == 0 and
                line.find(main_url) != -1 and
                line.find(step_connect_start) != -1
        ):
            main_process_init_info.main_resource_connect_start = get_trace_line_time(line)
            print(f"main_resource_connect_start : {main_process_init_info.main_resource_connect_start}")

        # 主资源网络线程连接完成
        if (
                main_process_init_info.main_resource_connect_end == 0 and
                line.find(main_url) != -1 and
                line.find(step_connect_end) != -1
        ):
            main_process_init_info.main_resource_connect_end = get_trace_line_time(line)
            print(f"main_resource_connect_end : {main_process_init_info.main_resource_connect_end}")

        # 主资源网络线程服务器开始响应
        if (
                main_process_init_info.main_resource_response_start == 0 and
                line.find(main_url) != -1 and
                line.find(request_time_step) != -1
        ):
            main_process_init_info.main_resource_response_start = get_trace_line_time(line)
            print(f"main_resource_response_start : {main_process_init_info.main_resource_response_start}")

        # if line.find("http://www.baidu.com/?from=1001192y") != -1:
        #     print(f"line: {line}")
        #     print(f"1: {main_process_init_info.main_resource_download_start == 0}")
        #     print(f"2: {line.find(main_url) != -1}")
        #     print(f"3: {line.find(response_time_step) != -1}")
        # 主资源网络线程服务器响应结束
        if (
                main_process_init_info.main_resource_download_start == 0 and
                line.find(main_url) != -1 and
                line.find(response_time_step) != -1
        ):
            main_process_init_info.main_resource_download_start = get_trace_line_time(line)
            print(f"main_resource_download_start : {main_process_init_info.main_resource_download_start}")

        # 主资源网络线程下载结束
        if (
                main_process_init_info.main_resource_download_end == 0 and
                line.find(main_url) != -1 and
                line.find(response_end_step) != -1
        ):
            main_process_init_info.main_resource_download_end = get_trace_line_time(line)
            print(f"main_resource_download_end : {main_process_init_info.main_resource_download_end}")

        # 主资源网络线程下载结束
        if (
                main_process_init_info.main_resource_net_end == 0 and
                line.find(main_url) != -1 and
                line.find(step_end_subresource_net) != -1
        ):
            main_process_init_info.main_resource_net_end = get_trace_line_time(line)
            print(f"main_resource_net_end : {main_process_init_info.main_resource_net_end}")

        # 主资源请求结束 / 开始commit
        if (
                main_process_init_info.main_resource_end == 0 and
                line.find(main_url) != -1 and
                any(key_step in line for key_step in step_end_MainResource)
        ):
            main_process_init_info.main_resource_end = get_trace_line_time(line)
            print(f"main_resource_end: {get_trace_line_time(line)}")
            print(f"main_resource_end line: {line}")
        # commit结束
        if (
                main_process_init_info.commit_end == 0 and
                any(key_step in line for key_step in step_end_CommitMainResource)
        ):
            main_process_init_info.commit_end = get_trace_line_time(line)

        if (
                line.find(useragent_changed) != -1
        ):
            main_process_init_info.useragent_changed = True
        # DCL时间
        if (
                main_process_init_info.dom_content_loaded_start == 0 and
                line.find(step_dom_content_loaded_event) != -1
        ):
            main_process_init_info.dom_content_loaded_start = get_trace_line_time(line)

        if (
                main_process_init_info.create_nweb_start and
                main_process_init_info.main_resource_start and
                main_process_init_info.main_resource_end and
                main_process_init_info.commit_end and
                main_process_init_info.main_resource_net_start and
                main_process_init_info.main_resource_net_end and
                main_process_init_info.main_resource_connect_start and
                main_process_init_info.main_resource_connect_end and
                main_process_init_info.main_resource_response_start and
                main_process_init_info.main_resource_download_start and
                main_process_init_info.main_resource_download_end
        ):
            break

    return main_process_init_info


def get_main_process_statistic(
        lines,
        basic_trace_info: BasicTraceInfo,
        main_process_init_info: MainProcessInitInfo
) -> MainProcessTraceStatistic:
    main_process_trace_statistic = MainProcessTraceStatistic(basic_trace_info.main_url)
    main_process_trace_statistic.case_name = basic_trace_info.case_name
    main_process_trace_statistic.touch_up_to_lcp_time = (
            basic_trace_info.end_time - basic_trace_info.start_time
    ) * 1000

    if basic_trace_info.first_paint != 0:
        main_process_trace_statistic.touch_up_to_white_screen_end = (
            basic_trace_info.first_paint - basic_trace_info.start_time
        ) * 1000
    elif basic_trace_info.first_swap_buffer_after_commit != 0:
        main_process_trace_statistic.touch_up_to_white_screen_end = (
            basic_trace_info.first_swap_buffer_after_commit - basic_trace_info.start_time
        ) * 1000

    print(f'create_nweb_start: {main_process_init_info.create_nweb_start}')
    print(f'start_time: {basic_trace_info.start_time}')
    print(f'end_time: {basic_trace_info.end_time}')
    # 从开始到createNWeb耗时
    if (
            main_process_init_info.create_nweb_start <= basic_trace_info.start_time or
            main_process_init_info.create_nweb_start >= basic_trace_info.end_time
    ):
        print('createNWeb时间点异常，页面可能使用了离线组件')
        main_process_trace_statistic.touch_up_to_nweb_time = 0
    else:
        main_process_trace_statistic.touch_up_to_nweb_time = (
            main_process_init_info.create_nweb_start - basic_trace_info.start_time
        ) * 1000

    # createNWeb耗时
    main_process_trace_statistic.nweb_create_time = get_function_time(lines, step_create_nweb)

    print('开始解析js执行时间...')
    # EvaluateScript耗时
    main_process_trace_statistic.evaluate_script_time = calculate_single_task_time(
        lines,
        step_EvaluateScript,
        basic_trace_info.start_time,
        basic_trace_info.end_time,
        basic_trace_info.render_process_id
    )

    # # 首帧EvaluateScript耗时
    # main_process_trace_statistic.first_frame_evaluate_script_time = calculate_single_task_time(
    #     lines,
    #     step_EvaluateScript,
    #     basic_trace_info.start_time,
    #     basic_trace_info.first_swap_buffer_after_commit,
    #     basic_trace_info.render_process_id
    # )

    # v8.callFunction耗时
    main_process_trace_statistic.v8_call_function_time = calculate_single_task_time(
        lines,
        step_v8_callFunction,
        basic_trace_info.start_time,
        basic_trace_info.end_time,
        basic_trace_info.render_process_id
    )

    # # 首帧v8.callFunction耗时
    # main_process_trace_statistic.first_frame_v8_call_function_time = calculate_single_task_time(
    #     lines,
    #     step_v8_callFunction,
    #     basic_trace_info.start_time,
    #     basic_trace_info.first_swap_buffer_after_commit,
    #     basic_trace_info.render_process_id
    # )

    if main_process_init_info.dom_content_loaded_start:
        main_process_trace_statistic.start_to_dcl_time = (
            main_process_init_info.dom_content_loaded_start -
            basic_trace_info.start_time
        ) * 1000

    if not basic_trace_info.main_url:
        print("未找到完整的主资源url")
        main_process_trace_statistic.show_info()
        return main_process_trace_statistic

    # 主资源连接耗时
    if (
            main_process_init_info.main_resource_connect_start == 0 or
            main_process_init_info.main_resource_connect_end == 0 or
            main_process_init_info.main_resource_connect_start > main_process_init_info.main_resource_connect_end
    ):
        # print('主资源时间点异常，页面可能存在iframe')
        main_process_trace_statistic.main_resource_connect_time = 0
    else:
        main_process_trace_statistic.main_resource_connect_time = (
                main_process_init_info.main_resource_connect_end - main_process_init_info.main_resource_connect_start
        ) * 1000

    # 主资源服务器响应耗时
    if (
            main_process_init_info.main_resource_response_start == 0 or
            main_process_init_info.main_resource_download_start == 0 or
            main_process_init_info.main_resource_response_start > main_process_init_info.main_resource_download_start
    ):
        # print('主资源时间点异常，页面可能存在iframe')
        main_process_trace_statistic.main_resource_response_time = 0
    else:
        main_process_trace_statistic.main_resource_response_time = (
                main_process_init_info.main_resource_download_start -
                main_process_init_info.main_resource_response_start
        ) * 1000

    # 主资源服务器响应耗时
    if (
            main_process_init_info.main_resource_download_start == 0 or
            main_process_init_info.main_resource_download_end == 0 or
            main_process_init_info.main_resource_download_start > main_process_init_info.main_resource_download_end
    ):
        # print('主资源时间点异常，页面可能存在iframe')
        main_process_trace_statistic.main_resource_download_time = 0
    else:
        main_process_trace_statistic.main_resource_download_time = (
                main_process_init_info.main_resource_download_end -
                main_process_init_info.main_resource_download_start
        ) * 1000

    # 主资源网络线程总耗时
    if (
            main_process_init_info.main_resource_net_end == 0 or
            main_process_init_info.main_resource_net_start == 0 or
            main_process_init_info.main_resource_net_start > main_process_init_info.main_resource_net_end
    ):
        # print('主资源时间点异常，页面可能存在iframe')
        main_process_trace_statistic.main_resource_net_time = 0
    else:
        main_process_trace_statistic.main_resource_net_time = (
                main_process_init_info.main_resource_net_end -
                main_process_init_info.main_resource_net_start
        ) * 1000

    # 主资源耗时
    if (
            main_process_init_info.main_resource_end == 0 or
            main_process_init_info.main_resource_start == 0
    ):
        print('主资源时间点异常，页面可能存在iframe')
        main_process_trace_statistic.main_resource_time = 0
    else:
        main_process_trace_statistic.main_resource_time = (
                main_process_init_info.main_resource_end - main_process_init_info.main_resource_start
        ) * 1000

    # commit耗时
    if main_process_init_info.main_resource_end and main_process_init_info.commit_end:
        main_process_trace_statistic.commit_time = (
                main_process_init_info.commit_end - main_process_init_info.main_resource_end
        ) * 1000

    # 渲染时间耗时： lcp时间 - 响应耗时（点击到组件创建） - createNWeb耗时 - 主资源耗时 - commit耗时
    main_process_trace_statistic.render_time = (
        main_process_trace_statistic.touch_up_to_lcp_time -
        main_process_trace_statistic.touch_up_to_nweb_time -
        main_process_trace_statistic.nweb_create_time -
        main_process_trace_statistic.main_resource_time -
        main_process_trace_statistic.commit_time
    )

    main_process_trace_statistic.useragent_changed = main_process_init_info.useragent_changed

    return main_process_trace_statistic


def parse_and_statistic_main_process(
        lines: list[str],
        basic_trace_info: BasicTraceInfo
) -> [MainProcessInitInfo, MainProcessTraceStatistic]:
    print(f'\n开始解析主流程-----------------------------')
    main_process_init_info = get_main_process_init_info(lines, basic_trace_info)
    main_process_trace_statistic = get_main_process_statistic(
        lines,
        basic_trace_info,
        main_process_init_info
    )

    return main_process_init_info, main_process_trace_statistic


def trace_statistic_info_write_to_excel(
        path: str,
        sheet_name: str,
        statistic_data_map_list: list[StatisticUnit]
):
    end_line_num = 1
    for data_map in statistic_data_map_list:
        title = data_map.title
        key_map = data_map.key_map
        data_map = data_map.data_map
        write_excel_xls_append(path, sheet_name, end_line_num, 1, title)
        end_line_num += 1
        write_excel_map(path, sheet_name, key_map, data_map, end_line_num)
        end_line_num += 4

# def get_framework_init_time(main_process_init_info, sub_resource_info_map: dict[str, dict]):
#     # 取出符合时间要求的图片子资源信息
#     first_image_info = {}
#     for url, sub_resource in sub_resource_info_map.items():
#         if any(suffix in url for suffix in ['.png', '.jpg', '.jpeg', 'svg']):
#             first_image_info = sub_resource
#
#     # TODO: 计算时间
