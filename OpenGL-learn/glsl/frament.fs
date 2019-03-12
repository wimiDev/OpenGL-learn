#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D emission;
    float shininess;
}; 

struct Light{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

};

uniform Material material;
uniform Light light;

void main()
{
	//���㻷������
	float ambientStrength = 0.1;
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	//�������������
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

	//���㾵�淴�����
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	// emission
    vec3 emission = (texture(material.emission, TexCoords).rgb + texture(material.specular, TexCoords).rgb);

	vec3 result = ambient + diffuse + specular + emission;
	FragColor = vec4(result, 1.0);
}