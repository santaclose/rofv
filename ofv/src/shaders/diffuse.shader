#shader vertex

/////////////////////////////////////////////////////////////////////////
#version 330 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 aUVs;

out vec3 normal;
//out vec3 fragPos;
out vec2 uvs;

uniform mat4 projectionMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

void main()
{
	//fragPos = vec3(viewMat * modelMat * aPosition);
	normal = vec3(aNormal);

	uvs = aUVs;

	//gl_Position = projectionMat * vec4(fragPos, 1.0);
	gl_Position = projectionMat * viewMat * modelMat * aPosition;
}

#shader fragment

////////////////////////////////////////////////////////////////////////
#version 330 core

out vec4 color;

in vec3 normal;
//in vec3 fragPos;
in vec2 uvs;

//uniform vec3 lightPos;
uniform vec3 lightDir;
uniform int noLighting;
uniform int useTexture;
uniform sampler2D tex;

void main()
{
	//vec3 lightDir = normalize(lightPos - fragPos);

	if (noLighting != 0)
	{
		color = vec4(0.2, 0.2, 0.2, 1.0);
	}
	else
	{
		float diff = max(dot(normalize(-normal), normalize(lightDir)), 0.0);
		vec4 textureColor;
		if (useTexture == 1)
		{
			textureColor = texture(tex, uvs);
		}
		else
		{
			textureColor = vec4(1.0);
		}
		color = vec4(vec3(diff) * textureColor.rgb, 1.0);
	}
};