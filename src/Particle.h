#pragma once

#include "MSACore.h"
#include "MSAFluidSolver.h"

using namespace MSA;

class Particle {
public:
    Vec2f pos, vel;
    float radius;
    float alpha;
    float mass;
    
    void init(float x, float y);
    void initWithVelc(float x, float y, float vx, float vy);
    void update( const FluidSolver &solver, 
                 const Vec2f &windowSize,
                 const Vec2f &invWindowSize );
	void updateVertexArrays( const Vec2f &invWindowSize, 
                             int i, float* posBuffer);
};