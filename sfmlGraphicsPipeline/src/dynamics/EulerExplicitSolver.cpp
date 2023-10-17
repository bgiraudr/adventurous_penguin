#include "./../../include/dynamics/EulerExplicitSolver.hpp"

EulerExplicitSolver::EulerExplicitSolver()
{

}

EulerExplicitSolver::~EulerExplicitSolver()
{

}

void EulerExplicitSolver::do_solve(const float& dt, std::vector<ParticlePtr>& particles)
{
    for(ParticlePtr p : particles)
    {
        if(!p->isFixed())
        {
            //TODO: Implement explicit euler solver
            //Functions to use:
            //Particle::getPosition(), Particle::getVelocity(), Particle::getMass(), Particle::getForce()
            //Particle::setPosition(), Particle::setVelocity()
            // glm::vec3 acc = p->getForce() / p->getMass();
            glm::vec3 next_velo = p->getVelocity() + (1/p->getMass()) * dt * p->getForce();
            p->setVelocity(next_velo);

            glm::vec3 next_pos = p->getPosition() + dt*p->getVelocity();
            p->setPosition(next_pos);
        }
    }
}
