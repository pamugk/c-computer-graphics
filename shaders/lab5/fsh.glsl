#version 450

struct Material {
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    vec3 emissiveColor;
    float specularExponent;
    
    float opaque;
    vec3 transmissionFilterColor;
    
    float refractionIndex;
};

uniform vec3 u_olpos; // Позиция источника света
uniform vec3 u_olcol; // Цвет света
uniform vec3 u_oeye; // Позиция наблюдателя
uniform float u_odmin; // Минимально допустимый уровень освещённости объекта в точке P
uniform float u_osfoc;  // сфокусированность зеркального блика на поверхности освещаемого объекта в точке P

buffer MaterialBlock {
    Material materials[];
};

uniform bool u_lie; // Признак использования модели освещения (вкл. / выкл.)

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
    
    o_color = mix(int(u_lie) * vec4(u_olcol * (material.ambientColor + material.diffuseColor * d + material.specularColor * s) + material.emissiveColor, material.opaque) + int(!u_lie) * vec4(material.ambientColor + material.emissiveColor, material.opaque), vec4(material.transmissionFilterColor, 1.0 - material.opaque), material.opaque);
}
