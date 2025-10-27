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

#include "ui/gl/init/gl_factory.h"
#include "content/browser/scheduler/browser_io_thread_delegate.h"
#include "content/browser/scheduler/browser_task_executor.h"
#include "content/browser/scheduler/browser_task_priority.h"
#include "content/browser/scheduler/browser_ui_thread_scheduler.h"


#define ARKWEB_UNITTESTS_CREATE_FOR_TESTING() \
    auto ui_sequence_manager_ = \
        base::sequence_manager::CreateUnboundSequenceManager( \
            base::sequence_manager::SequenceManager::Settings::Builder() \
                .SetPrioritySettings( \
                    content::internal::CreateBrowserTaskPrioritySettings()) \
                .Build()); \
    auto browser_ui_thread_scheduler = \
        content::BrowserUIThreadScheduler::CreateForTesting( \
            ui_sequence_manager_.get()); \
    content::BrowserTaskExecutor::CreateForTesting( \
        std::move(browser_ui_thread_scheduler), \
        std::make_unique<content::BrowserIOThreadDelegate>());