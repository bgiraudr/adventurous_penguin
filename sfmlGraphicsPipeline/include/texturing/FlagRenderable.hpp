#ifndef FLAGRENDERABLE_H
#define FLAGRENDERABLE_H

#include "TexturedMeshRenderable.hpp"
#include <vector>
#include <string>
#include "dynamics/SpringForceField.hpp"

class FlagRenderable : public TexturedMeshRenderable
{
public:
    FlagRenderable(ShaderProgramPtr program,
        float width, float height,
        int horizontal_resolution, int vertical_resolution,
        float stiffness, float damping, const std::string & texture_filename);
    ~FlagRenderable();

    void do_draw();
    std::vector<ParticlePtr> & getParticles();
    const std::vector<SpringForceFieldPtr> & getSprings();

private:
    std::vector<ParticlePtr> m_particles;
    std::vector<SpringForceFieldPtr> m_springs;
};

#endif // FLAGRENDERABLE_H
