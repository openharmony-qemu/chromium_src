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

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/platform_test.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wheader-hygiene"

#include "base/files/file_path.h"

namespace base {

class FilePathStaTest : public PlatformTest {
protected:
  void SetUp() override { PlatformTest::SetUp(); }

  void TearDown() override { PlatformTest::TearDown(); }
};

TEST_F(FilePathStaTest, IsDataShareUrl) {
  const std::string& url1 = "datashare://";
  FilePath input_datashare("datashare://abc");
  EXPECT_TRUE(input_datashare.IsDataShareUrl(url1));

  const std::string& url2 = "dataability://";
  FilePath input_dataability("dataability://abc");
  EXPECT_TRUE(input_dataability.IsDataShareUrl(url2));

  const std::string& url3 = "file://media/";
  FilePath input_media("file://media/abc");
  EXPECT_TRUE(input_media.IsDataShareUrl(url3));

  const std::string& url4 = "file://media/";
  FilePath input_docs("file://media/abc");
  EXPECT_TRUE(input_docs.IsDataShareUrl(url4));

  const std::string& url5 = "abc::/";
  FilePath input_false("abc::/def");
  EXPECT_FALSE(input_false.IsDataShareUrl(url5));
}

}  // namespace base