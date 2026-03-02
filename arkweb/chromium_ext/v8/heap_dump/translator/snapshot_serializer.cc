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

#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(OH_ENABLE_HEAP_DUMP_TEST))
#include "snapshot_serializer.h"

#include <stdio.h>
#include "arkweb/ohos_nweb_ex/third_party/securec/include/securec.h"
#include "src/base/logging.h"

namespace dfx {

class SnapshotJSONSerializer::SerializeWriter final : public BinaryWriter {
 public:
  // 1MB buffer
  explicit SerializeWriter() : BinaryWriter(kBufferMB) {}
  void AddCharacter(char ch) {
    WriteBinBlock(reinterpret_cast<uint8_t*>(&ch), sizeof(ch));
  }
  void AddString(const char* s) {
    uint32_t len = static_cast<uint32_t>(strlen(s));
    WriteBinBlock(reinterpret_cast<const uint8_t*>(s), len);
  }
  void AddNumber(uint32_t n) {
    char buf[32] = {0};
    int len = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%u", n);
    CHECK(len >= 0);
    WriteBinBlock(reinterpret_cast<uint8_t*>(buf), static_cast<uint32_t>(len));
  }
  bool aborted() { return false; }
};

SnapshotJSONSerializer::SnapshotJSONSerializer(SnapshotGenerator* generator)
    : generator_(generator) {}

void SnapshotJSONSerializer::Serialize() {
  writer_ = std::make_unique<SerializeWriter>();
  writer_->OpenFile("rawheap.heapsnapshot");

  trace_function_count_ = 0;
  SerializeImpl();

  writer_->CloseFile();
}

void SnapshotJSONSerializer::SerializeImpl() {
  writer_->AddCharacter('{');
  writer_->AddString("\"snapshot\":{");
  SerializeSnapshot();
  if (writer_->aborted()) {
    return;
  }
  writer_->AddString("},\n");
  writer_->AddString("\"nodes\":[");
  SerializeNodes();
  if (writer_->aborted()) {
    return;
  }
  writer_->AddString("],\n");
  writer_->AddString("\"edges\":[");
  SerializeEdges();
  if (writer_->aborted()) {
    return;
  }
  writer_->AddString("],\n");

  writer_->AddString("\"trace_function_infos\":[");
  SerializeTraceNodeInfos();
  if (writer_->aborted()) {
    return;
  }
  writer_->AddString("],\n");
  writer_->AddString("\"trace_tree\":[");
  SerializeTraceTree();
  if (writer_->aborted()) {
    return;
  }
  writer_->AddString("],\n");

  writer_->AddString("\"samples\":[");
  SerializeSamples();
  if (writer_->aborted()) {
    return;
  }
  writer_->AddString("],\n");

  writer_->AddString("\"locations\":[");
  SerializeLocations();
  if (writer_->aborted()) {
    return;
  }
  writer_->AddString("],\n");

  writer_->AddString("\"strings\":[");
  SerializeStrings();
  if (writer_->aborted()) {
    return;
  }
  writer_->AddCharacter(']');
  writer_->AddCharacter('}');
}

void SnapshotJSONSerializer::SerializeSnapshot() {
  writer_->AddString("\"meta\":");
  // The object describing node serialization layout.
  // We use a set of macros to improve readability.

  // clang-format off
#define JSON_A(s) "[" s "]"
#define JSON_S(s) "\"" s "\""
  writer_->AddString("{"
    JSON_S("node_fields") ":["
        JSON_S("type") ","
        JSON_S("name") ","
        JSON_S("id") ","
        JSON_S("self_size") ","
        JSON_S("edge_count") ",");
  if (trace_function_count_) writer_->AddString(JSON_S("trace_node_id") ",");
  writer_->AddString(
        JSON_S("detachedness")
    "],"
    JSON_S("node_types") ":" JSON_A(
        JSON_A(
            JSON_S("hidden") ","
            JSON_S("array") ","
            JSON_S("string") ","
            JSON_S("object") ","
            JSON_S("code") ","
            JSON_S("closure") ","
            JSON_S("regexp") ","
            JSON_S("number") ","
            JSON_S("native") ","
            JSON_S("synthetic") ","
            JSON_S("concatenated string") ","
            JSON_S("sliced string") ","
            JSON_S("symbol") ","
            JSON_S("bigint") ","
            JSON_S("object shape")) ","
        JSON_S("string") ","
        JSON_S("number") ","
        JSON_S("number") ","
        JSON_S("number") ","
        JSON_S("number")) ","
    JSON_S("edge_fields") ":" JSON_A(
        JSON_S("type") ","
        JSON_S("name_or_index") ","
        JSON_S("to_node")) ","
    JSON_S("edge_types") ":" JSON_A(
        JSON_A(
            JSON_S("context") ","
            JSON_S("element") ","
            JSON_S("property") ","
            JSON_S("internal") ","
            JSON_S("hidden") ","
            JSON_S("shortcut") ","
            JSON_S("weak")) ","
        JSON_S("string_or_number") ","
        JSON_S("node")) ","
    JSON_S("trace_function_info_fields") ":" JSON_A(
        JSON_S("function_id") ","
        JSON_S("name") ","
        JSON_S("script_name") ","
        JSON_S("script_id") ","
        JSON_S("line") ","
        JSON_S("column")) ","
    JSON_S("trace_node_fields") ":" JSON_A(
        JSON_S("id") ","
        JSON_S("function_info_index") ","
        JSON_S("count") ","
        JSON_S("size") ","
        JSON_S("children")) ","
    JSON_S("sample_fields") ":" JSON_A(
        JSON_S("timestamp_us") ","
        JSON_S("last_assigned_id")) ","
    JSON_S("location_fields") ":" JSON_A(
        JSON_S("object_index") ","
        JSON_S("script_id") ","
        JSON_S("line") ","
        JSON_S("column"))
  "}");
// clang-format on
#undef JSON_S
#undef JSON_A
  writer_->AddString(",\"node_count\":");
  writer_->AddNumber(static_cast<uint32_t>(generator_->nodes().size()));
  writer_->AddString(",\"edge_count\":");
  writer_->AddNumber(static_cast<uint32_t>(generator_->EdgeCount()));
  writer_->AddString(",\"trace_function_count\":");
  writer_->AddNumber(trace_function_count_);
}

void SnapshotJSONSerializer::SerializeNodes() {
  const std::vector<Node*>& nodes = generator_->nodes();
  for (const Node* node : nodes) {
    SerializeNode(node);
    if (writer_->aborted()) {
      return;
    }
  }
}

void SnapshotJSONSerializer::SerializeNode(const Node* node) {
  if (node->id_) {  // only first node no need ','
    writer_->AddCharacter(',');
  }
  CHECK(static_cast<uint8_t>(node->type_) <
        static_cast<uint8_t>(Node::Type::kNumTypes));
  writer_->AddNumber(node->type_);
  writer_->AddCharacter(',');
  CHECK(node->name_ < generator_->strings().size());
  writer_->AddNumber(node->name_);
  writer_->AddCharacter(',');
  CHECK(node->id_ < generator_->nodes().size() &&
        generator_->nodes()[node->id_]->id_ == node->id_);
  writer_->AddNumber(node->id_);
  writer_->AddCharacter(',');
  writer_->AddNumber(node->self_size_);
  writer_->AddCharacter(',');
  writer_->AddNumber(static_cast<uint32_t>(node->childs_.size()));
  CHECK(trace_function_count_ == 0);
  writer_->AddCharacter(',');
  writer_->AddNumber(node->detachedness_);
  writer_->AddCharacter('\n');
}

void SnapshotJSONSerializer::SerializeEdges() {
  const std::vector<Node*>& nodes = generator_->nodes();
  uint32_t count = 0;
  for (const Node* node : nodes) {
    for (const Edge* edge : node->childs_) {
      if (count++) {
        writer_->AddCharacter(',');
      }
      SerializeEdge(edge);
    }
  }
  CHECK(count == generator_->EdgeCount());
}

void SnapshotJSONSerializer::SerializeEdge(const Edge* edge) {
  writer_->AddNumber(edge->type_);
  writer_->AddCharacter(',');
  writer_->AddNumber(edge->name_or_index_);
  writer_->AddCharacter(',');
  writer_->AddNumber(to_node_index(edge->to_node_->id_));
  writer_->AddCharacter('\n');
}

void SnapshotJSONSerializer::SerializeStrings() {
  auto& strings = generator_->strings();
  std::vector<std::string_view> string_table;
  string_table.resize(strings.size());
  for (auto& it : strings) {
    string_table[it.second] = it.first;
  }
  CHECK(string_table.size() >= 1 && string_table[0] == "<dummy>");
  writer_->AddString("\"<dummy>\"");
  for (uint32_t i = 1; i < string_table.size(); ++i) {
    writer_->AddCharacter(',');
    SerializeString(string_table[i]);
    if (writer_->aborted()) {
      return;
    }
  }
}

void SnapshotJSONSerializer::SerializeString(std::string_view str) {
  writer_->AddCharacter('\n');
  writer_->AddCharacter('\"');
  for (auto ch : str) {
    switch (ch) {
      case '\b':
        writer_->AddString("\\b");
        continue;
      case '\f':
        writer_->AddString("\\f");
        continue;
      case '\n':
        writer_->AddString("\\n");
        continue;
      case '\r':
        writer_->AddString("\\r");
        continue;
      case '\t':
        writer_->AddString("\\t");
        continue;
      case '\"':
      case '\\':
        writer_->AddCharacter('\\');
        writer_->AddCharacter(ch);
        continue;
      default:
        if (ch > 31 && static_cast<uint8_t>(ch) < 128) {
          writer_->AddCharacter(ch);
        } else if (ch <= 31) {
          UNREACHABLE();
          // Special character with no dedicated literal.
        } else {
          UNREACHABLE();
          // Convert UTF-8 into \u UTF-16 literal.
        }
    }
  }
  writer_->AddCharacter('\"');
}

uint32_t SnapshotJSONSerializer::to_node_index(uint32_t node_index) {
  return node_index * (trace_function_count_
                           ? kNodeFieldsCountWithTraceNodeId
                           : kNodeFieldsCountWithoutTraceNodeId);
}

const uint32_t SnapshotJSONSerializer::kNodeFieldsCountWithTraceNodeId = 7;
const uint32_t SnapshotJSONSerializer::kNodeFieldsCountWithoutTraceNodeId = 6;

void SnapshotJSONSerializer::SerializeTraceNodeInfos() {}
void SnapshotJSONSerializer::SerializeTraceTree() {}
void SnapshotJSONSerializer::SerializeSamples() {}
void SnapshotJSONSerializer::SerializeLocations() {}

}  // namespace dfx
#endif
