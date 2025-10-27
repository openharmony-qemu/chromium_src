/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

namespace net_service {

namespace {

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
CefRefPtr<CefResponse> ExtractHttpErrorResponse(
    const net::HttpResponseHeaders* headers) {
  CefRefPtr<CefResponse> response = CefResponse::Create();
  response->SetStatus(headers->response_code());
  response->SetStatusText(headers->GetStatusText());

  size_t headers_line = 0;
  std::string header_name, header_value;
  CefResponse::HeaderMap map;
  while (headers->EnumerateHeaderLines(&headers_line, &header_name,
                                       &header_value)) {
    map.insert({CefString(header_name), CefString(header_value)});
  }
  response->SetHeaderMap(map);
  std::string mime_type;
  std::string encoding;
  headers->GetMimeType(&mime_type);
  headers->GetCharset(&encoding);
  response->SetMimeType(CefString(mime_type));
  response->SetCharset(CefString(encoding));
  return response;
}
#endif

}  // namespace

class InterceptedRequestUtils {
 public:
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  static void RestartExt(const GURL original_url,
                         raw_ptr<InterceptedRequest> obj) {
    obj->factory_->request_handler_->OnBeforeRequest(
        obj->id_, &obj->request_, obj->request_was_redirected_,
        obj->weak_factory_.GetWeakPtr(),
        base::BindOnce(&InterceptedRequest::BeforeRequestReceived,
                       obj->weak_factory_.GetWeakPtr(), original_url),
        base::BindOnce(&InterceptedRequest::CancelRequest,
                       obj->weak_factory_.GetWeakPtr()));
  }
#endif

  static void OnReceiveResponseExt(raw_ptr<InterceptedRequest> obj) {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
    bool must_download = content::download_utils::MustDownload(
        nullptr, obj->request_.url, obj->current_response_->headers.get(),
        obj->current_response_->mime_type);
    bool known_mime_type =
        blink::IsSupportedMimeType(obj->current_response_->mime_type);
    obj->is_download_ = !obj->current_response_->intercepted_by_plugin &&
                        (must_download || !known_mime_type);
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
    const uint32_t response_code_400 = 400;
    if (obj->current_response_->headers &&
        static_cast<uint32_t>(obj->current_response_->headers->response_code()) >= response_code_400) {
      // The WebViewClient onReceivedHttpError callback will be invoked for any
      // resource (such as main page, iframe, image, etc.) with status code >= 4
      auto error_reponse =
          ExtractHttpErrorResponse(obj->current_response_->headers.get());
      CefRefPtr<ArkWebRequestImplExt> request = new ArkWebRequestImplExt();
      request->SetURL(CefString(obj->request_.url.spec()));
      request->SetMethod(CefString(obj->request_.method));
      request->Set(obj->request_.headers);
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
      request->SetDestination(obj->request_.destination);
      obj->OnHttpErrorForUIThread(obj->id_, request, request->IsMainFrame(),
                                  obj->request_.has_user_gesture,
                                  error_reponse);
#endif
    }
#endif
  }

  static void FollowRedirectExt(
      std::vector<std::string> removed_headers,
      net::HttpRequestHeaders modified_headers,
      const net::HttpRequestHeaders& modified_cors_exempt_headers,
      const std::optional<GURL>& new_url,
      raw_ptr<InterceptedRequest> obj) {
#if BUILDFLAG(IS_ARKWEB)
    // We will not create a new url loader for redirects. However, the cef
    // controls the add/save of cookies, so we need to load cookies and then
    // transfer them to the network layer. Will only merge cookie headers
    // bellow.
    modified_headers.MergeFrom(obj->request_.headers);
    if (obj->target_loader_) {
      obj->target_loader_->FollowRedirect(removed_headers, modified_headers,
                                          modified_cors_exempt_headers,
                                          new_url);
    }
#endif
  }

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  static void ContinueAfterInterceptWithOverrideExt(
      std::unique_ptr<ResourceResponse> response,
      raw_ptr<InterceptedRequest> obj) {
    if (obj->request_.method == "OPTIONS") {
      obj->current_request_uses_header_client_ = false;

      DCHECK(!obj->stream_loader_);
      obj->stream_loader_ = std::make_unique<StreamReaderURLLoader>(
          obj->id_, obj->request_,
          obj->proxied_client_receiver_.BindNewPipeAndPassRemote(),
          mojo::NullRemote(), obj->traffic_annotation_,
          std::move(obj->current_cached_metadata_),
          std::make_unique<InterceptDelegate>(std::move(response),
                                              obj->weak_factory_.GetWeakPtr()));
    } else {
      obj->current_request_uses_header_client_ = true;

      obj->stream_loader_ = std::make_unique<StreamReaderURLLoader>(
          obj->id_, obj->request_,
          obj->proxied_client_receiver_.BindNewPipeAndPassRemote(),
          obj->header_client_receiver_.BindNewPipeAndPassRemote(),
          obj->traffic_annotation_, std::move(obj->current_cached_metadata_),
          std::make_unique<InterceptDelegate>(std::move(response),
                                              obj->weak_factory_.GetWeakPtr()));
    }
#endif
  }
};

#if BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)
void InterceptedRequest::OnTransferDataWithSharedMemory(
    base::ReadOnlySharedMemoryRegion region,
    uint64_t buffer_size) {
#if BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)
  LOG(DEBUG)
      << "shared-memory InterceptedRequest::OnTransferDataWithSharedMemory "
         "buffer_size="
      << buffer_size;
#endif
  target_client_->OnTransferDataWithSharedMemory(std::move(region),
                                                 buffer_size);
}
#endif

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
void InterceptedRequest::CancelRequest(int error_code) {
  // Donn't cancel network requests. Network requests should be canceled by the
  // holder instead of following the tab, such as serviceworker download, etc.
  // Although the tab is destroyed, the request still needs to be maintained.
  if (!is_download_) {
    network::URLLoaderCompletionStatus status(error_code);
    status.abort_due_to_cef_browser_destroyed = true;
    SendErrorStatusAndCompleteImmediately(status);
  }
}
#endif  //  ARKWEB_EX_DOWNLOAD

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
void InterceptedRequest::OnHttpErrorForUIThread(
    int32_t id,
    CefRefPtr<CefRequest> request,
    bool is_main_frame,
    bool has_user_gesture,
    CefRefPtr<CefResponse> error_response) {
  if (!factory_) {
    LOG(INFO) << "factory is invalid";
    return;
  }
  if (!factory_->request_handler_) {
    LOG(INFO) << "request handler is invalid";
    return;
  }
  factory_->request_handler_->OnHttpError(id, request, is_main_frame,
                                          has_user_gesture, error_response);
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
// static
void ProxyURLLoaderFactory::CreateProxy(
    content::BrowserContext* browser_context,
    network::URLLoaderFactoryBuilder& factory_builder,
    mojo::PendingRemote<network::mojom::TrustedURLLoaderHeaderClient>*
        header_client,
    std::unique_ptr<InterceptedRequestHandler> request_handler,
    network::mojom::URLLoaderFactoryOverridePtr* factory_override) {
  CEF_REQUIRE_UIT();
  DCHECK(request_handler);
  mojo::PendingReceiver<network::mojom::URLLoaderFactory> proxied_receiver;
  mojo::PendingRemote<network::mojom::URLLoaderFactory> target_factory_remote;
  if (factory_override) {
    // We are interested in factories "inside" of CORS, so use
    // |factory_override|.
    *factory_override = network::mojom::URLLoaderFactoryOverride::New();
    proxied_receiver =
        (*factory_override)
            ->overriding_factory.InitWithNewPipeAndPassReceiver();
    (*factory_override)->overridden_factory_receiver =
        target_factory_remote.InitWithNewPipeAndPassReceiver();
    (*factory_override)->skip_cors_enabled_scheme_check = true;
  } else {
    // In this case, |factory_override| is not given. But all callers of
    // ContentBrowserClient::WillCreateURLLoaderFactory guarantee that
    // |factory_override| is null only when the security features on the network
    // service is no-op for requests coming to the URLLoaderFactory. Hence we
    // can use |factory_builder| here.
    std::tie(proxied_receiver, target_factory_remote) =
        factory_builder.Append();
  }
  mojo::PendingReceiver<network::mojom::TrustedURLLoaderHeaderClient>
      header_client_receiver;
  if (header_client) {
    header_client_receiver = header_client->InitWithNewPipeAndPassReceiver();
  }

  content::ResourceContext* resource_context =
      browser_context->GetResourceContext();
  DCHECK(resource_context);

  CEF_POST_TASK(
      CEF_IOT,
      base::BindOnce(
          &ProxyURLLoaderFactory::CreateOnIOThread, std::move(proxied_receiver),
          std::move(target_factory_remote), std::move(header_client_receiver),
          base::Unretained(resource_context), std::move(request_handler)));
}
#endif

#if BUILDFLAG(ARKWEB_DOWNLOAD)
void ProxyURLLoaderFactory::CreateLoaderAndStartForDownloadRequest(
    mojo::PendingReceiver<network::mojom::URLLoader> receiver,
    int32_t request_id,
    uint32_t options,
    network::ResourceRequest request,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client,
    const net::MutableNetworkTrafficAnnotationTag& traffic_annotation) {
  network::ResourceRequest request_for_ua = request;
  if (request_handler_ && !request_handler_->GetCustomUserAgent().empty()) {
    request_for_ua.headers.SetHeaderIfMissing(
        net::HttpRequestHeaders::kUserAgent,
        request_handler_->GetCustomUserAgent());
  }

  if (target_factory_) {
    target_factory_->CreateLoaderAndStart(
        std::move(receiver), request_id, options, request_for_ua,
        std::move(client), traffic_annotation);
  }
}
#endif

#if BUILDFLAG(ARKWEB_COOKIE)
void ModifyOptions(uint32_t& options) {
  if (!NetHelpers::IsAllowAcceptCookies()) {
    options |= network::mojom::kURLLoadOptionBlockAllCookies;
  } else if (!NetHelpers::IsThirdPartyCookieAllowed()) {
    options |= network::mojom::kURLLoadOptionBlockThirdPartyCookies;
  }
}
#endif

}  // namespace net_service
