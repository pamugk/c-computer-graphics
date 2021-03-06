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

uniform vec3 u_olpos; // Позиция источника света
uniform vec3 u_olcol; // Цвет света
uniform vec3 u_oeye; // Позиция наблюдателя
uniform float u_odmin; // Минимально допустимый уровень освещённости объекта в точке P
uniform float u_osfoc;  // Сфокусированность зеркального блика на поверхности освещаемого объекта в точке P
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

in vec3 v_pos; // Позиция вершины
in vec3 v_normal; // Нормаль
in flat int i_material;
in vec2 v_texCoord;

layout(location = 0) out vec4 o_color;

void main()
{
    Material material = materials[i_material];
    
    vec3 l = normalize(v_pos - u_olpos); // Нормированный вектор падения света
    float cosa = dot(l, v_normal); // Косинус угла падения луча света
    float d = max(cosa, u_odmin); // Коэффициент диффузного освещения
    vec3 r = reflect(l, v_normal); // Вектор отражения
    vec3 e = normalize(u_oeye - v_pos); // Ось зрения наблюдателя
    float s = max(pow(dot(r, e), material.specularExponent), 0.0) * (int(cosa >= 0.0)); // Коэффициент зеркального блика, при cosa < 0 обнуляется для устранения бликов на обратной источнику света стороне
    
    vec3 ambientColor = material.ambientColor + texture(u_ambientMap, vec3(v_texCoord, material.ambientTextureIdx)).rgb;
    vec3 diffuseColor = material.diffuseColor + texture(u_diffuseMap, vec3(v_texCoord, material.diffuseTextureIdx)).rgb;
    vec3 specularColor = material.specularColor + texture(u_specularMap, vec3(v_texCoord, material.specularTextureIdx)).rgb;
    
    o_color = mix(int(u_lie) * vec4(u_olcol * (u_alstr * ambientColor + diffuseColor * d + specularColor * s) + material.emissiveColor, material.opacity) + int(!u_lie) * vec4(u_alstr * ambientColor + material.emissiveColor, material.opacity), vec4(material.transmissionFilterColor, 1.0 - material.opacity), material.opacity);
}
