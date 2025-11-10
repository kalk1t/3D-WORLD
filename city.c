
#include "city.h"

static float frand01(unsigned int *state) {
    // simple xorshift
    unsigned int x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return (x / (float)UINT_MAX);
}

static float lerpf(float a, float b, float t) { return a + (b - a)*t; }

void GenerateCity(City* outCity, CityConfig cfg)
{
    *outCity = (City){0};
    outCity->cfg = cfg;

    unsigned int rng = cfg.seed ? cfg.seed : 1234567u;

    // Precompute counts (rough estimates)
    int lotsPerBlock = 6; // 6x6 small building lots per block
    int totalLots = cfg.blocksX * cfg.blocksZ * lotsPerBlock * lotsPerBlock;

    // Allocate rough upper bounds; will fill and track counts
    outCity->buildings = (Building*)MemAlloc(sizeof(Building) * totalLots);
    outCity->roads = (Road*)MemAlloc(sizeof(Road) * (cfg.blocksX + cfg.blocksZ) * 6); // generous
    outCity->sidewalks = (Sidewalk*)MemAlloc(sizeof(Sidewalk) * (cfg.blocksX + cfg.blocksZ) * 12);
    outCity->parks = (Park*)MemAlloc(sizeof(Park) * (totalLots/4 + 8));
    outCity->marks = (Mark*)MemAlloc(sizeof(Mark) * (cfg.blocksX + cfg.blocksZ) * 200);

    outCity->buildingCount = outCity->roadCount = outCity->sidewalkCount = outCity->parkCount = outCity->markCount = 0;

    float cityW = cfg.blocksX * cfg.blockSize;
    float cityH = cfg.blocksZ * cfg.blockSize;
    float curbH = 0.25f;

    // Generate grid roads (X and Z axes)
    // Horizontal roads along Z
    for (int bz = 0; bz <= cfg.blocksZ; bz++) {
        float z = bz * cfg.blockSize;
        Vector3 size = (Vector3){ cityW + cfg.roadWidth, 0.1f, cfg.roadWidth };
        Vector3 pos  = (Vector3){ cityW*0.5f, 0.05f, z };
        outCity->roads[outCity->roadCount++] = (Road){ pos, size };

        // Sidewalk stripes along the road
        Vector3 swSize = (Vector3){ cityW + cfg.roadWidth, curbH, 1.0f };
        outCity->sidewalks[outCity->sidewalkCount++] = (Sidewalk){ (Vector3){ cityW*0.5f, curbH*0.5f, z - cfg.roadWidth*0.5f - swSize.z*0.5f }, swSize };
        outCity->sidewalks[outCity->sidewalkCount++] = (Sidewalk){ (Vector3){ cityW*0.5f, curbH*0.5f, z + cfg.roadWidth*0.5f + swSize.z*0.5f }, swSize };

        // Dashed center line marks every ~4m
        int dashes = (int)((cityW + cfg.roadWidth)/4.0f);
        for (int i = 0; i < dashes; i++) {
            float x = (i+0.5f)*4.0f;
            outCity->marks[outCity->markCount++] = (Mark){ (Vector3){ x, 0.11f, z }, (Vector3){ 1.5f, 0.02f, 0.2f } };
        }
    }

    // Vertical roads along X
    for (int bx = 0; bx <= cfg.blocksX; bx++) {
        float x = bx * cfg.blockSize;
        Vector3 size = (Vector3){ cfg.roadWidth, 0.1f, cityH + cfg.roadWidth };
        Vector3 pos  = (Vector3){ x, 0.05f, cityH*0.5f };
        outCity->roads[outCity->roadCount++] = (Road){ pos, size };

        // Sidewalk stripes along the road
        Vector3 swSize = (Vector3){ 1.0f, curbH, cityH + cfg.roadWidth };
        outCity->sidewalks[outCity->sidewalkCount++] = (Sidewalk){ (Vector3){ x - cfg.roadWidth*0.5f - swSize.x*0.5f, curbH*0.5f, cityH*0.5f }, swSize };
        outCity->sidewalks[outCity->sidewalkCount++] = (Sidewalk){ (Vector3){ x + cfg.roadWidth*0.5f + swSize.x*0.5f, curbH*0.5f, cityH*0.5f }, swSize };

        // Dashed center line marks every ~4m
        int dashes = (int)((cityH + cfg.roadWidth)/4.0f);
        for (int i = 0; i < dashes; i++) {
            float z = (i+0.5f)*4.0f;
            outCity->marks[outCity->markCount++] = (Mark){ (Vector3){ x, 0.11f, z }, (Vector3){ 0.2f, 0.02f, 1.5f } };
        }
    }

    // Inside each block, place a grid of lots with small buildings or parks
    for (int bz = 0; bz < cfg.blocksZ; bz++) {
        for (int bx = 0; bx < cfg.blocksX; bx++) {
            float bMinX = bx * cfg.blockSize + cfg.roadWidth*0.5f + 1.0f;
            float bMaxX = (bx+1) * cfg.blockSize - cfg.roadWidth*0.5f - 1.0f;
            float bMinZ = bz * cfg.blockSize + cfg.roadWidth*0.5f + 1.0f;
            float bMaxZ = (bz+1) * cfg.blockSize - cfg.roadWidth*0.5f - 1.0f;

            int lots = 6;
            for (int lz = 0; lz < lots; lz++) {
                for (int lx = 0; lx < lots; lx++) {
                    float t0x = lx/(float)lots, t1x = (lx+1)/(float)lots;
                    float t0z = lz/(float)lots, t1z = (lz+1)/(float)lots;
                    float lx0 = lerpf(bMinX, bMaxX, t0x);
                    float lx1 = lerpf(bMinX, bMaxX, t1x);
                    float lz0 = lerpf(bMinZ, bMaxZ, t0z);
                    float lz1 = lerpf(bMinZ, bMaxZ, t1z);

                    float cx = (lx0 + lx1)*0.5f;
                    float cz = (lz0 + lz1)*0.5f;
                    float lw = (lx1 - lx0) * 0.85f;
                    float lzlen = (lz1 - lz0) * 0.85f;

                    // Decide park or building
                    float p = frand01(&rng);
                    if (p < cfg.parkChance) {
                        outCity->parks[outCity->parkCount++] = (Park){ (Vector3){ cx, 0.06f, cz }, (Vector3){ lw, 0.12f, lzlen } };
                    } else {
                        // building size within min/max with slight random
                        float sx = lerpf(cfg.minBuilding.x, cfg.maxBuilding.x, frand01(&rng));
                        float sz = lerpf(cfg.minBuilding.z, cfg.maxBuilding.z, frand01(&rng));
                        float sy = lerpf(cfg.minBuilding.y, cfg.maxBuilding.y, frand01(&rng));

                        // Clamp to lot area
                        if (sx > lw) sx = lw;
                        if (sz > lzlen) sz = lzlen;

                        outCity->buildings[outCity->buildingCount++] =
                            (Building){ (Vector3){ cx, sy*0.5f + 0.06f, cz },
                                        (Vector3){ sx, sy, sz },
                                        (int)(frand01(&rng)*1000.0f) };
                    }
                }
            }
        }
    }
}

void UnloadCity(City* city)
{
    if (!city) return;
    if (city->buildings) MemFree(city->buildings);
    if (city->roads) MemFree(city->roads);
    if (city->sidewalks) MemFree(city->sidewalks);
    if (city->parks) MemFree(city->parks);
    if (city->marks) MemFree(city->marks);
    *city = (City){0};
}
