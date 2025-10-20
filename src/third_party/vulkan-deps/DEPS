# This file is used to manage Vulkan dependencies for several repos. It is
# used by gclient to determine what version of each dependency to check out, and
# where.

# Avoids the need for a custom root variable.
use_relative_paths = True
git_dependencies = 'SYNC'

vars = {
  'chromium_git': 'https://chromium.googlesource.com',

  # Current revision of glslang, the Khronos SPIRV compiler.
  'glslang_revision': '3289b1d61b69a6c66c4b7cd2c6d3ab2a6df031e5',

  # Current revision of Lunarg VulkanTools
  'lunarg_vulkantools_revision': 'a906345b8a7bccc416b006b2048e13f40d9b2327',

  # Current revision of spirv-cross, the Khronos SPIRV cross compiler.
  'spirv_cross_revision': 'b8fcf307f1f347089e3c46eb4451d27f32ebc8d3',

  # Current revision fo the SPIRV-Headers Vulkan support library.
  'spirv_headers_revision': '3397e1e4fe0a9964e1837c2934b81835093494b8',

  # Current revision of SPIRV-Tools for Vulkan.
  'spirv_tools_revision': '392b4893c4955125c1873c33a97f2a8ee8363bd3',

  # Current revision of Khronos Vulkan-Headers.
  'vulkan_headers_revision': 'd1cd37e925510a167d4abef39340dbdea47d8989',

  # Current revision of Khronos Vulkan-Loader.
  'vulkan_loader_revision': 'fe92c7d7e54664b1d3f3a0d734fd6f2ffd92e485',

  # Current revision of Khronos Vulkan-Tools.
  'vulkan_tools_revision': '8ce6f121d1fcbdf60f0f4264e23fbcd247b9101d',

  # Current revision of Khronos Vulkan-Utility-Libraries.
  'vulkan_utility_libraries_revision': 'a528f95dc2f92bdd83c0c32efe2d13c806428c9d',

  # Current revision of Khronos Vulkan-ValidationLayers.
  'vulkan_validation_revision': '88a897d5921f737c9826fdd4db1ae2010d23dbb3',
}

deps = {
  'glslang/src': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/glslang@{glslang_revision}',
  },

  'lunarg-vulkantools/src': {
    'url': '{chromium_git}/external/github.com/LunarG/VulkanTools@{lunarg_vulkantools_revision}',
  },

  'spirv-cross/src': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Cross@{spirv_cross_revision}',
  },

  'spirv-headers/src': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Headers@{spirv_headers_revision}',
  },

  'spirv-tools/src': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Tools@{spirv_tools_revision}',
  },

  'vulkan-headers/src': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Headers@{vulkan_headers_revision}',
  },

  'vulkan-loader/src': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Loader@{vulkan_loader_revision}',
  },

  'vulkan-tools/src': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Tools@{vulkan_tools_revision}',
  },

  'vulkan-utility-libraries/src': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Utility-Libraries@{vulkan_utility_libraries_revision}',
  },

  'vulkan-validation-layers/src': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-ValidationLayers@{vulkan_validation_revision}',
  },
}
