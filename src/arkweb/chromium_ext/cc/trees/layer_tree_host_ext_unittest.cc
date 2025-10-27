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

#include "arkweb/chromium_ext/cc/trees/layer_tree_host_ext.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/test/layer_tree_test.h"
#include "cc/test/test_task_graph_runner.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::_;

namespace cc {
namespace {

class MockLayer : public Layer {
 public:
  explicit MockLayer(int id) : id_(id) {}
  int id() const { return id_; }
  MOCK_METHOD1(OnLayerRectUpdate, void(const gfx::Rect&));
  MOCK_METHOD1(OnLayerRectVisibilityChange, void(bool));
  MOCK_METHOD1(OnLayerBoundsUpdate, void(const gfx::Rect&));
  MOCK_METHOD1(CleanupVisibilityForRemovedLayer, void(bool));

 private:
  int id_;
};

class LayerTreeHostExtTestBase : public LayerTreeTest {
 protected:
  void SetupTree() override {
    LayerTreeTest::SetupTree();
    root_ = Layer::Create();
    layer_tree_host()->SetRootLayer(root_);
    host_ext_ = static_cast<LayerTreeHostExt*>(layer_tree_host());
  }

  void AddMockLayer(int id) {
    scoped_refptr<MockLayer> layer = new MockLayer(id);
    mock_layers_[id] = layer.get();
    root_->AddChild(layer);
  }

  MockLayer* GetMockLayer(int id) { return mock_layers_[id]; }
  LayerTreeHostExt* host_ext() { return host_ext_; }

 private:
  scoped_refptr<Layer> root_;
  raw_ptr<LayerTreeHostExt> host_ext_ = nullptr;
  std::unordered_map<int, MockLayer*> mock_layers_;
};

class RegisterClippedSelectionBoundsSameTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    gfx::Rect bounds(10, 10, 100, 100);
    host_ext()->RegisterClippedVisualViewportSelectionBounds(bounds);
    host_ext()->RegisterClippedVisualViewportSelectionBounds(bounds);
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(RegisterClippedSelectionBoundsSameTest);

class RegisterClippedSelectionBoundsDiffTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    gfx::Rect bounds1(10, 10, 100, 100);
    gfx::Rect bounds2(20, 20, 200, 200);
    host_ext()->RegisterClippedVisualViewportSelectionBounds(bounds1);
    host_ext()->RegisterClippedVisualViewportSelectionBounds(bounds2);
    EXPECT_EQ(host_ext()->clipped_selection_bounds(), bounds2);
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(RegisterClippedSelectionBoundsDiffTest);

class OnLayerRectUpdateValidTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    const int layer_id = 1;
    AddMockLayer(layer_id);
    gfx::Rect rect(0, 0, 100, 100);
    host_ext()->OnLayerRectUpdate(layer_id, rect);
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(OnLayerRectUpdateValidTest);

class OnLayerRectUpdateInvalidTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    host_ext()->OnLayerRectUpdate(999, gfx::Rect(0, 0, 100, 100));
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(OnLayerRectUpdateInvalidTest);

class CleanupRemovedLayerVisibleTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    const int layer_id = 1;
    AddMockLayer(layer_id);
    host_ext()->CleanupVisibilityForRemovedLayer(GetMockLayer(layer_id));
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(CleanupRemovedLayerVisibleTest);

class CleanupRemovedLayerInvisibleTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    const int layer_id = 1;
    AddMockLayer(layer_id);
    EXPECT_CALL(*GetMockLayer(layer_id), CleanupVisibilityForRemovedLayer(_))
        .Times(0);
    host_ext()->CleanupVisibilityForRemovedLayer(GetMockLayer(layer_id));
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(CleanupRemovedLayerInvisibleTest);

class OnLayerBoundsUpdateValidTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    const int layer_id = 1;
    AddMockLayer(layer_id);
    gfx::Rect bounds(0, 0, 200, 200);
    host_ext()->OnLayerBoundsUpdate(layer_id, bounds);
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(OnLayerBoundsUpdateValidTest);

class OnLayerBoundsUpdateInvalidTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    host_ext()->OnLayerBoundsUpdate(999, gfx::Rect(0, 0, 200, 200));
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(OnLayerBoundsUpdateInvalidTest);

class OnLayerRectVisibilityChangeFalseTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    const int layer_id = 1;
    AddMockLayer(layer_id);
    host_ext()->OnLayerRectVisibilityChange(layer_id, false);
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(OnLayerRectVisibilityChangeFalseTest);

class OnLayerRectUpdateValidLayerTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    auto layer = Layer::Create();
    host_ext()->RegisterLayer(layer.get());
    const int layer_id = layer->id();
    gfx::Rect rect(0, 0, 100, 100);
    host_ext()->OnLayerRectUpdate(layer_id, rect);
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(OnLayerRectUpdateValidLayerTest);

class OnLayerRectVisibilityChangeLayerTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    auto layer = Layer::Create();
    host_ext()->RegisterLayer(layer.get());
    const int layer_id = layer->id();
    host_ext()->OnLayerRectVisibilityChange(layer_id, false);
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(OnLayerRectVisibilityChangeLayerTest);

class CleanupRemovedLayerVisibleNullLayerTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    host_ext()->CleanupVisibilityForRemovedLayer(nullptr);
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(CleanupRemovedLayerVisibleNullLayerTest);

class OnLayerBoundsUpdateWithLayerTest : public LayerTreeHostExtTestBase {
 protected:
  void BeginTest() override {
    auto layer = Layer::Create();
    host_ext()->RegisterLayer(layer.get());
    const int layer_id = layer->id();
    gfx::Rect bounds(0, 0, 200, 200);
    host_ext()->OnLayerBoundsUpdate(layer_id, bounds);
    EndTest();
  }
};

SINGLE_AND_MULTI_THREAD_TEST_F(OnLayerBoundsUpdateWithLayerTest);

}  //namespace
}  // namespace cc