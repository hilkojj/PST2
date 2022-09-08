
#include "IsoTileMap.h"

#include <utils/gu_error.h>

#include <string.h>

IsoTileMap::IsoTileMap(const uvec3 &size) : size(size)
{
    tiles.resize(size.x * size.y * size.z, IsoTile { IsoTileShape::empty });
}

const IsoTile &IsoTileMap::getTile(uint x, uint y, uint z) const
{
    const uint index = getTileVectorIndex(x, y, z);
    if (index >= tiles.size())
    {
        throw gu_err("Tried to GET tile " + to_string(uvec3(x, y, z)) + " from a room with size " + to_string(size) +
                            ".\nTile vector size: " + std::to_string(tiles.size()));
    }
    return tiles[index];
}

void IsoTileMap::setTile(uint x, uint y, uint z, const IsoTile &tile)
{
    const uint index = getTileVectorIndex(x, y, z);
    if (index >= tiles.size())
    {
        throw gu_err("Tried to SET tile " + to_string(uvec3(x, y, z)) + " in a room with size " + to_string(size) +
                     ".\nTile vector size: " + std::to_string(tiles.size()));
    }
    tiles[index] = tile;
}

bool IsoTileMap::isValidPosition(uint x, uint y, uint z) const
{
    return x < size.x && y < size.y && z < size.z;
}

uint IsoTileMap::getTileVectorIndex(uint x, uint y, uint z) const
{
    return (size.x * size.z * y) + (size.x * z) + x;
}

void IsoTileMap::toBinary(std::vector<char> &out) const
{
    const ulong tilesOffset = out.size();
    assert(tiles.size() == size.x * size.y * size.z);
    out.resize(tilesOffset + tiles.size() * sizeof(IsoTile));
    memcpy(&out[tilesOffset], tiles.data(), tiles.size() * sizeof(IsoTile));
}

void IsoTileMap::fromBinary(const char *data, int size)
{
    const ulong numBytesToCopy = tiles.size() * sizeof(IsoTile);
    if (numBytesToCopy != size)
    {
        throw gu_err("Trying to load a IsoTileMap with invalid size!");
    }
    memcpy(tiles.data(), &data[0], numBytesToCopy);
}
