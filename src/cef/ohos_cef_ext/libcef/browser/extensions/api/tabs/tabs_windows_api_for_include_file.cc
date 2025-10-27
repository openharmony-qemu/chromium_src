/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

void TabUpdated(int tab_id,
                content::WebContents* contents,
                std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
                std::unique_ptr<NWebExtensionTab> tab);

void TabActivated(int tab_id, int window_id, content::BrowserContext* browser_context);

void TabRemoved(int tab_id,
                bool is_window_closing,
                int window_id,
                content::WebContents* contents);

void TabAttached(int tab_id,
                content::WebContents* web_contents,
                int32_t new_position,
                int32_t new_window_id);

void TabCreated(int tab_id,
                content::BrowserContext* browserContext,
                std::unique_ptr<NWebExtensionTab> tab);

void TabDetached(content::WebContents* web_contents,
                int tab_id,
                int old_position,
                int old_window_id);

void TabHighlighted(content::WebContents* web_contents,
                    NWebExtensionTabHighlightInfo& highlight_info);

void TabMoved(content::WebContents* web_contents,
              int tab_id,
              std::unique_ptr<NWebExtensionTabMoveInfo> move_info);

void TabReplaced(content::WebContents* web_contents,
                 int32_t added_tab_id,
                 int32_t removed_tab_id);

void RegisterTabZoomObserver(content::WebContents* web_contents);
 
void UnregisterTabZoomObserver(content::WebContents* web_contents);
