
#include <utils/gu_error.h>
#include "MegaSpriteSheet.h"
#include "../../game/Game.h"

MegaSpriteSheet::MegaSpriteSheet() : size(Game::settings.graphics.megaSpriteSheetSize)
{
    used.resize(getChunksPerRow() * getChunksPerRow());
    fbo = new FrameBuffer(size, size);
    fbo->addColorTexture(GL_RGB8, GL_RGB, GL_NEAREST, GL_NEAREST, GL_UNSIGNED_BYTE);
    fbo->addDepthBuffer();
}

void MegaSpriteSheet::add(const aseprite::Sprite &sprite)
{
    // todo, reimplement.
}

bool MegaSpriteSheet::tryReserve(const ivec2 &chunkOffset, const ivec2 &chunkSize)
{
    for (int x = chunkOffset.x; x < chunkOffset.x + chunkSize.x; x++)
        for (int y = chunkOffset.y; y <= chunkOffset.y + chunkSize.y; y++)
            if (used[x + y * getChunksPerRow()])
                return false;
    for (int x = chunkOffset.x; x < chunkOffset.x + chunkSize.x; x++)
        for (int y = chunkOffset.y; y <= chunkOffset.y + chunkSize.y; y++)
            used[x + y * getChunksPerRow()] = true;

    return true;
}

const MegaSpriteSheet::SubSheet &MegaSpriteSheet::spriteInfo(const aseprite::Sprite &s) const
{
    return subSheets.at(&s);
}

float MegaSpriteSheet::getUsage() const
{
    float usedChunks = 0;
    for (auto u : used)
        if (u)
            usedChunks += 1.0f;
    return usedChunks / float(used.size());
}

void MegaSpriteSheet::printUsage() const
{
    int usage = round(100.f * getUsage());
    std::cout << usage << "% used of MegaSpriteSheet\n";
}

SharedModelSprite MegaSpriteSheet::getModelSpriteByName(const std::string &name) const
{
    auto it = modelSprites.find(name);
    if (it == modelSprites.end())
    {
        throw gu_err("Model sprite named " + name + " not found!");
    }
    return it->second;
}

ModelSprite::Orientation::Frame &MegaSpriteSheet::addFrame(const std::string &modelSpriteName, SharedModelSprite &modelSprite,
                               const std::string &animationName, float yaw, float pitch)
{
    modelSprites[modelSpriteName] = modelSprite;

    auto &frame = modelSprite->getClosestOrientation(yaw, pitch).animations[animationName].emplace_back();

    uvec2
        chunkOffset(0u, 0u),
        chunkSize(ceil(vec2(modelSprite->renderProperties.spriteSize * Game::settings.graphics.pixelsPerMeter) / float(CHUNK_SIZE)));

    for (chunkOffset.y = 0u; chunkOffset.y < getChunksPerRow() - chunkSize.y; chunkOffset.y++)
    {
        for (chunkOffset.x = 0u; chunkOffset.x < getChunksPerRow() - chunkSize.x; chunkOffset.x++)
        {
            if (tryReserve(chunkOffset, chunkSize))
            {
                frame.spriteSheetOffset = chunkOffset * CHUNK_SIZE;
                return frame;
            }
        }
    }
    throw gu_err("MegaSpriteSheet is full! Try increasing it's size in settings.json!");
    return frame;
}

const uint MegaSpriteSheet::getChunksPerRow() const
{
    return size / CHUNK_SIZE;
}

