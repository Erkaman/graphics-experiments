#pragma once

enum Collisiontypes {
    COL_NOTHING = 0, //<Collide with nothing
    COL_CAR = 1<<1, //<Collide with cars
    COL_STATIC = 1<<2, //<Collide with static
};

constexpr int carCollidesWith = COL_STATIC;
constexpr int staticCollidesWith = COL_NOTHING; // terrain and static objects
