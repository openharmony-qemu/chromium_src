import re
import copy


def get_process_id_by_trace_line(line) -> str:
    return re.search(r'[\w\W]+-(\d+)', line).group(1)


def get_package_name_by_trace_line(line) -> str:
    match = re.search(r'([\w\W]+?)-\d+', line)
    if match:
        return match.group(1)
    else:
        return ""


def get_url_by_trace_line(line) -> str:
    match = re.search(r'\burl=([^ |]+)', line)
    if match:
        return match.group(1).strip()
    else:
        return ""


def get_http_version_by_trace_line(line) -> str:
    match = re.search(r'\bhttp=(http/\d\.\d)', line)
    if match:
        return match.group(1)
    else:
        return ""


def get_method_by_trace_line(line) -> str:
    match = re.search(r' method=([^\s|]+)', line)
    if match:
        return match.group(1)
    else:
        return ""


def get_id_by_trace_line(line) -> str:
    match = re.search(r'^| id=([^\s]+)', line)
    if match:
        return match.group(1).strip()
    else:
        return ""


def get_request_id(line: str) -> str:
    request_info_list = line.split(" | id=")
    if len(request_info_list) < 2:
        print(f"get_request_id 参数不完整: {line}")
        return ""
    return request_info_list[1].strip()


def get_decode_size_by_trace_line(line) -> int:
    match = re.search(r'decoded_body_length=([^\s|]+)', line)
    if match:
        return int(match.group(1))
    else:
        return 0


def get_encode_size_by_trace_line(line) -> int:
    match = re.search(r'encoded_body_length=([^\s|]+)', line)
    if match:
        return int(match.group(1))
    else:
        return 0


def get_response_code_by_trace_line(line) -> str:
    match = re.search(r'response_code=([^\s|]+)', line)
    if match:
        return match.group(1)
    else:
        return ""


def need_break(line, end_name, end_time: float) -> bool:
    if line.find(end_name) == -1:
        return False
    if end_name.find("largestContentfulPaint") != -1:
        if get_trace_line_time(line) == end_time:
            return True
        else:
            return False
    else:
        return True


def parseResourceTrace(str, tag):
    if tag == 'TraceStart':
        time = str.split('.... ')[1].split(': sched_wakeup')[0]
    else:
        time = str.split('.... ')[1].split(': tracing_mark_write')[0]

    url = str.split('url=')[1].strip()
    # print(f'url:{url}')
    return time, url


# def get_trace_line_time(line, tag) -> str:
#     try:
#         if tag == 'TraceStart':
#             return line.split('.... ')[1].split(': sched_wakeup')[0]
#         else:
#             return line.split('.... ')[1].split(': tracing_mark_write')[0]
#     except IndexError:
#         print(f"error: get_trace_line_time line: {line}, tag: {tag} 不存在time")
#         return ""

def get_trace_line_time(line) -> float:
    try:
        # print(line)
        # return float(re.split(r"\] [^\s]+ ", line)[1].split(': ')[0])
        return float(line.split(' .... ')[1].split(': ')[0])
    except IndexError or ValueError:
        # print(f"Error: get_trace_line_time line: {line}")
        return 0


def parseOtherTrace(line: str, tag):
    try:
        if tag == 'TraceStart':
            time = line.split('.... ')[1].split(': sched_wakeup')[0]
        else:
            time = line.split('.... ')[1].split(': tracing_mark_write')[0]
            # time = re.split(r"\] [^\s]+ ", line)[1].split(': tracing_mark_write')[0]
        return time
    except IndexError:
        print(f"Error: parseOtherTrace line: {line}")
        return 0


def calculate_single_task_time(
        lines: [str],
        task_name: str,
        start_time: float,
        end_time: float,
        render_id: str
) -> float:
    total_time = 0.00
    js_flag = 0
    stack_deep = 0
    js_start_time = 0
    times = 0
    render_line_id = 0

    for idx, line in enumerate(lines):
        # 到结束trace点，break;
        # if need_break(line, end_name, end_time):
        #     return total_time

        time = get_trace_line_time(line)
        if time < start_time:
            continue

        if time > end_time:
            print(f'共匹配到 {times} 段: {task_name}')
            return total_time

        if line.find(task_name) != -1 and line.find('-' + render_id) != -1 and js_flag == 0:
            # print(f'calculate_single_task_time: {line}')
            js_start_time = get_trace_line_time(line)
            # print(f"render_id: {render_id}")
            process_id = line.split('tracing_mark_write: B|')[1].split('|H:')[0]
            stack_deep += 1
            js_flag = 1
            continue
        if js_flag == 1 and line.find('-' + render_id) != -1:
            if line.find('tracing_mark_write: B|' + process_id) != -1:
                # print(f'deep: {stack_deep} line:{line}')
                stack_deep += 1
            if line.find('tracing_mark_write: E|' + process_id) != -1:
                stack_deep -= 1
                if stack_deep == 0:
                    # print(f'end :{line}')
                    js_end_time = get_trace_line_time(line)
                    current_time = js_end_time - js_start_time
                    # print(f'end: {js_end_time} - start: {js_start_time} = current_time :{total_time}')
                    total_time += current_time * 1000
                    times += 1
                    js_flag = 0
    return total_time


def get_time_from_begin_to_end(lines, begin_steps, end_steps) -> float:
    begin_time = 0.00
    end_time = 0.00
    for idx, line in enumerate(lines):
        if any(key_step in line for key_step in begin_steps) and begin_time == 0.00:
            begin_time = get_trace_line_time(line)

        if any(key_step in line for key_step in end_steps) and end_time == 0.00:
            end_time = get_trace_line_time(line)
            break
    return (end_time - begin_time) * 1000


def get_step_first_time(lines, key_step_list) -> float:
    begin_time = 0.00
    for idx, line in enumerate(lines):
        if (
                all(condition in line for condition in key_step_list) and
                begin_time == 0.00
        ):
            begin_time = get_trace_line_time(line)
            break
    return begin_time * 1000


def get_step_last_time(lines, key_step_list) -> float:
    begin_time = 0.00
    for line in lines:
        if (
                all(condition in line for condition in key_step_list)
        ):
            print(f"dispatch: {line}")
            begin_time = get_trace_line_time(line)
    print(f"get_step_last_time: {begin_time}")
    return begin_time


def get_step_first_time_any(lines, key_step_list_any) -> float:
    begin_time = 0.00
    for idx, line in enumerate(lines):
        if (
                any(condition in line for condition in key_step_list_any) and
                begin_time == 0.00
        ):
            begin_time = get_trace_line_time(line)
            break
    return begin_time


def get_function_time(lines, function_name) -> float:
    total_time = 0.00
    js_flag = 0
    begin_time = 0.00
    stack = 0
    package_name = ""
    for idx, line in enumerate(lines):
        if line.find(function_name) != -1 and js_flag == 0:
            package_name = get_package_name_by_trace_line(line)
            time = parseOtherTrace(line, function_name)
            begin_time = float(time)
            renderUIId = line.split('tracing_mark_write: B|')[1].split('|H:')[0]
            stack += 1
            js_flag = 1
            continue
        if js_flag == 1 and line.find(package_name) != -1 and line.find(renderUIId) != -1:
            if line.find('tracing_mark_write: B|' + renderUIId) != -1:
                # print(f'line get_function_time:{line}')
                # print(f'line callFunction +1')
                stack += 1
            if line.find('tracing_mark_write: E|' + renderUIId) != -1:
                # print(f'line get_function_time:{line}')
                # print(f'line callFunction -1')
                stack -= 1
                if stack == 0:
                    time = parseOtherTrace(line, renderUIId)
                    end_time = float(time)
                    total_time += (end_time - begin_time) * 1000
                    js_flag = 0
    return total_time


def filter_block_sub_resource(
        sleeping_time_list: list,
        sub_resource_info_map: dict,
) -> list:
    return get_block_sub_resource_normal(sleeping_time_list, sub_resource_info_map)


def get_block_sub_resource_normal(sleeping_time_list: list, sub_resource_detail_list) -> list:
    # normal_block_list = []
    # time_list = copy.deepcopy(sleeping_time_list)
    # for time_segment in time_list:
    #     start_time = time_segment.get('sleeping_start')
    #     end_time = time_segment.get('sleeping_end')
    #     if not start_time or not end_time:
    #         continue
    #     for url in sub_resource_detail_list:
    #         request_resource_time = sub_resource_detail_list.get(url).get('request_resource_time')
    #         receive_response_time = sub_resource_detail_list.get(url).get('receive_response_time')
    #         if not request_resource_time or not receive_response_time:
    #             continue
    #         if request_resource_time <= start_time < receive_response_time <= end_time:
    #             detail_time: dict = copy.deepcopy(time_segment)
    #             detail_time.update(sub_resource_detail_list[url])
    #             normal_block_list.append(detail_time)
    # return normal_block_list

    normal_block_list = []
    time_list = copy.deepcopy(sleeping_time_list)
    resource_list = copy.deepcopy(sub_resource_detail_list)
    for time_segment in time_list:
        start_time = time_segment.get('sleeping_start')
        end_time = time_segment.get('sleeping_end')
        if not start_time or not end_time:
            continue
        time_segment['matched_resource'] = []
        gap_time = 0
        current_matched_resource = None
        for index, url in enumerate(sub_resource_detail_list):
            request_resource_time = sub_resource_detail_list.get(url).get('request_resource_time')
            receive_response_time = sub_resource_detail_list.get(url).get('receive_response_time')
            if not request_resource_time or not receive_response_time:
                continue
            current_gap_time = end_time - receive_response_time
            if current_gap_time <= 0 and current_matched_resource is not None:
                time_segment['matched_resource'].append(current_matched_resource)
                normal_block_list.append(time_segment)
                break
            elif current_gap_time > 0:
                current_matched_resource = sub_resource_detail_list.get(url)
            # if request_resource_time <= start_time < receive_response_time <= end_time:
            #     detail_time: dict = copy.deepcopy(time_segment)
            #     detail_time.update(sub_resource_detail_list[url])
            #     normal_block_list.append(detail_time)

    return normal_block_list


def get_block_sub_resource_deep(sleeping_time_list: list, sub_resource_detail_list: list) -> list:
    deep_block_list = []
    merged_sleeping_time_list = []
    index = 0
    for sleep_segment in sleeping_time_list:
        start_sleeping_time = sleep_segment['sleeping_start']
        sleeping_end_time = sleep_segment['sleeping_end']

    return deep_block_list
