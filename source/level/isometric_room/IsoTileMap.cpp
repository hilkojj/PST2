
#include "IsoTileMap.h"

#include <utils/gu_error.h>
#include <utils/base64.h>

#include <zlib.h>

#include <string.h>

IsoTileMap::IsoTileMap(const uvec3 &size) : size(size)
{
    if (size.x % CHUNK_WIDTH != 0 || size.z % CHUNK_WIDTH != 0)
    {
        throw gu_err("Tile map width (along x and z axes) must be dividable by CHUNK_WIDTH (" + std::to_string(CHUNK_WIDTH) + ")");
    }
    tiles.resize(size.x * size.y * size.z, IsoTile { IsoTileShape::empty, 0u });
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
    updatedTiles.emplace_back(x, y, z);
}

bool IsoTileMap::isValidPosition(uint x, uint y, uint z) const
{
    return x < size.x && y < size.y && z < size.z;
}

void IsoTileMap::nextFrame()
{
    updatedTiles.clear();
}

const std::vector<uvec3> &IsoTileMap::getUpdatedTiles() const
{
    return updatedTiles;
}

uint IsoTileMap::getTileVectorIndex(uint x, uint y, uint z) const
{
    return (size.x * size.z * y) + (size.x * z) + x;
}

void IsoTileMap::toBinary(std::vector<char> &out) const
{
    const auto tilesOffset = out.size();
    assert(tiles.size() == size.x * size.y * size.z);
    out.resize(tilesOffset + tiles.size() * sizeof(IsoTile));
    memcpy(&out[tilesOffset], tiles.data(), tiles.size() * sizeof(IsoTile));
}

void IsoTileMap::fromBinary(const char *data, int size)
{
    const auto numBytesToCopy = tiles.size() * sizeof(IsoTile);
    if (numBytesToCopy != size)
    {
        throw gu_err("Trying to load a IsoTileMap with invalid size!");
    }
    memcpy(tiles.data(), &data[0], numBytesToCopy);
}

void to_json(json &j, const IsoTile &t)
{
    j = json::array({ int(t.shape), int(t.rotation) });
}

void from_json(const json &j, IsoTile &t)
{
    t.shape = IsoTileShape(j[0].get<int>());
    t.rotation = j[1].get<int>();
}

void to_json(json &j, const IsoTileMap &map)
{
    j = map.tiles;
}

void from_json(const json &j, IsoTileMap &map)
{
    auto oldSize = map.tiles.size();
    map.tiles = j.get<std::vector<IsoTile>>();
    if (oldSize != map.tiles.size())
    {
        throw gu_err("Trying to load a IsoTileMap with invalid size!");
    }
}

const IsoTileMaterial &IsoTileMap::getMaterial(uint index)
{
    static std::vector<IsoTileMaterial> materials;
    if (index < materials.size())
    {
        return materials[index];
    }
    else
    {
        static bool fileLoaded = false;
        if (!fileLoaded)
        {
            materials = asset<json>("tile_materials")->get<decltype(materials)>();
            fileLoaded = true;
            return getMaterial(index);
        }
        throw gu_err("Tried getting material " + std::to_string(index));
    }
}
