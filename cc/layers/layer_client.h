
#ifndef CC_LAYERS_LAYER_CLIENT_H_
#define CC_LAYERS_LAYER_CLIENT_H_

#include <stddef.h>

namespace {
class Rect;
}

namespace cc {

class LayerClient {
 public:
  LayerClient() = default;
  LayerClient(const LayerClient&) = default;
  LayerClient(LayerClient&&) = default;
  LayerClient& operator=(const LayerClient&) = default;
  LayerClient& operator=(LayerClient&&) = default;

  virtual void OnLayerRectChange(int x, int y, int width, int height) = 0;
 protected:
  virtual ~LayerClient() = default;

};

} // namespace

#endif //  CC_LAYERS_LAYER_CLIENT_H_
