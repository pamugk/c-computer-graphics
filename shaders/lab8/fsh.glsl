#version 450

struct Material {
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    vec3 emissiveColor;
    float specularExponent;
    
    float opacity;
    vec3 transmissionFilterColor;
    
    float refractionIndex;
    int illum;
    
    int ambientTextureIdx;
    int diffuseTextureIdx;
    int specularTextureIdx;
    int specularHighlightComponentIdx;
    int alphaTextuerIdx;
    int bumpTextureIdx;
    int displacementIdx;
    int stencilDecalTextureIdx;
    int normalTextureIdx;
};

uniform vec3 u_olcol; // Цвет света
uniform float u_odmin; // Минимально допустимый уровень освещённости объекта в точке P
uniform float u_osfoc;  // сфокусированность зеркального блика на поверхности освещаемого объекта в точке P
uniform float u_alstr; // Сила окружающего освещения

layout(std430) buffer MaterialBlock {
    Material materials[];
};

uniform bool u_lie; // Признак использования модели освещения (вкл. / выкл.)

uniform sampler2DArray u_ambientMap;
uniform sampler2DArray u_diffuseMap;
uniform sampler2DArray u_specularMap;
uniform sampler2DArray u_specularHighlightComponent;
uniform sampler2DArray u_alphaMap;
uniform sampler2DArray u_bumpMap;
uniform sampler2DArray u_displacementMap;
uniform sampler2DArray u_stencilDecalMap;
uniform sampler2DArray u_normalMap;

in vec3 v_ray;
in vec3 v_eye;
in vec2 v_texCoord;
in flat int i_material;

layout(location = 0) out vec4 o_color;

void main()
{
    Material material = materials[i_material];
    
    vec3 v_normal = texture(u_normalMap, vec3(v_texCoord, material.normalTextureIdx)).rgb;
    float cosa = dot(v_ray, v_normal); // Косинус угла падения луча света
    float d = max(cosa, u_odmin); // Коэффициент диффузного освещения
    vec3 r = reflect(v_ray, v_normal); // Вектор отражения
    float s = pow(max(dot(r, v_eye), 0.0), material.specularExponent) * (int(cosa >= 0.0)); // Коэффициент зеркального блика, при cosa < 0 обнуляется для устранения бликов на обратной источнику света стороне
    
    vec3 ambientColor = material.ambientColor + texture(u_ambientMap, vec3(v_texCoord, material.ambientTextureIdx)).rgb;
    vec3 diffuseColor = material.diffuseColor + texture(u_diffuseMap, vec3(v_texCoord, material.diffuseTextureIdx)).rgb;
    vec3 specularColor = material.specularColor + texture(u_specularMap, vec3(v_texCoord, material.specularTextureIdx)).rgb;
    
    o_color = mix(int(u_lie) * vec4(u_olcol * (u_alstr * ambientColor + diffuseColor * d + specularColor * s) + material.emissiveColor, material.opacity) + int(!u_lie) * vec4(u_alstr * ambientColor + material.emissiveColor, material.opacity), vec4(material.transmissionFilterColor, 1.0 - material.opacity), material.opacity);
}
