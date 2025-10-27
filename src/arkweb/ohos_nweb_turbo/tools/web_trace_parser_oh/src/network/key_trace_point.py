# 子资源 的起始点和结束点
step_begin_SubResource = "ResourceFetcher::requestResource"
step_end_SubResource = "ThrottlingURLLoader::OnReceiveResponse"

# 请求发起时间 （网络线程）
step_begin_subresource_net = "URLLoader::URLLoader"

# 排队
step_queue = "HttpCacheTransaction::Start"
step_queue_end_1 = "URLLoader::ResumeStart"
step_queue_end_2 = "URLLoader::ScheduleStart"

# connect_start到connect_stop
# step_connect_start = "HttpStreamRequest::HttpStreamRequest | connect_start"
# step_connect_end = "HttpStreamRequest::~HttpStreamRequest | connect_stop"
step_connect_start = "HttpNetworkTransaction::DoCreateStream |"
step_connect_end = "HttpNetworkTransaction::DoCreateStreamComplete |"

# 主资源
main_resource_step = "NavigationControllerImpl::LoadURLWithParams"

# method
method_step = "ResourceFetcher::requestResource"

# size
size_step = "URLLoader::NotifyCompleted"

# request_time
request_time_step = "HttpNetworkTransaction::DoSendRequestComplete"

# response_time
# response_time_step = "HttpNetworkTransaction::DoReadHeadersComplete"
response_time_step = "HttpNetworkTransaction::DoReadHeadersComplete"

# response_time
# response_end_step = "HttpNetworkTransaction::DoSendRequestComplete"
response_end_step = "HttpNetworkTransaction::DoReadBodyComplete"

# http_version
http_version_step = "URLLoader::BuildResponseHead"

# 请求结束时间 （网络线程）
step_end_subresource_net = "URLLoader::NotifyCompleted"

# 资源是否走了代理
is_intercepted_step = "InterceptedRequest::InterceptResponseReceived"

# 开始传输数据
intercepted_start_send_data = "InterceptedRequestHandlerWrapper::OnRequestResponse"

# 数据传输完成
intercepted_send_data_complete = "StreamReaderURLLoader::OnReaderReadCompleted"

# render进程开始接收数据
intercepted_render_start_receive = "ThrottlingURLLoader::OnReceiveResponse"

# render进程开始接收数据
intercepted_render_receive_complete = "ResourceRequestSender::OnRequestComplete"

# render进程数据接收完成


# 资源是否存在回调
has_callback = "XHRReadyStateChange"

# id_map
request_id_map = "ResourceRequestSender::SendAsync"

# response code
response_code_step = "URLLoader::NotifyCompleted"

# timing_info
network_timing_info = "URLLoader::PrintNetworkTimingInfo"

# cache_info
network_cache_info = "URLLoader::PrintNetworkCacheInfo"
