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

#ifndef SNAPSHOT_GENERATOR_H
#define SNAPSHOT_GENERATOR_H
#if defined(ON_ENABLE_HEAP_TRANSLATE)
#include <set>

#include "include/v8-profiler.h"
#include "src/objects/visitors.h"

// https://learn.microsoft.com/en-us/microsoft-edge/devtools/memory-problems/heap-snapshots

// see HeapSnapshotJSONSerializer::SerializeSnapshot in
// heap-snapshot-generator.cc
namespace dfx {
class SnapshotGenerator;
class Node;

// copy from heap-snapshot-generator.h
// edge_fields
class Edge {
 public:
  enum class Type : uint8_t {
    kContextVariable = v8::HeapGraphEdge::kContextVariable,
    kElement = v8::HeapGraphEdge::kElement,
    kProperty = v8::HeapGraphEdge::kProperty,
    kInternal = v8::HeapGraphEdge::kInternal,
    kHidden = v8::HeapGraphEdge::kHidden,
    kShortcut = v8::HeapGraphEdge::kShortcut,
    kWeak = v8::HeapGraphEdge::kWeak,
    kNumTypes
  };
  Edge(Type type, uint32_t from, uint32_t name_or_index, Node* to);
  uint32_t type_ : 8;
  uint32_t from_index_ : 24;
  uint32_t name_or_index_;  // index to string
  Node* to_node_;
};

// node_fields
class Node {
 public:
  enum class Type : uint8_t {
    kHidden = v8::HeapGraphNode::kHidden,
    kArray = v8::HeapGraphNode::kArray,
    kString = v8::HeapGraphNode::kString,
    kObject = v8::HeapGraphNode::kObject,
    kCode = v8::HeapGraphNode::kCode,
    kClosure = v8::HeapGraphNode::kClosure,
    kRegExp = v8::HeapGraphNode::kRegExp,
    kHeapNumber = v8::HeapGraphNode::kHeapNumber,
    kNative = v8::HeapGraphNode::kNative,
    kSynthetic = v8::HeapGraphNode::kSynthetic,
    kConsString = v8::HeapGraphNode::kConsString,
    kSlicedString = v8::HeapGraphNode::kSlicedString,
    kSymbol = v8::HeapGraphNode::kSymbol,
    kBigInt = v8::HeapGraphNode::kBigInt,
    kObjectShape = v8::HeapGraphNode::kObjectShape,
    kNumTypes,
    kNotInit,
  };
  Node() = default;
  void SetReference(Edge::Type type,
                    Node* child_node,
                    uint32_t name_or_index,
                    SnapshotGenerator* generator);
  uint32_t type_ : 8 {0};
  uint32_t name_ : 24 {0};  // index to string
  uint32_t id_;
  uint32_t self_size_{0};
  uint8_t detachedness_{0};
  // a node will visited only once if use heap iterator
  std::vector<Edge*> childs_;
};

class SnapshotGenerator {
 public:
  explicit SnapshotGenerator();
  ~SnapshotGenerator();
  // get node index from address; get address from
  Node* GetNode(v8::internal::Address obj_addr);
  Node* AddNode(v8::internal::Address obj_addr,
                Node::Type type,
                std::string name,
                uint32_t self_size);
  void AddSyntheticRootNodes();

  std::vector<Node*>& nodes();

  void AdvanceEdgeCount();
  uint32_t EdgeCount();
  std::unordered_map<std::string, uint32_t>& strings();

  uint32_t GetStringId(std::string str);

  // reference
  void SetContextReference();
  void SetNativeBindReference();
  void SetElementReference();
  void SetInternalReference();
  void SetHiddenReference(v8::internal::Address parent,
                          uint32_t index,
                          v8::internal::Address child);
  void SetWeakReference(v8::internal::Address parent,
                        uint32_t index,
                        v8::internal::Address child,
                        std::optional<int> field_offset);
  void SetPropertyReference();
  void SetDataOrAccessorPropertyReference();

  void SetUserGlobalReference();
  void SetGcSubrootReference(v8::internal::Root root,
                             std::string description,
                             bool is_weak,
                             v8::internal::Address root_address);

 private:
  Node* CreateNode(Node::Type type, std::string name, uint32_t self_size);
  void AddRootNode();
  void AddGcRootNode();

  void SetReference(Edge::Type type,
                    v8::internal::Address parent,
                    uint32_t name_or_index,
                    v8::internal::Address child);

  std::vector<Node*> nodes_;
  Node* root_{nullptr};
  Node* gc_root_{nullptr};
  std::vector<Node*> gc_subroot_nodes_;
  uint32_t edge_count_{0};
  std::unordered_map<v8::internal::Address, uint32_t> node_by_addr_;

  std::unordered_map<std::string, uint32_t> strings_;
};
}  // namespace dfx

#endif
#endif
