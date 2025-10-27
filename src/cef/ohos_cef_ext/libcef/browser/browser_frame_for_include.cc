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

#if BUILDFLAG(ARKWEB_CLIPBOARD)
void OnGetImageForContextNode(
    cef::mojom::GetImageForContextNodeParamsPtr params,
    int command_id) override;

void OnGetImageForContextNodeNull(int command_id) override;
#endif
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void ShouldOverrideUrlLoading(
    const std::string& url,
    const std::string& request_method,
    bool user_gesture,
    bool is_redirect,
    bool is_outermost_main_frame,
    cef::mojom::BrowserFrame::ShouldOverrideUrlLoadingCallback callback)
    override;
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void UpdateHitTestData(int32_t type, const std::string& extra_data) override;
#endif

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
void OverrideErrorPage(
    const std::string& url,
    const std::string& request_method,
    bool user_gesture,
    bool is_redirect,
    bool is_outermost_main_frame,
    int error_code,
    const std::string& error_text,
    cef::mojom::BrowserFrame::OverrideErrorPageCallback callback)
    override;
#endif