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

#include "ohos_nweb/bridge/ark_web_spanstring_convert_html_callback_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebSpanstringConvertHtmlCallbackImpl::ArkWebSpanstringConvertHtmlCallbackImpl(
    std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback> nweb_spanstring_convert_html_callback)
    : nweb_spanstring_convert_html_callback_(nweb_spanstring_convert_html_callback) {}

ArkWebString ArkWebSpanstringConvertHtmlCallbackImpl::SpanstringConvertHtml(const ArkWebUint8Vector &content)
{
    std::vector<uint8_t> vecContent = ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(content);
    std::string htmlString = nweb_spanstring_convert_html_callback_->SpanstringConvertHtml(vecContent);
    return ArkWebStringClassToStruct(htmlString);
}

} // namespace OHOS::ArkWeb
