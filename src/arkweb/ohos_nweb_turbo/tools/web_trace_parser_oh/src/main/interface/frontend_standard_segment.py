# TODO: 前端指标解析
FrontendStandardSegmentMap: dict[str, str] = {
    'main_resource_download_time': '主资源下载耗时',
    'dom_content_loaded_time': '框架初始化耗时',
    'json_data_download_time': '布局json下载耗时',
    'json_data_parsing_time': 'json解析耗时',
    'parallel_download_time': '图片资源并行下载耗时',
}


class FrontendStandardSegment:
    def __init__(self):
        self._main_resource_download_time = 0
        self._dom_content_loaded_time = 0
        self._json_data_download_time = 0
        self._json_data_parsing_time = 0
        self._parallel_download_time = 0

    @property
    def main_resource_download_time(self) -> float:
        return self._main_resource_download_time

    @main_resource_download_time.setter
    def main_resource_download_time(self, value: float):
        self._main_resource_download_time = value

    @property
    def dom_content_loaded_time(self) -> float:
        return self._dom_content_loaded_time

    @dom_content_loaded_time.setter
    def dom_content_loaded_time(self, value: float):
        self._dom_content_loaded_time = value

    @property
    def json_data_download_time(self) -> float:
        return self._json_data_download_time

    @json_data_download_time.setter
    def json_data_download_time(self, value: float):
        self._json_data_download_time = value

    @property
    def json_data_parsing_time(self) -> float:
        return self._json_data_parsing_time

    @json_data_parsing_time.setter
    def json_data_parsing_time(self, value: float):
        self._json_data_parsing_time = value

    @property
    def parallel_download_time(self) -> float:
        return self._parallel_download_time

    @parallel_download_time.setter
    def parallel_download_time(self, value: float):
        self._parallel_download_time = value
