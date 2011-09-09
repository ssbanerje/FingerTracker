#include "Particle.h"

static const float MOMENTUM = 0.5f;
static const float FLUID_FORCE = 0.6f;

//--------------------------------------------------------------
void Particle::init(float x, float y) {
	pos = Vec2f( x, y );
	vel = Vec2f(0, 0);
	radius = 5;
	alpha  = Rand::randFloat( 0.3f, 1 );
	mass = Rand::randFloat( 0.1f, 1 );
}

//--------------------------------------------------------------
void Particle::initWithVelc(float x, float y, float vx, float vy) {
	pos = Vec2f( x, y );
	vel = Vec2f( vx, vy );
	radius = 5;
	alpha  = Rand::randFloat( 0.3f, 1 );
	mass = Rand::randFloat( 0.1f, 1 );
}

//--------------------------------------------------------------
void Particle::update( const FluidSolver &solver, const Vec2f &windowSize, const Vec2f &invWindowSize ) {
	if( alpha == 0 )
		return;
	vel = solver.getVelocityAtPos( pos * invWindowSize ) * (mass * FLUID_FORCE ) * windowSize + vel * MOMENTUM;
	pos += vel;	
	if( pos.x < 0 ) {
		pos.x = 0;
		vel.x *= -1;
	}
	else if( pos.x > windowSize.x ) {
		pos.x = windowSize.x;
		vel.x *= -1;
	}
	if( pos.y < 0 ) {
		pos.y = 0;
		vel.y *= -1;
	}
	else if( pos.y > windowSize.y ) {
		pos.y = windowSize.y;
		vel.y *= -1;
	}
	if( vel.lengthSquared() < 1 ) {
		vel += Rand::randVec2f() * 0.5f;
	}
	alpha *= 0.999f;
	if( alpha < 0.01f )
		alpha = 0;
}

//--------------------------------------------------------------
void Particle::updateVertexArrays( const Vec2f &invWindowSize, int i, float* posBuffer) {
	int vi = i * 4;
	posBuffer[vi++] = pos.x - vel.x;
	posBuffer[vi++] = pos.y - vel.y;
	posBuffer[vi++] = pos.x;
	posBuffer[vi++] = pos.y;
}
