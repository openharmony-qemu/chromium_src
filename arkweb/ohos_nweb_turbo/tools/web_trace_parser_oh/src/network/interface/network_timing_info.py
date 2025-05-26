# 子资源请求各时间点拆解
NetworkTimingInfoMap: dict[str, str] = {
    "url": "url",
    "method": "method",
    "socket_reused": "是否复用",
    "request_start": "开始请求",
    "queue_end": "排队结束",
    "stalled_end": "stalled结束",
    "dns_start": "开始dns解析",
    "dns_end": "dns解析结束",
    "connect_start": "开始连接",
    "connect_end": "连接结束",
    "ssl_start": "开始ssl",
    "ssl_end": "结束ssl",
    "send_start": "开始发送请求",
    "receive_headers_start": "开始接收服务器响应",
    "request_end": "请求结束",
    "http_version": "请求版本",
    "decoded_size": "解码大小",
    "encoded_size": "编码大小",
    "response_code": "状态码",
    "idempotency": "idempotency",
    "age": "Age",  # "缓存存在时长"
    "last_modified": "Last-Modified",  # 最后修改时间
    "expires": "Expires",  # 过期时间
    "cache_control": "Cache-Control",  # 缓存指令
    "etag": "ETag",  # 资源的唯一标识符
    "is_zero": "资源是否过期",
    "load_flags": "load_flags",
    "is_intercepted": "是否走代理",
    "start_send_data": "开始传输数据",
    "send_data_complete": "数据传输完成",
    "render_start_receive_data": "render开始接收数据",
    "render_receive_data_complete": "接收数据完成",
    "is_before_first_frame": "请求是否在第一帧前结束",
    "was_fetched_via_cache": "是否来自缓存"
}


class NetworkTimingInfo:
    def __init__(self, url: str, start_id: str, method: str, request_resource: float):
        # 与NetworkTimingInfoMap相对应
        # 基本信息
        self.url = url
        self.start_id = start_id
        self.method = method
        self.request_id = ""
        # 耗时相关
        self.socket_reused = 0
        self.request_resource = request_resource
        self.request_start = 0
        self.queue_end = 0
        self.stalled_end = 0
        self.dns_start = 0
        self.dns_end = 0
        self.connect_start = 0
        self.connect_end = 0
        self.ssl_start = 0
        self.ssl_end = 0
        self.send_start = 0
        self.receive_headers_start = 0
        self.request_end = 0
        self.http_version = ""
        self.decoded_size = 0
        self.encoded_size = 0
        self.response_code = 0
        # 缓存相关
        self.idempotency = 0
        self.age = "unset"
        self.last_modified = "unset"
        self.expires = "unset"
        self.cache_control = "unset"
        self.etag = "unset"
        self.is_zero = "unset"
        self.load_flags = 0
        self.was_fetched_via_cache = -1
        # 代理相关
        self.is_intercepted = 0
        self.start_send_data = 0
        self.send_data_complete = 0
        self.render_start_receive_data = 0
        self.render_receive_data_complete = 0

        self.is_extra = False

        self.is_before_first_frame = False

    def __getitem__(self, key):
        return getattr(self, key)

    def __setitem__(self, key, value):
        setattr(self, key, value)

    def get_queue_time(self) -> float:
        # 通过trace点单独计算，并非取自timingInfo
        if self.queue_end < self.request_resource:
            return 0
        return (self.queue_end - self.request_resource) * 1000

    def get_stalled_time(self) -> float:
        # if self.dns_start:
        #     self.stalled_end = self.dns_start
        # elif self.connect_start:
        #     self.stalled_end = self.connect_start
        # elif self.send_start:
        #     self.stalled_end = self.send_start
        #
        # return self.send_start - self.stalled_end
        if self.is_intercepted:
            return self.get_request_time() - self.get_intercept_download_time()
        else:
            return (
                self.get_request_time() - self.get_dns_time() - self.get_connect_time() -
                self.get_response_time() - self.get_download_time()
            )

    def get_dns_time(self) -> float:
        return self.dns_end - self.dns_start

    def get_connect_time(self) -> float:
        return self.connect_end - self.connect_start

    def get_ssl_time(self) -> float:
        return self.ssl_end - self.ssl_start

    def get_response_time(self) -> float:
        return self.receive_headers_start - self.send_start

    def get_download_time(self) -> float:
        return self.request_end - self.receive_headers_start

    def get_request_time(self) -> float:
        if self.is_intercepted:
            return (self.render_receive_data_complete - self.request_resource) * 1000
        else:
            return self.request_end - self.request_start

    def get_age(self) -> float | str:
        if self.age == 'unset':
            return self.age
        else:
            return int(self.age) / 1000

    def get_is_zero(self) -> bool | str:
        if self.is_zero == 'unset':
            return self.is_zero
        else:
            return bool(int(self.is_zero))

    def get_intercept_send_time(self) -> float:
        if self.send_data_complete >= self.start_send_data:
            return (self.send_data_complete - self.start_send_data) * 1000
        else:
            return 0

    def get_intercept_download_time(self) -> float:
        if self.render_receive_data_complete >= self.render_start_receive_data:
            return (self.render_receive_data_complete - self.render_start_receive_data) * 1000
        else:
            return 0

    def get_cache_type(self) -> str:
        # if self.is_intercepted or self.was_fetched_via_cache == "0":
        #     return "no cache"
        if self.was_fetched_via_cache == "1":
            return "disk cache"
        elif self.was_fetched_via_cache == -1 and self.render_receive_data_complete == 0:
            return "memory cache"
        else:
            return "no cache"


# 子资源请求各时间点拆解
NetworkCalculatedTimingMap: dict[str, str] = {
    "url": "url",
    "method": "method",
    "http_version": "http协议版本",
    "socket_reused": "是否复用",
    "queue_time": "排队耗时",
    "stalled_time": "已停止",
    "dns_time": "dns解析耗时",
    "connect_time": "连接耗时",
    "ssl_time": "ssl耗时",
    "response_time": "服务器响应耗时",
    "download_time": "下载耗时",
    "request_time": "请求总耗时",
    "decoded_size": "解码大小",
    "encoded_size": "编码大小",
    "is_intercepted": "是否走代理",
    "intercepted_send_time": "传输耗时",
    "intercepted_receive_time": "接收耗时",
    "idempotency": "idempotency",
    "was_fetched_via_cache": "是否来自缓存",
    "age": "Age",  # "缓存存在时长"
    "last_modified": "Last-Modified",  # 最后修改时间
    "expires": "Expires",  # 过期时间
    "cache_control": "Cache-Control",  # 缓存指令
    "etag": "ETag",  # 资源的唯一标识符
    "is_zero": "资源是否过期",
    "load_flags": "load_flags",
    "is_before_first_frame": "请求是否在第一帧前结束"
}


class NetworkCalculatedTiming:
    def __init__(self):
        # 与NetworkTimingInfoMap相对应
        self.url = ""
        self.request_id = ""
        self.method = ""
        self.http_version = ""
        self.socket_reused = False
        self.queue_time = 0
        self.stalled_time = 0
        self.dns_time = 0
        self.connect_time = 0
        self.ssl_time = 0
        self.response_time = 0
        self.download_time = 0
        self.request_time = 0
        self.decoded_size = 0
        self.encoded_size = 0
        self.is_intercepted = False
        self.intercepted_send_time = 0
        self.intercepted_receive_time = 0
        self.response_code = 0
        # 缓存相关
        self.idempotency = 0
        self.was_fetched_via_cache = "no cache"
        self.age = "unset"
        self.last_modified = "unset"
        self.expires = "unset"
        self.cache_control = "unset"
        self.etag = "unset"
        self.is_zero = "unset"
        self.load_flags = 0,

        self.is_before_first_frame = False

    def __getitem__(self, key):
        return getattr(self, key)

    def __setitem__(self, key, value):
        setattr(self, key, value)


# 子资源汇总统计数据
NetWorkStatisticMap: dict[str, str] = {
    'total_sub_resource_num': "子资源个数",
    'connect_times': "子资源建立连接数",
    'request_times': "子资源实际发起请求数",
    'post_times': 'POST请求数',
    'http_20': 'http 2.0协议个数',
    'http_11': 'http 1.1协议个数',
    'total_connect_time': "子资源连接总耗时",
    'total_server_response_time': "子资源服务器响应总耗时",
    'total_download_time': "子资源下载总耗时",
    'total_request_time': "子资源总耗时",
    'total_intercept_time': "资源拦截替换总耗时",
    'total_decoded_size': "资源总大小（编码前）",
    'total_encoded_size': "资源总大小（编码后）",
    'total_proxy_time': "本地资源替换个数",
    'end_before_first_frame_num': '在第一帧前结束的请求个数',
    'server_response_time_percentage': "服务器响应时间占比",
    'download_time_percentage': "下载时长占比"
}


class NetWorkStatistic:
    def __init__(self):
        # 与NetWorkStatisticMap相对应
        self.connect_times = 0
        self.request_times = 0
        self.post_times = 0
        self.http_20 = 0
        self.http_11 = 0
        self.total_connect_time = 0
        self.total_server_response_time = 0
        self.total_download_time = 0
        self.total_request_time = 0
        self.total_intercept_time = 0
        self.total_sub_resource_num = 0
        self.total_decoded_size = 0
        self.total_encoded_size = 0
        self.total_proxy_time = 0
        self.server_response_time_percentage = 0
        self.download_time_percentage = 0
        self.end_before_first_frame_num = 0

    def __getitem__(self, key):
        return getattr(self, key)

    def __setitem__(self, key, value):
        setattr(self, key, value)
