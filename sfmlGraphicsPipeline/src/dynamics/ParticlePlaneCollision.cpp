#include "./../../include/dynamics/ParticlePlaneCollision.hpp"

ParticlePlaneCollision::~ParticlePlaneCollision()
{}


ParticlePlaneCollision::ParticlePlaneCollision(ParticlePtr particle, PlanePtr plane, float restitution) :
    Collision(restitution)
{
    m_particle = particle;
    m_plane = plane;
}

void ParticlePlaneCollision::do_solveCollision()
{
    //Don't process fixed particles (Let's assume that the ground plane is fixed)
    if (m_particle->isFixed()) return;

    //TODO: Solve ParticlePlane collisions, update particle position and velocity after collision
    //Functions to use:
    //glm::dot(v1, v2): Return the dot product of two vector.
    //Plane::distanceToOrigin(): Return the distance to origin from the plane
    //Plane::normal(): Return the normal of the plane
    //Particle::getRadius(), Particle::getPosition(), Particle::getVelocity(), Particle::setPosition(), Particle::setVelocity()

    //Compute particle-plane distance
    float d2plane = abs(glm::dot(m_particle->getPosition(), m_plane->normal()) - m_plane->distanceToOrigin());

    //Project the particle on the plane

    m_particle->setPosition(m_particle->getPosition() - (d2plane - m_particle->getRadius()) * m_plane->normal());

    //Compute post-collision velocity
    m_particle->setVelocity(m_particle->getVelocity() - (1.0f + m_restitution) * glm::dot(m_particle->getVelocity(), m_plane->normal()) * m_plane->normal());


}



bool testParticlePlane(const ParticlePtr &particle, const PlanePtr &plane)
{
    /* Equation of a plane passing through A and normal to n:
   * dot( p - A, n ) = dot( p, n ) - dot( A, n ) = 0
   * dot( A, n ) is stored in the "distanceToOrigin" of the plane.
   *
   * Equation of a particle of radius r centered in c:
   * dot( p - c, p - c ) = r²
   *
   * distance( plane, particle )
   *   = min( distance( plane, c ) - r, 0 )              //definition
   *   = min( abs( dot( c - A, n ) ) - r, 0 )
   *   = min( abs( dot( c, n ) - dot( A, n ) ) - r, 0 )
   *
   * So, there is intersection if distance( plane, particle ) = 0
   * <=> abs( dot( c, n ) - dot( A, n ) ) - r <= 0
   * <=> abs( dot( c, n ) - dot( A, n ) ) <= r
   */

    //TODO: Test collision between particle and plane
    //Functions to use:
    //glm::dot(v1, v2): Return the dot product of two vector.
    //Plane::distanceToOrigin(): Return the distance to origin from the plane
    //Plane::normal(): Return the normal of the plane
    //Particle::getRadius(), Particle::getPosition()

    return abs(glm::dot(particle->getPosition() - plane->distanceToOrigin(), plane->normal())) <= particle->getRadius();
}
