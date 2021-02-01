#version 330

//lighting
uniform vec3 u_l_halfplane;
uniform vec3 u_l_direction;
uniform vec4 u_l_ambient;
uniform vec4 u_l_diffuse;
uniform vec4 u_l_specular;

//material
uniform vec4 u_m_ambient;
uniform vec4 u_m_diffuse;
uniform vec4 u_m_specular;
uniform float u_m_shininess;

//attributes
attribute vec3 a_position;
attribute vec3 a_normal;

//matricies
uniform mat4 u_mvpmatrix;
uniform mat4 u_mvmatrix;

//varying
varying vec4 v_colour;


void main()
{

    vec3 ecNormal = vec3(u_mvmatrix*vec4(a_normal, 0)); // 0 because direction vector
    ecNormal = ecNormal/length(ecNormal); //normalise it

    float ndotl = max(0.0, dot(ecNormal,u_l_direction));
    float ndoth = max(0.0, dot(ecNormal,u_l_halfplane));

    vec4 ambientLight = u_l_ambient * u_m_ambient;
    vec4 diffuseLight = u_l_diffuse * u_m_diffuse * ndotl;

    vec4 specularLight = vec4(0.0);
    if(ndoth > 0.0)
    {
        specularLight = u_l_specular * u_m_specular * pow(ndoth, u_m_shininess);
    }
    
    v_colour = ambientLight + diffuseLight + specularLight;

    gl_Position = u_mvpmatrix * vec4(a_position, 1); // 1 position vector
}