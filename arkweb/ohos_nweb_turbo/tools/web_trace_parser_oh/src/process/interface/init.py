# 进程空闲时间
RenderProcessSleepingMap: dict[str, str] = {
    'sleeping_start': "空闲开始时间",
    'sleeping_end': "睡眠结束时间",
    'during_time': "持续时间",
    # 'matched_resources': "可能相关的子资源"
}


class RenderProcessSleepingUnit:
    def __init__(self):
        # 与 RenderProcessSleepingMap 对应
        self._sleeping_start = 0
        self._sleeping_end = 0
        self._during_time = 0
        self._matched_resources: list[str] = []

    @property
    def sleeping_start(self) -> float:
        return self._sleeping_start

    @sleeping_start.setter
    def sleeping_start(self, value: float):
        self._sleeping_start = value

    @property
    def sleeping_end(self) -> float:
        return self._sleeping_end

    @sleeping_end.setter
    def sleeping_end(self, value: float):
        self._sleeping_end = value

    @property
    def during_time(self) -> float:
        return self._during_time

    @during_time.setter
    def during_time(self, value: float):
        self._during_time = value

    @property
    def matched_resources(self) -> list[str]:
        return self._matched_resources

    @matched_resources.setter
    def matched_resources(self, value: str):
        self._matched_resources.append(value)


