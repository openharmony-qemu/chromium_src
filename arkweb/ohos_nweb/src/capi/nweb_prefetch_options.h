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
    CefString urlCef;
    CefString additionalHttpHeadersCef;
    int32_t minTimeBetweenPrefetchesMs = 500;
    bool ignoreCacheControlNoStore = false;

    PrefetchOptions(std::string url, std::string Headers) {
        urlCef.FromString(url);
        additionalHttpHeadersCef.FromString(Headers);
    }
  
    PrefetchOptions(std::string url, std::string Headers,
                    int32_t Time, bool flag) : minTimeBetweenPrefetchesMs(Time),  
                    ignoreCacheControlNoStore(flag) {
        urlCef.FromString(url);
        additionalHttpHeadersCef.FromString(Headers);
    }
};

#endif  // OHOS_NWEB_SRC_CAPI_NWEB_PREFETCH_OPTIONS_H_
