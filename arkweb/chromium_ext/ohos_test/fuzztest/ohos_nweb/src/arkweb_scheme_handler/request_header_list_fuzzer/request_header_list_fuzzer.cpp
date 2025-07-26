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

#include <fuzzer/FuzzedDataProvider.h>

#include "arkweb_scheme_handler.h"
#include "cef_request.h"
#include "ndk/scheme_handler/resource_request.h"

namespace OHOS::NWeb {
bool LLVMFuzzerTestSetup() {
  return true;
}

extern "C" void OH_ArkWebRequestHeaderList_Create(
    const ArkWeb_ResourceRequest* resource_request,
    ArkWeb_RequestHeaderList** request_header_list);

void FuzzApi(const uint8_t* data, size_t size) {
  FuzzedDataProvider dataProvider(data, size);
  CefRefPtr<CefRequest> cef_request = CefRequest::Create();
  CefRequest::HeaderMap headerMap;
  std::string header_key = dataProvider.ConsumeRandomLengthString();
  std::string header_value = dataProvider.ConsumeRandomLengthString();
  headerMap.insert(std::make_pair(header_key, header_value));
  cef_request->SetHeaderMap(headerMap);
  ArkWeb_ResourceRequest* resource_request =
      new ArkWeb_ResourceRequest(cef_request);
  ArkWeb_RequestHeaderList* request_header_list = nullptr;
  OH_ArkWebRequestHeaderList_Create(resource_request, &request_header_list);
  OH_ArkWebRequestHeaderList_GetSize(request_header_list);
  int32_t index = dataProvider.ConsumeIntegral<int32_t>();
  char* key = nullptr;
  char* value = nullptr;
  OH_ArkWebRequestHeaderList_GetHeader(request_header_list, index, &key,
                                       &value);
  OH_ArkWebResourceRequest_SetUserData(resource_request, nullptr);
  OH_ArkWebResourceRequest_GetUserData(resource_request);
  char* method = nullptr;
  OH_ArkWebResourceRequest_GetMethod(resource_request, &method);
  char* url = nullptr;
  OH_ArkWebResourceRequest_GetUrl(resource_request, &url);
  ArkWeb_HttpBodyStream* http_body_stream = nullptr;
  OH_ArkWebResourceRequest_GetHttpBodyStream(resource_request,
                                             &http_body_stream);
  OH_ArkWebResourceRequest_DestroyHttpBodyStream(http_body_stream);
  OH_ArkWebResourceRequest_GetResourceType(resource_request);
  char* frame_url = nullptr;
  OH_ArkWebResourceRequest_GetFrameUrl(resource_request, &frame_url);
  char* referrer = nullptr;
  OH_ArkWebResourceRequest_GetReferrer(resource_request, &referrer);
  ArkWeb_RequestHeaderList* head_list = nullptr;
  OH_ArkWebResourceRequest_GetRequestHeaders(resource_request, &head_list);
  OH_ArkWebResourceRequest_IsRedirect(resource_request);
  OH_ArkWebResourceRequest_IsMainFrame(resource_request);
  OH_ArkWebResourceRequest_HasGesture(resource_request);
  OH_ArkWebRequestHeaderList_Destroy(request_header_list);
}
}  // namespace OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size < sizeof(int32_t))) {
    return 0;
  }
  static bool init = OHOS::NWeb::LLVMFuzzerTestSetup();
  if (init) {
    OHOS::NWeb::FuzzApi(data, size);
  }
  return 0;
}
