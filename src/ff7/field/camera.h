/****************************************************************************/
//    Copyright (C) 2022 Cosmos                                             //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#pragma once

#include "matrix.h"

namespace ff7::field
{
    class Camera
    {
        public:
            Camera() = default;
            ~Camera() = default;

            void setScrollingDir(float x, float y);
            float getScrollingDirX();
            float getScrollingDirY();

            void setScrollingOffset(float x, float y);
            float getScrollingOffsetX();
            float getScrollingOffsetY();

        private:
            vector2<float> scrollingDir = { 0.0, 0.0 };
            vector2<float> scrollingOffset = { 0.0, 0.0 };
    };

    inline float Camera::getScrollingDirX()
    {
        return scrollingDir.x;
    }

    inline float Camera::getScrollingDirY()
    {
        return scrollingDir.y;
    }

    inline float Camera::getScrollingOffsetX()
    {
        return scrollingOffset.x;
    }

    inline float Camera::getScrollingOffsetY()
    {
        return scrollingOffset.y;
    }

    extern Camera camera;
}
