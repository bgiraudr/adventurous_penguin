#include "./../include/IndexedCubeRenderable.hpp"
#include "./../include/gl_helper.hpp"
#include "./../include/log.hpp"
#include "./../include/Utils.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

IndexedCubeRenderable::IndexedCubeRenderable(ShaderProgramPtr shaderProgram)
  : Renderable(shaderProgram), m_vBuffer(0), m_cBuffer(0), m_iBuffer(0)
{
	// Build the geometry : just a simple triangle for now
	m_positions.push_back( glm::vec3 (-1 ,-1 ,1) );
	m_positions.push_back( glm::vec3 (1 ,-1 ,1) );
	m_positions.push_back( glm::vec3 (1 ,1 ,1) );
	m_positions.push_back( glm::vec3 (-1 ,1 ,1) );

	m_positions.push_back( glm::vec3 (-1 ,-1 ,-1) );
	m_positions.push_back( glm::vec3 (1 ,-1 ,-1) );
	m_positions.push_back( glm::vec3 (1 ,1 ,-1) );
	m_positions.push_back( glm::vec3 (-1 ,1 ,-1) );

    m_index = {
        // front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
    };

	// Set the model matrix to identity
	m_model = glm::mat4(1.0);
	m_model  = glm::translate(glm::mat4(), glm::vec3(3,0,-1));

	// //Create buffers
	glGenBuffers(1, &m_vBuffer); //vertices

	// //Activate buffer and send data to the graphics card
	glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_iBuffer); //index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index.size() * sizeof(unsigned int), m_index.data(), GL_STATIC_DRAW);

	srand (time(0));
	for(int i = 0; i < 8; i++) {
			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			m_colors.push_back(glm::vec4(r, g, b, 1.0)); //(r,g,b,a)
	}

	glGenBuffers(1, &m_cBuffer); //colors
	glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_colors.size()*sizeof(glm::vec4), m_colors.data(), GL_STATIC_DRAW);
}

void IndexedCubeRenderable::do_draw()
{
	// Get the identifier ( location ) of the uniform modelMat in the shader program
	int modelLocation = m_shaderProgram->getUniformLocation("modelMat");
	// Send the data corresponding to this identifier on the GPU
	glUniformMatrix4fv( modelLocation , 1, GL_FALSE , glm::value_ptr( m_model ));

	// Get the identifier of the attribute vPosition in the shader program
	int positionLocation = m_shaderProgram->getAttributeLocation("vPosition");
	// Activate the attribute array at this location
	glEnableVertexAttribArray( positionLocation );
	// Bind the position buffer on the GL_ARRAY_BUFFER target
	glBindBuffer( GL_ARRAY_BUFFER , m_vBuffer );
	// Specify the location and the format of the vertex position attribute
	glVertexAttribPointer( positionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer);

	int colorLocation = m_shaderProgram->getAttributeLocation("vColor");
	glEnableVertexAttribArray( colorLocation );
	glBindBuffer( GL_ARRAY_BUFFER , m_cBuffer );
	glVertexAttribPointer( colorLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//draw the triangles
	glDrawElements(GL_TRIANGLES, m_index.size(), GL_UNSIGNED_INT, (void*)0);
	
	// Release the vertex attribute array
	glDisableVertexAttribArray( positionLocation );
	glDisableVertexAttribArray( colorLocation );
}

IndexedCubeRenderable::~IndexedCubeRenderable()
{
    glcheck(glDeleteBuffers(1, &m_vBuffer));
    glcheck(glDeleteBuffers(1, &m_cBuffer));
    glcheck(glDeleteBuffers(1, &m_iBuffer));
}
