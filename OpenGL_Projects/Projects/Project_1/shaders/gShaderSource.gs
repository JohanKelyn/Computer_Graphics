#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	vec3 a = ( gl_in[1].gl_Position - gl_in[0].gl_Position).xyz;
	vec3 b = ( gl_in[2].gl_Position - gl_in[0].gl_Position).xyz;
	vec3 N = normalize(cross(a,b));

	for (int i = 0; i < gl_in.length(); ++i)
	{
		FragPos = vec3(model * gl_in[i].gl_Position);;
		gl_Position = projection * view * vec4(FragPos, 1.0);
		normal = mat3(transpose(inverse(model))) * N;
		EmitVertex();
	}

	EndPrimitive();
}