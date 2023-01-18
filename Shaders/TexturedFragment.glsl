 # version 400 core
  uniform sampler2D blendMap;
  uniform sampler2D grassTex;
  uniform sampler2D snowTex;
  uniform sampler2D rockTex;
  uniform sampler2D grassBump ;
  uniform sampler2D snowBump ;
  uniform sampler2D rockBump ;
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
	vec3 colour ;
	vec2 texCoord ;
	vec3 normal ;
	vec3 tangent ; // New !
	vec3 binormal ; // New !
	vec3 worldPos ;
	vec4 shadowProj ; 
 } IN ;

 out vec4 fragColour;

 void main ( void ) {

	vec3 incident = normalize ( lightPos - IN.worldPos );
	vec3 viewDir = normalize ( cameraPos - IN.worldPos );
	vec3 halfDir = normalize ( incident + viewDir );

	vec3 spotIncident = normalize(spotPos - IN.worldPos);
	float theta = dot(spotIncident, -normalize(spotDirection));


	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

	vec4 blendMapColour = texture(blendMap, IN.texCoord);

	vec2 tilesCoords = IN.texCoord * 60.0f;
	vec4 grassColour = texture(grassTex, tilesCoords/6.0f) * blendMapColour.b;
	vec4 snowColour = (texture(snowTex, tilesCoords) * blendMapColour.r) * 2;
	vec4 rockColour = (texture(rockTex, tilesCoords) * blendMapColour.g) *0.7;
	rockColour.a = 1; // set alpha back to not being transparent
	snowColour.a *= 0.5;

	vec4 finalColour = (snowColour + rockColour + grassColour);

	vec3 gBumpNormal = texture (grassBump, tilesCoords/6).rgb * blendMapColour.b;
	gBumpNormal = normalize(TBN * normalize(gBumpNormal * 2.0 - 1.0));
	
	vec3 sBumpNormal = texture (snowBump, tilesCoords).rgb * blendMapColour.r ;
	sBumpNormal = normalize(TBN * normalize(sBumpNormal * 2.0 - 1.0));
	
	vec3 rBumpNormal = texture (rockBump, tilesCoords).rgb * blendMapColour.g ;
	rBumpNormal = normalize(TBN * normalize(rBumpNormal * 2.0 - 1.0));

	vec3 finalBump = normalize(gBumpNormal + sBumpNormal + rBumpNormal);


	float lambert = max( dot( incident , finalBump ) , 0.0f );
	float distance = length( lightPos - IN.worldPos );
	float attenuation = 1.0 - clamp( distance / lightRadius , 0.0 , 1.0);

	float specFactor = clamp( dot( halfDir , finalBump ) ,0.0 ,1.0);
	//specFactor = pow ( specFactor , 0 );


	float shadow = 1.0; 
	vec3 shadowNDC = IN . shadowProj.xyz / IN.shadowProj.w ;
	vec3 biasCoord = shadowNDC *0.5f + 0.5f ;
	
	if( abs ( shadowNDC . x ) < 1.0f && abs ( shadowNDC . y ) < 1.0f 
									&& abs ( shadowNDC . z ) < 1.0f ) {
	vec3 biasCoord = shadowNDC *0.5f + 0.5f ;
	float shadowZ = texture ( shadowTex , biasCoord . xy ). x ;
	
		if( shadowZ < biasCoord . z ) {
		shadow = 0.4f;
		}
	}

	if(theta > spotAngle && shadow != 0.4) {
		vec3 surface = ( finalColour.rgb * lightColour.rgb );
		vec3 spotHalfDir = normalize ( spotIncident + viewDir );
		float spotLambert = max( dot( spotIncident , finalBump ) , 0.0f );
		float spotlightDistance = length( spotPos - IN.worldPos );
		float spotAttenuation = 1.0 - clamp( distance / spotRadius , 0.0 , 1.0);
		fragColour.rgb += surface * spotLambert * spotAttenuation ;
		fragColour.rgb += ( spotColour.rgb * spotAttenuation );
		fragColour.rgb += mix(spotColour.rgb,finalColour.rgb, 0.5) ;
		fragColour.a = finalColour.a;
	}
	else {
		vec3 surface = ( finalColour.rgb * lightColour.rgb );
		fragColour.rgb += surface * lambert * attenuation ;
		fragColour.rgb += ( lightColour.rgb * attenuation )* specFactor *0.33;
		fragColour.rgb += surface * 0.8f ; // ambient !
		fragColour.rgb *= shadow;
		fragColour.a = finalColour.a ;
	}
}
