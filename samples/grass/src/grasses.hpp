#pragma once

#include "grass.hpp"

#include "ewa/random.hpp"

class Exp : public Grass {

public:

    Exp(): Grass("img/grass.png") { }

    virtual void MakeGrass(std::vector<GrassBlade>& blades) override {
	// this thing.

	Random rng(11);

	constexpr int LOD =5;
	constexpr int BLADES =50; // 30

	for(int i = 0; i < BLADES; ++i) {

	    GrassBlade blade;

	    blade.vertexPosition =Vector3f( rng.RandomFloat(-0.5f, +0.5f),0, rng.RandomFloat(-0.3f, +0.3f));

	    blade.grassHeight = rng.RandomFloat(0.8f, 1.2f);
	    blade.grassWidth = rng.RandomFloat(0.05f, 0.07f);
	    blade.windDirection = Vector3f(rng.RandomFloat(-0.15f, +0.15f),0,0*rng.RandomFloat(-0.15f, +0.15f));
	    blade.lod = LOD;

	    blades.push_back(blade);
	}
    }
};
