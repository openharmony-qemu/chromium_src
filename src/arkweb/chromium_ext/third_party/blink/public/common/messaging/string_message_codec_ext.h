 /* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARKWEB_CHROME_EXT_THIRD_PARTY_PUBLIC_COMMON_MESSAGING_STRING_MESSAGE_CODEC_EXT_H_
#define ARKWEB_CHROME_EXT_THIRD_PARTY_PUBLIC_COMMON_MESSAGING_STRING_MESSAGE_CODEC_EXT_H_

#include <optional>
#include <string>
#include <vector>

#include "base/containers/span.h"
#include "third_party/abseil-cpp/absl/types/variant.h"
#include "third_party/blink/public/common/common_export.h"
#include "third_party/blink/public/common/messaging/transferable_message.h"

#if BUILDFLAG(ARKWEB_MSGPORT)
#include "third_party/blink/public/common/messaging/web_message_port.h"
#endif

namespace blink {

#if BUILDFLAG(ARKWEB_MSGPORT)
BLINK_COMMON_EXPORT TransferableMessage
EncodeWebMessagePayload(struct WebMessagePort::Message& encoded_data);

BLINK_COMMON_EXPORT bool DecodeToWebMessagePayload(
    const TransferableMessage& encoded_data,
    struct WebMessagePort::Message& decoded_data);
#endif
}  // namespace blink

#endif  // ARKWEB_CHROME_EXT_THIRD_PARTY_PUBLIC_COMMON_MESSAGING_STRING_MESSAGE_CODEC_EXT_H_
