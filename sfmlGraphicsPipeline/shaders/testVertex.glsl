#version 400

uniform mat4 projMat, viewMat, modelMat;

uniform mat3 NIT = mat3(1.0);

in vec3 vPosition;
in vec2 vTexCoord;
in vec3 vNormal;
in vec4 vColor;

out vec3 surfel_position;
out vec3 surfel_normal;
out vec2 surfel_texCoord;
out vec4 surfel_color;

out vec3 cameraPosition;

void main()
{

    surfel_position = vec3(modelMat*vec4(vPosition,1.0f));
    surfel_normal = normalize( NIT * vNormal);
    surfel_color  = vColor;
    surfel_texCoord = vTexCoord;

    // Compute the position of the camera in world space
    cameraPosition = - vec3( viewMat[3] ) * mat3( viewMat );

    // Define the fragment position on the screen
    gl_Position = projMat*viewMat*vec4(surfel_position,1.0f);
}
