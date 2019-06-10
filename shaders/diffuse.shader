#shader vertex

#version 330 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;

out vec3 normal;
//out vec3 fragPos;

uniform mat4 projectionMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

void main()
{
	//fragPos = vec3(viewMat * modelMat * aPosition);
	normal = vec3(aNormal);

	//gl_Position = projectionMat * vec4(fragPos, 1.0);
	gl_Position = projectionMat * viewMat * modelMat * aPosition;
}

#shader fragment

#version 330 core

out vec4 color;

in vec3 normal;
in vec3 fragPos;

//uniform vec3 lightPos;
uniform vec3 lightDir;
uniform int noLighting;

void main()
{
	//vec3 lightDir = normalize(lightPos - fragPos);

	if (noLighting == 1)
	{
		color = vec4(0.2, 0.2, 0.2, 1.0);
	}
	else
	{
		float diff = max(dot(normalize(-normal), normalize(lightDir)), 0.0);
		color = vec4(vec3(diff), 1.0);
	}
};