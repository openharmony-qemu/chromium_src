# step_end_TouchUp2NWeb = ["CreateNWeb", "Navigator::OnBeginNavigation"]
# 响应
step_end_TouchUp2NWeb = [
    "CreateNWeb",
    "NavigationControllerImpl::LoadURLWithParams",
    "LocalFrame::Navigate",
    "FrameLoader::updateForSameDocumentNavigation"
]

# web组件初始化 的开始点和结束点集合
step_begin_NWebInit = ["CreateNWeb"]
step_end_NWebInit = ["CreateNWeb"]
step_create_nweb = "CreateNWeb"

# 主资源下载 的开始点和结束点集合
step_begin_main_resource = ["NavigationControllerImpl::LoadURLWithParams", "LocalFrame::Navigate"]
# step_begin_MainResource = ["NavigationControllerImpl::LoadURLWithParams", "Navigator::Navigate",
#                            "Navigator::OnBeginNavigation"]
# step_end_MainResource = ["RenderFrameHostImpl::SendCommitNavigation"]
step_end_MainResource = ["ThrottlingURLLoader::OnReceiveResponse"]

# Commit主资源 的结束点集合
step_end_CommitMainResource = ["NavigationBodyLoader::StartLoadingBody", "DocumentLoader::BodyDataReceived"]


# 子资源 的起始点和结束点集合（render进程）
step_begin_SubResource = ["ResourceFetcher::requestResource"]
step_end_SubResource = ["ThrottlingURLLoader::OnReceiveResponse"]

# 子资源状态码
step_response_code = ["URLLoader::NotifyCompleted | responseCode"]

# EvaluateScript和v8.callFunction
step_EvaluateScript = "EvaluateScript"
step_v8_callFunction = "v8.callFunction"

# 运行在render进程关键方法名称
render_key_function = [
    "ResourceFetcher::requestResource",
    "EvaluateScript"
]

# DCL
step_dom_content_loaded_event = "RenderFrameImpl::DidDispatchDOMContentLoadedEvent"

# 备用结束时间
step_swap_buffers = "SkiaOutputSurfaceImplOnGpu::SwapBuffers"

# 请求发起时间 （网络线程）
step_begin_subresource_net = "URLLoader::URLLoader"

step_connect_start = "HttpNetworkTransaction::DoCreateStream |"
step_connect_end = "HttpNetworkTransaction::DoCreateStreamComplete |"

# request_time
request_time_step = "HttpNetworkTransaction::DoSendRequestComplete"

# response_time
response_time_step = "HttpNetworkTransaction::DoReadHeadersComplete"

# download_time
response_end_step = "HttpNetworkTransaction::DoReadBodyComplete"

# http_version
http_version_step = "URLLoader::BuildResponseHead"

step_end_subresource_net = "ThrottlingURLLoader::OnReceiveResponse"

# useragent changed
useragent_changed = "WebContentsImpl::SetUserAgentOverride"

# FP
first_paint = "firstPaint"

# FCP
first_contentful_paint = "firstContentfulPaint"
