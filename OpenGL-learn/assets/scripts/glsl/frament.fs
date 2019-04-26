#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{   
	vec4 txtColor = texture(texture1, TexCoords);
	if(txtColor.a < 0.1)
		discard;
    FragColor = txtColor;
}