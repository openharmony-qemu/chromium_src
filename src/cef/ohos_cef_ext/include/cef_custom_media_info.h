
#ifndef CEF_INCLUDE_CEF_CUSTOM_MEDIA_INFO_H_
#define CEF_INCLUDE_CEF_CUSTOM_MEDIA_INFO_H_

#include <cstdint>
#include <map>
#include <string>
#include <vector>

struct CefMediaSourceInfo {
  uint32_t source_type = 0;
  std::string media_source;
  std::string media_format;
};

struct CefNativeMediaPlayerSurfaceInfo {
  std::string id;
  int32_t x = 0;
  int32_t y = 0;
  int32_t width = 0;
  int32_t height = 0;
};

struct CefCustomMediaInfo {
  std::string embed_id;
  uint32_t media_type = 0;
  std::vector<CefMediaSourceInfo> media_src_list;
  CefNativeMediaPlayerSurfaceInfo surface_info;
  bool controls = false;
  std::vector<std::string> controlslist;
  bool muted = false;
  std::string poster_url;
  uint32_t preload = 0;
  std::map<std::string, std::string> https_headers;
  std::map<std::string, std::string> attributes;
};

#endif  // CEF_INCLUDE_CEF_CUSTOM_MEDIA_INFO_H_
