std::string simple_light_frag = R"(
#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    vec3 viewFragPos;
    vec3 viewNormal;
} fs_in;

struct PointLight {
  vec3 position;

  float constant;
  float linear;
  float quadratic;
  float intensity;

 // vec3 ambient;
  vec3 diffuse;
  //vec3 specular;
};

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
uniform samplerCube reflectionMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool lightRim;
uniform bool reflective;
#define NR_POINT_LIGHTS 2
uniform PointLight pointLights[NR_POINT_LIGHTS];

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float bias = 0.002;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

vec3 CalculatePointLight(PointLight light, vec3 norm, vec3 FragPos, vec3 viewDir, float intensity)
{
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);

  vec3 reflectDir = reflect(-lightDir, norm);
 // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  float distance = length(light.position - FragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  //vec3 ambient = light.ambient * vec3(texture(diffuseTexture, fs_in.TexCoords));
  vec3 diffuse = diff * light.diffuse * vec3(texture(diffuseTexture, fs_in.TexCoords));
  //vec3 specular = spec * light.specular * vec3(texture(specularTexture, fs_in.TexCoords));

 // ambient *= attenuation;
  diffuse *= attenuation;
  //specular *= attenuation;

  //return (ambient + diffuse + specular);
  return diffuse * intensity;
}

void main()
{           
    float reflectionAmount = 0.1;
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewNorm = normalize(fs_in.viewNormal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.1 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);   
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
    {
      diffuse += CalculatePointLight(pointLights[i], normal, fs_in.FragPos, viewDir, pointLights[i].intensity);
      //diffuse += vec3(1.0, 0.0, 0.0);
    }

    if (lightRim) {
      vec3 eye = normalize(-fs_in.viewFragPos.xyz);
      vec3 n = normalize(fs_in.Normal);
      vec3 coltest = vec3(0.56, 0.83, 1.0);
      float rimIntensity = 0.2;
      float rim = 1.0 - max(dot(viewDir, n), 0.0);
      rim = smoothstep(0.6, 1.0, rim);
      rim *= rimIntensity;
      coltest = vec3(rim * coltest.r, rim * coltest.g, rim * coltest.b);
      diffuse *= coltest;
      ambient += coltest;
    }
    vec4 reflectiveColour = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 ref = vec3(1.0, 1.0, 1.0);
    if (reflective)
    {
      float ratio = 1.00 / 1.52;
      vec3 I = normalize(fs_in.FragPos - viewPos);
      vec3 R = reflect(I, normalize(fs_in.Normal));
      vec3 RF = refract(I, normalize(fs_in.Normal), ratio);
      ref = texture(reflectionMap, RF).rgb;
      reflectiveColour = vec4(texture(reflectionMap, R).rgb, 1.0);
    } else {
      reflectionAmount = 0.0;
    }
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    FragColor = mix(vec4(lighting, 1.0), reflectiveColour, reflectionAmount);
}
)";
