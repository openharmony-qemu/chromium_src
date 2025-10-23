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
#define private public
#include "skia_memory_tracer.h"
#undef private

namespace skia {
constexpr uint32_t MEMUNIT_RATE = 1024;

class SkiaMemoryTracerTest : public testing::Test {
protected:
    void SetUp() override {
        resourceMap_ = {
            {"texture", "gpu/texture"},
            {"buffer", "gpu/buffer"}
        };
    }

    std::vector<ResourcePair> resourceMap_;
};

TEST_F(SkiaMemoryTracerTest, test001) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    EXPECT_EQ(0.0f, tracer.GetGpuMemorySizeInMB());
}

TEST_F(SkiaMemoryTracerTest, test002) {
    SkiaMemoryTracer tracer("category", true);
    EXPECT_EQ(0.0f, tracer.GetGpuMemorySizeInMB());
}

TEST_F(SkiaMemoryTracerTest, test003) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.dumpNumericValue("element1", "size", "bytes", 1024);
    EXPECT_NE(0.0f, tracer.GetGpuMemorySizeInMB()); // Not processed yet
    
    tracer.dumpNumericValue("element2", "size", "bytes", 2048);
}

TEST_F(SkiaMemoryTracerTest, test004) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.dumpNumericValue("element", "size", "bytes", 1048576);
}

TEST_F(SkiaMemoryTracerTest, test005) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.dumpNumericValue("element", "other", "bytes", 1024);
    EXPECT_EQ(0.0f, tracer.GetGpuMemorySizeInMB());
}

TEST_F(SkiaMemoryTracerTest, test006) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.dumpNumericValue("element", "size", "bytes", 1048576);
    tracer.dumpNumericValue("element", "purgeable_size", "bytes", 524288);
}

TEST_F(SkiaMemoryTracerTest, test007) {
    SkiaMemoryTracer tracer(resourceMap_, true);
    tracer.dumpNumericValue("element", "size", "bytes", 1024);
    tracer.dumpNumericValue("element", "type", "texture", 0);
}

TEST_F(SkiaMemoryTracerTest, test008) {
    SkiaMemoryTracer tracer(resourceMap_, true);
    tracer.dumpNumericValue("element", "size", "bytes", 1024);
}

TEST_F(SkiaMemoryTracerTest, test009) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    const char* mapped = tracer.MapName("texture/123");
    EXPECT_STREQ("gpu/texture", mapped);
}

TEST_F(SkiaMemoryTracerTest, test010) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    const char* mapped = tracer.MapName("unknown/123");
    EXPECT_EQ(nullptr, mapped);
}

TEST_F(SkiaMemoryTracerTest, test011) {
    SkiaMemoryTracer tracer("category", false);
    tracer.dumpNumericValue("element", "size", "bytes", 1024);
    tracer.dumpNumericValue("element", "category", "gpu/resource", 0);
}

TEST_F(SkiaMemoryTracerTest, test012) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.dumpNumericValue("texture/123", "size", "bytes", 1024);
}

TEST_F(SkiaMemoryTracerTest, test013) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.dumpNumericValue("element1", "size", "bytes", 1048576);
}

TEST_F(SkiaMemoryTracerTest, test014) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.dumpNumericValue("element", "size", "bytes", 1048576);
    tracer.dumpNumericValue("element", "size", "bytes", 1048576);
}

TEST_F(SkiaMemoryTracerTest, test015) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.dumpNumericValue("element", "size", "bytes", 1048576);
}

TEST_F(SkiaMemoryTracerTest, test016) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.dumpNumericValue("element", "size", "KB", 1024);
}

TEST_F(SkiaMemoryTracerTest, test017) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.dumpNumericValue("element", "size", "MB", 5);
}

TEST_F(SkiaMemoryTracerTest, test018) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.dumpStringValue("element", "name", "value");
}

TEST_F(SkiaMemoryTracerTest, test019) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    EXPECT_EQ(SkTraceMemoryDump::kLight_LevelOfDetail, tracer.getRequestedDetails());
}

TEST_F(SkiaMemoryTracerTest, test020) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    EXPECT_TRUE(tracer.shouldDumpWrappedObjects());
}

TEST_F(SkiaMemoryTracerTest, test021) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.currentValues_.emplace("purgeable_size",
        SkiaMemoryTracer::TraceValue("bytes", 1));
    tracer.ProcessPurgeableSize();
    EXPECT_NE(tracer.purgeableSize_.count, 0);
}

TEST_F(SkiaMemoryTracerTest, test022) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    std::string result = tracer.ProcessType();
    EXPECT_EQ(result, "");
    SkiaMemoryTracer tracer1(resourceMap_, true);
    result = tracer1.ProcessType();
    EXPECT_EQ(result, "Other");
    tracer1.currentValues_.emplace("type",
        SkiaMemoryTracer::TraceValue("bytes", 1));
    result = tracer1.ProcessType();
    EXPECT_EQ(result, "bytes");
}

TEST_F(SkiaMemoryTracerTest, test023) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    tracer.categoryKey_ = "category_key";
    tracer.currentElement_.clear();
    auto result1 = tracer.ProcessResourceName();
    tracer.currentValues_.emplace("category_key",
        SkiaMemoryTracer::TraceValue("bytes", 1));
    auto result2 = tracer.ProcessResourceName();
    EXPECT_NE(result1, result2);
}

TEST_F(SkiaMemoryTracerTest, test024) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    const char* resourceName = "resource_name";
    std::string key = "key";
    tracer.currentValues_.emplace("size",
        SkiaMemoryTracer::TraceValue("bytes", 1));
    std::unordered_map<std::string, SkiaMemoryTracer::TraceValue> values;
    values.emplace(key,
        SkiaMemoryTracer::TraceValue("KB", 1));
    tracer.results_.emplace("resource_name", values);
    tracer.ProcessResults(resourceName, key);
    values.clear();
    values.emplace(key,
        SkiaMemoryTracer::TraceValue("bytes", 1));
    tracer.results_.clear();
    tracer.results_.emplace("resource_name", values);
    tracer.ProcessResults(resourceName, key);
    auto result = tracer.results_.find(resourceName);
    auto& resourceValues = result->second;
    auto typeResult = resourceValues.find(key);
    EXPECT_NE(typeResult->second.count, 1);
}

TEST_F(SkiaMemoryTracerTest, test025) {
    SkiaMemoryTracer tracer(resourceMap_, false);
    SkiaMemoryTracer::TraceValue value{"bytes", MEMUNIT_RATE * MEMUNIT_RATE};
    auto result = tracer.ConvertToMB(value);
    EXPECT_EQ(result, 1);
    value.units = "KB";
    result = tracer.ConvertToMB(value);
    EXPECT_EQ(result, MEMUNIT_RATE);
    value.units = "MB";
    result = tracer.ConvertToMB(value);
    EXPECT_EQ(result, MEMUNIT_RATE * MEMUNIT_RATE);
}

TEST_F(SkiaMemoryTracerTest, test026) {
    SkiaMemoryTracer tracer(resourceMap_, true);
    tracer.currentElement_ = "size";
    tracer.categoryKey_ = "size";
    tracer.currentValues_.emplace("size",
        SkiaMemoryTracer::TraceValue("bytes", 1));
    tracer.ProcessElement();
    EXPECT_TRUE(tracer.currentElement_.empty());
    EXPECT_TRUE(tracer.currentValues_.empty());
}

} // namespace skia