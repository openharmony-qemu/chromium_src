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

#if defined(ON_ENABLE_HEAP_TRANSLATE)
#include "snapshot_generator.h"

#include "include/v8-internal.h"

namespace dfx {
using namespace v8;

////////// Edge
Edge::Edge(Type type, uint32_t from, uint32_t name_or_index, Node* to)
    : type_(static_cast<uint8_t>(type)),
      from_index_(from),
      name_or_index_(name_or_index),
      to_node_(to) {}

////////// Node
void Node::SetReference(Edge::Type type,
                        Node* child_node,
                        uint32_t name_or_index,
                        SnapshotGenerator* generator) {
  Edge* e = new Edge(type, this->id_, name_or_index, child_node);
  childs_.push_back(e);
  generator->AdvanceEdgeCount();
}

////////// SnapshotGenerator

SnapshotGenerator::SnapshotGenerator() {
  // see SerializeStrings, the first is <dummy>
  (void)GetStringId("<dummy>");
}
SnapshotGenerator::~SnapshotGenerator() {
  for (auto node : nodes_) {
    for (auto edge : node->childs_) {
      delete edge;
    }
    delete node;
  }
}

std::vector<Node*>& SnapshotGenerator::nodes() {
  return nodes_;
}

void SnapshotGenerator::AdvanceEdgeCount() {
  edge_count_++;
}

uint32_t SnapshotGenerator::EdgeCount() {
  return edge_count_;
}

std::unordered_map<std::string, uint32_t>& SnapshotGenerator::strings() {
  return strings_;
}

Node* SnapshotGenerator::GetNode(internal::Address obj_addr) {
  auto it = node_by_addr_.find(obj_addr);
  if (it != node_by_addr_.end()) {
    return nodes_[it->second];
  }
  return nullptr;
}

Node* SnapshotGenerator::CreateNode(Node::Type type,
                                    std::string name,
                                    uint32_t self_size) {
  // Note: trace_node_id is not support now
  Node* n = new Node();
  n->type_ = static_cast<uint32_t>(type);
  n->name_ = GetStringId(name);
  // Note: id_ is used when compare two snapshot, get from dump
  n->id_ = static_cast<uint32_t>(nodes_.size());
  n->self_size_ = self_size;
  nodes_.push_back(n);
  return n;
}

Node* SnapshotGenerator::AddNode(internal::Address obj_addr,
                                 Node::Type type,
                                 std::string name,
                                 uint32_t self_size) {
  if (Node* n = GetNode(obj_addr); n != nullptr) {
    if (n->type_ == static_cast<uint32_t>(Node::Type::kNotInit)) {
      n->type_ = static_cast<uint32_t>(type);
      n->name_ = GetStringId(name);
      n->self_size_ = self_size;
    }
    return n;
  }
  Node* n = CreateNode(type, name, self_size);
  node_by_addr_[obj_addr] = n->id_;
  return n;
}

void SnapshotGenerator::AddSyntheticRootNodes() {
  AddRootNode();
  AddGcRootNode();
  CHECK(nodes_.size() == 2 && root_ && gc_root_);
  for (int root = 0; root < static_cast<int>(i::Root::kNumberOfRoots); root++) {
    gc_subroot_nodes_.push_back(
        CreateNode(Node::Type::kSynthetic,
                   i::RootVisitor::RootName(static_cast<i::Root>(root)), 0));
    // maybe SetIndexedAutoIndexReference?
    gc_root_->SetReference(Edge::Type::kElement, gc_subroot_nodes_.back(),
                           static_cast<uint32_t>(gc_root_->childs_.size() + 1),
                           this);
  }
}
void SnapshotGenerator::AddRootNode() {
  CHECK(nodes_.empty());
  root_ = CreateNode(Node::Type::kSynthetic, "", 0);
}
void SnapshotGenerator::AddGcRootNode() {
  CHECK(nodes_.size() == 1 && root_ && !gc_root_);
  gc_root_ = CreateNode(Node::Type::kSynthetic, "(GC roots)", 0);
  // see SetRootGcRootsReference in heap-snapshot-generator.cc
  // SetIndexedAutoIndexReference
  root_->SetReference(Edge::Type::kElement, gc_root_,
                      static_cast<uint32_t>(root_->childs_.size()) + 1, this);
}

uint32_t SnapshotGenerator::GetStringId(std::string str) {
  auto it = strings_.find(str);
  if (it != strings_.end()) {
    return it->second;
  }
  uint32_t id = static_cast<uint32_t>(strings_.size());
  strings_.emplace(str, id);
  return id;
}

void SnapshotGenerator::SetReference(Edge::Type type,
                                     internal::Address parent,
                                     uint32_t name_or_index,
                                     internal::Address child) {
  // edges_[parent].insert(child);
  Node* parent_node = GetNode(parent);
  Node* child_node = GetNode(child);
  CHECK(parent_node && child_node);
  parent_node->SetReference(type, child_node, name_or_index, this);
}

// SetHiddenReference in heap-snapshot-generator.cc
void SnapshotGenerator::SetHiddenReference(internal::Address parent,
                                           uint32_t index,
                                           internal::Address child) {
  // Note: Check IsEssentialObject and IsEssentialHiddenReference
  SetReference(Edge::Type::kHidden, parent, index, child);
}

void SnapshotGenerator::SetWeakReference(v8::internal::Address parent,
                                         uint32_t index,
                                         v8::internal::Address child,
                                         std::optional<int> field_offset) {
  // NIY:IsEssentialObject
  SetReference(Edge::Type::kWeak, parent,
               GetStringId(std::to_string(index).c_str()), child);
}

void SnapshotGenerator::SetGcSubrootReference(
    v8::internal::Root root,
    std::string description,
    bool is_weak,
    v8::internal::Address root_address) {
  // !!!root_address must not be smi, check it in dump_roots.cc
  CHECK(root_ && gc_root_ &&
        static_cast<size_t>(root) < gc_subroot_nodes_.size());
  Node* root_node = gc_subroot_nodes_[static_cast<uint32_t>(root)];
  Node* root_obj_node = AddNode(root_address, Node::Type::kNotInit, "", 0);

  Edge::Type edge_type = is_weak ? Edge::Type::kWeak : Edge::Type::kInternal;
  // maybe need description? see SetGcSubrootReference in
  // heap-snapshot-generator.cc
  root_node->SetReference(edge_type, root_obj_node, GetStringId(description),
                          this);
}

}  // namespace dfx
#endif
