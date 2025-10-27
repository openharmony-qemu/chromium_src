# 子资源时间段拆解
SubResourceKeyInfoMap: dict[str, str] = {
    "url": "url",
    "connect_time": "连接耗时",
    "server_response_time": "服务器耗时",
    "download_time": "下载耗时",
    "net_request_total_time": "网络线程请求总耗时",
    "render_request_total_time": "render线程请求总耗时",
    "decode_size": "编码前资源大小",
    "encode_size": "编码后资源大小",
    "method": "请求方法",
    "status_code": "状态码",
    "http_version": "http协议版本",
}

# 子资源数据
SubResourceInfoMap: dict[str, str] = {
    "url": "url",
    "connect_start": "连接开始时间点",
    "connect_end": "连接结束时间点",
    "request_time": "开始发送请求到服务器时间点",
    "response_time": "服务器响应时间点",
    "response_end": "开始下载资源时间点",
    "decode_size": "编码前资源大小",
    "encode_size": "编码后资源大小",
    "method": "请求方法",
    "status_code": "状态码",
    "http_version": "http协议版本",
    "request_resource_time": "请求发起时间（render）",
    "receive_response_time": "请求结束时间（render）",
    "net_request_resource_time": "请求发起时间（网络线程）",
    "net_receive_response_time": "请求结束时间（网络线程）",
    "is_intercepted": "是否走代理",
}

# 子资源汇总统计数据
SubResourceStatisticMap: dict[str, str] = {
    'total_sub_resource_num': "子资源个数",
    'connect_times': "子资源建立连接数",
    'request_times': "子资源实际发起请求数",
    'post_times': 'POST请求数',
    'http_20': 'http 2.0协议个数',
    'http_11': 'http 1.1协议个数',
    'total_connect_time': "子资源连接总耗时",
    'total_server_response_time': "子资源服务器响应总耗时",
    'total_download_time': "子资源下载总耗时",
    'net_total_time': "子资源总耗时",
    'total_decode_size': "资源总大小（编码前）",
    'total_encode_size': "资源总大小（编码后）",
    'total_proxy_time': "走代理个数",
    'server_response_time_percentage': "服务器响应时间占比",
    'download_time_percentage': "下载时长占比"
}


class SubResourceKeyInfo:
    def __init__(self, url):
        # 与SubResourceKeyInfoMap相对应
        self._url = url
        self._connect_time = 0
        self._server_response_time = 0
        self._download_time = 0
        self._net_request_total_time = 0
        self._render_request_total_time = 0
        self._decode_size = 0
        self._encode_size = 0
        self._method = ""
        self._status_code = 0
        self._http_version = ""

    @property
    def url(self) -> str:
        return self._url

    @url.setter
    def url(self, value):
        self._url = value

    @property
    def connect_time(self) -> float:
        return self._connect_time

    @connect_time.setter
    def connect_time(self, value):
        self._connect_time = value

    @property
    def server_response_time(self) -> float:
        return self._server_response_time

    @server_response_time.setter
    def server_response_time(self, value):
        self._server_response_time = value

    @property
    def download_time(self) -> float:
        return self._download_time

    @download_time.setter
    def download_time(self, value):
        self._download_time = value

    @property
    def decode_size(self) -> int:
        return self._decode_size

    @decode_size.setter
    def decode_size(self, value):
        self._decode_size = value

    @property
    def encode_size(self) -> int:
        return self._encode_size

    @encode_size.setter
    def encode_size(self, value):
        self._encode_size = value

    @property
    def method(self) -> str:
        return self._method

    @method.setter
    def method(self, value):
        self._method = value

    @property
    def status_code(self) -> int:
        return self._status_code

    @status_code.setter
    def status_code(self, value):
        self._status_code = value

    @property
    def http_version(self) -> str:
        return self._http_version

    @http_version.setter
    def http_version(self, value):
        self._http_version = value

    @property
    def render_request_total_time(self) -> float:
        return self._render_request_total_time

    @render_request_total_time.setter
    def render_request_total_time(self, value):
        self._render_request_total_time = value

    @property
    def net_request_total_time(self) -> float:
        return self._net_request_total_time

    @net_request_total_time.setter
    def net_request_total_time(self, value):
        self._net_request_total_time = value


class SubResourceInfo:
    def __init__(self, url):
        # 与SubResourceInfoMap相对应
        self._url = url
        self._connect_start = 0
        self._connect_end = 0
        self._request_time = 0
        self._response_time = 0
        self._response_end = 0
        self._decode_size = 0
        self._encode_size = 0
        self._method = ""
        self._status_code = 0
        self._http_version = ""
        self._request_resource_time = 0
        self._receive_response_time = 0
        self._net_request_resource_time = 0
        self._net_receive_response_time = 0
        self._is_intercepted = False

    @property
    def url(self) -> str:
        return self._url

    @url.setter
    def url(self, value):
        self._url = value

    @property
    def connect_start(self) -> float:
        return self._connect_start

    @connect_start.setter
    def connect_start(self, value):
        self._connect_start = value

    @property
    def connect_end(self) -> float:
        return self._connect_end

    @connect_end.setter
    def connect_end(self, value):
        self._connect_end = value

    @property
    def request_time(self) -> float:
        return self._request_time

    @request_time.setter
    def request_time(self, value):
        self._request_time = value

    @property
    def response_time(self) -> float:
        return self._response_time

    @response_time.setter
    def response_time(self, value):
        self._response_time = value

    @property
    def response_end(self) -> float:
        return self._response_end

    @response_end.setter
    def response_end(self, value):
        self._response_end = value

    @property
    def decode_size(self) -> int:
        return self._decode_size

    @decode_size.setter
    def decode_size(self, value):
        self._decode_size = value

    @property
    def encode_size(self) -> int:
        return self._encode_size

    @encode_size.setter
    def encode_size(self, value):
        self._encode_size = value

    @property
    def method(self) -> str:
        return self._method

    @method.setter
    def method(self, value):
        self._method = value

    @property
    def status_code(self) -> int:
        return self._status_code

    @status_code.setter
    def status_code(self, value):
        self._status_code = value

    @property
    def http_version(self) -> str:
        return self._http_version

    @http_version.setter
    def http_version(self, value):
        self._http_version = value

    @property
    def request_resource_time(self) -> float:
        return self._request_resource_time

    @request_resource_time.setter
    def request_resource_time(self, value):
        self._request_resource_time = value

    @property
    def receive_response_time(self) -> float:
        return self._receive_response_time

    @receive_response_time.setter
    def receive_response_time(self, value) -> float:
        self._receive_response_time = value

    @property
    def net_request_resource_time(self) -> float:
        return self._net_request_resource_time

    @net_request_resource_time.setter
    def net_request_resource_time(self, value):
        self._net_request_resource_time = value

    @property
    def net_receive_response_time(self) -> float:
        return self._net_receive_response_time

    @net_receive_response_time.setter
    def net_receive_response_time(self, value):
        self._net_receive_response_time = value

    @property
    def is_intercepted(self) -> bool:
        return self._is_intercepted

    @is_intercepted.setter
    def is_intercepted(self, value: bool):
        self._is_intercepted = value


class SubResourceStatistic:
    def __init__(self):
        # 与SubResourceStatisticMap相对应
        self._connect_times = 0
        self._request_times = 0
        self._post_times = 0
        self._http_20 = 0
        self._http_11 = 0
        self._total_connect_time = 0
        self._total_server_response_time = 0
        self._total_download_time = 0
        self._net_total_time = 0
        self._total_sub_resource_num = 0
        self._total_decode_size = 0
        self._total_encode_size = 0
        self._total_proxy_time = 0
        self._server_response_time_percentage = 0
        self._download_time_percentage = 0


    @property
    def connect_times(self) -> int:
        return self._connect_times

    @connect_times.setter
    def connect_times(self, value):
        self._connect_times = value

    @property
    def request_times(self) -> int:
        return self._request_times

    @request_times.setter
    def request_times(self, value):
        self._request_times = value

    @property
    def post_times(self) -> int:
        return self._post_times

    @post_times.setter
    def post_times(self, value):
        self._post_times = value

    @property
    def http_20(self) -> int:
        return self._http_20

    @http_20 .setter
    def http_20(self, value):
        self._http_20 = value

    @property
    def http_11(self) -> int:
        return self._http_11

    @http_11.setter
    def http_11(self, value):
        self._http_11 = value

    @property
    def total_connect_time(self) -> int:
        return self._total_connect_time

    @total_connect_time.setter
    def total_connect_time(self, value):
        self._total_connect_time = value

    @property
    def total_server_response_time(self) -> int:
        return self._total_server_response_time

    @total_server_response_time.setter
    def total_server_response_time(self, value):
        self._total_server_response_time = value

    @property
    def total_download_time(self) -> int:
        return self._total_download_time

    @total_download_time.setter
    def total_download_time(self, value):
        self._total_download_time = value

    @property
    def net_total_time(self) -> int:
        return self._net_total_time

    @net_total_time.setter
    def net_total_time(self, value):
        self._net_total_time = value

    @property
    def total_sub_resource_num(self) -> int:
        return self._total_sub_resource_num

    @total_sub_resource_num.setter
    def total_sub_resource_num(self, value):
        self._total_sub_resource_num = value

    @property
    def total_decode_size(self) -> int:
        return self._total_decode_size

    @total_decode_size.setter
    def total_decode_size(self, value):
        self._total_decode_size = value

    @property
    def total_encode_size(self) -> int:
        return self._total_encode_size

    @total_encode_size.setter
    def total_encode_size(self, value):
        self._total_encode_size = value

    @property
    def total_proxy_time(self) -> int:
        return self._total_proxy_time

    @total_proxy_time.setter
    def total_proxy_time(self, value):
        self._total_proxy_time = value

    @property
    def server_response_time_percentage(self) -> float:
        return self._server_response_time_percentage

    @server_response_time_percentage.setter
    def server_response_time_percentage(self, value):
        self._server_response_time_percentage = value

    @property
    def download_time_percentage(self) -> float:
        return self._download_time_percentage

    @download_time_percentage.setter
    def download_time_percentage(self, value):
        self._download_time_percentage = value
