
#ifndef GAME_UISCREENWIDGETS_H
#define GAME_UISCREENWIDGETS_H

#include "../sprites/ModelSprite.h"

namespace UIScreenWidgets
{

    bool modelSpriteImageButton(const char *buttonID, const char *modelName, bool inset = false, bool flipX = false);

    bool modelSpriteImageButton(const char *buttonID, const SharedModelSprite &modelSprite, const ModelSprite::Orientation &orientation, bool inset = false, bool flipX = false);

};


#endif //GAME_UISCREENWIDGETS_H
