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

#include "ui/base/resource/mock_resource_bundle_delegate.h"
#include "base/test/test_simple_task_runner.h"
#include "arkweb/chromium_ext/content/renderer/media/ohos/native_texture_factory.h"

namespace content {

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnArg;

extern void InitializeWebKitExt(scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_);
extern const base::TimeDelta& GetMaxIPCLoginInterval();
extern base::TimeTicks& GetLastRendererLogSentTime(int policy);
extern void SetLastRendererLogSentTime(int policy, const base::TimeTicks& time);
extern bool& GetHasRendererLogDropped(int policy);
extern void SetHasRendererLogDropped(int policy, bool value);
extern void CallReportRendererLogOnMainThread(int policy, const std::string& msg);
extern bool CallHandleFatalMessageForCrashpad(int severity,
                                              const char* file,
                                              int line,
                                              size_t message_start,
                                              const std::string& string);
extern bool CallRenderProcessLogMessageHandler(int severity,
                                               const char* file,
                                               int line,
                                               size_t message_start,
                                               const std::string& str);

class RenderThreadImplExtUnittest : public RenderThreadImplBrowserTest {
public:
  void TestNotifyLocaleChanged(const std::string& locale) {
    thread_->NotifyLocaleChanged(locale);
  }

  void TestOnChannelConnected(int32_t peer_pid) {
    thread_->OnChannelConnected(peer_pid);
  }

  void TestOnChannelListenError() {
    thread_->OnChannelListenError();
  }
};

TEST_F(RenderThreadImplExtUnittest, InitializeWebKitExt) {
  testing::internal::CaptureStderr();
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(base::MakeRefCounted<base::TestSimpleTaskRunner>());
  InitializeWebKitExt(task_runner);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(
      log_output.find("compositor task runner is nullptr"),
      std::string::npos);
}

TEST_F(RenderThreadImplExtUnittest, InitializeWebKitExt_WithNullParam) {
  testing::internal::CaptureStderr();
  InitializeWebKitExt(nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find("compositor task runner is nullptr"),
      std::string::npos);
}

TEST_F(RenderThreadImplExtUnittest, ReportRendererLogOnMainThread_InvalidPolicy) {
  int policy = 0;
  CallReportRendererLogOnMainThread(policy, "test message");
  policy = -1;
  CallReportRendererLogOnMainThread(policy, "test message");
  policy = logging::LOGGING_MAX;
  CallReportRendererLogOnMainThread(policy, "test message");
  EXPECT_FALSE(GetHasRendererLogDropped(policy));
}

TEST_F(RenderThreadImplExtUnittest, ReportRendererLogOnMainThread_FirstMessage) {
  const int policy = logging::LOGGING_FEEDBACK;
  const std::string message = "test message";
  
  CallReportRendererLogOnMainThread(policy, message);
  
  EXPECT_FALSE(GetHasRendererLogDropped(policy));
  EXPECT_NE(GetLastRendererLogSentTime(policy), base::TimeTicks());
}

TEST_F(RenderThreadImplExtUnittest, ReportRendererLogOnMainThread_TooFrequent) {
  const int policy = logging::LOGGING_URL;
  const std::string message = "test message";
  EXPECT_FALSE(GetHasRendererLogDropped(policy));
  CallReportRendererLogOnMainThread(policy, message);
  CallReportRendererLogOnMainThread(policy, message);
  EXPECT_TRUE(GetHasRendererLogDropped(policy));
}

TEST_F(RenderThreadImplExtUnittest, ReportRendererLogOnMainThread_AfterInterval) {
  const int policy = logging::LOGGING_FEEDBACK;
  const std::string message = "test message";
  CallReportRendererLogOnMainThread(policy, message);
  SetLastRendererLogSentTime(policy, base::TimeTicks::Now() - GetMaxIPCLoginInterval() - base::Milliseconds(1));
  CallReportRendererLogOnMainThread(policy, message);
  EXPECT_FALSE(GetHasRendererLogDropped(policy));
}

TEST_F(RenderThreadImplExtUnittest, ReportRendererLogOnMainThread_Interval) {
  const int policy = logging::LOGGING_FEEDBACK;
  const std::string message = "test message";
  CallReportRendererLogOnMainThread(policy, message);
  SetLastRendererLogSentTime(policy, base::TimeTicks::Now() - GetMaxIPCLoginInterval() + base::Milliseconds(1));
  auto has_renderer_log_dropped = GetHasRendererLogDropped(policy);
  SetHasRendererLogDropped(policy, true);
  CallReportRendererLogOnMainThread(policy, message);
  EXPECT_TRUE(GetHasRendererLogDropped(policy));
  SetHasRendererLogDropped(policy, has_renderer_log_dropped);
}

TEST_F(RenderThreadImplExtUnittest, HandleFatalMessageForCrashpad_NonFatal) {
  bool result = CallHandleFatalMessageForCrashpad(
      logging::LOGGING_ERROR, "file.cpp", 123, 0, "error message");
  EXPECT_FALSE(result);
}

TEST_F(RenderThreadImplExtUnittest, HandleFatalMessageForCrashpad_Fatal) {
  bool result = CallHandleFatalMessageForCrashpad(
      logging::LOGGING_FATAL, "path/to/file.cpp", 123, 0, "fatal message");
  EXPECT_FALSE(result);
}

TEST_F(RenderThreadImplExtUnittest, HandleFatalMessageForCrashpad_Fatal_FileName) {
  bool result = CallHandleFatalMessageForCrashpad(
      logging::LOGGING_FATAL, "file.cpp", 123, 0, "fatal message");
  EXPECT_FALSE(result);
}

TEST_F(RenderThreadImplExtUnittest, HandleFatalMessageForCrashpad_Fatal_EmptyFileName) {
  bool result = CallHandleFatalMessageForCrashpad(
      logging::LOGGING_FATAL, nullptr, 123, 0, "fatal message");
  EXPECT_FALSE(result);
}

TEST_F(RenderThreadImplExtUnittest, RenderProcessLogMessageHandler_UnsupportedSeverity) {
  bool result = CallRenderProcessLogMessageHandler(
      logging::LOGGING_INFO, "file.cpp", 123, 0, "info message");
  EXPECT_FALSE(result);
}

TEST_F(RenderThreadImplExtUnittest, RenderProcessLogMessageHandler_FeedbackSeverity) {
  bool result = CallRenderProcessLogMessageHandler(
      logging::LOGGING_FEEDBACK, "file.cpp", 123, 0, "feedback message");
  EXPECT_TRUE(result);
}

TEST_F(RenderThreadImplExtUnittest, RenderProcessLogMessageHandler_UrlSeverity) {
  bool result = CallRenderProcessLogMessageHandler(
      logging::LOGGING_URL, "file.cpp", 123, 0, "url message");
  EXPECT_TRUE(result);
}

TEST_F(RenderThreadImplExtUnittest, RenderProcessLogMessageHandler_WarningSeverity) { // 失败
  bool result = CallRenderProcessLogMessageHandler(
      logging::LOGGING_WARNING, "file.cpp", 123, 0, "warning message");
  EXPECT_FALSE(result);
}

TEST_F(RenderThreadImplExtUnittest, RenderProcessLogMessageHandler_FatalSeverity) {
  bool result = CallRenderProcessLogMessageHandler(
      logging::LOGGING_FATAL, "file.cpp", 123, 0, "fatal message");  
  EXPECT_FALSE(result);
}

TEST_F(RenderThreadImplExtUnittest, NotifyLocaleChanged_NoSharedInstance) {
  ui::ResourceBundle* orig_instance =
      ui::ResourceBundle::SwapSharedInstanceForTesting(nullptr);
  std::string locale = "en-US";
  testing::internal::CaptureStderr();
  TestNotifyLocaleChanged(locale);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("render thread update locale failed"), std::string::npos);
  ui::ResourceBundle::SwapSharedInstanceForTesting(orig_instance);
}

TEST_F(RenderThreadImplExtUnittest, NotifyLocaleChanged_HasSharedInstance_NoPakExist) {
  ui::MockResourceBundleDelegate delegate;
  ui::ResourceBundle resource_bundle(&delegate);
  ui::ResourceBundle* orig_instance =
    ui::ResourceBundle::SwapSharedInstanceForTesting(&resource_bundle);
  
  std::string locale = "not_a_real_locale";
  EXPECT_FALSE(ui::ResourceBundle::LocaleDataPakExists(locale));

  testing::internal::CaptureStderr();
  TestNotifyLocaleChanged(locale);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("render thread update locale failed"), std::string::npos);

  ui::ResourceBundle::SwapSharedInstanceForTesting(orig_instance);
}

TEST_F(RenderThreadImplExtUnittest, NotifyLocaleChanged_LocaleEqual) {
  ui::MockResourceBundleDelegate delegate;
  ui::ResourceBundle resource_bundle(&delegate);
  ui::ResourceBundle* orig_instance =
    ui::ResourceBundle::SwapSharedInstanceForTesting(&resource_bundle);

  std::string locale = "en-US";
  EXPECT_TRUE(ui::ResourceBundle::LocaleDataPakExists(locale));

  ui::ResourceBundle::GetSharedInstance().SetLoadedLocaleForTesting("en-US");

  testing::internal::CaptureStderr();
  TestNotifyLocaleChanged(locale);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("render thread no need to update locale"), std::string::npos);

  ui::ResourceBundle::SwapSharedInstanceForTesting(orig_instance);
}

TEST_F(RenderThreadImplExtUnittest, NotifyLocaleChanged_LocaleNotEqual) {
  ui::MockResourceBundleDelegate delegate;
  ui::ResourceBundle resource_bundle(&delegate);
  ui::ResourceBundle* orig_instance =
    ui::ResourceBundle::SwapSharedInstanceForTesting(&resource_bundle);
  std::string locale = "en-US";
  EXPECT_TRUE(ui::ResourceBundle::LocaleDataPakExists(locale));
  ui::ResourceBundle::GetSharedInstance().SetLoadedLocaleForTesting("zh-CN");

  testing::internal::CaptureStderr();
  TestNotifyLocaleChanged(locale);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("CefFrameImpl update locale failed"), std::string::npos);

  ui::ResourceBundle::SwapSharedInstanceForTesting(orig_instance);
}

TEST_F(RenderThreadImplExtUnittest, NotifyLocaleChanged_LocaleNotEqual_NoResult) {
  ui::MockResourceBundleDelegate delegate;
  ui::ResourceBundle resource_bundle(&delegate);
  ui::ResourceBundle* orig_instance =
    ui::ResourceBundle::SwapSharedInstanceForTesting(&resource_bundle);

  std::string locale = "en-US";
  EXPECT_TRUE(ui::ResourceBundle::LocaleDataPakExists(locale));

  EXPECT_CALL(delegate, GetPathForLocalePack(_, _))
      .WillRepeatedly(Return(base::FilePath()))
      .RetiresOnSaturation();

  EXPECT_EQ("", ui::ResourceBundle::GetSharedInstance().ReloadLocaleResources(locale));

  ui::ResourceBundle::GetSharedInstance().SetLoadedLocaleForTesting("zh-CN");

  testing::internal::CaptureStderr();
  TestNotifyLocaleChanged(locale);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("CefFrameImpl update locale failed"), std::string::npos);

  ui::ResourceBundle::SwapSharedInstanceForTesting(orig_instance);
}

TEST_F(RenderThreadImplExtUnittest, NotifyLocaleChanged_LocaleNotEqual_WithResult) {
  ui::MockResourceBundleDelegate delegate;
  ui::ResourceBundle resource_bundle(&delegate);
  ui::ResourceBundle* orig_instance =
    ui::ResourceBundle::SwapSharedInstanceForTesting(&resource_bundle);

  std::string locale = "en-US";
  EXPECT_TRUE(ui::ResourceBundle::LocaleDataPakExists(locale));

  EXPECT_CALL(delegate, GetPathForLocalePack(_, _))
      .WillRepeatedly(ReturnArg<0>());

  EXPECT_EQ(locale, ui::ResourceBundle::GetSharedInstance().ReloadLocaleResources(locale));
  ui::ResourceBundle::GetSharedInstance().SetLoadedLocaleForTesting("zh-CN");
  TestNotifyLocaleChanged(locale);

  ui::ResourceBundle::SwapSharedInstanceForTesting(orig_instance);
}

TEST_F(RenderThreadImplExtUnittest, SetBlanklessDumpInfo_Valid) {  
  uint32_t nweb_id = 1;
  uint64_t blankless_key = 2;
  uint64_t frame_sink_id = 3;
  int32_t lcp_time = 4;
  int64_t pref_hash = 5;
    
  thread_->SetBlanklessDumpInfo(nweb_id, blankless_key, frame_sink_id, lcp_time, pref_hash);
}

TEST_F(RenderThreadImplExtUnittest, OnChannelListenError_NoSwitch) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->RemoveSwitch(switches::kEnableLoggerReport);
  EXPECT_FALSE(command_line->HasSwitch(switches::kEnableLoggerReport));
  TestOnChannelListenError();
}

TEST_F(RenderThreadImplExtUnittest, OnChannelListenError_WithSwitch_NoLogHandler) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->AppendSwitch(switches::kEnableLoggerReport);
  EXPECT_TRUE(command_line->HasSwitch(switches::kEnableLoggerReport));
  logging::SetLogMessageHandler(nullptr);
  TestOnChannelListenError();
  EXPECT_FALSE(logging::GetLogMessageHandler());
}

TEST_F(RenderThreadImplExtUnittest, OnChannelListenError_WithSwitch_WithLogHandler) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->AppendSwitch(switches::kEnableLoggerReport);
  EXPECT_TRUE(command_line->HasSwitch(switches::kEnableLoggerReport));
  logging::SetLogMessageHandler(CallRenderProcessLogMessageHandler);
  testing::internal::CaptureStderr();
  TestOnChannelListenError();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("remove log message handler for "), std::string::npos);
  EXPECT_FALSE(logging::GetLogMessageHandler());
}

TEST_F(RenderThreadImplExtUnittest, OnChannelConnected_NoSwitch) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->RemoveSwitch(switches::kEnableLoggerReport);
  EXPECT_FALSE(command_line->HasSwitch(switches::kEnableLoggerReport));
  TestOnChannelConnected(123);
}

TEST_F(RenderThreadImplExtUnittest, OnChannelConnected_WithSwitch_NoLogHandler) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->AppendSwitch(switches::kEnableLoggerReport);
  EXPECT_TRUE(command_line->HasSwitch(switches::kEnableLoggerReport));

  logging::SetLogMessageHandler(nullptr);
  TestOnChannelConnected(123);
  EXPECT_TRUE(logging::GetLogMessageHandler());
}

TEST_F(RenderThreadImplExtUnittest, OnChannelConnected_WithSwitch_WithLogHandler) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->AppendSwitch(switches::kEnableLoggerReport);
  EXPECT_TRUE(command_line->HasSwitch(switches::kEnableLoggerReport));

  logging::SetLogMessageHandler(CallRenderProcessLogMessageHandler);
  EXPECT_TRUE(logging::GetLogMessageHandler());
  testing::internal::CaptureStderr();
  TestOnChannelConnected(123);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find(
            "maybe you runs in single process mode, log message handler had been setted by other"),
            std::string::npos);
}
 
#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
TEST_F(RenderThreadImplExtUnittest, VideoLoadOpt_UpdateOptimizationConfigTest) {
  bool enable_ = true;
  int preloadTime = 6;
  int maxCacheTime = 9;
  int minCacheTime = 3;
  int moovSize = 200;
  int bitRate = 800;
  std::vector<std::string> support_domains;
  std::string domainUrl = "https://www.baidu.com/";
  support_domains.emplace_back(domainUrl);

  thread_->UpdateVideoLoadOptimizationConfigData(enable_, preloadTime, minCacheTime, maxCacheTime,
                                                moovSize, bitRate, support_domains);
  EXPECT_EQ(thread_->GetVideoPreloadTimeDefault(), 6);
  EXPECT_EQ(thread_->GetVideoMinCacheTimeDefault(), 3);
  EXPECT_EQ(thread_->GetVideoMaxCacheTimeDefault(), 9);
  EXPECT_EQ(thread_->GetVideoMoovSizeDefault(), 200);
  EXPECT_EQ(thread_->GetVideoBitrateDefault(), 800);
}


TEST_F(RenderThreadImplExtUnittest, VideoLoadOpt_ConfigEnableTest) {
  bool enable_ = true;
  int preloadTime = 6;
  int maxCacheTime = 9;
  int minCacheTime = 3;
  int moovSize = 200;
  int bitRate = 800;
  std::vector<std::string> support_domains;
  std::string domainUrl = "https://www.baidu.com/";
  std::string domainOtherUrl = "https://www.mgtv.com:8554/";
  support_domains.emplace_back(domainUrl);
  support_domains.emplace_back(domainOtherUrl);

  thread_->UpdateVideoLoadOptimizationConfigData(enable_, preloadTime, minCacheTime, maxCacheTime,
                                                moovSize, bitRate, support_domains);

  std::string testBaiduUrl = "https://www.baidu.com/";
  EXPECT_TRUE(thread_->IsVideoLoadOptimizationEnabled(testBaiduUrl));

  std::string testMgtvUrl = "https://www.mgtv.com:8554/";
  EXPECT_TRUE(thread_->IsVideoLoadOptimizationEnabled(testMgtvUrl));

  std::string testMgtvUrl_Fir = "https://www.mgtv.com:8559/";
  EXPECT_FALSE(thread_->IsVideoLoadOptimizationEnabled(testMgtvUrl_Fir));

  std::string testMgtvUrl_Sec = "https://www.mgtv.com/";
  EXPECT_FALSE(thread_->IsVideoLoadOptimizationEnabled(testMgtvUrl_Sec));  
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

}  // namespace content