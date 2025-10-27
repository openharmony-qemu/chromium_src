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

void DispatchTabUpdatedEvent(
    int tab_id,
    content::WebContents* contents,
    std::unique_ptr<NWebExtensionTabChangeInfo> change_info,
    std::unique_ptr<NWebExtensionTab> tab);

void DispatchTabActivatedEvent(
    int tab_id,
    int window_id,
    content::BrowserContext* browser_context);

void DispatchTabRemovedEvent(
    int tab_id,
    bool is_window_closing,
    int window_id,
    content::WebContents* contents);

void DispatchTabAttachedEvent(
    int tab_id,
    content::WebContents* contents,
    int32_t new_position,
    int32_t new_window_id);

void DispatchTabCreatedEvent(
    int tab_id,
    content::BrowserContext* browserContext,
    std::unique_ptr<NWebExtensionTab> tab);

void DispatchTabDetachedEvent(
    content::WebContents* contents,
    int tab_id,
    int old_position,
    int old_window_id);

void DispatchTabHighlightedEvent(
    content::WebContents* contents,
    NWebExtensionTabHighlightInfo& highlight_info);

void DispatchTabMovedEvent(
    content::WebContents* contents,
    int tab_id,
    std::unique_ptr<NWebExtensionTabMoveInfo> move_info);

void DispatchTabReplacedEvent(
    content::WebContents* contents,
    int32_t addedTabId,
    int32_t removedTabId);

void RegisterTabZoomObserver(content::WebContents* web_contents);
 
void UnregisterTabZoomObserver(content::WebContents* web_contents);