// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on SkiaMemoryTracer.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#ifndef RS_SKIA_MEMORY_TRACER_H
#define RS_SKIA_MEMORY_TRACER_H
#include <unordered_map>
#include <vector>

#include "include/core/SkString.h"
#include "include/core/SkTraceMemoryDump.h"

typedef std::pair<const char*, const char*> ResourcePair;

class SkiaMemoryTracer : public SkTraceMemoryDump {
 public:
  SkiaMemoryTracer(const char* categoryKey, bool itemizeType);
  SkiaMemoryTracer(const std::vector<ResourcePair>& resourceMap,
                   bool itemizeType);
  ~SkiaMemoryTracer() override {}

  float GetGpuMemorySizeInMB();
  void dumpNumericValue(const char* dumpName,
                        const char* valueName,
                        const char* units,
                        uint64_t value) override;

  void dumpStringValue(const char* dumpName,
                       const char* valueName,
                       const char* value) override {
    dumpNumericValue(dumpName, valueName, value, 0);
  }

  void setMemoryBacking(const char*, const char*, const char*) override {}
  void setDiscardableMemoryBacking(const char*,
                                   const SkDiscardableMemory&) override {}

  LevelOfDetail getRequestedDetails() const override {
    return SkTraceMemoryDump::kLight_LevelOfDetail;
  }

  bool shouldDumpWrappedObjects() const override { return true; }

 private:
  struct TraceValue {
    TraceValue(const char* units, uint64_t value)
        : units(units), value(value), count(1) {}
    TraceValue(const TraceValue& v)
        : units(v.units), value(v.value), count(v.count) {}
    TraceValue& operator=(const TraceValue& v) {
      units = v.units;
      value = v.value;
      count = v.count;
      return *this;
    }
    SkString units;
    float value;
    int count;
  };

  const char* MapName(const char* resourceName);
  void ProcessElement();
  bool ProcessSize();
  void ProcessPurgeableSize();
  std::string ProcessType();
  const char* ProcessResourceName();
  void ProcessResults(const char* resourceName, const std::string& key);
  float ConvertToMB(const TraceValue& value);

  const std::vector<ResourcePair> resourceMap_;
  const char* categoryKey_ = nullptr;
  const bool itemizeType_;

  TraceValue totalSize_;
  TraceValue purgeableSize_;
  std::string currentElement_;
  std::unordered_map<std::string, TraceValue> currentValues_;
  std::unordered_map<std::string, std::unordered_map<std::string, TraceValue>>
      results_;
};
#endif
