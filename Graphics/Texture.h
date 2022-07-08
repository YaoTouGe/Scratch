/**
 * @file Texture.h
 * @author wangyudong
 * @brief Texture object in graphics, 2d, 3d texture and cube map. TODO: should they be one kind of buffer object?
 * @version 0.1
 * @date 2022-06-07
 */

#pragma once

#include <memory>

namespace RenderSystem
{
    class Texture
    {
    public:
        typedef std::shared_ptr<Texture> SP;
    };
}