/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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
private:
    int frameCounter;
    int frequency;

public:
    OneCallEffectDecorator(int frequency): frameCounter(0), frequency(frequency) {};
    void callEffectFunction(uint32_t function) override;
};

class PauseEffectDecorator: public EffectDecorator
{
private:
    int frameCounter;
    int frequency;
    byte* isBattlePaused;

public:
    PauseEffectDecorator(int frequency, byte* isBattlePausedExt): frameCounter(0), frequency(frequency), isBattlePaused(isBattlePausedExt) {};
    void callEffectFunction(uint32_t function) override;
};

class FixCounterEffectDecorator: public EffectDecorator
{
private:
    int frameCounter;
    int frequency;
    uint16_t *effectCounter;
    bool *isAddFunctionDisabled;

public:
    FixCounterEffectDecorator(int frequency, uint16_t* effectCounter, bool* isAddFunctionDisabled): frameCounter(0),
                                                                                                    frequency(frequency),
                                                                                                    effectCounter(effectCounter),
                                                                                                    isAddFunctionDisabled(isAddFunctionDisabled) {};
    void callEffectFunction(uint32_t function) override;
};

class InterpolationEffectDecorator: public EffectDecorator
{
private:
    int frameCounter;
    int frequency;
    byte* isBattlePaused;
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

