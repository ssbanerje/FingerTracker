#include "ParticleSystem.h"

//--------------------------------------------------------------
ParticleSystem::ParticleSystem() {
	curIndex = 0;
	setWindowSize( Vec2f( 1, 1 ) );
}

//--------------------------------------------------------------
void ParticleSystem::setWindowSize( Vec2f winSize ) {
	windowSize = winSize;
	invWindowSize = Vec2f( 1.0f / winSize.x, 1.0f / winSize.y );
}

//--------------------------------------------------------------
void ParticleSystem::updateAndDraw() {
    // Update positions
    #pragma omp parallel for
	for(int i=0; i<MAX_PARTICLES; i++) {
		if(particles[i].alpha > 0) {
			particles[i].update( *solver, windowSize, invWindowSize );
			particles[i].updateVertexArrays( invWindowSize, i, posArray);
		}
	}
    
    // Draw to screen    
	glEnable(GL_BLEND);
	glDisable( GL_TEXTURE_2D );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
    ofSetColor(255, 255, 255);
    #pragma omp parallel for
    for(register int i=0;i<MAX_PARTICLES*2;i+=2) {
        ofCircle(posArray[i], posArray[i+1], 2);
    }
	
	glDisable(GL_BLEND);
}


//--------------------------------------------------------------
void ParticleSystem::addParticles( const Vec2f &pos, int count ){
    #pragma omp parallel for
	for(int i=0; i<count; i++)
		addParticle( pos + Rand::randVec2f() * 15 );
}


//--------------------------------------------------------------
void ParticleSystem::addParticle( const Vec2f &pos ) {
	particles[curIndex].init( pos.x, pos.y );
	curIndex++;
	if(curIndex >= MAX_PARTICLES) curIndex = 0;
}

//--------------------------------------------------------------
void ParticleSystem::addParticlesWithVelc( const Vec2f &pos, const Vec2f &vel, int count ){
    #pragma omp parallel for
	for(int i=0; i<count; i++)
		addParticleWithVelc( pos + Rand::randVec2f()*15, vel );
}

//--------------------------------------------------------------
void ParticleSystem::addParticleWithVelc( const Vec2f &pos, const Vec2f &vel ) {
	particles[curIndex].initWithVelc( pos.x, pos.y, vel.x, vel.y );
	curIndex++;
	if(curIndex >= MAX_PARTICLES) curIndex = 0;
}
