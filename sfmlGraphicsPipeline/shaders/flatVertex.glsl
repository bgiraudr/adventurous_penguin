#version 400

uniform mat4 projMat, viewMat, modelMat;

in vec3 vPosition;
in vec4 vColor;
in vec3 vNormal;
out vec4 surfel_color;

float map(float value,float min1, float min2,float max1,float max2){
    return min2 + (value - min1) * (max2 - min2)/(max1 - min1);
}

vec3 mapVec(vec3 vect,float min1, float min2,float max1,float max2){
    return vec3(map(vect.x,min1,min2,max1,max2),map(vect.y,min1,min2,max1,max2),map(vect.z,min1,min2,max1,max2));
}


void main()
{
    gl_Position = projMat*viewMat*modelMat*vec4(vPosition, 1.0f);
    surfel_color = vec4(mapVec(vNormal,-1,0,1,1),1.0f);
    //surfel_color = vColor;
}