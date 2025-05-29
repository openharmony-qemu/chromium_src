# 解析trace中web主要加载流程
MainProcessTraceStatisticMap: dict[str, str] = {
    'case_name': '用例名',
    'main_url': 'main_url',
    'touch_up_to_nweb_time': '从点击到响应耗时',
    'nweb_create_time': 'NWeb组件创建耗时',
    # 'main_resource_connect_time': '主资源连接耗时',
    # 'main_resource_response_time': '主资源服务器响应耗时',
    # 'main_resource_download_time': '主资源下载耗时',
    # 'main_resource_net_time': '主资源网络线程总耗时',
    'main_resource_time': '主资源总耗时',
    # 'commit_time': 'commit主资源耗时',
    'commit_time': 'Render启动阻塞时长',
    'render_time': '主页面解析及渲染耗时',
    'evaluate_script_time': 'EvaluateScript总耗时',
    'v8_call_function_time': 'v8.callFunction总耗时',
    'touch_up_to_lcp_time': '从点击到LCP/最后一帧总耗时',
    'useragent_changed': 'UA是否改变',
    # 'start_to_dcl_time': '从点击到DCL总耗时',
    'network_percentage': '网络阻塞耗时占比',
    'js_percentage': 'js耗时占比',
}


class MainProcessTraceStatistic:
    def __init__(self, main_url=""):
        self.main_url = main_url
        self.case_name = ""
        self.touch_up_to_lcp_time = 0
        self.touch_up_to_nweb_time = 0
        self.nweb_create_time = 0
        self.main_resource_connect_time = 0
        self.main_resource_response_time = 0
        self.main_resource_download_time = 0
        self.main_resource_net_time = 0
        self.main_resource_time = 0
        self.commit_time = 0
        self.touch_up_to_white_screen_end = 0
        self.render_time = 0
        self.first_frame_evaluate_script_time = 0
        self.first_frame_v8_call_function_time = 0
        self.evaluate_script_time = 0
        self.v8_call_function_time = 0
        self.start_to_dcl_time = 0
        self.network_percentage = 0
        self.js_percentage = 0
        self.useragent_changed = False

    def show_info(self):
        print(f'\ntrace主流程数据统计-------------------------------------')
        print(f'开始到结束总耗时: {self.touch_up_to_lcp_time}')
        print(f'开始到NWeb组件创建: {self.touch_up_to_nweb_time}')
        print(f'NWeb组件创建耗时: {self.nweb_create_time}')
        print(f'主资源加载耗时: {self.main_resource_time}')
        print(f'主资源commit到render进程耗时: {self.commit_time}')
        print(f'从点击到主资源加载完后首帧耗时: {self.touch_up_to_white_screen_end}')
        print(f'渲染耗时: {self.render_time}')
        print(f'EvaluateScript执行耗时: {self.evaluate_script_time}')
        print(f'v8.callFunction执行耗时: {self.v8_call_function_time}')
        # TODO: 解析DCL时间
        # print(f'从开始到DCL执行耗时: {self._start_to_dcl_time}')
        print(f'-------------------------------------------------------')

    def __getitem__(self, key):
        return getattr(self, key)

    def __setitem__(self, key, value):
        setattr(self, key, value)


FirstFrameInfoMap: dict[str, str] = {
    'touch_up_to_white_screen_end': "从点击到主资源加载完后首帧耗时",
    "first_frame_evaluate_script_time": "首帧前EvaluateScript执行耗时",
    "first_frame_v8_call_function_time": "首帧前V8.callFunction执行耗时",
    "end_before_first_frame_num": "首帧前网络请求个数"
}


class FirstFrameInfo:
    def __init__(
            self,
            touch_up_to_white_screen_end,
            first_frame_evaluate_script_time,
            first_frame_v8_call_function_time,
            end_before_first_frame_num
    ):
        self.touch_up_to_white_screen_end = touch_up_to_white_screen_end
        self.first_frame_evaluate_script_time = first_frame_evaluate_script_time
        self.first_frame_v8_call_function_time = first_frame_v8_call_function_time
        self.end_before_first_frame_num = end_before_first_frame_num

    def __getitem__(self, key):
        return getattr(self, key)

    def __setitem__(self, key, value):
        setattr(self, key, value)