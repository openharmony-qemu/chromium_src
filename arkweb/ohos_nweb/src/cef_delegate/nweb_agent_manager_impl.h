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

#ifndef NWEB_AGENT_MANAGER_IMPL_H
#define NWEB_AGENT_MANAGER_IMPL_H

#include "ohos_nweb/include/nweb_agent_manager.h"

#include "nweb_delegate.h"
#include "nweb_content_change_detection.h"
#include "nweb_highlight_specified_content.h"

namespace OHOS::NWeb {
class NWebAgentManagerImpl : public NWebAgentManager {
public:
    explicit NWebAgentManagerImpl(
        base::WeakPtr<NWebDelegate> nweb_delegate);
    ~NWebAgentManagerImpl() override = default;

    void SetAgentEnabled(bool enabled) override;

    bool IsAgentEnabled() override;

    void SetContentChangeDetectionConfig(int32_t min_report_time, float text_content_ratio) override;

    void SetAgentNeedHighlight(bool enabled) override;
private:
    base::WeakPtr<NWebDelegate> nweb_delegate_;

    std::unique_ptr<NWebContentChangeDetection> content_change_detection_;

    std::unique_ptr<NWebHighlightSpecifiedContent> highlight_specified_content_;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_AGENT_MANAGER_IMPL_H