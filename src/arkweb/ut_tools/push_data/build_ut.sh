#!/bin/bash
cd ../../;
pwd;
echo -e "\033[32m 1. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ base_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ base_unittests
echo -e "\033[32m 2. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ mojo_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ mojo_unittests
echo -e "\033[32m 3. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ accessibility_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ accessibility_unittests
echo -e "\033[32m 4. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ perfetto_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ perfetto_unittests
echo -e "\033[32m 5. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ storage_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ storage_unittests
echo -e "\033[32m 6. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ ui_base_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ ui_base_unittests
echo -e "\033[32m 7. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ events_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ events_unittests
echo -e "\033[32m 8. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ device_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ device_unittests
echo -e "\033[32m 9. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ sql_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ sql_unittests
echo -e "\033[32m 10. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ liburlpattern_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ liburlpattern_unittests
echo -e "\033[32m 11. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ cast_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ cast_unittests
echo -e "\033[32m 12. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ url_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ url_unittests
echo -e "\033[32m 13. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ display_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ display_unittests
echo -e "\033[32m 14. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ libjingle_xmpp_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ libjingle_xmpp_unittests
echo -e "\033[32m 15. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ crypto_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ crypto_unittests
echo -e "\033[32m 16. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ midi_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ midi_unittests
echo -e "\033[32m 17. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ color_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ color_unittests
echo -e "\033[32m 18. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ ui_touch_selection_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ ui_touch_selection_unittests
echo -e "\033[32m 19. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ zlib_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ zlib_unittests
echo -e "\033[32m 20. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ courgette_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ courgette_unittests
echo -e "\033[32m 21. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ gwp_asan_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ gwp_asan_unittests
echo -e "\033[32m 22. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ gin_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ gin_unittests
echo -e "\033[32m 23. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ printing_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ printing_unittests
echo -e "\033[32m 24. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ filesystem_service_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ filesystem_service_unittests
echo -e "\033[32m 25. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ histogram_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ histogram_unittests
echo -e "\033[32m 26. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ ced_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ ced_unittests
echo -e "\033[32m 27. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ service_manager_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ service_manager_unittests
echo -e "\033[32m 28. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ latency_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ latency_unittests
echo -e "\033[32m 29. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ native_theme_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ native_theme_unittests
echo -e "\033[32m 30. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ shell_dialogs_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ shell_dialogs_unittests
echo -e "\033[32m 31. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ blink_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ blink_unittests
echo -e "\033[32m 32. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ blink_platform_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ blink_platform_unittests
echo -e "\033[32m 33. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ blink_common_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ blink_common_unittests
echo -e "\033[32m 34. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ blink_heap_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ blink_heap_unittests
echo -e "\033[32m 35. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ media_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ media_unittests
echo -e "\033[32m 36. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ media_mojo_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ media_mojo_unittests
echo -e "\033[32m 37. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ capture_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ capture_unittests
echo -e "\033[32m 38. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ audio_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ audio_unittests
echo -e "\033[32m 39. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ libjpeg_turbo_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ libjpeg_turbo_unittests
echo -e "\033[32m 40. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ media_learning_mojo_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ media_learning_mojo_unittests
echo -e "\033[32m 41. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ components:components_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ components:components_unittests
echo -e "\033[32m 42. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ gfx_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ gfx_unittests
echo -e "\033[32m 43. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ cc_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ cc_unittests
echo -e "\033[32m 44. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ gpu_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ gpu_unittests
echo -e "\033[32m 45. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ viz_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ viz_unittests
echo -e "\033[32m 46. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ compositor_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ compositor_unittests
echo -e "\033[32m 47. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ skia_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ skia_unittests
echo -e "\033[32m 48. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ gl_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ gl_unittests
echo -e "\033[32m 49. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ ohos_nweb_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ ohos_nweb_unittests
echo -e "\033[32m 50. building-------- ./third_party/depot_tools/ninja  -C out/musl_64/ libcef_static_unittests\033[0m"
./third_party/depot_tools/ninja -C out/musl_64/ libcef_static_unittests
