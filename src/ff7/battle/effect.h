/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
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

#include "../../ff7.h"

#include <set>
#include <unordered_map>
#include <memory>

namespace ff7::battle
{
    struct interpolationable_data
    {
        rotation_matrix rot_matrix;
        material_anim_ctx material_ctx;
        color_ui8 color;
        palette_extra palette;
    };

    class EffectDecorator
    {
    public:
        virtual void callEffectFunction(uint32_t function) = 0;
    };

    class NoEffectDecorator: public EffectDecorator
    {
    public:
        NoEffectDecorator() = default;
        void callEffectFunction(uint32_t function) override;
    };

    class OneCallEffectDecorator: public EffectDecorator
    {
    protected:
        int frameCounter;
        int frequency;
        OneCallEffectDecorator() = default;

    public:
        OneCallEffectDecorator(int frequency): frameCounter(0), frequency(frequency) {};
        void callEffectFunction(uint32_t function) override;
    };

    class PauseEffectDecorator: public EffectDecorator
    {
    protected:
        int frameCounter;
        int frequency;
        byte* isBattlePaused;
        PauseEffectDecorator() = default;

    public:
        PauseEffectDecorator(int frequency, byte* isBattlePausedExt): frameCounter(0), frequency(frequency), isBattlePaused(isBattlePausedExt) {};
        void callEffectFunction(uint32_t function) override;
    };

    class FixCounterEffectDecorator: public EffectDecorator
    {
    protected:
        int frameCounter;
        int frequency;
        uint16_t *effectActive;
        short *effectCounter;
        bool *isAddFunctionDisabled;
        FixCounterEffectDecorator() = default;

    public:
        FixCounterEffectDecorator(int frequency, uint16_t* effectActive, short* effectCounter, bool* isAddFunctionDisabled): frameCounter(0),
                                                                                                        frequency(frequency),
                                                                                                        effectActive(effectActive),
                                                                                                        effectCounter(effectCounter),
                                                                                                        isAddFunctionDisabled(isAddFunctionDisabled) {};
        void callEffectFunction(uint32_t function) override;
    };

    class FixCounterExceptionEffectDecorator: public FixCounterEffectDecorator
    {
    protected:
        std::set<short> excludedFrames;

    public:
        inline FixCounterExceptionEffectDecorator(int frequency, uint16_t* effectActive, short* effectCounter,
                                                bool* isAddFunctionDisabled, std::set<short> excludedFrames) : FixCounterEffectDecorator(frequency, effectActive, effectCounter, isAddFunctionDisabled)
        {
            this->excludedFrames = excludedFrames;
        }
        void callEffectFunction(uint32_t function) override;
    };

    class InterpolationEffectDecorator: public PauseEffectDecorator
    {
    protected:
        std::unordered_map<uint64_t, interpolationable_data> previousFrameDataMap;
        int textureCallIdx;
        int textureNumCalls;
        bool _doInterpolation;

    public:
        InterpolationEffectDecorator(int frequency, byte* isBattlePausedExt);
        void callEffectFunction(uint32_t function) override;
        uint64_t getCantorHash(uint32_t x, uint32_t y);

        inline bool doInterpolation(){return _doInterpolation;}
        inline void addTextureIndex(){textureCallIdx++;}
        inline int getTextureNumCalls(){return textureNumCalls;}

        void saveInterpolationData(interpolationable_data &&currData, uint32_t materialAddress);
        void interpolateRotationMatrix(rotation_matrix* nextRotationMatrix, uint32_t materialAddress);
        void interpolateMaterialContext(material_anim_ctx &nextMaterialCtx, uint32_t materialAddress);
        void interpolateColor(color_ui8 *color, uint32_t materialAddress);
        void interpolatePalette(palette_extra &paletteExtraData, uint32_t materialAddress);
    };

    class ModelInterpolationEffectDecorator: public PauseEffectDecorator
    {
    protected:
        uint16_t *effectActive;
        int actorID;
        int finalFrame = -1;
        bool usePauseTrick = true;
        int threshold = 1000;

        vector3<short> previousPosition;
        vector3<short> nextPosition;

        bool isSmoothMovement(vector3<short> previous, vector3<short> next);

    public:
        ModelInterpolationEffectDecorator(int frequency, byte* isBattlePausedExt, bool usePauseTrick, uint16_t* effectActive,
                                        int actorID) : PauseEffectDecorator(frequency, isBattlePausedExt), usePauseTrick(usePauseTrick),
                                                        effectActive(effectActive), actorID(actorID) {};

        ModelInterpolationEffectDecorator(int frequency, byte* isBattlePausedExt, bool usePauseTrick, uint16_t* effectActive,
                                        int actorID, int threshold) : PauseEffectDecorator(frequency, isBattlePausedExt), usePauseTrick(usePauseTrick),
                                                                        effectActive(effectActive), actorID(actorID), threshold(threshold) {};

        void callEffectFunction(uint32_t function) override;
    };

    class CameraInterpolationEffectDecorator: public PauseEffectDecorator
    {
    protected:
        uint16_t *effectActive;
        int finalFrame = -1;
        int threshold = 1500;

        vector3<short> previousCameraPosition;
        vector3<short> previousCameraFocalPoint;
        vector3<short> nextCameraPosition;
        vector3<short> nextCameraFocalPoint;

        bool isSmoothMovement(vector3<short> previous, vector3<short> next);

    public:
        CameraInterpolationEffectDecorator(int frequency, byte* isBattlePausedExt, uint16_t* effectActive): PauseEffectDecorator(frequency, isBattlePausedExt),
                                                                                                            effectActive(effectActive) {};

        CameraInterpolationEffectDecorator(int frequency, byte* isBattlePausedExt, uint16_t* effectActive, int threshold): PauseEffectDecorator(frequency, isBattlePausedExt),
                                                                                                                        effectActive(effectActive),
                                                                                                                        threshold(threshold) {};

        void callEffectFunction(uint32_t function) override;
    };

    class AuxiliaryEffectHandler
    {
    private:
        bool isFirstTimeRunning;
        std::shared_ptr<EffectDecorator> effectDecorator;

    public:
        AuxiliaryEffectHandler();

        inline std::shared_ptr<EffectDecorator> getEffectDecorator() {return effectDecorator;}
        inline bool isFirstFrame() {return isFirstTimeRunning;}

        inline void setEffectDecorator(std::shared_ptr<EffectDecorator> effectDecorator) {this->effectDecorator = std::move(effectDecorator);}
        inline void disableFirstFrame() {this->isFirstTimeRunning = false;}
        inline void executeEffectFunction(uint32_t effectFunction) {effectDecorator->callEffectFunction(effectFunction);}
    };

    template <typename T>
    T interpolateValue(T previous, T next, int step, int n_steps)
    {
        return previous + ((next - previous) * step) / n_steps;
    }
}