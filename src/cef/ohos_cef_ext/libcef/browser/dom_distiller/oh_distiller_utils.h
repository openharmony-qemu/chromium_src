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

#ifndef CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_DISTILLER_UTILS_H_
#define CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_DISTILLER_UTILS_H_

#include <memory>
#include <string>

#include "url/gurl.h"

namespace dom_distiller {
class DistilledArticleProto;
}

namespace oh_dom_distiller {

inline constexpr int kNoPageErrorCode = -2;
inline constexpr int kNoContentErrorCode = -3;
inline constexpr int kNoChaptersErrorCode = -4;
inline constexpr int kDistillCancelledErrorCode = -5;
inline constexpr char kErrorMsgNoPage[] = "no page distilled";
inline constexpr char kErrorMsgNoContent[] = "no content";
inline constexpr char kErrorMsgNoChapters[] = "no chapters";
inline constexpr char kErrorMsgDistillCancelled[] = "distill is cancelled.";

inline constexpr char kResultCode[] = "resultCode";
inline constexpr char kResultMessage[] = "resultMessage";

/**
 * @brief Converts a DistilledArticleProto protocol buffer object to a
 * standardized JSON format string
 *
 * @param article_proto Constant pointer to the protocol buffer object to
 * convert. Ownership remains with caller. Requirements: Must be non-null and
 * contain at least one valid field
 *
 * @return std::string Standardized JSON data string. Example format:
 */
std::unique_ptr<std::string> FormatDistilledArticleProtoToJsonData(
    const dom_distiller::DistilledArticleProto* article_proto,
    const GURL& url);
}  // namespace oh_dom_distiller

#endif  // CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_DISTILLER_UTILS_H_
