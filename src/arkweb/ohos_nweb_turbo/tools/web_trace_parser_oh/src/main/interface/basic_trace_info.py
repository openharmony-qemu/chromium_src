# 解析trace前需要获取到的关键信息
class BasicTraceInfo:
    def __init__(self, start_point=None, end_point=None):
        if start_point is None:
            start_point = ["DispatchTouchEvent", "type=1"]
        if end_point is None:
            end_point = ["largestContentfulPaint"]
        self.case_name = ""
        self.start_point = start_point
        self.end_point = end_point
        self.render_process_id = ""
        self.render_process_name = ""
        self.commit_end = 0
        self.start_time = 0
        self.end_time = 0
        self.main_url = ""
        self.first_paint = 0
        self.first_contentful_paint = 0
        self.first_swap_buffer_after_commit = 0
        self.last_swap_buffer_time = 0

    def need_break(self) -> bool:
        if self.start_time == 0:
            print('未检测到开始时间，暂停解析')
            return True

        if self.end_time == 0:
            print('未检测到结束时间，暂停解析')
            return True
        return False

    def show_info(self):
        print(f'\ntrace基本信息-------------------------------------')
        print(f'用例名: {self.case_name}')
        print(f'render进程id: {self.render_process_id}')
        print(f'render进程名称: {self.render_process_name}')
        print(f'起始点: {self.start_point}')
        print(f'结束点: {self.end_point}')
        print(f'起始时间: {self.start_time}')
        print(f'首帧时间: {self.first_swap_buffer_after_commit}')
        print(f'FP: {self.first_paint}')
        print(f'FCP: {self.first_contentful_paint}')
        print(f'结束时间: {self.end_time}')
        print(f'最后一帧时间: {self.last_swap_buffer_time}')
        print(f'main_url: {self.main_url}')
        print(f'-------------------------------------------------')

    def __getitem__(self, key):
        return getattr(self, key)

    def __setitem__(self, key, value):
        setattr(self, key, value)
