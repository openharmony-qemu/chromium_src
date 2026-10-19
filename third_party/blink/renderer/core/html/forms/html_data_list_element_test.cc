// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/html/forms/html_data_list_element.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/id_target_observer.h"
#include "third_party/blink/renderer/core/dom/id_target_observer_registry.h"
#include "third_party/blink/renderer/core/html/html_document.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html_names.h"
#include "third_party/blink/renderer/core/testing/null_execution_context.h"
#include "third_party/blink/renderer/core/testing/sim/sim_request.h"
#include "third_party/blink/renderer/core/testing/sim/sim_test.h"
#include "third_party/blink/renderer/platform/heap/thread_state.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"

namespace blink {

class HTMLDataListElementTest : public SimTest {};

TEST_F(HTMLDataListElementTest, FinishedParsingChildren) {
  SimRequest main_resource("https://example.com/", "text/html");

  LoadURL("https://example.com/");
  main_resource.Complete("<datalist id=list></datalist>");

  auto* data_list = GetDocument().getElementById(AtomicString("list"));
  ASSERT_TRUE(data_list);
  EXPECT_TRUE(data_list->IsFinishedParsingChildren());
}

namespace {

class MutatingIdTargetObserver final : public IdTargetObserver {
 public:
  MutatingIdTargetObserver(IdTargetObserverRegistry& registry,
                           Element* target_element)
      : IdTargetObserver(registry, target_element->GetIdAttribute()),
        target_element_(target_element) {}

  void Trace(Visitor* visitor) const override {
    visitor->Trace(target_element_);
    IdTargetObserver::Trace(visitor);
  }

  void IdTargetChanged() override {
    Unregister();
    // Add an 8th/9th attribute to force reallocation of the element's
    // AttributeCollection buffer while NotifyObservers(GetIdAttribute()) is on
    // the stack.
    target_element_->setAttribute(AtomicString("b"), g_empty_atom);
    // Also mutate the id attribute to trigger a nested NotifyObservers call.
    target_element_->setAttribute(html_names::kIdAttr, AtomicString("new_id"));
  }

 private:
  Member<Element> target_element_;
};

}  // namespace

TEST_F(HTMLDataListElementTest,
       NotifyObserversWithAttributeReallocationAndReentrancy) {
  SimRequest main_resource("https://example.com/", "text/html");
  LoadURL("https://example.com/");
  main_resource.Complete(
      "<datalist a0 a1 a2 a3 a4 a5 a6 id=options><option value=a></datalist>");

  Element* list = GetDocument().getElementById(AtomicString("options"));
  ASSERT_TRUE(list);

  auto* observer = MakeGarbageCollected<MutatingIdTargetObserver>(
      GetDocument().EnsureIdTargetObserverRegistry(), list);
  EXPECT_TRUE(GetDocument().EnsureIdTargetObserverRegistry().HasObservers(
      AtomicString("options")));

  // Removing children calls
  // IdTargetObserverRegistry::NotifyObservers(GetIdAttribute()).
  list->RemoveChildren();

  EXPECT_FALSE(GetDocument().EnsureIdTargetObserverRegistry().HasObservers(
      AtomicString("options")));
  observer->Unregister();
}

TEST(HTMLDataListElementTest2, DecrementedAfterGc) {
  test::TaskEnvironment task_environment;
  ScopedNullExecutionContext execution_context;
  Persistent<Document> document =
      HTMLDocument::CreateForTest(execution_context.GetExecutionContext());
  document->write("<body><datalist id=x></datalist></body>");
  EXPECT_TRUE(document->HasAtLeastOneDataList());
  auto* data_list = document->getElementById(AtomicString("x"));
  ASSERT_TRUE(data_list);
  data_list->parentElement()->RemoveChild(data_list);
  data_list = nullptr;
  blink::ThreadState::Current()->CollectAllGarbageForTesting();
  EXPECT_FALSE(document->HasAtLeastOneDataList());
}

}  // namespace blink
