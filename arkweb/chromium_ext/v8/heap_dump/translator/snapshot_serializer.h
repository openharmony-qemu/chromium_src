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

#ifndef SNAPSHOT_SERIALIZER_H
#define SNAPSHOT_SERIALIZER_H
#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(OH_ENABLE_HEAP_DUMP_TEST))

#include "arkweb/chromium_ext/v8/heap_dump/binary_writer.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_format.h"
#include "snapshot_generator.h"

namespace dfx {
class SnapshotJSONSerializer {
 public:
  class SerializeWriter;
  explicit SnapshotJSONSerializer(SnapshotGenerator* generator);
  ~SnapshotJSONSerializer() = default;

  void Serialize();

  // see heap-snapshot-generator.h
  static const uint32_t kNodeFieldsCountWithTraceNodeId;
  static const uint32_t kNodeFieldsCountWithoutTraceNodeId;

 private:
  // see heap-snapshot-generator-inl.h
  uint32_t to_node_index(uint32_t node_index);

  void SerializeEdge(const Edge* edge);
  void SerializeEdges();
  void SerializeImpl();
  void SerializeNode(const Node* node);
  void SerializeNodes();
  void SerializeSnapshot();
  void SerializeTraceTree();
  void SerializeTraceNode();
  void SerializeTraceNodeInfos();
  void SerializeSamples();
  void SerializeString(std::string_view str);
  void SerializeStrings();
  void SerializeLocation();
  void SerializeLocations();

  SnapshotGenerator* generator_;
  std::unique_ptr<SerializeWriter> writer_;
  uint32_t trace_function_count_{0};
};
}  // namespace dfx

#endif
#endif
