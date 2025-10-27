/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "components/viz/service/display/frame_dump_copy_output_request.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace viz {
namespace {
const std::string DUMP_FILE_PATH = "/data/storage/el2/base/haps/entry/files/";
const std::string DUMP_FILE_PRE = "web_frame_";
const std::string DUMP_FILE_TYPE = ".png";
const int MAX_DUMP_FRAME_SIZE = 1000;
}

class FrameDumpCopyOutputRequestTest : public testing::Test {
 public:
   FrameDumpCopyOutputRequestTest() = default;
   ~FrameDumpCopyOutputRequestTest() = default;

   void TestConstructor() {
     uint64_t test_id = 12345;
     std::string test_path = "/test/dump/path/";
     FrameDumpCopyOutputRequest request(test_id, test_path);
     EXPECT_EQ(request.result_format(), CopyOutputRequest::ResultFormat::RGBA);
     EXPECT_EQ(request.result_destination(), CopyOutputRequest::ResultDestination::kSystemMemory);
     EXPECT_TRUE(request.has_result_task_runner() == false);
     EXPECT_FALSE(request.is_scaled());
     EXPECT_FALSE(request.has_area());
     EXPECT_FALSE(request.has_result_selection());
   }

   void TestConstructorWithZeroId() {
     uint64_t test_id = 0;
     std::string test_path = "";
     FrameDumpCopyOutputRequest request(test_id, test_path);
     EXPECT_EQ(request.result_format(), CopyOutputRequest::ResultFormat::RGBA);
     EXPECT_EQ(request.result_destination(), CopyOutputRequest::ResultDestination::kSystemMemory);
   }
};

TEST_F(FrameDumpCopyOutputRequestTest, Constructor) {
  TestConstructor();
}

TEST_F(FrameDumpCopyOutputRequestTest, ConstructorWithZeroId) {
  TestConstructorWithZeroId();
}
}  // namespace viz