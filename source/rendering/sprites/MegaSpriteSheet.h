
#ifndef GAME_MEGASPRITESHEET_H
#define GAME_MEGASPRITESHEET_H

#include <utils/aseprite/Aseprite.h>
#include <graphics/frame_buffer.h>
#include <map>

#include "ModelSprite.h"

/**
 * a huge texture containing all sprites of the game
 */
class MegaSpriteSheet
{
  public:

    struct SubSheet
    {
        std::vector<uvec2> frameOffsets;
    };

    FrameBuffer *fbo = nullptr;

    const static uint CHUNK_SIZE = 8u;
    const uint size;

    MegaSpriteSheet();

    void add(const aseprite::Sprite &);

    ModelSprite::Orientation::Frame &addFrame(const std::string &modelSpriteName, SharedModelSprite &modelSprite, const std::string &animationName, float yaw, float pitch);

    const SubSheet &spriteInfo(const aseprite::Sprite &) const;

    const SharedModelSprite &getModelSpriteByName(const std::string &) const;

    float getUsage() const;

    void printUsage() const;

  private:

    std::vector<bool> used;

    std::map<const aseprite::Sprite *, SubSheet> subSheets;

    std::map<std::string, SharedModelSprite> modelSprites;

    const uint getChunksPerRow() const;

    bool tryReserve(const uvec2 &chunkOffset, const uvec2 &chunkSize);

};


#endif
