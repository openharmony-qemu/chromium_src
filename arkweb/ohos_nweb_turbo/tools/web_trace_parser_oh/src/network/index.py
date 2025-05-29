from enum import IntFlag
from src.tools.trace_handler import (
    get_url_by_trace_line,
    get_http_version_by_trace_line,
    get_response_code_by_trace_line,
    get_trace_line_time,
    get_id_by_trace_line,
    get_request_id
)
from src.network.interface.network_timing_info import (
    NetworkTimingInfoMap,
    NetworkTimingInfo,
    NetworkCalculatedTimingMap,
    NetworkCalculatedTiming,
    NetWorkStatisticMap,
    NetWorkStatistic
)

from src.network.interface.network_timing_interval import NetWorkTimgInterval

from src.tools.excel_writer import write_excel_data
from src.network.key_trace_point import (
    step_begin_SubResource,
    http_version_step,
    is_intercepted_step,
    request_id_map,
    step_queue_end_1,
    step_queue_end_2,
    response_code_step,
    network_timing_info,
    network_cache_info,
    main_resource_step,
    step_begin_subresource_net,
    intercepted_render_start_receive,
    intercepted_render_receive_complete,
    intercepted_send_data_complete,
    intercepted_start_send_data
)


def parse_network_info(
        path: str,
        sheet_name: str,
        lines: list[str],
        filter_url_list: list[str],
        basic_trace_info
) -> dict[str, NetworkTimingInfo]:
    network_info_map = {}
    temp_network_info_map = {}
    main_network_info = {}
    for line in lines:
        # 主资源开始请求
        if (
                line.find(main_resource_step) != -1
        ):
            # print(f'main_url: {basic_trace_info.main_url}')
            main_network_info = NetworkTimingInfo(
                basic_trace_info.main_url,
                "main_url",
                "GET",
                get_trace_line_time(line)
            )

        # 获取主资源request_id
        if (
            line.find(step_begin_subresource_net) != -1 and
            get_url_by_trace_line(line) == basic_trace_info.main_url
        ):
            request_id = get_request_id(line)
            if main_network_info and isinstance(main_network_info, NetworkTimingInfo):
                network_info_map[request_id] = main_network_info

        # 开始请求
        if line.find(step_begin_SubResource) != -1:
            request_info_list = line.split(" | ")
            if len(request_info_list) < 3:
                print(f"start request 参数不完整: {line}")
                continue
            request_resource_time = get_trace_line_time(line)
            # 只取在开始加载后发起请求的资源
            # if (
            #         request_resource_time < basic_trace_info.start_time or
            #         request_resource_time > basic_trace_info.end_time
            # ):
            #     continue
            url = get_url_by_trace_line_without_method(request_info_list[1])
            if url in filter_url_list:
                continue
            method = request_info_list[2].replace("method=", "")
            start_id = get_request_id(line)
            request_resource_time = get_trace_line_time(line)
            temp_network_info_map[start_id] = NetworkTimingInfo(
                url,
                start_id,
                method,
                request_resource_time
            )

        # 找到request_id和start_id的映射
        if line.find(request_id_map) != -1:
            request_info_list = line.split(" | ")
            if len(request_info_list) < 3:
                print(f"id map 参数不完整: {line}")
                continue
            start_id = request_info_list[1].replace("start_id=", "").strip()
            request_id = request_info_list[2].replace("request_id=", "").strip()
            if temp_network_info_map.get(start_id, None) is not None:
                network_info_map[request_id] = temp_network_info_map[start_id]
                temp_network_info_map.pop(start_id)

        # 排队结束
        if line.find(step_queue_end_1) != -1 or line.find(step_queue_end_2) != -1:
            request_id = get_request_id(line)
            if not request_id:
                continue
            if network_info_map.get(request_id, None) is None:
                continue
            timing_info = network_info_map[request_id]
            timing_info.queue_end = get_trace_line_time(line)

        # http协议
        if line.find(http_version_step) != -1:
            request_id = get_request_id(line)
            if not request_id:
                continue
            if network_info_map.get(request_id, None) is None:
                continue
            timing_info = network_info_map[request_id]
            timing_info.http_version = get_http_version_by_trace_line(line)

        # 是否走代理
        if line.find(is_intercepted_step) != -1:
            request_id = get_request_id(line)
            if not request_id:
                continue
            if network_info_map.get(request_id, None) is None:
                continue
            timing_info = network_info_map[request_id]
            timing_info.is_intercepted = 1

        # 资源替换-开始传输数据
        if line.find(intercepted_start_send_data) != -1:
            request_id = get_request_id(line)
            if not request_id:
                continue
            if network_info_map.get(request_id, None) is None:
                continue
            timing_info = network_info_map[request_id]
            timing_info.start_send_data = get_trace_line_time(line)

        # 资源替换-数据传输完毕
        if line.find(intercepted_send_data_complete) != -1:
            request_id = get_request_id(line)
            if not request_id:
                continue
            if network_info_map.get(request_id, None) is None:
                continue
            timing_info = network_info_map[request_id]
            timing_info.send_data_complete = get_trace_line_time(line)

        # 资源替换-render开始接收数据
        if line.find(intercepted_render_start_receive) != -1:
            request_id = get_request_id(line)
            if not request_id:
                continue
            if network_info_map.get(request_id, None) is None:
                continue
            timing_info = network_info_map[request_id]
            timing_info.render_start_receive_data = get_trace_line_time(line)

        # 资源替换-render接收数据完毕
        if line.find(intercepted_render_receive_complete) != -1:
            request_id = get_request_id(line)
            if not request_id:
                continue
            if network_info_map.get(request_id, None) is None:
                continue
            timing_info = network_info_map[request_id]
            timing_info.render_receive_data_complete = get_trace_line_time(line)

        # 状态码
        if line.find(response_code_step) != -1:
            request_id = get_request_id(line)
            if not request_id:
                continue
            if network_info_map.get(request_id, None) is None:
                continue
            timing_info = network_info_map[request_id]
            timing_info.response_code = get_response_code_by_trace_line(line)
            time = get_trace_line_time(line)
            if time < basic_trace_info.first_swap_buffer_after_commit:
                timing_info.is_before_first_frame = True

        # 找到时间汇总
        if line.find(network_timing_info) != -1:
            request_id = get_request_id(line)
            if not request_id:
                continue
            if network_info_map.get(request_id, None) is None:
                continue
            timing_info = network_info_map[request_id]
            info_str = line.split("=")[1]
            info_list = info_str.split(";")
            for item in info_list:
                key = item.split(":")[0]
                value = float(item.split(":")[1].replace(" | id", ""))
                timing_info[key] = value

        # 找到缓存相关字段
        if line.find(network_cache_info) != -1:
            request_id = get_request_id(line)
            if not request_id:
                continue
            if network_info_map.get(request_id, None) is None:
                continue
            timing_info = network_info_map[request_id]
            info_str = line.split(" | info=")[1]
            info_list = info_str.split(";")
            # print(f"network_cache_info: {line}")
            for item in info_list:
                key_value = item.split(": ")
                if len(key_value) <= 1:
                    continue
                key = key_value[0]
                value = key_value[1].replace(f" | id={request_id}", "")
                timing_info[key] = value

    # memory cache
    if len(temp_network_info_map) > 0:
        for key, value in temp_network_info_map.items():
            network_info_map[key] = temp_network_info_map[key]

    # print(network_info_map)
    print(f'开始写入子资源原始数据 excel sheet: {sheet_name}...')
    write_excel_data(path, sheet_name, NetworkTimingInfoMap, network_info_map)
    return network_info_map


def get_network_calculated_info(
        path: str,
        sheet_name: str,
        network_info_map: dict[str, NetworkTimingInfo]
) -> dict[str, NetworkCalculatedTiming]:
    network_calculated_info_map = {}
    for request_id in network_info_map:
        network_calculated_info = NetworkCalculatedTiming()
        timing_info = network_info_map[request_id]
        network_calculated_info.request_id = timing_info.request_id
        network_calculated_info.url = timing_info.url
        network_calculated_info.method = timing_info.method
        network_calculated_info.idempotency = timing_info.idempotency
        network_calculated_info.age = timing_info.get_age()
        network_calculated_info.last_modified = timing_info.last_modified
        network_calculated_info.expires = timing_info.expires
        network_calculated_info.cache_control = timing_info.cache_control
        network_calculated_info.etag = timing_info.etag
        network_calculated_info.is_zero = timing_info.get_is_zero()
        network_calculated_info.load_flags = get_load_flags(int(timing_info.load_flags))
        network_calculated_info.is_intercepted = bool(timing_info.is_intercepted)
        network_calculated_info.http_version = timing_info.http_version
        network_calculated_info.queue_time = timing_info.get_queue_time()
        network_calculated_info.stalled_time = timing_info.get_stalled_time()
        network_calculated_info.dns_time = timing_info.get_dns_time()
        network_calculated_info.connect_time = timing_info.get_connect_time()
        network_calculated_info.ssl_time = timing_info.get_ssl_time()
        network_calculated_info.response_time = timing_info.get_response_time()
        network_calculated_info.download_time = timing_info.get_download_time()
        network_calculated_info.request_time = timing_info.get_request_time()
        network_calculated_info.decoded_size = timing_info.decoded_size
        network_calculated_info.encoded_size = timing_info.encoded_size
        network_calculated_info.socket_reused = bool(timing_info.socket_reused)
        network_calculated_info.is_before_first_frame = timing_info.is_before_first_frame
        network_calculated_info.intercepted_send_time = timing_info.get_intercept_send_time()
        network_calculated_info.intercepted_receive_time = timing_info.get_intercept_download_time()
        network_calculated_info.was_fetched_via_cache = timing_info.get_cache_type()
        network_calculated_info_map[request_id] = network_calculated_info

    print(f"network_calculated_info_map: {network_calculated_info_map}")
    print(f'开始写入子资源分段耗时数据 excel sheet: {sheet_name}...')
    write_excel_data(path, sheet_name, NetworkCalculatedTimingMap, network_calculated_info_map)
    return network_calculated_info_map


# 将子资源数据map中筛选出非cache资源，并将其数据拆分成各个时间段
def get_network_statistic_info_map(
        network_calculated_info_map: dict[str, NetworkCalculatedTiming]
) -> NetWorkStatistic:
    network_statistic: NetWorkStatistic = NetWorkStatistic()
    for url, item in network_calculated_info_map.items():
        # 计算并统计子资源数据
        # 子资源编码前总大小
        if item.decoded_size:
            network_statistic.total_decoded_size += item.decoded_size
        # 子资源编码后总大小
        if item.encoded_size:
            network_statistic.total_encoded_size += item.encoded_size
        # 实际发起请求数
        if item.response_time:
            network_statistic.request_times += 1
        # 子资源POST请求数
        if item.method == "POST":
            network_statistic.post_times += 1
        # 子资源http 2.0协议数
        if item.http_version == 'http/2.0':
            network_statistic.http_20 += 1
        # 子资源http 1.1协议数
        if item.http_version == 'http/1.1':
            network_statistic.http_11 += 1
        # 子资源本地资源替换个数
        if item.is_intercepted:
            network_statistic.total_proxy_time += 1
        if item.is_before_first_frame:
            network_statistic.end_before_first_frame_num += 1

        # 子资源建立连接数
        if not item.socket_reused:
            network_statistic.connect_times += 1
        # 存在服务器响应时间计入非cache子资源
        if item.response_time:
            network_statistic.total_request_time += item.request_time
            network_statistic.total_server_response_time += item.response_time

        # 总连接耗时
        if item.connect_time:
            network_statistic.total_connect_time += item.connect_time

        # 总下载耗时
        if item.download_time:
            network_statistic.total_download_time += item.download_time

        # 本地资源替换总耗时
        if item.intercepted_send_time:
            network_statistic.total_intercept_time += item.intercepted_send_time

    # 计算服务器响应时间占比
    if network_statistic.total_server_response_time != 0 and network_statistic.total_request_time != 0:
        network_statistic.server_response_time_percentage = round(
            network_statistic.total_server_response_time / network_statistic.total_request_time,
            4
        )
    else:
        print('子资源全部走缓存或子资源解析异常')

    # 计算资源下载耗时占比
    if network_statistic.total_download_time != 0 and network_statistic.total_request_time != 0:
        network_statistic.download_time_percentage = round(
            network_statistic.total_download_time / network_statistic.total_request_time,
            4
        )
    else:
        print('下载耗时为0或子资源解析异常')

    network_statistic.total_sub_resource_num = len(network_calculated_info_map)
    return network_statistic


# 直接从trace行字符串中取出不带method的url
def get_url_by_trace_line_without_method(line) -> str:
    url = get_url_by_trace_line(line)
    return check_and_remove_method(url)


# 去掉url中的method
def check_and_remove_method(url) -> str:
    if url.find(" | method=") == -1:
        return url
    else:
        return url.split(" | method=")[0].strip()


class LoadFlags(IntFlag):
    LOAD_NORMAL = 0,
    LOAD_VALIDATE_CACHE = 1 << 0,
    LOAD_BYPASS_CACHE = 1 << 1,
    LOAD_SKIP_CACHE_VALIDATION = 1 << 2,
    LOAD_ONLY_FROM_CACHE = 1 << 3,
    LOAD_DISABLE_CACHE = 1 << 4,
    LOAD_DISABLE_CERT_NETWORK_FETCHES = 1 << 5,
    LOAD_DO_NOT_SAVE_COOKIES = 1 << 6,
    LOAD_BYPASS_PROXY = 1 << 7,
    LOAD_MAIN_FRAME_DEPRECATED = 1 << 8,
    LOAD_PREFETCH = 1 << 9,
    LOAD_IGNORE_LIMITS = 1 << 10,
    LOAD_DO_NOT_USE_EMBEDDED_IDENTITY = 1 << 11,
    LOAD_DISABLE_CONNECTION_MIGRATION_TO_CELLULAR = 1 << 12,
    LOAD_SKIP_VARY_CHECK = 1 << 13,
    LOAD_SUPPORT_ASYNC_REVALIDATION = 1 << 14,
    LOAD_RESTRICTED_PREFETCH = 1 << 15,
    LOAD_CAN_USE_RESTRICTED_PREFETCH = 1 << 16,
    LOAD_DO_NOT_SEND_COOKIES = 1 << 17,


def get_load_flags(load_flags_num: int):
    flags = str(load_flags_num)
    for flag in LoadFlags:
        if load_flags_num & flag:
            flags += f" | {flag.name}"
    return flags


def merge_network_timing_interval(
    path: str,
    sheet_name: str,
    network_timing_info: dict[str, NetworkTimingInfo]
) -> list[NetWorkTimgInterval]:
    timing_interval_list: list[NetWorkTimgInterval] = []
    for timing_info in network_timing_info.values():
        if timing_info.request_start != 0 and timing_info.request_end != 0:
            if len(timing_interval_list) == 0:
                new_timing_interval = NetWorkTimgInterval(timing_info.request_start, timing_info.request_end)
                timing_interval_list.append(new_timing_interval)
                continue

            for timing_interval in timing_interval_list:
                if (
                        timing_interval.start_time <= timing_info.request_start <= timing_interval.end_time
                ):
                    if timing_interval.end_time < timing_info.request_end:
                        timing_interval.end_time = timing_info.request_end
                    break
                else:
                    new_timing_interval = NetWorkTimgInterval(timing_info.request_start, timing_info.request_end)
                    timing_interval_list.append(new_timing_interval)
                    break

    length = len(timing_interval_list)
    need_to_remove = []
    for i in range(length):
        value = timing_interval_list[i]
        for j in range(i + 1, length):
            item = timing_interval_list[j]
            if value.start_time <= item.start_time <= value.end_time:
                if value.end_time < item.end_time:
                    value.end_time = item.end_time
                need_to_remove.append(item)

    timing_interval_list = [item for item in timing_interval_list if item not in need_to_remove]
    for value in timing_interval_list:
        print(value.start_time)
        print(value.end_time)
        print("--------------")
