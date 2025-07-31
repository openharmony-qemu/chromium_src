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

#include "gtest/gtest.h"
#include "SkHeifDecoderAPIOhos.h"
#include "include/core/SkData.h"
#include "include/core/SkStream.h"
#include <fstream>
#include <vector>

namespace skia {

class HeifDecoderTest : public testing::Test {
protected:
    void SetUp() {
        // Create a simple test HEIF file (in practice, you'd use real test files)
        CreateTestHeifFile();
    }

    void TearDown() {
        std::remove("red.heic");
    }

    void CreateTestHeifFile() {
        // In a real scenario, you would have actual small HEIF test files
        // This is just a placeholder - real tests would use real HEIF files
        std::ofstream testFile("red.heic", std::ios::binary);
        testFile.close();
    }

    std::unique_ptr<SkStream> CreateTestStream(const std::string& path) {
        auto stream = SkFILEStream::Make(path.c_str());
        return stream ? std::move(stream) : nullptr;
    }
};

TEST_F(HeifDecoderTest, test001) {
    HeifDecoder decoder;
    HeifFrameInfo info;
    
    auto emptyStream = SkMemoryStream::Make(nullptr);
    ASSERT_FALSE(decoder.init(std::move(emptyStream), &info));
}

TEST_F(HeifDecoderTest, test002) {
    HeifDecoder decoder;
    HeifFrameInfo info;
    auto stream = CreateTestStream("red.heic");
    
    ASSERT_FALSE(decoder.init(std::move(stream), &info));
}

TEST_F(HeifDecoderTest, test003) {
    HeifDecoder decoder;
    
    ASSERT_TRUE(decoder.setOutputColor(kHeifColorFormat_RGBA_8888));
    ASSERT_TRUE(decoder.setOutputColor(kHeifColorFormat_RGB565));
    ASSERT_TRUE(decoder.setOutputColor(kHeifColorFormat_BGRA_8888));
    ASSERT_TRUE(decoder.setOutputColor(kHeifColorFormat_RGBA_1010102));
}

TEST_F(HeifDecoderTest, test004) {
    HeifDecoder decoder;
    
    ASSERT_FALSE(decoder.decode(nullptr));
}

TEST_F(HeifDecoderTest, test005) {
    HeifDecoder decoder;
    HeifFrameInfo info;
    auto stream = CreateTestStream("real_test.heic");  // Use real HEIF file
    
    if (stream && decoder.init(std::move(stream), &info) && decoder.decode(&info)) {
        uint64_t size = 0;
        void* data = decoder.getDecodeData(size);
        
        if (data) {
            decoder.closeDecodeData(data, size);
        }
    }
}

TEST_F(HeifDecoderTest, test006) {
    HeifDecoder decoder;
    HeifFrameInfo info;
    auto stream = CreateTestStream("real_test.heic");  // Use real HEIF file
    
    if (stream && decoder.init(std::move(stream), &info) && decoder.decode(&info)) {
        int32_t stride = decoder.getStride();
    }
}

} // namespace skia