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

#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>

#include "base/test/task_environment.h"
#include "base/test/test_mock_time_task_runner.h"
#include "content/browser/scheduler/browser_io_thread_delegate.h"
#include "content/browser/scheduler/browser_task_executor.h"
#include "content/browser/scheduler/browser_task_priority.h"
#include "content/browser/scheduler/browser_ui_thread_scheduler.h"
#include "content/public/browser/web_contents.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_thread.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/init/gl_factory.h"
#define private public
#define protected public
#include "content/browser/media/session/media_session_impl.h"
#include "content/public/test/browser_task_environment.h"
#include "ohos_audio_output_stream.h"
#undef protected
#undef private

using namespace testing;
using namespace content;

namespace media {
namespace {
constexpr int kNumHops = 13;
constexpr int kNumTasks = 8;

const char kDeathMatcher[] = "Check failed:.*\n*.*BrowserTaskEnvironment";

void PostTaskToUIThread(int iteration, base::subtle::Atomic32* tasks_run);

void PostToThreadPool(int iteration, base::subtle::Atomic32* tasks_run) {
  if (iteration > 0)
    base::subtle::NoBarrier_AtomicIncrement(tasks_run, 1);

  if (iteration == kNumHops)
    return;

  base::ThreadPool::PostTask(
      FROM_HERE, base::BindOnce(&PostTaskToUIThread, iteration + 1, tasks_run));
}

void PostTaskToUIThread(int iteration, base::subtle::Atomic32* tasks_run) {
  if (iteration > 0)
    base::subtle::NoBarrier_AtomicIncrement(tasks_run, 1);

  if (iteration == kNumHops)
    return;

  GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(&PostToThreadPool, iteration + 1, tasks_run));
}

}

class OHOSAudioOutputStreamTest : public ::testing::Test {
 public:
  OHOSAudioOutputStreamTest() = default;
  
  void SetUp() override {
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
    CreateOutputStream();

  }
  
  void CreateOutputStream() {
    params_ = AudioParameters(AudioParameters::Format::AUDIO_PCM_LINEAR,
                      ChannelLayoutConfig::FromLayout<CHANNEL_LAYOUT_MONO>(),
                      8000,
                      160);
    change_callback_ = std::make_unique<OHOSAudioOutputStream>(nullptr, params_, false);
    ASSERT_NE(change_callback_, nullptr);
  }
  
  void TearDown() override {
    change_callback_.reset();
  }

 protected:
  AudioParameters params_;
  std::unique_ptr<OHOSAudioOutputStream> change_callback_;
};
}  // namespace media