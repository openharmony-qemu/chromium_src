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

public:
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
void OnRequestError(int32_t request_id,
                    const network::ResourceRequest& request,
                    int error_code,
                    bool safebrowsing_hit) override {
  if (init_state_ && wrapper_helper_) {
    wrapper_helper_->OnRequestError(init_state_->browser_, init_state_->frame_,
                                    request_id, request, error_code,
                                    safebrowsing_hit);
  }
}
#endif

void GetOhosResourceHandlerResult(
    int32_t request_id,
    CefRefPtr<CefResourceHandler> resource_handler,
    ShouldInterceptRequestResultCallback callback) {
  CEF_REQUIRE_IOT();
  RequestState* state = GetState(request_id);
  if (!state || !state->request_) {
    std::move(callback).Run(nullptr);
    return;
  }

#if BUILDFLAG(ARKWEB_PRECOMPILE)
  auto response_cache = oh_code_cache::ResponseCache::CreateResponseCache(
      state->request_->url.spec());
  if (response_cache != nullptr && response_cache->CanUseCache()) {
    TRACE_EVENT1("net", "Response Cache InterceptRequest", "url",
                 state->request_->url.spec().c_str());
    LOG(DEBUG) << "Use intercept request with response cache. url: "
               << url::LogUtils::ConvertUrlWithMask(state->request_->url.spec());
    auto resource_response = std::make_unique<oh_code_cache::ResourceResponse>(
        std::move(response_cache));
    std::move(callback).Run(std::move(resource_response));
    return;
  }
#endif

  if (!resource_handler && state->scheme_factory_) {
    // Does the scheme factory want to handle the request?
    resource_handler = state->scheme_factory_->Create(
        init_state_->browser_, init_state_->frame_,
        state->request_->url.scheme(), state->pending_request_.get());
  }

#if BUILDFLAG(ARKWEB_PREFETCH_POST)
  std::optional<std::string> key =
      state->request_->headers.GetHeader(POST_CACHE_KEY);
  if (!resource_handler &&
      state->request_->method == net::HttpRequestHeaders::kPostMethod &&
      key.has_value()) {
    // TRACE_EVENT0("net",
    //              "GetOhosResourceHandlerResult get post prefetched cache");
    state->request_->headers.RemoveHeader(POST_CACHE_KEY);
    std::vector<CefBrowserContext*> browser_context_all =
        CefBrowserContext::GetAll();
    if (browser_context_all.size() != 0) {
      CefBrowserContext* context = browser_context_all[0];
      content::BrowserContext* browser_context = context->AsBrowserContext();
      if (browser_context) {
        ohos_predictors::LoadingPredictor* loading_predictor =
            ohos_predictors::LoadingPredictorFactory::GetForBrowserContext(
                browser_context);
        resource_handler = loading_predictor->GetResourceHandler(
            state->request_->url, key.value());
      }
    }
  }
#endif

  std::unique_ptr<ResourceResponse> resource_response;
  if (resource_handler) {
    resource_response = CreateResourceResponse(request_id, resource_handler);
    DCHECK(resource_response);
    state->was_custom_handled_ = true;
  } else {
    // The request will be handled by the NetworkService. Remove the
    // "Accept-Language" header here so that it can be re-added in
    // URLRequestHttpJob::AddExtraHeaders with correct ordering applied.
#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
    // Get the chunked data pipe remote back.
    if (state->pending_request_) {
      CefRefPtr<ArkWebCefPostDataStream> post_data_stream =
          state->pending_request_->AsArkWebRequestExt()->GetUploadStream();
      if (post_data_stream && post_data_stream->IsChunked()) {
        LOG(INFO) << "scheme_handler get the chunked stream back.";
        post_data_stream->GetChunkedDataPipeGetter(
            state->request_->request_body.get());
      }
    }
#endif
  }

  // Continue the request.
  std::move(callback).Run(std::move(resource_response));
}

void GetOhosResourceHandlerInUiTask(
    int32_t request_id,
    ShouldInterceptRequestResultCallback callback,
    std::string url) {
  LOG(INFO) << "Get Ohos ResourceHandler";
  CEF_REQUIRE_UIT();
  RequestState* state = GetState(request_id);
  if (!state || !state->request_) {
    std::move(callback).Run(nullptr);
    return;
  }
  CefRefPtr<CefResourceHandler> resource_handler;

  if (state->handler_) {
    // Does the client want to handle the request?
    resource_handler = state->handler_->GetResourceHandler(
        init_state_->browser_, init_state_->frame_,
        state->pending_request_.get());
  }

  // Try to get scheme handler from ets UI thread.
  if (!resource_handler && state->scheme_factory_) {
    // Does the scheme factory want to handle the request?
    if (!url.starts_with("hwweb")) {
      resource_handler = state->scheme_factory_->Create(
          init_state_->browser_, init_state_->frame_,
          state->request_->url.scheme(), state->pending_request_.get());
    }
  }

  CEF_POST_TASK(
      CEF_IOT,
      base::BindOnce(
          &InterceptedRequestHandlerWrapper::GetOhosResourceHandlerResult,
          weak_ptr_factory_.GetWeakPtr(), request_id, resource_handler,
          std::move(callback)));
}

void GetOhosResourceHandlerResultInIO(
    int32_t request_id,
    ShouldInterceptRequestResultCallback callback,
    CefRefPtr<CefResourceHandler> resource_handler) {
  CEF_REQUIRE_IOT();
  RequestState* state = GetState(request_id);
  if (!state) {
    std::move(callback).Run(nullptr);
    return;
  }
  GetOhosResourceHandlerResult(request_id, resource_handler,
                               std::move(callback));
}

void GetOhosResourceHandlerStillInIO(
    int32_t request_id,
    ShouldInterceptRequestResultCallback callback) {
  CEF_REQUIRE_IOT();
  RequestState* state = GetState(request_id);
  if (!state || !state->request_) {
    std::move(callback).Run(nullptr);
    return;
  }
  CefRefPtr<OhosInterceptCallbackWrapper> callback_ptr =
      new OhosInterceptCallbackWrapper(base::BindOnce(
          &InterceptedRequestHandlerWrapper::GetOhosResourceHandlerResultInIO,
          weak_ptr_factory_.GetWeakPtr(), request_id, std::move(callback)));

  if (state->handler_) {
    // Does the client want to handle the request?
    state->pending_request_->AsArkWebRequestExt()->SetDestination(
        state->request_->destination);
    state->handler_->AsArkWebResourceRequestHandlerExt()
        ->GetResourceHandlerByIO(init_state_->browser_, init_state_->frame_,
                                 state->pending_request_.get(), callback_ptr,
                                 state->scheme_factory_,
                                 state->request_->url.scheme());
  } else {
    GetOhosResourceHandlerFromETS(state->scheme_factory_,
                                  state->pending_request_.get(),
                                  state->request_->url.scheme(), callback_ptr);
  }
}

void GetOhosResourceHandlerFromETS(
    CefRefPtr<CefSchemeHandlerFactory> scheme_factory,
    CefRefPtr<CefRequest> request,
    const CefString& scheme,
    CefRefPtr<CefInterceptCallback> callback) {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(
            &InterceptedRequestHandlerWrapper::GetOhosResourceHandlerFromETS,
            weak_ptr_factory_.GetWeakPtr(), scheme_factory, request, scheme,
            callback));
    return;
  }

  CefRefPtr<CefResourceHandler> resource_handler = nullptr;
  if (scheme_factory) {
    // Does the scheme factory want to handle the request?
    resource_handler = scheme_factory->Create(
        init_state_->browser_, init_state_->frame_, scheme, request);
  }
  callback->ContinueLoad(resource_handler);
}

void GetOhosResourceHandler(int32_t request_id,
                            ShouldInterceptRequestResultCallback callback) {
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  RequestState* state = GetState(request_id);
  if (state && state->request_) {
    // Add fetch meta data headers.
    bool old_flag = state->pending_request_->IsReadOnly();
    state->pending_request_->SetReadOnly(false);
    if (state->request_->request_initiator.has_value()) {
      CefString frame_url =
          init_state_->real_frame_ ? init_state_->real_frame_->GetURL() : "";
      state->pending_request_->AsArkWebRequestExt()->SetFrameUrl(frame_url);
    }

    if (state->pending_request_->AsArkWebRequestExt()->GetResourceType() ==
        RT_SUB_FRAME) {
      CefRefPtr<CefFrame> parent_frame =
          init_state_->real_frame_ ? init_state_->real_frame_->GetParent()
                                   : nullptr;
      CefString frame_url = "";
      if (parent_frame) {
        frame_url = parent_frame->GetURL();
      }
      state->pending_request_->AsArkWebRequestExt()->SetFrameUrl(frame_url);
    }
    bool has_user_activation = false;
    if (state->request_->trusted_params) {
      has_user_activation =
          state->request_->trusted_params->has_user_activation;
    }
    std::map<std::string, std::string> headers =
        network::GetFetchMetadataHeaders(
            state->request_->url, state->request_->mode, has_user_activation,
            state->request_->destination, state->request_->request_initiator);
    for (auto& entry : headers) {
      state->pending_request_->SetHeaderByName(entry.first, entry.second,
                                               false);
    }
    state->pending_request_->SetReadOnly(old_flag);
  }
#endif

  CEF_REQUIRE_IOT();
  GetOhosResourceHandlerStillInIO(request_id, std::move(callback));
}

#if BUILDFLAG(ARKWEB_USERAGENT)
std::string GetCustomUserAgent() override {
  if (init_state_) {
    return init_state_->custom_user_agent_;
  }
  return "";
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
void OnHttpError(int32_t request_id,
                 CefRefPtr<CefRequest> request,
                 bool is_main_frame,
                 bool has_user_gesture,
                 CefRefPtr<CefResponse> error_response) override {
  if (wrapper_helper_) {
    wrapper_helper_->OnHttpError(init_state_->browser_, request, is_main_frame,
                                 has_user_gesture, error_response);
  }
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
void GetSettingOfNetHelper(const GURL& url, struct NetHelperSetting& setting) override {
  if (wrapper_helper_) {
    wrapper_helper_->GetSettingOfNetHelper(
        url, init_state_ ? init_state_->browser_ : nullptr, setting);
  }
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  void RedirectSavedCookieDone(int32_t request_id,
                               network::ResourceRequest* request,
                               OnRequestResponseResultCallback callback,
                               const GURL& new_url) {
    auto exec_callback = base::BindOnce(
        std::move(callback), ResponseMode::CONTINUE, nullptr, new_url);
    RequestState* state = GetState(request_id);
    if (!state) {
      // The request may have been canceled while the async callback was
      // pending.
      std::move(exec_callback).Run();
      return;
    }
    // Clear the cookie  first. we will get cookie for this redirect.
    request->headers.RemoveHeader(net::HttpRequestHeaders::kCookie);

    MaybeLoadCookies(request_id, state, new_url, std::move(exec_callback));
  }
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
std::string OnRewriteUrlForNavigation(
    const std::string& original_url,
    const std::string& referrer) override {
  if (wrapper_helper_) {
    return wrapper_helper_->OnRewriteUrlForNavigation(init_state_->browser_, original_url, referrer);
  }
  return "";
}
#endif

private:
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
std::unique_ptr<ArkWebInterceptedRequestHandlerWrapperHelper> wrapper_helper_;
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)
