/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "nweb_highlight_specified_content.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arkweb/ohos_nweb/src/mock_nweb_delegate.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "ohos_resources/highlight_specified_content/grit/highlight_specified_content_resources.h"
#include "ui/base/resource/resource_bundle.h"

namespace OHOS::NWeb {
class NWebHighlightSpecifiedContentTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
  void CreateDelegate(const std::list<std::string>& args_list);
  void ReleaseArgs();

  std::shared_ptr<NWebDelegate> nweb_delegate_;
  std::shared_ptr<NWebHighlightSpecifiedContent>
      g_nweb_highlight_specified_content;

 private:
  int argc_ = 0;
  char** argv_ = nullptr;
  std::list<std::string> web_engine_args_;
};

void NWebHighlightSpecifiedContentTest::SetUpTestCase(void) {}

void NWebHighlightSpecifiedContentTest::TearDownTestCase(void) {}

void NWebHighlightSpecifiedContentTest::SetUp() {
  web_engine_args_.emplace_back("--no-sandbox");
  CreateDelegate(web_engine_args_);
  base::WeakPtrFactory<NWebDelegate> weak_factory{nweb_delegate_.get()};
  g_nweb_highlight_specified_content =
      std::make_shared<NWebHighlightSpecifiedContent>(
          weak_factory.GetWeakPtr());
  ASSERT_NE(g_nweb_highlight_specified_content, nullptr);
}

void NWebHighlightSpecifiedContentTest::TearDown(void) {
  g_nweb_highlight_specified_content = nullptr;
  ReleaseArgs();
}

void NWebHighlightSpecifiedContentTest::ReleaseArgs() {
  argc_ = 0;
  if (argv_) {
    delete[] argv_;
    argv_ = nullptr;
  }
}

void NWebHighlightSpecifiedContentTest::CreateDelegate(
    const std::list<std::string>& args_list) {
  ReleaseArgs();
  argc_ = args_list.size();
  if (argc_ > 0) {
    int i = 0;
    argv_ = new char* [argc_];
    for (auto it = args_list.begin(); i < argc_; ++i, ++it) {
      argv_[i] = (char*)it->c_str();
    }
  }
  nweb_delegate_ = std::make_shared<NWebDelegate>(argc_, (const char**)argv_);
}

#if BUILDFLAG(ARKWEB_AI)
TEST_F(NWebHighlightSpecifiedContentTest,
       SetHighlightSpecifiedContentEnable_NoDelegate) {
  g_nweb_highlight_specified_content->nweb_delegate_ = nullptr;
  g_nweb_highlight_specified_content->SetHighlightSpecifiedContentEnable(true);
}

TEST_F(NWebHighlightSpecifiedContentTest,
       SetHighlightSpecifiedContentEnable_001) {
  g_nweb_highlight_specified_content->SetHighlightSpecifiedContentEnable(true);
}

TEST_F(NWebHighlightSpecifiedContentTest,
       SetHighlightSpecifiedContentEnable_002) {
  g_nweb_highlight_specified_content->SetHighlightSpecifiedContentEnable(false);
}

TEST_F(NWebHighlightSpecifiedContentTest, RegisterHighlight_NoDelegate) {
  g_nweb_highlight_specified_content->nweb_delegate_ = nullptr;
  g_nweb_highlight_specified_content->enable_ = true;
  g_nweb_highlight_specified_content->RegisterHighlight();
  g_nweb_highlight_specified_content->enable_ = false;
  g_nweb_highlight_specified_content->RegisterHighlight();
}

TEST_F(NWebHighlightSpecifiedContentTest, RegisterHighlight_001) {
  g_nweb_highlight_specified_content->enable_ = false;
  g_nweb_highlight_specified_content->RegisterHighlight();
}

TEST_F(NWebHighlightSpecifiedContentTest, RegisterHighlight_002) {
  g_nweb_highlight_specified_content->enable_ = true;
  g_nweb_highlight_specified_content->RegisterHighlight();
}

TEST_F(NWebHighlightSpecifiedContentTest, RemoveHighlightFunctions_NoDelegate) {
  g_nweb_highlight_specified_content->nweb_delegate_ = nullptr;
  g_nweb_highlight_specified_content->enable_ = true;
  g_nweb_highlight_specified_content->RemoveHighlightFunctions();
  g_nweb_highlight_specified_content->enable_ = false;
  g_nweb_highlight_specified_content->RemoveHighlightFunctions();
}

TEST_F(NWebHighlightSpecifiedContentTest, RemoveHighlightFunctions_001) {
  g_nweb_highlight_specified_content->enable_ = false;
  g_nweb_highlight_specified_content->RemoveHighlightFunctions();
}

TEST_F(NWebHighlightSpecifiedContentTest, RemoveHighlightFunctions_002) {
  g_nweb_highlight_specified_content->enable_ = true;
  g_nweb_highlight_specified_content->RemoveHighlightFunctions();
}
#endif
}  // namespace OHOS::NWeb