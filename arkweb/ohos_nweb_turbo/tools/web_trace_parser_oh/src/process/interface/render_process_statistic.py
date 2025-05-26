
# 进程空闲时间与相关子资源统计
RenderProcessSleepingStatisticMap: dict[str, str] = {
    'total_sleeping_time': "总空闲时长",
    # 'max_sleeping_start_time': '最长空闲开始时间',
    # 'max_sleeping_end_time': '最长空闲结束时间',
    'max_sleeping_during_time': "最长空闲时长",
    # 'max_sleeping_matched_resources': '最长空闲可能相关的子资源',
    # 'max_sleeping_matched_resources_num': '最长空闲可能相关的子资源个数',
    # 'matched_sub_resource_num': '可能造成空闲的子资源个数',
}


class RenderProcessSleepingStatistic:
    def __init__(self):
        # 与 RenderProcessSleepingStatistic 对应
        self.max_sleeping_start = 0
        self.max_sleeping_end = 0
        self.max_sleeping_during_time = 0
        self.max_sleeping_matched_resources: list[str] = []
        self.max_sleeping_matched_resources_num: int = 0
        self.total_sleeping_time = 0
        self.matched_sub_resource_num = 0

    def __getitem__(self, key):
        return getattr(self, key)

    def __setitem__(self, key, value):
        setattr(self, key, value)

