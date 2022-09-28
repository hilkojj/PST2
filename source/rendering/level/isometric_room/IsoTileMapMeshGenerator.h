
#ifndef GAME_ISOTILEMAPMESHGENERATOR_H
#define GAME_ISOTILEMAPMESHGENERATOR_H

#include "../../../level/isometric_room/IsoTileMap.h"

#include <graphics/3d/mesh.h>

#include <set>
#include <functional>

class IsoTileMapMeshGenerator
{

  public:
    explicit IsoTileMapMeshGenerator(IsoTileMap *);

    void update();

    uint getNrOfChunksAlongXAxis() const;

    uint getNrOfChunksAlongZAxis() const;

    const SharedMesh &getMeshForChunk(uint x, uint z) const;

    /**
     * Calls the callback for each triangle belonging to the tile. If the callback returns false, the loop is stopped.
     */
    void loopThroughTileTris(const uvec3 &tilePos, const std::function<bool(const vec3 &a, const vec3 &b, const vec3 &c, const vec3 &normal)> &callback);

    void setPreviewTiles(const IsoTile *inPreviewTile, const uvec3 &inPreviewFrom, const uvec3 &inPreviewTo);

  private:

    void undirt();

    struct Chunk
    {
        struct TrisPerTile
        {
            std::set<uint> indices; // indices of the first vertex of each triangle.
        };

        Chunk(IsoTileMapMeshGenerator *, IsoTileMap *, const uvec3 &offset);

        void update(const uvec3 &from, const uvec3 &to);

        void updateTile(uint x, uint y, uint z);

        void uploadMesh();

        void setUpMesh();

        TrisPerTile &getTrisPerTile(uint x, uint y, uint z);

        void removeTri(uint index);

        void addTri(const uvec3 &tile, TrisPerTile &, vec3 a, vec3 b, vec3 c, uint8 rotation = 0u);

        void rotateTri(const uvec3 &tile, vec3 &a, vec3 &b, vec3 &c, uint8 rotation) const;

        IsoTileMapMeshGenerator *generator;
        IsoTileMap *map;

        const uvec3 offset;

        uint nrOfVerticesUsed = 0u;

        bool dirtyMesh = false;

        SharedMesh mesh;

        std::vector<TrisPerTile> trisPerTile;

        std::vector<uvec3> tilePerTri;  // stores for each triangle (NOT vertex) in the mesh, to which tile it belongs
    };

    friend Chunk;

    Chunk &getChunkForPosition(const uvec3 &);

    IsoTileMap *map;
    std::vector<Chunk> chunks;

    bool showPreviewTiles = false;
    IsoTile previewTile;
    uvec3 previewFrom = uvec3(0u), previewTo = uvec3(0u);
};


#endif //GAME_ISOTILEMAPMESHGENERATOR_H
