/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef OHOS_NWEB_RESOUCE_REQUEST_H_
#define OHOS_NWEB_RESOUCE_REQUEST_H_
 
#include "ohos_nweb/src/nweb_url_resource_request_impl.h"
#include "ohos_nweb/src/capi/nweb_c_api.h"
#include "ohos_nweb/src/nweb_hilog.h"
 
struct NWebResourceRequest {
  int nweb_id_;
  std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequestImpl> resource_request_;
 
  NWebResourceRequest() : nweb_id_(-1) {
    WVLOG_I("NWebResourceRequest() is called");
  }
 
  ~NWebResourceRequest() { WVLOG_I("~NWebResourceRequest() is called"); }
 
  NWebResourceRequest(int nweb_id, std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequestImpl> request)
      : nweb_id_(nweb_id), resource_request_(request) {
    WVLOG_I("NWebResourceRequest() is called");
  }
 
  std::map<std::string, std::string> GetRequestHeader() {
    return resource_request_->RequestHeaders();
  }
  bool IsRequestGesture() { return resource_request_->FromGesture(); }
  bool IsMainFrame() { return resource_request_->IsAboutMainFrame(); }
  bool IsRedirect() { return resource_request_->IsRequestRedirect(); }
  std::string GetRequestMethod() { return resource_request_->Method(); }
  int32_t GetPageTransition() { return resource_request_->GetPageTransition(); }
  int32_t GetRequestType() {return resource_request_->GetRequestType();}
 
  std::string GetRequestUrl() { return resource_request_->Url(); }
};
 
#endif  //  OHOS_NWEB_RESOUCE_REQUEST_RESPONSE_H_