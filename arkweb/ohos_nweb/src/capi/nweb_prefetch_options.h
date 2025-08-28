/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_SRC_CAPI_NWEB_PREFETCH_OPTIONS_H_
#define OHOS_NWEB_SRC_CAPI_NWEB_PREFETCH_OPTIONS_H_

#include <stddef.h>
#include <string>

#include "cef/include/cef_base.h"


struct PrefetchOptions {
    CefString url_cef;
    CefString additional_http_headers_cef;
    int32_t min_time_between_prefetches = 500;
    bool ignore_cache_control_no_store = false;

    PrefetchOptions(std::string url, std::string headers) {
        url_cef.FromString(url);
        additional_http_headers_cef.FromString(headers);
    }
  
    PrefetchOptions(std::string url, std::string headers,
                    int32_t Time, bool flag) : min_time_between_prefetches(Time),  
                    ignore_cache_control_no_store(flag) {
        url_cef.FromString(url);
        additional_http_headers_cef.FromString(headers);
    }
};

#endif  // OHOS_NWEB_SRC_CAPI_NWEB_PREFETCH_OPTIONS_H_
