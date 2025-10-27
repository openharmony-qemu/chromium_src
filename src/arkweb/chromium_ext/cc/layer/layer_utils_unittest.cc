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

#include "arkweb/chromium_ext/cc/layer/layer_utils.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace cc {

class TestLayer : public cc::Layer {
public:
    TestLayer() : cc::Layer() {}
    void SetNeedsPushProperties() {
        set_needs_push_properties_called_ = true;
    }
    bool set_needs_push_properties_called() const {
        return set_needs_push_properties_called_;
    }
    void reset_set_needs_push_properties_called() {
        set_needs_push_properties_called_ = false;
    }
    
private:
    bool set_needs_push_properties_called_ = false;
};

TEST(LayerUtilsTest, SetNativeEmbedId001) {
    TestLayer layer;
    cc::LayerUtils utils(&layer);
    utils.SetNativeEmbedId(100);
    EXPECT_EQ(utils.native_embed_id(), 100);
}

TEST(LayerUtilsTest, SetNativeEmbedId002) {
    TestLayer layer;
    cc::LayerUtils utils(&layer);
    utils.SetNativeEmbedId(100);
    layer.reset_set_needs_push_properties_called();
    utils.SetNativeEmbedId(100);
    EXPECT_EQ(utils.native_embed_id(), 100);
    EXPECT_FALSE(layer.set_needs_push_properties_called());
}

}  // namespace cc