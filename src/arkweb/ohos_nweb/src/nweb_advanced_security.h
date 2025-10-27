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

#ifndef NWEB_ADVANCED_SECURITY_H_
#define NWEB_ADVANCED_SECURITY_H_

#include <cstdint>

namespace OHOS::NWeb {
  class NWebAdvancedSecurityHelper {
    public:
      ~NWebAdvancedSecurityHelper() = default;

      enum Feature {
        SECURE_SHIELD_ENABLED = (1<<0),
        ENABLE_WEBASM = (1<<1),
        ENABLE_JIT = (1<<2),
        ENABLE_WEBGL = (1<<3),
        ENABLE_PDFVIEWER = (1<<4),
        ENABLE_MATHML = (1<<5),
        ENABLE_SPEECHAPI = (1<<6),
        ENABLE_RTCDATACHANNEL = (1<<7),
        ENABLE_GETUSERMEDIA = (1<<8),
        ENABLE_SERVICEWORKER = (1<<9),
        ENABLE_WEBRTC = (1<<10),
      };

      static NWebAdvancedSecurityHelper Inst() {
        static NWebAdvancedSecurityHelper inst;
        return inst;
      }

      /*
        return true when security feature is opened.
        return false when security feature is banned.
        for SECURE_SHIELD_ENABLED 1 represent true.
        for other bits 0 represent true.
      */
      bool IsSecFeatureEnabled(uint32_t cursor) {
        if (cursor == SECURE_SHIELD_ENABLED) {
          return cursor & adstat_;
        }
        return (!(cursor & adstat_)) && (adstat_ & SECURE_SHIELD_ENABLED);
      }

      uint32_t GetAdStat() {
        return adstat_;
      }

    private:
      NWebAdvancedSecurityHelper();
      uint32_t adstat_ = 0;
  };
} // namespace OHOS::NWeb
#endif