
#ifndef CITY_H
#define CITY_H

#include "raylib.h"

typedef struct {
    unsigned int seed;
    int blocksX;
    int blocksZ;
    float blockSize;
    float roadWidth;
    Vector3 minBuilding;
    Vector3 maxBuilding;
    float parkChance;
} CityConfig;

typedef struct {
    Vector3 pos;
    Vector3 size;
    int colorVar;
} Building;

typedef struct {
    Vector3 pos;
    Vector3 size;
} Road;

typedef struct {
    Vector3 pos;
    Vector3 size;
} Sidewalk;

typedef struct {
    Vector3 pos;
    Vector3 size;
} Park;

typedef struct {
    Vector3 pos;
    Vector3 size;
} Mark; // road marking

typedef struct {
    CityConfig cfg;

    Building* buildings;
    int buildingCount;

    Road* roads;
    int roadCount;

    Sidewalk* sidewalks;
    int sidewalkCount;

    Park* parks;
    int parkCount;

    Mark* marks;
    int markCount;
} City;

void GenerateCity(City* outCity, CityConfig cfg);
void UnloadCity(City* city);

#endif // CITY_H
