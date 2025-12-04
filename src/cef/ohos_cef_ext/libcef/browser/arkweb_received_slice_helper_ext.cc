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

#include "ohos_cef_ext/libcef/browser/arkweb_received_slice_helper_ext.h"

#include <vector>

#include "base/format_macros.h"
#include "base/strings/stringprintf.h"
#include "base/strings/string_number_conversions.h"
#include "components/download/public/common/download_item.h"
#include "include/capi/cef_base_capi.h"

namespace arkweb_received_slice_helper_ext {

std::vector<download::DownloadItem::ReceivedSlice> FromString(
    const std::string& input_str) {
  std::vector<download::DownloadItem::ReceivedSlice> received_slices;
  if (input_str.length() == 0) {
    return received_slices;
  }
  std::string::size_type num_pos = 0;
  int param_offset = 0;

  int64_t offset = 0;
  int64_t received_bytes = 0;
  bool finished = false;
  bool convert_result = false;
  for (std::string::size_type i = 0; i < input_str.size(); ++i) {
    if (input_str[i] == ',' || input_str[i] == ')') {
      if (num_pos < i) {
        std::string num_str = input_str.substr(num_pos, i - num_pos);
        int64_t num = 0;
        convert_result = base::StringToInt64(num_str, &num);
        if (!convert_result) {
          break;
        }
        if (param_offset == 0) {
          offset = num;
        } else if (param_offset == 1) {
          received_bytes = num;
        } else {
          finished = !!num;
          auto slice = download::DownloadItem::ReceivedSlice(
              offset, received_bytes, finished);
          received_slices.push_back(slice);
        }
        param_offset = (param_offset + 1) % 3;
      }
    }
    if (input_str[i] == '(' || input_str[i] == ',') {
      if (i < input_str.size() - 1 && input_str[i + 1] >= '0' &&
          input_str[i + 1] <= '9') {
        num_pos = i + 1;
      }
    }
  }
  if (!convert_result) {
    received_slices.clear();
  }
  return received_slices;
}

std::string SerializeToString(
    std::vector<download::DownloadItem::ReceivedSlice> slices) {
  std::string slices_str;
  for (auto slice : slices) {
    std::string offset_str = base::StringPrintf("%" PRId64, slice.offset);
    std::string received_bytes_str =
        base::StringPrintf("%" PRId64, slice.received_bytes);
    std::string finish_str = base::StringPrintf("%d", slice.finished);
    std::string slice_str =
        base::StringPrintf("(%s,%s,%s)", offset_str.c_str(),
                           received_bytes_str.c_str(), finish_str.c_str());
    slices_str = slices_str + slice_str;
  }
  return slices_str;
}
}  //  namespace arkweb_received_slice_helper_ext
