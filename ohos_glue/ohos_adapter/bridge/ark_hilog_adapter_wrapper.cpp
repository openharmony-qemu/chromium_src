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

#include "hilog_adapter.h"
#include "ohos_adapter/bridge/ark_web_adapter_bridge_helper.h"

namespace OHOS::NWeb {

int HiLogAdapter::PrintLog(LogLevelAdapter level, const char *tag,
                           const char *fmt, ...) {
  using HiLogAdapterPrintLogFunc =
      int (*)(uint32_t level, const char *tag, const char *fmt, va_list ap);
  static HiLogAdapterPrintLogFunc hi_log_adapter_print_log =
      reinterpret_cast<HiLogAdapterPrintLogFunc>(
          ArkWeb::ArkWebAdapterBridgeHelper::GetInstance(false).LoadFuncSymbol(
              "HiLogAdapterPrintLog", false));
  if (!hi_log_adapter_print_log) {
    return -1;
  }

  int ret;
  va_list ap;
  va_start(ap, fmt);
  ret = hi_log_adapter_print_log((uint32_t)level, tag, fmt, ap);
  va_end(ap);
  return ret;
}

} // namespace OHOS::NWeb
