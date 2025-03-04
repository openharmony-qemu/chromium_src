// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on SkiaMemoryTracer.cpp originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "skia_memory_tracer.h"

#include <numeric>

constexpr uint32_t MEMUNIT_RATE = 1024;

SkiaMemoryTracer::SkiaMemoryTracer(const std::vector<ResourcePair>& resourceMap,
                                   bool itemizeType)
    : resourceMap_(resourceMap),
      itemizeType_(itemizeType),
      totalSize_("bytes", 0),
      purgeableSize_("bytes", 0) {}

SkiaMemoryTracer::SkiaMemoryTracer(const char* categoryKey, bool itemizeType)
    : categoryKey_(categoryKey),
      itemizeType_(itemizeType),
      totalSize_("bytes", 0),
      purgeableSize_("bytes", 0) {}

const char* SkiaMemoryTracer::MapName(const char* resourceName) {
  for (auto& resource : resourceMap_) {
    if (SkStrContains(resourceName, resource.first)) {
      return resource.second;
    }
  }
  return nullptr;
}

void SkiaMemoryTracer::ProcessElement() {
  if (currentElement_.empty()) {
    return;
  }

  if (!ProcessSize()) {
    currentElement_.clear();
    currentValues_.clear();
    return;
  }

  ProcessPurgeableSize();
  std::string type = ProcessType();
  std::string key = (itemizeType_) ? type : "size";

  const char* resourceName = ProcessResourceName();
  if (resourceName == nullptr) {
    resourceName = currentElement_.c_str();
  }

  ProcessResults(resourceName, key);

  currentElement_.clear();
  currentValues_.clear();
}

bool SkiaMemoryTracer::ProcessSize() {
  auto sizeResult = currentValues_.find("size");
  if (sizeResult == currentValues_.end()) {
    return false;
  }

  totalSize_.value += sizeResult->second.value;
  totalSize_.count++;
  return true;
}

void SkiaMemoryTracer::ProcessPurgeableSize() {
  auto purgeableResult = currentValues_.find("purgeable_size");
  if (purgeableResult != currentValues_.end()) {
    purgeableSize_.value += purgeableResult->second.value;
    purgeableSize_.count++;
  }
}

std::string SkiaMemoryTracer::ProcessType() {
  auto typeResult = currentValues_.find("type");
  if (typeResult != currentValues_.end()) {
    return typeResult->second.units.c_str();
  } else if (itemizeType_) {
    return "Other";
  }
  return "";
}

const char* SkiaMemoryTracer::ProcessResourceName() {
  const char* resourceName = MapName(currentElement_.c_str());
  if (categoryKey_ != nullptr) {
    auto categoryResult = currentValues_.find(categoryKey_);
    if (categoryResult != currentValues_.end()) {
      resourceName = categoryResult->second.units.c_str();
    }
  }
  return resourceName;
}

void SkiaMemoryTracer::ProcessResults(const char* resourceName,
                                      const std::string& key) {
  auto result = results_.find(resourceName);
  if (result == results_.end()) {
    std::string strResourceName = resourceName;
    TraceValue sizeValue = currentValues_.find("size")->second;
    currentValues_.clear();
    currentValues_.insert({key, sizeValue});
    results_.insert({strResourceName, currentValues_});
  } else {
    auto& resourceValues = result->second;
    auto typeResult = resourceValues.find(key);
    if (typeResult == resourceValues.end()) {
      resourceValues.insert({key, currentValues_.find("size")->second});
    } else {
      typeResult->second.value += currentValues_.find("size")->second.value;
      typeResult->second.count++;
    }
  }
}

void SkiaMemoryTracer::dumpNumericValue(const char* dumpName,
                                        const char* valueName,
                                        const char* units,
                                        uint64_t value) {
  if (currentElement_ != dumpName) {
    ProcessElement();
    currentElement_ = dumpName;
  }
  currentValues_.insert({valueName, {units, value}});
}

float SkiaMemoryTracer::GetGpuMemorySizeInMB() {
  ProcessElement();
  return ConvertToMB(totalSize_);
}

float SkiaMemoryTracer::ConvertToMB(const TraceValue& value) {
  if (value.units == SkString("bytes")) {
    return value.value / MEMUNIT_RATE / MEMUNIT_RATE;
  }
  if (value.units == SkString("KB")) {
    return value.value / MEMUNIT_RATE;
  }
  return value.value;
}
