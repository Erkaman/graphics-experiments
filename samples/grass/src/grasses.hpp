#pragma once

#include "grass.hpp"

#include "ewa/random.hpp"

// thick grass leaves.
class Exp : public Grass {

public:

    Exp(): Grass(std::vector<std::string>{"img/grass.png"}     , Vector4f(6.76f,4.52f,4.33f, 1.0f)) { }

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
	    blade.grassTexture = 0;

	    blades.push_back(blade);
	}
    }
};


// thin grass. not very wide.
class Exp2 : public Grass {

public:

    Exp2(): Grass(std::vector<std::string>{"img/grass.png"}, Vector4f(6.76f,4.52f,4.33f, 1.0f)) { }

    virtual void MakeGrass(std::vector<GrassBlade>& blades) override {
	// this thing.

	Random rng(11);

	constexpr int LOD =5;
	constexpr int BLADES =30; // 30

	for(int i = 0; i < BLADES; ++i) {

	    GrassBlade blade;

	    blade.vertexPosition =Vector3f( rng.RandomFloat(-0.5f, +0.5f),0, rng.RandomFloat(-0.3f, +0.3f));

	    blade.grassHeight = rng.RandomFloat(0.8f, 1.2f);
	    blade.grassWidth = rng.RandomFloat(0.02f, 0.05f);
	    blade.windDirection = Vector3f(rng.RandomFloat(-0.15f, +0.15f),0,0*rng.RandomFloat(-0.15f, +0.15f));
	    blade.lod = LOD;
	    blade.grassTexture = 0;

	    blades.push_back(blade);
	}
    }
};


// very thick bush.
class Exp3 : public Grass {

public:

    Exp3(): Grass(std::vector<std::string>{"img/grass.png"}, Vector4f(6.76f,4.52f,4.33f, 1.0f)) { }

    virtual void MakeGrass(std::vector<GrassBlade>& blades) override {
	// this thing.

	Random rng(11);

	constexpr int LOD =5;
	constexpr int BLADES =300; // 30

	for(int i = 0; i < BLADES; ++i) {

	    GrassBlade blade;

	    blade.vertexPosition =Vector3f( rng.RandomFloat(-0.1f, +0.1f),0, rng.RandomFloat(-0.3f, +0.3f));

	    blade.grassHeight = rng.RandomFloat(0.8f, 1.2f);
	    blade.grassWidth = rng.RandomFloat(0.02f, 0.05f);
	    blade.windDirection = Vector3f(rng.RandomFloat(-0.15f, +0.15f),0,0*rng.RandomFloat(-0.15f, +0.15f));
	    blade.lod = LOD;
	    blade.grassTexture = 0;

	    blades.push_back(blade);
	}
    }
};

class Exp4 : public Grass {

public:

    Exp4(): Grass(std::vector<std::string>{"img/grass.png","img/grass2.png"}, Vector4f(6.76f,4.52f,4.33f, 1.0f)) { }

    virtual void MakeGrass(std::vector<GrassBlade>& blades) override {
	// this thing.

	Random rng(11);


/*
  constexpr int LOD =5;
	constexpr int BLADES =90; // 30

	for(int i = 0; i < BLADES; ++i) {

	    GrassBlade blade;

	    blade.vertexPosition =Vector3f( rng.RandomFloat(-0.1f, +0.1f),0, rng.RandomFloat(-0.3f, +0.3f));

	    blade.grassHeight = rng.RandomFloat(0.8f, 1.2f);
	    blade.grassWidth = rng.RandomFloat(0.02f, 0.05f);
	    blade.windDirection = Vector3f(rng.RandomFloat(-0.20f, +0.20f),0,0*rng.RandomFloat(-0.20f, +0.20f));
	    blade.lod = 10;
	    blade.grassTexture = 1;

	    blades.push_back(blade);
	    }
*/



	constexpr int LOD =5;
	constexpr int BLADES =200; // 30

	for(int i = 0; i < BLADES; ++i) {

	    GrassBlade blade;

	    blade.vertexPosition =Vector3f( rng.RandomFloat(-0.1f, +0.1f),0, rng.RandomFloat(-0.3f, +0.3f));

	    blade.grassHeight = rng.RandomFloat(0.8f, 1.2f);
	    blade.grassWidth = rng.RandomFloat(0.02f, 0.03f);

	    float ratio = (float)i / (float)BLADES;

	    blade.windDirection = Vector3f(
		-0.20f + ratio*0.4f
		,0,0);
	    blade.lod = 10;
	    blade.grassTexture = 1;

	    blades.push_back(blade);
	    }



    }
};
