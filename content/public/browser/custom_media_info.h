
#ifndef CONTENT_PUBLIC_BROWSER_CUSTOM_MEDIA_INFO_H_
#define CONTENT_PUBLIC_BROWSER_CUSTOM_MEDIA_INFO_H_

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace content {

struct MediaInfo {
  enum class MediaType : uint32_t {
    VIDEO = 0,
    AUDIO,
  };
  enum class SourceType : uint32_t {
    URL = 0,
    MSE,
  };
  enum class Preload : uint32_t {
    NONE = 0,
    METADATA,
    AUTO,
  };
  struct MediaSourceInfo {
    SourceType source_type = SourceType::URL;
    std::string media_source;
    std::string media_format;
  };
  struct NativeMediaPlayerSurfaceInfo {
    std::string id;
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
  };

  std::string embed_id;
  MediaType media_type = MediaType::VIDEO;
  std::vector<MediaSourceInfo> media_src_list;
  NativeMediaPlayerSurfaceInfo surface_info;
  bool controls = false;
  std::vector<std::string> controlslist;
  bool muted = false;
  std::string poster_url;
  Preload preload = Preload::AUTO;
  std::map<std::string, std::string> https_headers;
  std::map<std::string, std::string> attributes;
};

} // namespace

#endif // CONTENT_PUBLIC_BROWSER_CUSTOM_MEDIA_INFO_H_
