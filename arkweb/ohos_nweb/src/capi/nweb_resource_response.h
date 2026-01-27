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
 
#ifndef OHOS_NWEB_RESOUCE_RESPONSE_H_
#define OHOS_NWEB_RESOUCE_RESPONSE_H_
 
#include "ohos_nweb/src/cef_delegate/nweb_url_resource_response_impl.h"
#include "ohos_nweb/src/capi/nweb_c_api.h"
#include "ohos_nweb/src/nweb_hilog.h"
 
struct NWebResourceResponse {
  int nweb_id_;
  std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponseImpl> resource_response_;
 
  NWebResourceResponse() : nweb_id_(-1) {
    WVLOG_I("NWebResourceResponse() is called");
  }
 
  ~NWebResourceResponse() { WVLOG_I("~NWebResourceResponse() is called"); }
 
  NWebResourceResponse(int nweb_id, std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponseImpl> response)
      : nweb_id_(nweb_id), resource_response_(response) {
    WVLOG_I("NWebResourceResponse() is called");
  }
 
  std::string GetEncoding() { return resource_response_->ResponseEncoding(); }
  int GetStatusCode() { return resource_response_->ResponseStatusCode(); }
  std::string GetReasonPhrase() { return resource_response_->ResponseStatus(); }
  std::map<std::string, std::string> GetResponseHeader() {
    return resource_response_->ResponseHeaders();
  }
  bool GetIsFromNetwork() { return resource_response_->ResponseIsFromNetwork(); }
 
  std::string GetMimeType() { return resource_response_->ResponseMimeType(); }
};
 
#endif  //  OHOS_NWEB_RESOUCE_RESPONSE_H_