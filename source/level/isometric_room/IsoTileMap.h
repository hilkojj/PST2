
#ifndef GAME_ISOTILEMAP_H
#define GAME_ISOTILEMAP_H

#include <utils/math_utils.h>

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
};

class IsoTileMap
{

  public:

    const uvec3 size;

    IsoTileMap(const uvec3 &size);

    const IsoTile &getTile(uint x, uint y, uint z) const;

    void setTile(uint x, uint y, uint z, const IsoTile &);

    bool isValidPosition(uint x, uint y, uint z) const;

    /**
     * Exports this map to binary data, which will be APPENDED to `out`
     */
    void toBinary(std::vector<char> &out) const;

    /**
     * Loads a map from binary data.
     */
    void fromBinary(const char *data, int size);

  private:

    uint getTileVectorIndex(uint x, uint y, uint z) const;

    std::vector<IsoTile> tiles;

};


#endif //GAME_ISOTILEMAP_H
