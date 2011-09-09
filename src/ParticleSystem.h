#pragma once

#include "Particle.h"
#include "ofMain.h"

#define MAX_PARTICLES		10000

using namespace MSA;

class ParticleSystem {
public:	
	
    float posArray[MAX_PARTICLES * 2 * 2];
	Vec2f				windowSize;
	Vec2f				invWindowSize;
	const FluidSolver	*solver;
	
    int curIndex;
	
    Particle particles[MAX_PARTICLES];
	
	ParticleSystem();
	void setFluidSolver( const FluidSolver *aSolver ) { solver = aSolver; }
	
    void updateAndDraw();
	void addParticles( const Vec2f &pos, int count );
	void addParticle( const Vec2f &pos );
	void addParticlesWithVelc( const Vec2f &pos, const Vec2f &vel, int count );
	void addParticleWithVelc( const Vec2f &pos, const Vec2f &vel );
	void setWindowSize( Vec2f winSize );
};
