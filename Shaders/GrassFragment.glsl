 # version 400 core
  
  uniform sampler2D grassTex;
  uniform sampler2D shadowTex ; 
  
  uniform vec3 cameraPos ;
  uniform vec4 lightColour ;
  uniform vec3 lightPos ;
  uniform float lightRadius ; 
  
  uniform vec4 spotColour ;
  uniform vec3 spotPos ;
  uniform float spotRadius ;
  uniform vec3 spotDirection ;
  uniform float spotAngle ;



 in Vertex {
	vec4 colour ;
	vec2 texCoord ;
	vec3 normal ;
	vec3 worldPos ;
	vec4 shadowProj;
 } IN ;

 out vec4 fragColour;

 void main ( void ) {

	vec3 incident = normalize ( lightPos - IN.worldPos );
	vec3 viewDir = normalize ( cameraPos - IN.worldPos );
	vec3 halfDir = normalize ( incident + viewDir );

	vec3 spotIncident = normalize(spotPos - IN.worldPos);
	float theta = dot(spotIncident, -normalize(spotDirection));

	float lambert = max( dot( incident , IN.normal ) , 0.0f );
	float distance = length( lightPos - IN.worldPos );
	float attenuation = 1.0 - clamp( distance / lightRadius , 0.0 , 1.0);

	float specFactor = clamp( dot( halfDir , IN.normal ) ,0.0 ,1.0);

	float shadow = 1.0; 
	vec3 shadowNDC = IN . shadowProj.xyz / IN.shadowProj.w ;
	vec3 biasCoord = shadowNDC *0.5f + 0.5f ;
	
	vec4 newGrassColour = texture(grassTex, IN.texCoord);

	vec4 finalColour = newGrassColour + IN.colour;

	if( abs(shadowNDC.x) < 1.0f && abs(shadowNDC.y) < 1.0f && abs(shadowNDC.z) < 1.0f ) {
		vec3 biasCoord = shadowNDC *0.5f + 0.5f ;
		float shadowZ = texture(shadowTex, biasCoord.xy).x ;
	
		if( shadowZ < biasCoord. z) {
			shadow = 0.0f;
		}
	}
	
	if(theta > spotAngle){ //&& shadow != 0.4) {
		vec3 surface = ( finalColour.rgb * lightColour.rgb );
		vec3 spotHalfDir = normalize ( spotIncident + viewDir );
		float spotLambert = max( dot( spotIncident , IN.normal ) , 0.0f );
		float spotlightDistance = length( spotPos - IN.worldPos );
		float spotAttenuation = 1.0 - clamp( distance / spotRadius , 0.0 , 1.0);
		fragColour.rgb += surface * spotLambert * spotAttenuation ;
		fragColour.rgb += ( spotColour.rgb * spotAttenuation );
		fragColour.rgb += mix(spotColour.rgb,finalColour.rgb, 0.5) ;
		fragColour.a = finalColour.a;
	} else {
		fragColour.rgb = finalColour.rgb;
		fragColour.rgb *= shadow;
		fragColour.a = finalColour.a ;
	}
}
