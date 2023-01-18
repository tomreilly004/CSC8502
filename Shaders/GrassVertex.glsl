# version 330 core

uniform mat4 textureMatrix;
uniform mat4 modelMatrix;
uniform mat4 shadowMatrix ;

uniform vec3 lightPos ;

in vec3 position;
in vec4 colour ;
in vec3 normal ; 
in vec2 texCoord;

out Vertex {
	 vec4 colour ;
	 vec2 texCoord ;
	 vec3 normal ;
	 vec3 worldPos ;
	 vec4 shadowProj;
} OUT;

void main(void)
{
	vec4 worldPos = ( modelMatrix * vec4 ( position, 1));
	OUT.worldPos = worldPos.xyz ;

	mat3 normalMatrix = transpose( inverse( mat3( modelMatrix )));
	OUT.normal = normalize (normalMatrix * normalize ( normal ));

	gl_Position = vec4(position, 1.0);

	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

	vec3 viewDir = normalize ( lightPos - worldPos.xyz );
	vec4 pushVal = vec4 ( OUT.normal , 0) * dot ( viewDir , OUT.normal );
	OUT.shadowProj = shadowMatrix * ( worldPos + pushVal );
	
}
