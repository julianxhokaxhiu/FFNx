/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Cosmos                                             //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
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

#include "effect.h"

#include "../../globals.h"

namespace ff7::battle
{
    AuxiliaryEffectHandler::AuxiliaryEffectHandler()
    {
        this->isFirstTimeRunning = true;
        this->effectDecorator = std::make_shared<NoEffectDecorator>();
    }

    void NoEffectDecorator::callEffectFunction(uint32_t function)
    {
        ((void(*)())function)();
    }

    void OneCallEffectDecorator::callEffectFunction(uint32_t function)
    {
        if(this->frameCounter % this->frequency == 0)
        {
            ((void(*)())function)();
        }
        this->frameCounter++;
    }

    void PauseEffectDecorator::callEffectFunction(uint32_t function)
    {
        byte wasPaused = *this->isBattlePaused;
        if(this->frameCounter % this->frequency != 0)
        {
            *this->isBattlePaused = 1;
        }

        ((void(*)())function)();

        if(this->frameCounter % this->frequency != 0)
        {
            *this->isBattlePaused = wasPaused;
        }
        this->frameCounter++;
    }

    void FixCounterEffectDecorator::callEffectFunction(uint32_t function)
    {
        uint16_t currentEffectActive = *this->effectActive;
        short currentCounter = *this->effectCounter;
        if(this->frameCounter % this->frequency == 0)
        {
            ((void(*)())function)();
        }
        else
        {
            *this->isAddFunctionDisabled = true;
            ((void(*)())function)();
            *this->isAddFunctionDisabled = false;
            *this->effectCounter = currentCounter;
        }

        // Change active status of the function only at the last repeated frame
        if(this->frameCounter % this->frequency != this->frequency - 1)
        {
            *this->effectActive = currentEffectActive;
        }

        this->frameCounter++;
    }

    void FixCounterExceptionEffectDecorator::callEffectFunction(uint32_t function)
    {
        uint16_t currentEffectActive = *this->effectActive;
        short currentCounter = *this->effectCounter;

        if(this->frameCounter % this->frequency == 0)
        {
            ((void(*)())function)();
        }
        else
        {
            *this->isAddFunctionDisabled = true;
            if(!this->excludedFrames.contains(currentCounter)){
                ((void(*)())function)();
            }
            else
            {
                *this->effectCounter = *this->effectCounter + 1;
                ((void(*)())function)();
            }
            *this->effectCounter = currentCounter;
            *this->isAddFunctionDisabled = false;
        }

        // Change active status of the function only at the last repeated frame
        if(this->frameCounter % this->frequency != this->frequency - 1)
        {
            *this->effectActive = currentEffectActive;
        }

        this->frameCounter++;
    }

    InterpolationEffectDecorator::InterpolationEffectDecorator(int frequency, byte* isBattlePausedExt)
    {
        this->frameCounter = 0;
        this->frequency = frequency;
        this->isBattlePaused = isBattlePausedExt;
        this->textureCallIdx = 0;
    }

    uint64_t InterpolationEffectDecorator::getCantorHash(uint32_t x, uint32_t y)
    {
        return ((x + y) * (x + y + 1)) / 2 + y;
    }

    void InterpolationEffectDecorator::callEffectFunction(uint32_t function)
    {
        byte wasPaused = *this->isBattlePaused;
        this->textureCallIdx = 0;

        if(this->frameCounter % this->frequency == 0)
        {
            this->previousFrameDataMap.clear();
            this->_doInterpolation = false;
            ((void(*)())function)();
            this->textureNumCalls = this->textureCallIdx;
        }
        else
        {
            this->_doInterpolation = true;
            *this->isBattlePaused = 1;
            ((void(*)())function)();
            *this->isBattlePaused = wasPaused;
        }

        this->frameCounter++;
    }

    void InterpolationEffectDecorator::saveInterpolationData(interpolationable_data &&currData, uint32_t returnAddress)
    {
        uint64_t hash = this->getCantorHash(returnAddress, this->textureCallIdx);
        this->previousFrameDataMap[hash] = std::move(currData);
    }

    void InterpolationEffectDecorator::interpolateRotationMatrix(rotation_matrix* nextRotationMatrix, uint32_t returnAddress)
    {
        uint64_t hash = this->getCantorHash(returnAddress, this->textureCallIdx);
        if(this->previousFrameDataMap.contains(hash))
        {
            int interpolationStep = this->frameCounter % this->frequency;
            const rotation_matrix &previousMatrix = this->previousFrameDataMap[hash].rot_matrix;
            for(int i = 0; i < 3; i++)
                for(int j = 0; j < 3; j++)
                    nextRotationMatrix->r3_sub_matrix[i][j] = interpolateValue(previousMatrix.r3_sub_matrix[i][j],nextRotationMatrix->r3_sub_matrix[i][j], interpolationStep, this->frequency);

            for(int i = 0; i < 3; i++)
                nextRotationMatrix->position[i] = interpolateValue(previousMatrix.position[i], nextRotationMatrix->position[i], interpolationStep, this->frequency);
        }
    }

    void InterpolationEffectDecorator::interpolateMaterialContext(material_anim_ctx &nextMaterialCtx, uint32_t returnAddress)
    {
        uint64_t hash = this->getCantorHash(returnAddress, this->textureCallIdx);
        if(this->previousFrameDataMap.contains(hash))
        {
            int interpolationStep = this->frameCounter % this->frequency;
            const material_anim_ctx &previousMaterialCtx = this->previousFrameDataMap[hash].material_ctx;
            nextMaterialCtx.transparency = interpolateValue(previousMaterialCtx.transparency, nextMaterialCtx.transparency, interpolationStep, this->frequency);
            nextMaterialCtx.field_8 = interpolateValue(previousMaterialCtx.field_8, nextMaterialCtx.field_8, interpolationStep, this->frequency);
        }
    }

    void InterpolationEffectDecorator::interpolateColor(color_ui8 *nextColor, uint32_t returnAddress)
    {
        uint64_t hash = this->getCantorHash(returnAddress, this->textureCallIdx);
        if(this->previousFrameDataMap.contains(hash))
        {
            int interpolationStep = this->frameCounter % this->frequency;
            const color_ui8 previousColor = this->previousFrameDataMap[hash].color;
            nextColor->b = interpolateValue(previousColor.b, nextColor->b, interpolationStep, this->frequency);
            nextColor->g = interpolateValue(previousColor.g, nextColor->g, interpolationStep, this->frequency);
            nextColor->r = interpolateValue(previousColor.r, nextColor->r, interpolationStep, this->frequency);
            nextColor->a = interpolateValue(previousColor.a, nextColor->a, interpolationStep, this->frequency);
        }
    }

    void InterpolationEffectDecorator::interpolatePalette(palette_extra &nextPalette, uint32_t returnAddress)
    {
        uint64_t hash = this->getCantorHash(returnAddress, this->textureCallIdx);
        if(this->previousFrameDataMap.contains(hash))
        {
            int interpolationStep = this->frameCounter % this->frequency;
            const palette_extra &previousPalette = this->previousFrameDataMap[hash].palette;
            nextPalette.x_offset = interpolateValue(previousPalette.x_offset, nextPalette.x_offset, interpolationStep, this->frequency);
            nextPalette.y_offset = interpolateValue(previousPalette.y_offset, nextPalette.y_offset, interpolationStep, this->frequency);
            nextPalette.z_offset = interpolateValue(previousPalette.z_offset, nextPalette.z_offset, interpolationStep, this->frequency);
            nextPalette.field_24 = interpolateValue(previousPalette.field_24, nextPalette.field_24, interpolationStep, this->frequency);
            nextPalette.z_offset_2 = interpolateValue(previousPalette.z_offset_2, nextPalette.z_offset_2, interpolationStep, this->frequency);
            nextPalette.scroll_v = interpolateValue(previousPalette.scroll_v, nextPalette.scroll_v, interpolationStep, this->frequency);
            nextPalette.v_offset = interpolateValue(previousPalette.v_offset, nextPalette.v_offset, interpolationStep, this->frequency);
        }
    }

    bool ModelInterpolationEffectDecorator::isSmoothMovement(vector3<short> previous, vector3<short> next)
    {
        double distance = std::sqrt(std::pow(next.x - previous.x, 2) + std::pow(next.y - previous.y, 2) + std::pow(next.z - previous.z, 2));
        return distance < this->threshold;
    }

    void ModelInterpolationEffectDecorator::callEffectFunction(uint32_t function)
    {
        byte wasPaused = *this->isBattlePaused;
        uint16_t currentEffectActive = *this->effectActive;

        if(this->frameCounter == 0)
        {
            ((void(*)())function)();
            this->nextPosition = ff7_externals.g_battle_model_state[this->actorID].modelPosition;
        }
        else
        {
            int interpolationStep = this->frameCounter % this->frequency;
            if((this->frameCounter - 1) % this->frequency == 0)
            {
                this->previousPosition = this->nextPosition;
                ((void(*)())function)();
                this->nextPosition = ff7_externals.g_battle_model_state[this->actorID].modelPosition;

                if(isSmoothMovement(this->previousPosition, this->nextPosition))
                {
                    ff7_externals.g_battle_model_state[this->actorID].modelPosition.x = interpolateValue(this->previousPosition.x, this->nextPosition.x, interpolationStep, this->frequency);
                    ff7_externals.g_battle_model_state[this->actorID].modelPosition.y = interpolateValue(this->previousPosition.y, this->nextPosition.y, interpolationStep, this->frequency);
                    ff7_externals.g_battle_model_state[this->actorID].modelPosition.z = interpolateValue(this->previousPosition.z, this->nextPosition.z, interpolationStep, this->frequency);
                }
                else
                {
                    ff7_externals.g_battle_model_state[this->actorID].modelPosition = this->previousPosition;
                }

                if(*this->effectActive == (uint16_t)-1 && currentEffectActive != *this->effectActive)
                {
                    *this->effectActive = currentEffectActive;
                    this->finalFrame = this->frameCounter + this->frequency - 1;
                }
            }
            else if((this->frameCounter - 1) % this->frequency == this->frequency - 1)
            {
                if(usePauseTrick)
                {
                    *this->isBattlePaused = 1;
                    ((void(*)())function)();
                    *this->isBattlePaused = wasPaused;
                }

                ff7_externals.g_battle_model_state[this->actorID].modelPosition = this->nextPosition;
            }
            else
            {
                if(usePauseTrick)
                {
                    *this->isBattlePaused = 1;
                    ((void(*)())function)();
                    *this->isBattlePaused = wasPaused;
                }

                if(isSmoothMovement(this->previousPosition, this->nextPosition))
                {
                    ff7_externals.g_battle_model_state[this->actorID].modelPosition.x = interpolateValue(this->previousPosition.x, this->nextPosition.x, interpolationStep, this->frequency);
                    ff7_externals.g_battle_model_state[this->actorID].modelPosition.y = interpolateValue(this->previousPosition.y, this->nextPosition.y, interpolationStep, this->frequency);
                    ff7_externals.g_battle_model_state[this->actorID].modelPosition.z = interpolateValue(this->previousPosition.z, this->nextPosition.z, interpolationStep, this->frequency);
                }
                else
                {
                    ff7_externals.g_battle_model_state[this->actorID].modelPosition = this->previousPosition;
                }
            }
        }

        if(this->finalFrame == this->frameCounter)
        {
            *this->effectActive = (uint16_t)-1;
        }

        this->frameCounter++;
    }

    bool CameraInterpolationEffectDecorator::isSmoothMovement(vector3<short> previous, vector3<short> next)
    {
        double distance = std::sqrt(std::pow(next.x - previous.x, 2) + std::pow(next.y - previous.y, 2) + std::pow(next.z - previous.z, 2));
        return distance < this->threshold;
    }

    void CameraInterpolationEffectDecorator::callEffectFunction(uint32_t function)
    {
        byte wasPaused = *this->isBattlePaused;
        uint16_t currentEffectActive = *this->effectActive;

        if(this->frameCounter == 0)
        {
            ((void(*)())function)();
            this->nextCameraPosition = *ff7_externals.g_battle_camera_position;
            this->nextCameraFocalPoint = *ff7_externals.g_battle_camera_focal_point;
        }
        else
        {
            int interpolationStep = this->frameCounter % this->frequency;
            if((this->frameCounter - 1) % this->frequency == 0)
            {
                this->previousCameraPosition = this->nextCameraPosition;
                this->previousCameraFocalPoint = this->nextCameraFocalPoint;
                ((void(*)())function)();
                this->nextCameraPosition = *ff7_externals.g_battle_camera_position;
                this->nextCameraFocalPoint = *ff7_externals.g_battle_camera_focal_point;

                if(isSmoothMovement(this->previousCameraPosition, this->nextCameraPosition) && isSmoothMovement(this->previousCameraFocalPoint, this->nextCameraFocalPoint))
                {
                    ff7_externals.g_battle_camera_position->x = interpolateValue(this->previousCameraPosition.x, this->nextCameraPosition.x, interpolationStep, this->frequency);
                    ff7_externals.g_battle_camera_position->y = interpolateValue(this->previousCameraPosition.y, this->nextCameraPosition.y, interpolationStep, this->frequency);
                    ff7_externals.g_battle_camera_position->z = interpolateValue(this->previousCameraPosition.z, this->nextCameraPosition.z, interpolationStep, this->frequency);
                    ff7_externals.g_battle_camera_focal_point->x = interpolateValue(this->previousCameraFocalPoint.x, this->nextCameraFocalPoint.x, interpolationStep, this->frequency);
                    ff7_externals.g_battle_camera_focal_point->y = interpolateValue(this->previousCameraFocalPoint.y, this->nextCameraFocalPoint.y, interpolationStep, this->frequency);
                    ff7_externals.g_battle_camera_focal_point->z = interpolateValue(this->previousCameraFocalPoint.z, this->nextCameraFocalPoint.z, interpolationStep, this->frequency);
                }
                else
                {
                    *ff7_externals.g_battle_camera_position = this->previousCameraPosition;
                    *ff7_externals.g_battle_camera_focal_point = this->previousCameraFocalPoint;
                }

                if(*this->effectActive == (uint16_t)-1 && currentEffectActive != *this->effectActive)
                {
                    *this->effectActive = currentEffectActive;
                    this->finalFrame = this->frameCounter + this->frequency - 1;
                }
            }
            else if((this->frameCounter - 1) % this->frequency == this->frequency - 1)
            {
                *this->isBattlePaused = 1;
                ((void(*)())function)();
                *this->isBattlePaused = wasPaused;

                *ff7_externals.g_battle_camera_position = this->nextCameraPosition;
                *ff7_externals.g_battle_camera_focal_point = this->nextCameraFocalPoint;
            }
            else
            {
                *this->isBattlePaused = 1;
                ((void(*)())function)();
                *this->isBattlePaused = wasPaused;

                if(isSmoothMovement(this->previousCameraPosition, this->nextCameraPosition) && isSmoothMovement(this->previousCameraFocalPoint, this->nextCameraFocalPoint))
                {
                    ff7_externals.g_battle_camera_position->x = interpolateValue(this->previousCameraPosition.x, this->nextCameraPosition.x, interpolationStep, this->frequency);
                    ff7_externals.g_battle_camera_position->y = interpolateValue(this->previousCameraPosition.y, this->nextCameraPosition.y, interpolationStep, this->frequency);
                    ff7_externals.g_battle_camera_position->z = interpolateValue(this->previousCameraPosition.z, this->nextCameraPosition.z, interpolationStep, this->frequency);
                    ff7_externals.g_battle_camera_focal_point->x = interpolateValue(this->previousCameraFocalPoint.x, this->nextCameraFocalPoint.x, interpolationStep, this->frequency);
                    ff7_externals.g_battle_camera_focal_point->y = interpolateValue(this->previousCameraFocalPoint.y, this->nextCameraFocalPoint.y, interpolationStep, this->frequency);
                    ff7_externals.g_battle_camera_focal_point->z = interpolateValue(this->previousCameraFocalPoint.z, this->nextCameraFocalPoint.z, interpolationStep, this->frequency);
                }
                else
                {
                    *ff7_externals.g_battle_camera_position = this->previousCameraPosition;
                    *ff7_externals.g_battle_camera_focal_point = this->previousCameraFocalPoint;
                }
            }
        }

        if(this->finalFrame == this->frameCounter)
        {
            *this->effectActive = (uint16_t)-1;
        }

        this->frameCounter++;
    }
}