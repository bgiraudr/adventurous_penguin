#include "./../../include/texturing/FlagRenderable.hpp"
#include "./../../include/gl_helper.hpp"
#include "./../../include/Utils.hpp"

FlagRenderable::~FlagRenderable()
{}

FlagRenderable::FlagRenderable(ShaderProgramPtr program,
    float width, float height,
    int horizontal_resolution, int vertical_resolution,
    float stiffness, float damping, const std::string & texture_filename):
    TexturedMeshRenderable(program, true)
{
    std::vector<glm::uvec3> indices;
    getFabric(width, height, horizontal_resolution, vertical_resolution, m_particles, m_springs, m_positions, indices, m_tcoords, stiffness, damping);
    for (int j = 0 ; j < vertical_resolution ; ++j){
        m_particles[j]->setFixed(true);
        m_particles[j+(horizontal_resolution-1)*vertical_resolution]->setFixed(true);
    }

    // m_particles[2+4*14]->setFixed(true);

    unpack(indices, m_indices);
    
    m_image.loadFromFile(texture_filename);
    if (m_tcoords.size() != m_positions.size()){
        m_tcoords.resize(m_positions.size(), glm::vec2(0.0));
    }
    m_original_tcoords = m_tcoords;
    m_image.flipVertically();

    update_all_buffers();
}

void FlagRenderable::do_draw()
{
    // Update the vertex positions
    for (int i = 0 ; i < m_particles.size() ; ++i)
        m_positions[i] = m_particles[i]->getPosition();
    update_positions_buffer();

    // Draw the 
    TexturedMeshRenderable::do_draw();
}

std::vector<ParticlePtr> & FlagRenderable::getParticles(){
    return m_particles;
}
const std::vector<SpringForceFieldPtr> & FlagRenderable::getSprings(){
    return m_springs;
}