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

namespace blink {

void UpdateExt(cc::Layer& layer,
               PendingLayer& pending_layer,
               LayerListBuilder& layer_list_builder,
               LayerListBuilder& layer_list_builder_for_video,
               LayerListBuilder& layer_list_builder_for_infinity,
               LayerListBuilder& layer_list_builder_for_overlay) {
  bool should_at_top_in_z_order = false;
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  if (!should_at_top_in_z_order) {
    should_at_top_in_z_order = layer.ShouldOverlay();
  }
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT)
  if (!should_at_top_in_z_order) {
    DisplayItemClient* display_item_client =
        reinterpret_cast<DisplayItemClient*>(
            pending_layer.FirstPaintChunk().id.client_id);
    if (display_item_client) {
      Node* node = DOMNodeIds::NodeForId(display_item_client->OwnerNodeId());
      if (node && node->ShouldOverlay()) {
        if (node->GetLayoutObject()->HasNonZeroEffectiveOpacity()) {
          should_at_top_in_z_order = true;
        }
      }
    }
  }
#endif  // ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT
  if (should_at_top_in_z_order) {
    layer_list_builder_for_video.Add(&layer);
  }
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  else if (layer.NativeEmbedOverlayInfinity()) {
    layer_list_builder_for_infinity.Add(&layer);
  } else if (layer.NativeEmbedOverlay()) {
    layer_list_builder_for_overlay.Add(&layer);
  }
#endif
  else {
    layer_list_builder.Add(&layer);
  }
}

}  // namespace blink
