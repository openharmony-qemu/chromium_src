
#ifndef OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_MEDIA_INFO_IMPL_H_
#define OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_MEDIA_INFO_IMPL_H_

#include "cef/include/cef_custom_media_info.h"
#include "ohos_nweb/include/nweb_native_media_player.h"

namespace OHOS::NWeb {

class NWebMediaInfoImpl : public NWebMediaInfo {
 public:
  NWebMediaInfoImpl(const CefCustomMediaInfo& media_info);
  virtual ~NWebMediaInfoImpl() override;

  Preload GetPreload() override;
  bool GetIsMuted() override;
  std::string GetEmbedId() override;
  std::string GetPosterUrl() override;
  MediaType GetMediaType() override;
  bool GetIsControlsShown() override;
  std::vector<std::string> GetControls() override;
  std::map<std::string, std::string> GetHeaders() override;
  std::map<std::string, std::string> GetAttributes() override;
  std::vector<std::shared_ptr<NWebMediaSourceInfo>> GetSourceInfos() override;
  std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> GetSurfaceInfo() override;

 private:
  CefCustomMediaInfo media_info_;
};

} // namespace

#endif // OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_MEDIA_INFO_IMPL_H_
