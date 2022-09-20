
#ifndef GAME_ISOTILEMAP_H
#define GAME_ISOTILEMAP_H

#include "../../generated/IsoTileMaterial.hpp"

#include <utils/math_utils.h>

#include <json.hpp>

#include <vector>

enum class IsoTileShape : unsigned char
{
    empty,
    full,
    slope,
    slope_corner
};

// An array of this struct will be copied directly to/from a binary file format.
struct IsoTile
{
    IsoTileShape shape;
    uint rotation : 2;
    uint material : 6;
};

void to_json(json &, const IsoTile &);

void from_json(const json &, IsoTile &);

class IsoTileMap
{

  public:

    static const uint CHUNK_WIDTH = 32u;
    constexpr static const float TILE_HEIGHT = 0.5f;

    const uvec3 size;

    IsoTileMap(const uvec3 &size);

    const IsoTile &getTile(uint x, uint y, uint z) const;

    void setTile(uint x, uint y, uint z, const IsoTile &);

    bool isValidPosition(uint x, uint y, uint z) const;

    void nextFrame();

    const std::vector<uvec3> &getUpdatedTiles() const;

    /**
     * Exports this map to binary data.
     */
    void toBinary(std::vector<char> &out) const;

    /**
     * Loads a map from binary data.
     */
    void fromBinary(const char *data, int size);

    static const struct IsoTileMaterial &getMaterial(uint index);

  private:

    uint getTileVectorIndex(uint x, uint y, uint z) const;

    std::vector<IsoTile> tiles;

    std::vector<uvec3> updatedTiles;

    friend void to_json(json &, const IsoTileMap &);

    friend void from_json(const json &, IsoTileMap &);

};

void to_json(json &, const IsoTileMap &);

void from_json(const json &, IsoTileMap &);


#endif //GAME_ISOTILEMAP_H
