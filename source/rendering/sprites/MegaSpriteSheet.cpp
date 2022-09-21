
#include <utils/gu_error.h>
#include "MegaSpriteSheet.h"
#include "../../game/Game.h"

MegaSpriteSheet::MegaSpriteSheet() : size(Game::settings.graphics.megaSpriteSheetSize)
{
    used.resize(getChunksPerRow() * getChunksPerRow());
    fbo = new FrameBuffer(size, size);
    fbo->addColorTexture(GL_RGBA8, GL_RGBA, GL_NEAREST, GL_NEAREST, GL_UNSIGNED_BYTE);
    fbo->addDepthBuffer();
}

void MegaSpriteSheet::add(const aseprite::Sprite &sprite)
{
    if (sprite.mode != aseprite::Mode::rgba)
    {
        throw gu_err("This game does not support aseprites that are NOT in RGBA mode.");
    }

    fbo->colorTexture->bind(0);

    ivec2 chunkSize(
        ceil(vec2(sprite.width, sprite.height) / float(CHUNK_SIZE))
    );

    SubSheet subSheet;

    for (auto &frame : sprite.frames)
    {
        uvec2 chunkOffset(0u);
        bool foundPlace = false;

        for (chunkOffset.y = 0u; chunkOffset.y < getChunksPerRow() - chunkSize.y; chunkOffset.y++)
        {
            for (chunkOffset.x = 0u; chunkOffset.x < getChunksPerRow() - chunkSize.x; chunkOffset.x++)
            {
                if (tryReserve(chunkOffset, chunkSize))
                {
                    foundPlace = true;
                    break;
                }
            }
            if (foundPlace)
                break;
        }
        if (!foundPlace)
            throw gu_err("MegaSpriteSheet is too small....");

        glTexSubImage2D(
            GL_TEXTURE_2D, 0, chunkOffset.x * CHUNK_SIZE, chunkOffset.y * CHUNK_SIZE, sprite.width, sprite.height,
            GL_RGBA, GL_UNSIGNED_BYTE, &frame.pixels[0]
        );
        subSheet.frameOffsets.push_back(chunkOffset * CHUNK_SIZE);
    }
    subSheets[&sprite] = subSheet;
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

const SharedModelSprite &MegaSpriteSheet::getModelSpriteByName(const std::string &name) const
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
        chunkSize(ceil(modelSprite->renderProperties.spriteSize * vec2(Game::settings.graphics.pixelsPerMeter) / float(CHUNK_SIZE)));

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

