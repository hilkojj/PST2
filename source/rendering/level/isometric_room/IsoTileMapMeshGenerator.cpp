
#include "IsoTileMapMeshGenerator.h"
#include "../../../level/isometric_room/IsoTileMap.h"
#include "../../../game/Game.h"

#include <gu/profiler.h>
#include <graphics/3d/vert_buffer.h>

#include <string.h>

IsoTileMapMeshGenerator::IsoTileMapMeshGenerator(IsoTileMap *map) : map(map)
{
    for (int z = 0; z < getNrOfChunksAlongZAxis(); z++)
    {
        for (int x = 0; x < getNrOfChunksAlongXAxis(); x++)
        {
            chunks.emplace_back(this, map, uvec3(x * IsoTileMap::CHUNK_WIDTH, 0u, z * IsoTileMap::CHUNK_WIDTH));
            chunks.back().update(
                chunks.back().offset,
                chunks.back().offset + uvec3(IsoTileMap::CHUNK_WIDTH - 1u, map->size.y - 1u, IsoTileMap::CHUNK_WIDTH - 1u)
            );
        }
    }
}

void IsoTileMapMeshGenerator::update()
{
    gu::profiler::Zone zone("IsoTileMapMeshGenerator::update");
    for (const uvec3 &pos : map->getUpdatedTiles())
    {
        // todo, if updated tiles are neighbouring (often the case) then this is inefficient:
        for (uint x = pos.x == 0u ? 0u : pos.x - 1u; x < pos.x + 1u; x++)
        {
            for (uint y = pos.y == 0u ? 0u : pos.y - 1u; y < pos.y + 1u; y++)
            {
                for (uint z = pos.z == 0u ? 0u : pos.z - 1u; z < pos.z + 1u; z++)
                {
                    if (map->isValidPosition(x, y, z))
                    {
                        getChunkForPosition(uvec3(x, y, z)).updateTile(x, y, z);
                    }
                }
            }
        }
    }
    undirt();
}

IsoTileMapMeshGenerator::Chunk &IsoTileMapMeshGenerator::getChunkForPosition(const uvec3 &pos)
{
    uint x = pos.x / IsoTileMap::CHUNK_WIDTH;
    uint z = pos.z / IsoTileMap::CHUNK_WIDTH;

    return chunks[x + z * getNrOfChunksAlongZAxis()];
}

uint IsoTileMapMeshGenerator::getNrOfChunksAlongXAxis() const
{
    return map->size.x / IsoTileMap::CHUNK_WIDTH;
}

uint IsoTileMapMeshGenerator::getNrOfChunksAlongZAxis() const
{
    return map->size.z / IsoTileMap::CHUNK_WIDTH;
}

const SharedMesh &IsoTileMapMeshGenerator::getMeshForChunk(uint x, uint z) const
{
    return chunks[x + z * getNrOfChunksAlongZAxis()].mesh;
}

void IsoTileMapMeshGenerator::loopThroughTileTris(const uvec3 &tilePos, const std::function<bool(const vec3 &a, const vec3 &b, const vec3 &c, const vec3 &normal)> &callback)
{
    if (!map->isValidPosition(tilePos.x, tilePos.y, tilePos.z))
    {
        return;
    }
    // todo, make this function const?
    Chunk &chunk = getChunkForPosition(tilePos);
    uvec3 tilePosChunkRelative = tilePos - chunk.offset;
    Chunk::TrisPerTile &tris = chunk.getTrisPerTile(tilePosChunkRelative.x, tilePosChunkRelative.y, tilePosChunkRelative.z);

    for (int firstTriVertexIndex : tris.indices)
    {
        bool continueLoop = callback(
            chunk.mesh->getRef<vec3>(firstTriVertexIndex, 0),
            chunk.mesh->getRef<vec3>(firstTriVertexIndex + 1, 0),
            chunk.mesh->getRef<vec3>(firstTriVertexIndex + 2, 0),
            chunk.mesh->getRef<vec3>(firstTriVertexIndex + 2, sizeof(vec3)) // normal
        );
        if (!continueLoop)
        {
            break;
        }
    }
}

void IsoTileMapMeshGenerator::setPreviewTiles(const IsoTile *inPreviewTile, const uvec3 &inPreviewFrom, const uvec3 &inPreviewTo)
{
    uvec3 updatePreviewFrom(min(inPreviewFrom.x, previewFrom.x), min(inPreviewFrom.y, previewFrom.y), min(inPreviewFrom.z, previewFrom.z));
    uvec3 updatePreviewTo(max(inPreviewTo.x, previewTo.x), max(inPreviewTo.y, previewTo.y), max(inPreviewTo.z, previewTo.z));
    if (!inPreviewTile)
    {
        showPreviewTiles = false;
    }
    else
    {
        if (showPreviewTiles && previewTile == *inPreviewTile && previewFrom == inPreviewFrom && previewTo == inPreviewTo)
        {
            return;
        }

        showPreviewTiles = true;
        previewTile = *inPreviewTile;
        previewFrom = inPreviewFrom;
        previewTo = inPreviewTo;
    }
    for (uint x = updatePreviewFrom.x == 0u ? 0u : updatePreviewFrom.x - 1u; x < updatePreviewTo.x + 1u; x++)
    {
        for (uint y = updatePreviewFrom.y == 0u ? 0u : updatePreviewFrom.y - 1u; y < updatePreviewTo.y + 1u; y++)
        {
            for (uint z = updatePreviewFrom.z == 0u ? 0u : updatePreviewFrom.z - 1u; z < updatePreviewTo.z + 1u; z++)
            {
                if (map->isValidPosition(x, y, z))
                {
                    getChunkForPosition(uvec3(x, y, z)).updateTile(x, y, z);
                }
            }
        }
    }
    undirt();
}

void IsoTileMapMeshGenerator::undirt()
{
    for (auto &chunk : chunks)
    {
        if (chunk.dirtyMesh)
        {
            chunk.uploadMesh();
        }
    }
}

#define VERTS_PER_SMALLEST_VERT_BUFFER (IsoTileMap::CHUNK_WIDTH * IsoTileMap::CHUNK_WIDTH * 30 * 2)

IsoTileMapMeshGenerator::Chunk::Chunk(IsoTileMapMeshGenerator *generator, IsoTileMap *map, const uvec3 &offset)
    : generator(generator), map(map), offset(offset)
{
    assert(offset.y == 0u);
    trisPerTile.resize(IsoTileMap::CHUNK_WIDTH * IsoTileMap::CHUNK_WIDTH * map->size.y);
    setUpMesh();
}

void IsoTileMapMeshGenerator::Chunk::update(const uvec3 &from, const uvec3 &to)
{
    for (uint x = from.x; x <= to.x; x++)
    {
        for (uint y = from.y; y <= to.y; y++)
        {
            for (uint z = from.z; z <= to.z; z++)
            {
                updateTile(x, y, z);
            }
        }
    }
}

vec2 currentSpriteSheetOffset(0);

void IsoTileMapMeshGenerator::Chunk::updateTile(uint x, uint y, uint z)
{
    dirtyMesh = true;

    uvec3 tilePosChunkRelative = uvec3(x, y, z) - offset;
    vec3 tilePos(x, y * IsoTileMap::TILE_HEIGHT, z);
    uvec3 tilePosU(x, y, z);

    bool inPreviewArea = generator->showPreviewTiles && all(greaterThanEqual(tilePosU, generator->previewFrom)) && all(lessThanEqual(tilePosU, generator->previewTo));

    auto &tile = inPreviewArea ? generator->previewTile : map->getTile(x, y, z);
    const IsoTileMaterial &material = IsoTileMap::getMaterial(tile.material);
    const aseprite::Sprite &tileset = material.tileset.get();
    currentSpriteSheetOffset = Game::spriteSheet->spriteInfo(tileset).frameOffsets.at(
        mu::randomIntFromX(float(x + (y * map->size.x) + (z * map->size.x * map->size.y)), tileset.frameCount)
    );

    auto &trisForThisTile = getTrisPerTile(x - offset.x, y - offset.y, z - offset.z);

    while (!trisForThisTile.indices.empty())
    {
        removeTri(*trisForThisTile.indices.begin());
    }

    switch (tile.shape) {
        case IsoTileShape::empty:
            break;
        case IsoTileShape::full:

            // y+
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 0),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 0)
            );
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 0)
            );

            // x-
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, 0, 0),
                tilePos + vec3(0, 0, 1),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 0)
            );
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, 0, 1),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 0)
            );

            // z-
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, 0, 0),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 0),
                tilePos + vec3(1, 0, 0)
            );
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 0),
                tilePos + vec3(1, 0, 0),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 0)
            );

            // x+
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(1, 0, 0),
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 0),
                tilePos + vec3(1, 0, 1)
            );
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(1, 0, 1),
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 0),
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 1)
            );

            // z+
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, 0, 1),
                tilePos + vec3(1, 0, 1),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 1)
            );
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(1, 0, 1)
            );
            break;
        case IsoTileShape::slope:
            // y+
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, 0, 0),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(1, 0, 0),
                tile.rotation
            );
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(1, 0, 0),
                tile.rotation
            );

            // x-
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, 0, 1),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(0, 0, 0),
                tile.rotation
            );

            // x+
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(1, 0, 1),
                tilePos + vec3(1, 0, 0),
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 1),
                tile.rotation
            );

            // z+
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, 0, 1),
                tilePos + vec3(1, 0, 1),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 1),
                tile.rotation
            );
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(0, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(1, 0, 1),
                tile.rotation
            );
            break;
        case IsoTileShape::slope_corner:
            // y+
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, 0, 0),
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(1, 0, 0),
                tile.rotation
            );
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(0, 0, 0),
                tilePos + vec3(0, 0, 1),
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 1),
                tile.rotation
            );

            // x+
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(1, 0, 1),
                tilePos + vec3(1, 0, 0),
                tile.rotation
            );

            // z+
            addTri(tilePosChunkRelative, trisForThisTile,
                tilePos + vec3(1, IsoTileMap::TILE_HEIGHT, 1),
                tilePos + vec3(0, 0, 1),
                tilePos + vec3(1, 0, 1),
                tile.rotation
            );
            break;
    }
}

void IsoTileMapMeshGenerator::Chunk::uploadMesh()
{
    dirtyMesh = false;

    if (mesh->vertBuffer)
    {
        if (mesh->getNrOfVertsReservedInVertBuffer() < mesh->nrOfVertices())
        {
            SharedMesh oldMesh = mesh;
            setUpMesh();
            mesh->vertices = oldMesh->vertices;
            uploadMesh();
            return;
        }
        mesh->vertBuffer->reuploadVertices(mesh, mesh->nrOfVertices());
    }
    else
    {
        auto vertBuffer = VertBuffer::with(mesh->attributes);
        vertBuffer->vboUsage = GL_DYNAMIC_DRAW;

        auto meshNumBytes = mesh->vertices.size();

        mesh->vertices.resize((mesh->nrOfVertices() / VERTS_PER_SMALLEST_VERT_BUFFER + 1) * VERTS_PER_SMALLEST_VERT_BUFFER * mesh->attributes.getVertSize());

        vertBuffer->add(mesh);
        vertBuffer->upload(false);

        mesh->vertices.resize(meshNumBytes);
    }
}

void IsoTileMapMeshGenerator::Chunk::setUpMesh()
{
    VertAttributes attributes;
    attributes.add({
        "POSITION",
        3
    });
    attributes.add({
        "NORMAL",
        3
    });
    attributes.add({
        "SPRITE_SHEET_OFFSET",
        2
    });
    mesh = std::make_shared<Mesh>("IsoTileMapMesh_" + to_string(offset), 0, attributes);
}

IsoTileMapMeshGenerator::Chunk::TrisPerTile &IsoTileMapMeshGenerator::Chunk::getTrisPerTile(uint x, uint y, uint z)
{
    return trisPerTile.at(y * (IsoTileMap::CHUNK_WIDTH * IsoTileMap::CHUNK_WIDTH) + z * IsoTileMap::CHUNK_WIDTH + x);
}

void IsoTileMapMeshGenerator::Chunk::removeTri(uint index)
{
    const auto vertSize = mesh->attributes.getVertSize();

    memcpy(&mesh->vertices.at(vertSize * index), &mesh->vertices.at((mesh->nrOfVertices() - 3) * vertSize), vertSize * 3);
    mesh->removeVertices(3);

    auto &removedOwner = tilePerTri.at(index / 3);

    assert(getTrisPerTile(removedOwner.x, removedOwner.y, removedOwner.z).indices.erase(index) == 1);

    auto &movedOwner = tilePerTri.back();
    tilePerTri[index / 3] = movedOwner;

    auto &movedOwnerIndices = getTrisPerTile(movedOwner.x, movedOwner.y, movedOwner.z).indices;

    int movedTriangleVertexIndex = (tilePerTri.size() - 1) * 3;
    movedOwnerIndices.erase(movedTriangleVertexIndex);
    if (movedTriangleVertexIndex != index)
    {
        movedOwnerIndices.emplace(index);
    }
    tilePerTri.pop_back();
}

void IsoTileMapMeshGenerator::Chunk::rotateTri(const uvec3 &tile, vec3 &a, vec3 &b, vec3 &c, uint8 rotation) const
{
    if (rotation == 0u)
    {
        return;
    }

    // todo, this function can be optimized by using (x,y)->(−y,x)

    vec3 center = vec3(tile + offset) + vec3(0.5f, 0.0f, 0.5f);
    a -= center;
    a = rotate(a, 90 * mu::DEGREES_TO_RAD * rotation, mu::Y);
    a += center;

    b -= center;
    b = rotate(b, 90 * mu::DEGREES_TO_RAD * rotation, mu::Y);
    b += center;

    c -= center;
    c = rotate(c, 90 * mu::DEGREES_TO_RAD * rotation, mu::Y);
    c += center;
}

void IsoTileMapMeshGenerator::Chunk::addTri(const uvec3 &tile, IsoTileMapMeshGenerator::Chunk::TrisPerTile &trisForThisTile,
                                       vec3 a, vec3 b, vec3 c, uint8 rotation)
{
    rotateTri(tile, a, b, c, rotation);

    tilePerTri.push_back(tile);
    int firstVertexIndex = mesh->nrOfVertices();
    trisForThisTile.indices.emplace(firstVertexIndex);
    mesh->addVertices(3);

    vec3 normal = mu::calculateNormal(a, b, c);

    mesh->set<vec3>(a, firstVertexIndex, 0);
    mesh->set<vec3>(normal, firstVertexIndex, sizeof(vec3));
    mesh->set<vec2>(currentSpriteSheetOffset, firstVertexIndex, sizeof(vec3) * 2);

    mesh->set<vec3>(b, firstVertexIndex + 1, 0);
    mesh->set<vec3>(normal, firstVertexIndex + 1, sizeof(vec3));
    mesh->set<vec2>(currentSpriteSheetOffset, firstVertexIndex + 1, sizeof(vec3) * 2);

    mesh->set<vec3>(c, firstVertexIndex + 2, 0);
    mesh->set<vec3>(normal, firstVertexIndex + 2, sizeof(vec3));
    mesh->set<vec2>(currentSpriteSheetOffset, firstVertexIndex + 2, sizeof(vec3) * 2);
}
