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
#include "base/lazy_instance.h"
#include "content/public/browser/render_process_host.h"
#include "content/test/storage_partition_test_helpers.h"
#include "content/browser/site_info.h"
#include "content/browser/site_instance_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "base/logging.h"
#include "content/browser/media/session/media_session_impl.h"
#include "base/task/single_thread_task_runner.h"
#include "url/origin.h"
#include "content/browser/child_process_security_policy_impl.h"
#include "content/browser/renderer_host/spare_render_process_host_manager_impl.h"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_base_ohos_api.h"
#include "arkweb/ohos_nweb/src/nweb_resize_helper.h"
#include "content/public/common/content_switches.h"
#define private public
#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_render_process_host_impl_utils.h"
#undef private
#define private public
#include "content/browser/renderer_host/render_process_host_impl.h"
#undef private
 
namespace content {
class MockRepeatingTimer : public base::RepeatingTimer {
  public:
    MOCK_METHOD(void, Start, (const base::Location&, base::TimeDelta, base::RepeatingClosure), (override));
    MOCK_METHOD(void, Stop, (), (override));
};
 
#if BUILDFLAG(ARKWEB_THEME_FONT)
const base::FilePath::CharType kAppThemePathA[] =
    FILE_PATH_LITERAL("/data/themes/a/app");
const base::FilePath::CharType kAppThemePathB[] =
    FILE_PATH_LITERAL("/data/themes/b/app");
const base::FilePath::CharType kAppThemeFontsDirName[] =
    FILE_PATH_LITERAL("fonts");
const base::FilePath::CharType kAppThemeFlagFileName[] =
    FILE_PATH_LITERAL("flag");
const base::FilePath::CharType kAppThemeFontsManifest[] =
    FILE_PATH_LITERAL("manifest.json");
#endif
 
class RenderProcessHostImplUtilsTest : public RenderViewHostTestHarness {
  public:
  RenderProcessHostImplUtilsTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME) {
  }
  ~RenderProcessHostImplUtilsTest() override = default;
  scoped_refptr<SiteInstanceImpl> CreateForUrl(const GURL& url) {
    std::unique_ptr<content::WebContents> web_contents(CreateTestWebContents());
    web_content_impl = static_cast<WebContentsImpl*>(web_contents.get());
    return SiteInstanceImpl::CreateForTesting(web_content_impl->GetBrowserContext(), url);
  }
  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    GURL test_url("http://foo.com");
    site_instance = CreateForUrl(test_url);
    process_ = site_instance->GetProcess();
    EXPECT_NE(process_, nullptr);
    auto* killer = DelayedRenderKiller::GetInstance();
    killer->timer_ = std::make_unique<MockRepeatingTimer>();
  }
  void TearDown() override {
    site_instance = nullptr;
    DelayedRenderKiller::GetInstance()->timer_ = std::make_unique<base::RepeatingTimer>();
    RenderViewHostTestHarness::TearDown();
  }
  scoped_refptr<SiteInstanceImpl> site_instance;
  RenderProcessHost* process_;
  WebContentsImpl* web_content_impl;
  DelayedRenderKiller* original_instance_;
  std::unique_ptr<ThemeFont> original_theme_font_;
  base::ScopedTempDir temp_dir_;
};

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
TEST_F(RenderProcessHostImplUtilsTest, RegisteProcessForSharedToken) {
  const std::string token = "test_token_1";
  ArkwebRenderProcessHostImplUtils::RegisteProcessForSharedToken(token, process_);
  RenderProcessHost* result = ArkwebRenderProcessHostImplUtils::GetProcessForSharedToken(token);
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(result->GetID(), process_->GetID());
}
 
TEST_F(RenderProcessHostImplUtilsTest, RemoveFromSharedRenderProcessMap) {
  const std::string token = "test_token_1";
  ASSERT_NO_FATAL_FAILURE(ArkwebRenderProcessHostImplUtils::RemoveFromSharedRenderProcessMap(process_));
  ArkwebRenderProcessHostImplUtils::RegisteProcessForSharedToken(token, process_);
  RenderProcessHost* result = ArkwebRenderProcessHostImplUtils::GetProcessForSharedToken(token);
  EXPECT_EQ(result->GetID(), process_->GetID());
  ArkwebRenderProcessHostImplUtils::RemoveFromSharedRenderProcessMap(process_);
  RenderProcessHost* result2 = ArkwebRenderProcessHostImplUtils::GetProcessForSharedToken(token);
  EXPECT_EQ(result2, nullptr);
}
#endif
 
TEST_F(RenderProcessHostImplUtilsTest, GetMaxRendererProcessCountEx) {
  RenderProcessHost::SetRenderProcessMode(RenderProcessMode::SINGLE_MODE);
  size_t maxRendererProcessCount = ArkwebRenderProcessHostImplUtils::GetMaxRendererProcessCountEx();
  size_t singleRenderCount = 1;
  EXPECT_EQ(maxRendererProcessCount, singleRenderCount);
}
 
TEST_F(RenderProcessHostImplUtilsTest, GetMaxRendererProcessCountExMaxCount) {
  RenderProcessHost::SetRenderProcessMode(RenderProcessMode::MULTIPLE_MODE);
  g_max_renderer_count_override = 1;
  size_t maxRendererProcessCount = ArkwebRenderProcessHostImplUtils::GetMaxRendererProcessCountEx();
  size_t renderCount = 5;
  EXPECT_EQ(maxRendererProcessCount, renderCount);
  g_max_renderer_count_override = 0;
}
 
TEST_F(RenderProcessHostImplUtilsTest, GetMaxRendererProcessCountExAmount) {
  RenderProcessHost::SetRenderProcessMode(RenderProcessMode::MULTIPLE_MODE);
  g_max_renderer_count_override = 0;
  size_t maxRendererProcessCount = ArkwebRenderProcessHostImplUtils::GetMaxRendererProcessCountEx();
  size_t minRenderCount = 3;
  EXPECT_TRUE(maxRendererProcessCount > minRenderCount);
}
 
TEST_F(RenderProcessHostImplUtilsTest, IsSuitableHostForArkwebFalse) {
  bool result = ArkwebRenderProcessHostImplUtils::IsSuitableHostForArkweb(process_, site_instance->GetSiteInfo());
  EXPECT_FALSE(result);
}
 
TEST_F(RenderProcessHostImplUtilsTest, IsSuitableHostForArkwebTest) {
  GURL pdf_url("http://example.com/document.pdf");
  auto pdf_site_instance = CreateForUrl(pdf_url);
  auto pdf_process = pdf_site_instance->GetProcess();
  pdf_process->special_render_numbers_[RenderProcessHost::RenderType::kPdf] = 1;
  auto siteInfo = pdf_site_instance->GetSiteInfo();
  siteInfo.is_pdf_ = true;
  bool result = ArkwebRenderProcessHostImplUtils::IsSuitableHostForArkweb(
      pdf_process, siteInfo);
  EXPECT_TRUE(result);
}
 
TEST_F(RenderProcessHostImplUtilsTest, IsSuitableHostForArkwebTest2) {
  GURL extension_url("chrome-extension://abcdefghijklmnopqrstuvwxyz/index.html");
  auto extension_site_instance = CreateForUrl(extension_url);
  auto extension_process = extension_site_instance->GetProcess();
  extension_process->special_render_numbers_[RenderProcessHost::RenderType::kExtension] = 1;
  bool result = ArkwebRenderProcessHostImplUtils::IsSuitableHostForArkweb(extension_process,
                                                                          extension_site_instance->GetSiteInfo());
  EXPECT_TRUE(result);
}

TEST_F(RenderProcessHostImplUtilsTest, IsSuitableHostForArkwebTest3) {
  GURL extension_url("arkweb-extension://abcdefghijklmnopqrstuvwxyz/index.html");
  auto extension_site_instance = CreateForUrl(extension_url);
  auto extension_process = extension_site_instance->GetProcess();
  extension_process->special_render_numbers_[RenderProcessHost::RenderType::kExtension] = 1;
  
  bool result = ArkwebRenderProcessHostImplUtils::IsSuitableHostForArkweb(extension_process,
                                                                          extension_site_instance->GetSiteInfo());
  EXPECT_TRUE(result);
}
 
TEST_F(RenderProcessHostImplUtilsTest, IsSuitableHostForArkwebTest4) {
  GURL extension_url("chrome-extension://abcdefghijklmnopqrstuvwxyz/index.html");
  auto extension_site_instance = CreateForUrl(extension_url);
  auto extension_process = extension_site_instance->GetProcess();
  extension_process->special_render_numbers_[RenderProcessHost::RenderType::kExtension] = 0;
  bool result = ArkwebRenderProcessHostImplUtils::IsSuitableHostForArkweb(extension_process,
                                                                          extension_site_instance->GetSiteInfo());
  EXPECT_FALSE(result);
}
 
TEST_F(RenderProcessHostImplUtilsTest, IsSuitableHostForArkwebTest5) {
  GURL extension_url("arkweb-extension://abcdefghijklmnopqrstuvwxyz/index.html");
  auto extension_site_instance = CreateForUrl(extension_url);
  auto extension_process = extension_site_instance->GetProcess();
  extension_process->special_render_numbers_[RenderProcessHost::RenderType::kExtension] = 0;
  
  bool result = ArkwebRenderProcessHostImplUtils::IsSuitableHostForArkweb(extension_process,
                                                                          extension_site_instance->GetSiteInfo());
  EXPECT_FALSE(result);
}
 
TEST_F(RenderProcessHostImplUtilsTest, GetProcessCountForLimitArkweb) {
  size_t processCountToIgnore = 1;
  size_t processCount = RenderProcessHostImpl::GetProcessCount();
  size_t result = ArkwebRenderProcessHostImplUtils::GetProcessCountForLimitArkweb(processCountToIgnore);
  EXPECT_TRUE(processCount - processCountToIgnore >= result);
}
 
#if BUILDFLAG(ARKWEB_THEME_FONT)
 
TEST_F(RenderProcessHostImplUtilsTest, IsThemeFontValidTest) {
  ArkwebRenderProcessHostImplUtils::g_theme_font_ = nullptr;
  EXPECT_FALSE(ArkwebRenderProcessHostImplUtils::IsThemeFontValid());
}
 
TEST_F(RenderProcessHostImplUtilsTest, IsThemeFontValidTest2) {
  ArkwebRenderProcessHostImplUtils::g_theme_font_ = std::make_unique<ThemeFont>();
  EXPECT_FALSE(ArkwebRenderProcessHostImplUtils::IsThemeFontValid());
}
 
TEST_F(RenderProcessHostImplUtilsTest, IsThemeFontValidTest3) {
  const base::FilePath::CharType appPath[] =
      FILE_PATH_LITERAL("/data/themes/a/app");
  base::FilePath temp_dir(appPath);
  base::FilePath flag_path = temp_dir.Append(FILE_PATH_LITERAL("flag"));
  base::CreateDirectory(flag_path);
  ArkwebRenderProcessHostImplUtils::g_theme_font_ = std::make_unique<ThemeFont>();
  ArkwebRenderProcessHostImplUtils::g_theme_font_->flag_path = flag_path;
  EXPECT_FALSE(ArkwebRenderProcessHostImplUtils::IsThemeFontValid());
}
 
TEST_F(RenderProcessHostImplUtilsTest, IsThemeFontValidTest4) {
  const base::FilePath::CharType appPath[] =
      FILE_PATH_LITERAL("/data/themes/a/app");
  base::FilePath temp_dir(appPath);
  base::FilePath flag_path = temp_dir.Append(FILE_PATH_LITERAL("flag"));
  base::FilePath manifest_path = temp_dir.Append(FILE_PATH_LITERAL("manifest.json"));
  base::CreateDirectory(flag_path);
  base::WriteFile(manifest_path, "{}");
  ArkwebRenderProcessHostImplUtils::g_theme_font_ = std::make_unique<ThemeFont>();
  ArkwebRenderProcessHostImplUtils::g_theme_font_->flag_path = flag_path;
  ArkwebRenderProcessHostImplUtils::g_theme_font_->manifest_path = manifest_path;
  EXPECT_FALSE(ArkwebRenderProcessHostImplUtils::IsThemeFontValid());
}

TEST_F(RenderProcessHostImplUtilsTest, IsThemeFontValidTest5) {
  const base::FilePath::CharType appPath[] =
      FILE_PATH_LITERAL("/data/themes/a/app");
  
  base::FilePath temp_dir(appPath); 
  base::FilePath flag_path = temp_dir.Append(FILE_PATH_LITERAL("flag"));
  base::FilePath font_path = temp_dir.Append(FILE_PATH_LITERAL("font"));
  base::FilePath manifest_path = temp_dir.Append(FILE_PATH_LITERAL("manifest.json"));
  base::CreateDirectory(flag_path);
  base::CreateDirectory(font_path);
  base::WriteFile(manifest_path, "{}");
  
  ArkwebRenderProcessHostImplUtils::g_theme_font_ = std::make_unique<ThemeFont>();
  ArkwebRenderProcessHostImplUtils::g_theme_font_->flag_path = flag_path;
  ArkwebRenderProcessHostImplUtils::g_theme_font_->manifest_path = manifest_path;
  ArkwebRenderProcessHostImplUtils::g_theme_font_->font_path = font_path;
  
  EXPECT_FALSE(ArkwebRenderProcessHostImplUtils::IsThemeFontValid());
}
 
TEST_F(RenderProcessHostImplUtilsTest, IsThemeFontValidTest6) {
  const base::FilePath::CharType appPath[] =
      FILE_PATH_LITERAL("/data/themes/a/app");
  
  base::FilePath temp_dir(appPath); 
  base::FilePath flag_path = temp_dir.Append(FILE_PATH_LITERAL("flag"));
  base::FilePath font_path = temp_dir.Append(FILE_PATH_LITERAL("font"));
  base::FilePath manifest_path = temp_dir.Append(FILE_PATH_LITERAL("manifest.json"));
  base::CreateDirectory(flag_path);
  base::CreateDirectory(font_path);
  base::WriteFile(manifest_path, "{}");
  std::vector<base::File> font_files;
  font_files.emplace_back(base::File(manifest_path, base::File::FLAG_OPEN | base::File::FLAG_READ));
  
  ArkwebRenderProcessHostImplUtils::g_theme_font_ = std::make_unique<ThemeFont>();
  ArkwebRenderProcessHostImplUtils::g_theme_font_->flag_path = flag_path;
  ArkwebRenderProcessHostImplUtils::g_theme_font_->manifest_path = manifest_path;
  ArkwebRenderProcessHostImplUtils::g_theme_font_->font_path = font_path;
  ArkwebRenderProcessHostImplUtils::g_theme_font_->font_files = std::move(font_files);
  
  EXPECT_TRUE(ArkwebRenderProcessHostImplUtils::IsThemeFontValid());
}
 
TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFont_AlreadyValid) {
  base::FilePath temp_dir(kAppThemePathA); 
  base::FilePath flag_path = temp_dir.Append(FILE_PATH_LITERAL("flag"));
  base::FilePath font_path = temp_dir.Append(FILE_PATH_LITERAL("font"));
  base::FilePath manifest_path = temp_dir.Append(FILE_PATH_LITERAL("manifest.json"));
  base::CreateDirectory(flag_path);
  base::CreateDirectory(font_path);
  base::WriteFile(manifest_path, "{}");
  std::vector<base::File> font_files;
  font_files.emplace_back(base::File(manifest_path, base::File::FLAG_OPEN | base::File::FLAG_READ));
  
  ArkwebRenderProcessHostImplUtils::g_theme_font_ = std::make_unique<ThemeFont>();
  ArkwebRenderProcessHostImplUtils::g_theme_font_->flag_path = flag_path;
  ArkwebRenderProcessHostImplUtils::g_theme_font_->manifest_path = manifest_path;
  ArkwebRenderProcessHostImplUtils::g_theme_font_->font_path = font_path;
  ArkwebRenderProcessHostImplUtils::g_theme_font_->font_files = std::move(font_files);
  
  EXPECT_TRUE(ArkwebRenderProcessHostImplUtils::IsThemeFontValid());
 
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_EQ(result, ArkwebRenderProcessHostImplUtils::g_theme_font_.get());
  base::DeletePathRecursively(flag_path);
  base::DeletePathRecursively(font_path);
  base::DeletePathRecursively(manifest_path);
}
 
TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest2) {
  ArkwebRenderProcessHostImplUtils::g_theme_font_.reset();
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_EQ(result, nullptr);
}
 
TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest3) {
  ArkwebRenderProcessHostImplUtils::g_theme_font_.reset();
  base::FilePath theme_path(kAppThemePathB);
  base::FilePath theme_flag_path = theme_path.Append(kAppThemeFlagFileName);
  base::CreateDirectory(theme_flag_path);
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_EQ(result, nullptr);
  base::DeletePathRecursively(theme_flag_path);
}
 
TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest4) {
  ArkwebRenderProcessHostImplUtils::g_theme_font_.reset();
  base::FilePath theme_path(kAppThemePathA);
  base::FilePath theme_path_flag(kAppThemePathA);
  base::FilePath theme_flag_path = theme_path_flag.Append(kAppThemeFlagFileName);
  base::FilePath theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  base::CreateDirectory(theme_path);
  base::CreateDirectory(theme_flag_path);
  base::CreateDirectory(theme_font_path);
  base::FilePath manifest_path(theme_font_path.Append(kAppThemeFontsManifest));
  base::WriteFile(manifest_path, "");
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_EQ(result, nullptr);
  base::DeletePathRecursively(theme_flag_path);
  base::DeletePathRecursively(theme_font_path);
}
 
TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest5) {
  base::FilePath theme_path(kAppThemePathA);
  base::FilePath theme_path_flag(kAppThemePathA);
  base::FilePath theme_flag_path = theme_path_flag.Append(kAppThemeFlagFileName);
  base::FilePath theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  base::CreateDirectory(theme_path);
  base::CreateDirectory(theme_flag_path);
  base::CreateDirectory(theme_font_path);
  base::FilePath manifest_path(theme_font_path.Append(kAppThemeFontsManifest));
  std::string json_content = base::StringPrintf(
    "{\"id\":\"%s\",\"origin\":\"%s\",\"src\":\"%s",
    "1", "preset", "default.ttf");
  EXPECT_EQ(base::WriteFile(manifest_path, json_content), true);
  LOG(INFO) << "manifest_path:" << manifest_path.FinalExtension();
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_EQ(result, nullptr);
  base::DeletePathRecursively(theme_flag_path);
  base::DeletePathRecursively(theme_font_path);
}

TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest6) {
  base::FilePath theme_path(kAppThemePathA);
  base::FilePath theme_path_flag(kAppThemePathA);
  base::FilePath theme_flag_path = theme_path_flag.Append(kAppThemeFlagFileName);
  base::FilePath theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  base::CreateDirectory(theme_path);
  base::CreateDirectory(theme_flag_path);
  base::CreateDirectory(theme_font_path);
  base::FilePath manifest_path(theme_font_path.Append(kAppThemeFontsManifest));
  std::string json_content = base::StringPrintf(
    "[\"id\",\"origin\",\"src\",\"srcExt\"]");
  EXPECT_EQ(base::WriteFile(manifest_path, json_content), true);
  LOG(INFO) << "manifest_path:" << manifest_path.Extension();
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_EQ(result, nullptr);
  base::DeletePathRecursively(theme_flag_path);
  base::DeletePathRecursively(theme_font_path);
}
 
TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest7) {
  base::FilePath theme_path(kAppThemePathA);
  base::FilePath theme_path_flag(kAppThemePathA);
  base::FilePath theme_flag_path = theme_path_flag.Append(kAppThemeFlagFileName);
  base::FilePath theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  base::CreateDirectory(theme_path);
  base::CreateDirectory(theme_flag_path);
  base::CreateDirectory(theme_font_path);
  base::FilePath manifest_path(theme_font_path.Append(kAppThemeFontsManifest));
  std::string json_content = base::StringPrintf(
    "{\"id\":\"%s\",\"origin\":\"%s\"}","1", "preset");
  base::WriteFile(manifest_path, json_content);
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_EQ(result, nullptr);
  base::DeletePathRecursively(theme_flag_path);
  base::DeletePathRecursively(theme_font_path);
}
 
TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest8) {
  base::FilePath theme_path(kAppThemePathA);
  base::FilePath theme_path_flag(kAppThemePathA);
  base::FilePath theme_flag_path = theme_path_flag.Append(kAppThemeFlagFileName);
  base::FilePath theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  base::CreateDirectory(theme_path);
  base::CreateDirectory(theme_flag_path);
  base::CreateDirectory(theme_font_path);
  base::FilePath manifest_path(theme_font_path.Append(kAppThemeFontsManifest));
  std::string json_content = base::StringPrintf(
    "{\"id\":\"%s\",\"origin\":\"%s\",\"src\":\"%s\",\"srcExt\":[\"%s\"]}",
    "1", "preset", "", "");
  base::WriteFile(manifest_path, json_content);
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_EQ(result, nullptr);
  base::DeletePathRecursively(theme_flag_path);
  base::DeletePathRecursively(theme_font_path);
}
 
TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest9) {
  base::FilePath theme_path(kAppThemePathA);
  base::FilePath theme_path_flag(kAppThemePathA);
  base::FilePath theme_flag_path = theme_path_flag.Append(kAppThemeFlagFileName);
  base::FilePath theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  base::CreateDirectory(theme_path);
  base::CreateDirectory(theme_flag_path);
  base::CreateDirectory(theme_font_path);
  base::FilePath manifest_path(theme_font_path.Append(kAppThemeFontsManifest));
  std::string json_content = base::StringPrintf(
    "{\"id\":\"%s\",\"origin\":\"%s\",\"ttfFileSrc\":\"%s\",\"srcExt\":[\"%s\"]}",
    "1", "preset", "default.ttf", "default2.ttf");
  base::WriteFile(manifest_path, json_content);
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_EQ(result, nullptr);
  base::DeletePathRecursively(theme_flag_path);
  base::DeletePathRecursively(theme_font_path);
}
 
TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest10) {
  base::FilePath theme_path(kAppThemePathA);
  base::FilePath theme_path_flag(kAppThemePathA);
  base::FilePath theme_flag_path = theme_path_flag.Append(kAppThemeFlagFileName);
  base::FilePath theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  base::CreateDirectory(theme_path);
  base::CreateDirectory(theme_flag_path);
  base::CreateDirectory(theme_font_path);
  base::FilePath manifest_path(theme_font_path.Append(kAppThemeFontsManifest));
  base::FilePath font_file = theme_font_path.Append("valid_font.ttf");
  base::WriteFile(font_file, "");
  std::string json_content = base::StringPrintf(
    "{\"id\":\"%s\",\"origin\":\"%s\",\"src\":\"%s\"}",
    "1", "preset", "valid_font.ttf");
  base::WriteFile(manifest_path, json_content);
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_NE(result, nullptr);
  base::DeletePathRecursively(theme_flag_path);
  base::DeletePathRecursively(theme_font_path);
}

TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest11) {
  base::FilePath theme_path(kAppThemePathA);
  base::FilePath theme_path_flag(kAppThemePathA);
  base::FilePath theme_flag_path = theme_path_flag.Append(kAppThemeFlagFileName);
  base::FilePath theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  base::CreateDirectory(theme_path);
  base::CreateDirectory(theme_flag_path);
  base::CreateDirectory(theme_font_path);
  base::FilePath manifest_path(theme_font_path.Append(kAppThemeFontsManifest));
  base::FilePath font_file = theme_font_path.Append("valid_font.ttf");
  base::WriteFile(font_file, "");
  std::string json_content = base::StringPrintf(
    "{\"id\":\"%s\",\"origin\":\"%s\",\"src\":\"%s\",\"srcExt\":[\"%s\"]}",
    "1", "preset", "valid_font.ttf", "");
  base::WriteFile(manifest_path, json_content);
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_NE(result, nullptr);
  base::DeletePathRecursively(theme_flag_path);
  base::DeletePathRecursively(theme_font_path);
}

TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest12) {
  base::FilePath theme_path(kAppThemePathA);
  base::FilePath theme_path_flag(kAppThemePathA);
  base::FilePath theme_flag_path = theme_path_flag.Append(kAppThemeFlagFileName);
  base::FilePath theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  base::CreateDirectory(theme_path);
  base::CreateDirectory(theme_flag_path);
  base::CreateDirectory(theme_font_path);
  base::FilePath manifest_path(theme_font_path.Append(kAppThemeFontsManifest));
  base::FilePath font_file = theme_font_path.Append("valid_font.ttf");
  base::WriteFile(font_file, "");
  std::string json_content = base::StringPrintf(
    "{\"id\":\"%s\",\"origin\":\"%s\",\"src\":\"%s\",\"srcExt\":[\"%s\"]}",
    "1", "preset", "valid_font.ttf", "default2.ttf");
  base::WriteFile(manifest_path, json_content);
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_NE(result, nullptr);
  base::DeletePathRecursively(theme_flag_path);
  base::DeletePathRecursively(theme_font_path);
}

TEST_F(RenderProcessHostImplUtilsTest, EnsureThemeFontTest13) {
  base::FilePath theme_path(kAppThemePathA);
  base::FilePath theme_path_flag(kAppThemePathA);
  base::FilePath theme_flag_path = theme_path_flag.Append(kAppThemeFlagFileName);
  base::FilePath theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  base::CreateDirectory(theme_path);
  base::CreateDirectory(theme_flag_path);
  base::CreateDirectory(theme_font_path);
  base::FilePath manifest_path(theme_font_path.Append(kAppThemeFontsManifest));
  base::FilePath font_file = theme_font_path.Append("valid_font.ttf");
  base::WriteFile(font_file, "");
  base::FilePath font_file_ext = theme_font_path.Append("valid_font1.ttf");
  base::WriteFile(font_file_ext, "");
  std::string json_content = base::StringPrintf(
    "{\"id\":\"%s\",\"origin\":\"%s\",\"src\":\"%s\",\"srcExt\":[\"%s\"]}",
    "1", "preset", "valid_font.ttf", "valid_font1.ttf");
  base::WriteFile(manifest_path, json_content);
  ThemeFont* result = ArkwebRenderProcessHostImplUtils::EnsureThemeFont();
  EXPECT_NE(result, nullptr);
  base::DeletePathRecursively(theme_flag_path);
  base::DeletePathRecursively(theme_font_path);
}

#endif
 
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
 
TEST_F(RenderProcessHostImplUtilsTest, StartTimerTest) {
  // Arrange
  auto* killer = DelayedRenderKiller::GetInstance();
  killer->rep_ = 5;
  EXPECT_CALL(*static_cast<MockRepeatingTimer*>(killer->timer_.get()), Start)
    .Times(1);
  killer->StartTimer();
  EXPECT_EQ(killer->rep_, 0); // 验证未重置
}
 
TEST_F(RenderProcessHostImplUtilsTest, ReportHisyeventTest) {
  ASSERT_NO_FATAL_FAILURE(ArkwebRenderProcessHostImplUtils::ReportHisyevent(200, "ReportRenderInitBlock"));
}
 
TEST_F(RenderProcessHostImplUtilsTest, ReportHisyeventTest2) {
  OHOS::NWeb::NWebResizeHelper::GetInstance().SetDragResizeStart(false);
  ArkwebRenderProcessHostImplUtils::ReportHisyevent(200, "ReportDragBlank");
  ASSERT_NO_FATAL_FAILURE(ArkwebRenderProcessHostImplUtils::ReportHisyevent(200, "ReportRenderInitBlock"));
}
 
TEST_F(RenderProcessHostImplUtilsTest, ReportHisyeventTest3) {
  OHOS::NWeb::NWebResizeHelper::GetInstance().SetDragResizeStart(true);
  ArkwebRenderProcessHostImplUtils::ReportHisyevent(200, "ReportDragBlank");
  ASSERT_NO_FATAL_FAILURE(ArkwebRenderProcessHostImplUtils::ReportHisyevent(200, "ReportRenderInitBlock"));
}
 
TEST_F(RenderProcessHostImplUtilsTest, RenderProcessModeTest) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  g_render_process_mode = RenderProcessMode::DEFAULT_MODE;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillRepeatedly(testing::Return(false));
  auto result = RenderProcessHost::render_process_mode();
  EXPECT_EQ(result, RenderProcessMode::SINGLE_MODE);
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(RenderProcessHostImplUtilsTest, RenderProcessModeTest2) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  g_render_process_mode = RenderProcessMode::DEFAULT_MODE;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillRepeatedly(testing::Return(false));
  auto result = RenderProcessHost::render_process_mode();
  EXPECT_EQ(result, RenderProcessMode::MULTIPLE_MODE);
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}
 
TEST_F(RenderProcessHostImplUtilsTest, RenderProcessModeTest3) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  g_render_process_mode = RenderProcessMode::DEFAULT_MODE;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillRepeatedly(testing::Return(true));
  auto result = RenderProcessHost::render_process_mode();
  EXPECT_EQ(result, RenderProcessMode::MULTIPLE_MODE);
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}
 
TEST_F(RenderProcessHostImplUtilsTest, RenderProcessModeTest4) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  g_render_process_mode = RenderProcessMode::DEFAULT_MODE;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillRepeatedly(testing::Return(true));
  auto result = RenderProcessHost::render_process_mode();
  EXPECT_EQ(result, RenderProcessMode::MULTIPLE_MODE);
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}
 
TEST_F(RenderProcessHostImplUtilsTest, GetProcessHostForSiteInstanceArkwebTest) {
  GURL extension_url("chrome-extension://test/index.html");
  auto extension_site_instance = CreateForUrl(extension_url);
  auto extension_process = extension_site_instance->GetProcess();
  auto site_info = extension_site_instance->GetSiteInfo();
  extension_process->special_render_numbers_[RenderProcessHost::RenderType::kExtension] = 0;
  ArkwebRenderProcessHostImplUtils::GetProcessHostForSiteInstanceArkweb(
      extension_process, site_info, web_content_impl->GetBrowserContext(), 1, extension_site_instance.get());
  EXPECT_EQ(extension_process->special_render_numbers_[RenderProcessHost::RenderType::kExtension], 1);
}
 
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
TEST_F(RenderProcessHostImplUtilsTest, GetProcessHostForSiteInstanceArkwebTest2) {
  GURL extension_url("arkweb-extension://test/index.html");
  auto extension_site_instance = CreateForUrl(extension_url);
  auto extension_process = extension_site_instance->GetProcess();
  auto site_info = extension_site_instance->GetSiteInfo();
  extension_process->special_render_numbers_[RenderProcessHost::RenderType::kExtension] = 0;
  ArkwebRenderProcessHostImplUtils::GetProcessHostForSiteInstanceArkweb(
      extension_process, site_info, web_content_impl->GetBrowserContext(), 1, extension_site_instance.get());
  EXPECT_EQ(extension_process->special_render_numbers_[RenderProcessHost::RenderType::kExtension], 1);
}
#endif
 
TEST_F(RenderProcessHostImplUtilsTest, GetProcessHostForSiteInstanceArkwebTest3) {
  GURL pdf_url("http://example.com/document.pdf");
  auto pdf_site_instance = CreateForUrl(pdf_url);
  auto pdf_process = pdf_site_instance->GetProcess();
  auto site_info = pdf_site_instance->GetSiteInfo();
  site_info.is_pdf_ = true;
  pdf_process->special_render_numbers_[RenderProcessHost::RenderType::kPdf] = 0;
  ArkwebRenderProcessHostImplUtils::GetProcessHostForSiteInstanceArkweb(
      pdf_process, site_info, web_content_impl->GetBrowserContext(), 1, pdf_site_instance.get());
  EXPECT_EQ(pdf_process->special_render_numbers_[RenderProcessHost::RenderType::kPdf], 1);
}
 
TEST_F(RenderProcessHostImplUtilsTest, GetProcessHostForSiteInstanceArkwebTest4) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  GURL test_url("https://www.example.com");
  g_render_process_mode = RenderProcessMode::DEFAULT_MODE;
  size_t count = 0;
 
  auto test_site_instance = CreateForUrl(test_url);
  auto test_process = test_site_instance->GetProcess();
  auto site_info = test_site_instance->GetSiteInfo();
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillRepeatedly(testing::Return(false));
  auto result = RenderProcessHost::render_process_mode();
  EXPECT_EQ(result, RenderProcessMode::MULTIPLE_MODE);
 
  auto maxProcessCount = RenderProcessHostImpl::GetMaxRendererProcessCount();
  count = maxProcessCount >=1 ? maxProcessCount - 1 : 0;
 
  ASSERT_NO_FATAL_FAILURE(ArkwebRenderProcessHostImplUtils::GetProcessHostForSiteInstanceArkweb(
      test_process, site_info, web_content_impl->GetBrowserContext(), count, test_site_instance.get()));
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}
 
TEST_F(RenderProcessHostImplUtilsTest, GetProcessHostForSiteInstanceArkwebTest5) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  GURL test_url("https://www.example.com");
  g_render_process_mode = RenderProcessMode::DEFAULT_MODE;
  size_t count = 0;
 
  auto test_site_instance = CreateForUrl(test_url);
  auto test_process = test_site_instance->GetProcess();
  auto site_info = test_site_instance->GetSiteInfo();
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillRepeatedly(testing::Return(false));
  auto result = RenderProcessHost::render_process_mode();
  EXPECT_EQ(result, RenderProcessMode::SINGLE_MODE);
 
  auto maxProcessCount = RenderProcessHostImpl::GetMaxRendererProcessCount();
  count = maxProcessCount >=1 ? maxProcessCount - 1 : 0;
 
  ASSERT_NO_FATAL_FAILURE(ArkwebRenderProcessHostImplUtils::GetProcessHostForSiteInstanceArkweb(
      test_process, site_info, web_content_impl->GetBrowserContext(), count, test_site_instance.get()));
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(RenderProcessHostImplUtilsTest, GetProcessHostForSiteInstanceArkwebTest6) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  GURL test_url("https://www.example.com");
  g_render_process_mode = RenderProcessMode::DEFAULT_MODE;
  size_t count = 0;
 
  auto test_site_instance = CreateForUrl(test_url);
  auto test_process = test_site_instance->GetProcess();
  auto site_info = test_site_instance->GetSiteInfo();
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillRepeatedly(testing::Return(false));
  auto result = RenderProcessHost::render_process_mode();
  EXPECT_EQ(result, RenderProcessMode::SINGLE_MODE);
 
  auto maxProcessCount = RenderProcessHostImpl::GetMaxRendererProcessCount();
 
  ASSERT_NO_FATAL_FAILURE(ArkwebRenderProcessHostImplUtils::GetProcessHostForSiteInstanceArkweb(
      test_process, site_info, web_content_impl->GetBrowserContext(), maxProcessCount, test_site_instance.get()));
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}
 
TEST_F(RenderProcessHostImplUtilsTest, GetProcessHostForSiteInstanceArkwebTest7) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  GURL test_url("https://www.example.com");
  g_render_process_mode = RenderProcessMode::DEFAULT_MODE;
  size_t count = 0;
 
  auto test_site_instance = CreateForUrl(test_url);
  auto test_process = test_site_instance->GetProcess();
  auto site_info = test_site_instance->GetSiteInfo();
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillRepeatedly(testing::Return(false));
  auto result = RenderProcessHost::render_process_mode();
  EXPECT_EQ(result, RenderProcessMode::MULTIPLE_MODE);
 
  auto maxProcessCount = RenderProcessHostImpl::GetMaxRendererProcessCount();
 
  ASSERT_NO_FATAL_FAILURE(ArkwebRenderProcessHostImplUtils::GetProcessHostForSiteInstanceArkweb(
      test_process, site_info, web_content_impl->GetBrowserContext(), maxProcessCount, test_site_instance.get()));
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}
 
TEST_F(RenderProcessHostImplUtilsTest, TryKillRenderTest) {
  auto* killer = DelayedRenderKiller::GetInstance();
  killer->rep_ = 15;
  auto* mock_timer = static_cast<MockRepeatingTimer*>(killer->timer_.get());
  EXPECT_CALL(*mock_timer, Stop())
    .Times(1);
  ASSERT_NO_FATAL_FAILURE(killer->TryKillRender());
}
 
TEST_F(RenderProcessHostImplUtilsTest, TryKillRenderTest2) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  g_max_renderer_count_override = 4;
  auto* killer = DelayedRenderKiller::GetInstance();
  killer->rep_ = 0;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillRepeatedly(testing::Return(false));
  auto result = RenderProcessHost::render_process_mode();
  EXPECT_EQ(result, RenderProcessMode::MULTIPLE_MODE);
  auto* mock_timer = static_cast<MockRepeatingTimer*>(killer->timer_.get());
  EXPECT_CALL(*mock_timer, Stop())
    .Times(1);
  ASSERT_NO_FATAL_FAILURE(killer->TryKillRender());
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}   
 
TEST_F(RenderProcessHostImplUtilsTest, TryKillRenderTest3) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  g_max_renderer_count_override = 4;
  auto* killer = DelayedRenderKiller::GetInstance();
  killer->rep_ = 0;
  auto& sys_info_utils_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(sys_info_utils_mock, IsPcDevice()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(sys_info_utils_mock, IsTabletDevice()).WillRepeatedly(testing::Return(false));
  auto result = RenderProcessHost::render_process_mode();
  EXPECT_EQ(result, RenderProcessMode::SINGLE_MODE);
  auto* mock_timer = static_cast<MockRepeatingTimer*>(killer->timer_.get());
  EXPECT_CALL(*mock_timer, Stop())
    .Times(1);
  ASSERT_NO_FATAL_FAILURE(killer->TryKillRender());
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}
 
#endif
}  // namespace content