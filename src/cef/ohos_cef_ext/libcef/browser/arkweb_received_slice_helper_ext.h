/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef CEF_LIBCEF_BROWSER_RECEIVED_SLICE_HELPER_EXT_H_
#define CEF_LIBCEF_BROWSER_RECEIVED_SLICE_HELPER_EXT_H_
#pragma once

#include <vector>

#include "components/download/public/common/download_item.h"

namespace arkweb_received_slice_helper_ext {

// Parse string to received_slice before resume an interrupted download.
std::vector<download::DownloadItem::ReceivedSlice> FromString(
    const std::string& cef_input_str);

// Parse received_slice to string before serialize
std::string SerializeToString(
    std::vector<download::DownloadItem::ReceivedSlice> slices);
}  // namespace arkweb_received_slice_helper_ext

#endif  //  CEF_LIBCEF_BROWSER_RECEIVED_SLICE_HELPER_EXT_H_
