#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
	//sampler2D emission;
    float shininess;
}; 

struct Light{
	vec3 position; 
	vec3 direction;
    float cutOff; //���н���Բ׶
	float outerCutOff;//���н���Բ׶

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
    float linear;
    float quadratic;
};

struct SpotLight{
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
    float linear;
    float quadratic;

	float cutOff;
	float outerCutOff;
};

uniform Material material;
uniform DirLight dirLight;

#define NR_POINT_LIGHTS 4

uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform SpotLight spotLight;

//ƽ�й�/�����
vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir){
	//���㻷������
	float ambientStrength = 0.1;
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	vec3 lightDir = normalize(-light.direction);

	//�������������
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

	//���㾵�淴�����
	float specularStrength = 0.5;
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	vec3 result = ambient + diffuse + specular;
	return result;
}

//�����Դ����
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir){
	float ambientStrength = 0.1;
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	vec3 lightDir = normalize(light.position - FragPos);

	//�������������
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

	//���㾵�淴�����
	float specularStrength = 0.5;
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;
	return result;
}

//�۹�
vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 viewDir){
	//���㻷������
	float ambientStrength = 0.1;
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	//�������������
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	//���㾵�淴�����
	float specularStrength = 0.5;
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;

	diffuse  *= intensity;
	specular *= intensity;

	vec3 result = ambient + diffuse + specular;
	return result;
}

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	//������
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	//�����Դ
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, viewDir);
	//�۹�
	result += CalcSpotLight(spotLight, norm, viewDir);
	FragColor = vec4(result, 1.0);
}