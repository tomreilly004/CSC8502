# version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform sampler2D blendMap;
uniform float time;
layout (points) in ;
layout (line_strip, max_vertices = 2) out ;

in Vertex {
	 vec4 colour ;
	 vec2 texCoord ;
	 vec3 normal ;
	 vec3 worldPos ;
	 vec4 shadowProj;
 } IN [];

 out Vertex {
	 vec4 colour ;
	 vec2 texCoord ;
	 vec3 normal ;
	 vec3 worldPos ;
	 vec4 shadowProj;
 } OUT ;

 float makeRandom(vec2 co) {
    return fract(sin(dot(co, vec2(1298.98, 7823.3))) * 4375854.53);
}

 void main () {

		 vec4 blendMapColour = texture(blendMap, IN[0].texCoord);
		
		mat4 mvp = projMatrix * viewMatrix * modelMatrix;

		vec4 start = modelMatrix * gl_in[0].gl_Position;

		float freq = time + makeRandom(start.xz + 7000) * 1000000;
        float amp = 10000 * makeRandom(start.xz + 8000);


		 if(blendMapColour.b > 0.8) {
			OUT.colour = IN[0].colour ;
			// top 
			gl_Position = mvp * (gl_in [0].gl_Position * 512 + vec4(sin(freq) * amp, 5000.0, (sin(freq) * amp)/6, 1.0) ) ;
			OUT.colour = vec4(makeRandom(start.xz)/4 - 1/6 ,makeRandom(start.xz+2)/3 - 1/8,0.1,1);
			EmitVertex ();
		
			// bottom
			gl_Position = mvp * (gl_in [0].gl_Position * 512 + vec4( 0.0, 0.0, 0.0, 1.0)) ;
			OUT.colour = vec4(0,makeRandom(start.xz+2)/3 - 1/8,0.1,1);
			EmitVertex ();

			EndPrimitive ();
		 }
 }
