#pragma once

#include <memory>
#include <string>

#include "OffscreenContext.h"

namespace offscreen_old {

std::shared_ptr<OffscreenContext> CreateOffscreenContextEGL(
    unsigned int width, unsigned int height, unsigned int majorGLVersion, 
    unsigned int minorGLVersion, bool gles, bool compatibilityProfile,
    const std::string& drmNode = "");

}  // namespace offscreen_old
