
#ifndef GAME_UISCREENWIDGETS_H
#define GAME_UISCREENWIDGETS_H

#include "../sprites/ModelSprite.h"

#include <utils/aseprite/Aseprite.h>

namespace UIScreenWidgets
{

    bool modelSpriteImageButton(const char *buttonID, const char *modelName, bool inset = false, bool flipX = false, float scale = 1.0f);

    bool modelSpriteImageButton(const char *buttonID, const SharedModelSprite &modelSprite, const ModelSprite::Orientation &orientation, bool inset = false, bool flipX = false, float scale = 1.0f);

    bool asepriteImageButton(const char *buttonID, const asset<aseprite::Sprite> &sprite, uint frame = 0u, bool inset = false, bool flipX = false, float scale = 1.0f);

    bool beginSecondaryMenuBar();

    void endSecondaryMenuBar();
};


#endif //GAME_UISCREENWIDGETS_H
