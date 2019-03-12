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
	vec3 direction;
    float cutOff; //光切角内圆锥
	float outerCutOff;//光切角外圆锥

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform Light light;

void main()
{
	
	//计算环境光照
	float ambientStrength = 0.1;
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);


	//计算漫反射光照
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

	//计算镜面反射光照
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	// emission
	vec3 emission = (texture(material.emission, TexCoords).rgb + texture(material.specular, TexCoords).rgb);

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;

	diffuse  *= intensity;
	specular *= intensity;

	vec3 result = ambient + diffuse + specular + emission;
	FragColor = vec4(result, 1.0);
}