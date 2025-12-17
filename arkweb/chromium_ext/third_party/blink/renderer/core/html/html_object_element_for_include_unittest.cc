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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/html/html_object_element.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "third_party/blink/renderer/core/html/html_param_element.h"
#include "third_party/blink/renderer/core/dom/container_node.h"

namespace blink {

class HTMLObjectElementForIncludeTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    document_ = &GetDocument();
    element_ = MakeGarbageCollected<HTMLObjectElement>(*document_, CreateElementFlags());
    document_->body()->AppendChild(element_.Get());
    UpdateAllLifecyclePhasesForTest();
  }

  Element::AttributeModificationParams CreateParams(const AtomicString& value) {
    return Element::AttributeModificationParams(
      html_names::kDirAttr,
      g_null_atom,
      value,
      Element::AttributeModificationReason::kDirectly
    );
  }

  AtomicString CreateAlignedAtomicString(const char* str) {
    size_t len = std::strlen(str);
    const size_t alignment = 8;
    const size_t buffer_size = len + alignment;
    char* raw_mem = static_cast<char*>(std::malloc(buffer_size));
    if (!raw_mem) {
      return AtomicString();
    }
    uintptr_t raw_addr = reinterpret_cast<uintptr_t>(raw_mem);
    uintptr_t aligned_addr = (raw_addr + alignment - 1) & ~(alignment - 1);
    char* aligned_ptr = reinterpret_cast<char*>(aligned_addr);
    size_t safe_copy_size = buffer_size - (aligned_addr - raw_addr);
    memcpy_s(aligned_ptr, safe_copy_size, str, len + 1);
    AtomicString result(aligned_ptr);
    std::free(raw_mem);
    return result;
  }

  Persistent<HTMLParamElement> CreateParamElement(const String& id, const String& name, const String& value) {
    auto* param = MakeGarbageCollected<HTMLParamElement>(*document_);
    if (!id.empty()) {
      param->setAttribute(html_names::kIdAttr, AtomicString(id));
    }
    if (!name.empty()) {
      param->setAttribute(html_names::kNameAttr, AtomicString(name));
    }
    return param;
  }

  Persistent<Document> document_;
  Persistent<HTMLObjectElement> element_;
};

TEST_F(HTMLObjectElementForIncludeTest, HandlesEmptyValue) {
  auto params = CreateParams(AtomicString(""));
  element_->NativeEmbedOverlay(params);
}

TEST_F(HTMLObjectElementForIncludeTest, ExactMatchOverlayInfinity) {
  AtomicString aligned_str = CreateAlignedAtomicString("display:overlay-infinity");
  auto params = CreateParams(aligned_str);
  element_->NativeEmbedOverlay(params);
}

TEST_F(HTMLObjectElementForIncludeTest, ContainsOverlayInfinity) {
  auto params = CreateParams(AtomicString("prefix display:overlay-infinity suffix"));
  element_->NativeEmbedOverlay(params);
}

TEST_F(HTMLObjectElementForIncludeTest, ExactMatchOverlay) {
  AtomicString aligned_str = CreateAlignedAtomicString("display:overlay");
  auto params = CreateParams(aligned_str);
  element_->NativeEmbedOverlay(params);
}

TEST_F(HTMLObjectElementForIncludeTest, ContainsOverlay) {
  AtomicString aligned_str = CreateAlignedAtomicString("prefix display:overlay suffix");
  auto params = CreateParams(aligned_str);
  element_->NativeEmbedOverlay(params);
}

TEST_F(HTMLObjectElementForIncludeTest, HandlesInvalidValue) {
  AtomicString aligned_str = CreateAlignedAtomicString("invalid_value");
  auto params = CreateParams(aligned_str);
  element_->NativeEmbedOverlay(params);
}

TEST_F(HTMLObjectElementForIncludeTest, AddParamChange) {
  Vector<ParamChangeInfo> param_changes;
  param_changes.push_back(ParamChangeInfo(ParamChangeInfo::Status::kAdd,
                                          AtomicString("test"),
                                          AtomicString("test"),
                                          AtomicString("test")));
  Persistent<HTMLParamElement> param = CreateParamElement("p1", "key", "val");
  element_->AddParamChange(param_changes, param.Get(), ParamChangeInfo::Status::kAdd);
}

TEST_F(HTMLObjectElementForIncludeTest, HandleParamAlterations001) {
  Persistent<HTMLParamElement> param = CreateParamElement("p_add", "k_add", "v_add");
  ContainerNode::ChildrenChange change =
      ContainerNode::ChildrenChange::ForInsertion(*param, nullptr, nullptr, ContainerNode::ChildrenChangeSource::kAPI);
  element_->HandleParamAlterations(change);
}

TEST_F(HTMLObjectElementForIncludeTest, HandleParamAlterations002) {
  Persistent<HTMLParamElement> param = CreateParamElement("p_add", "k_add", "v_add");
  ContainerNode::ChildrenChange change =
      ContainerNode::ChildrenChange::ForRemoval(*param, nullptr, nullptr, ContainerNode::ChildrenChangeSource::kAPI);
  element_->HandleParamAlterations(change);
}

TEST_F(HTMLObjectElementForIncludeTest, HandleParamAlterations003) {
  Persistent<HTMLParamElement> param1 = CreateParamElement("p1", "key1", "val1");
  Persistent<HTMLParamElement> param2 = CreateParamElement("p2", "key2", "val2");
  HeapVector<Member<Node>> removed_nodes;
  removed_nodes.push_back(param1.Get());
  removed_nodes.push_back(param2.Get());
  ContainerNode::ChildrenChange change = {
      .type = ContainerNode::ChildrenChangeType::kAllChildrenRemoved,
      .by_parser = ContainerNode::ChildrenChangeSource::kAPI,
      .affects_elements = ContainerNode::ChildrenChangeAffectsElements::kYes,
      .removed_nodes = removed_nodes,
  };
  element_->HandleParamAlterations(change);
}

}  // namespace blink