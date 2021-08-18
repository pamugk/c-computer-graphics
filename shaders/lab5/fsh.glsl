#version 450

struct Material {
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float specularExponent;
    
    float opaque;
    vec3 transmissionFilterColor;
    
    float refractionIndex;
    int illum;
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
    vec3 l = normalize(v_pos - u_olpos); // Нормированный вектор падения света
    float cosa = dot(l, v_normal); // Косинус угла падения луча света
    float d = max(cosa, u_odmin); // Коэффициент диффузного освещения
    vec3 r = reflect(l, v_normal); // Вектор отражения
    vec3 e = normalize(u_oeye - v_pos); // Ось зрения наблюдателя
    float s = max(pow(dot(r, e), u_osfoc), 0.0) * (int(cosa >= 0.0)); // Коэффициент зеркального блика, при cosa < 0 обнуляется для устранения бликов на обратной источнику света стороне
    vec4 texColor = vec4(materials.length());
    o_color = int(u_lie) * vec4(u_olcol * (d * texColor.xyz + s), 1.0) + int(!u_lie) * texColor;
    //o_color = vec4(MaterialBlock.materials[i_material].diffuseColor, MaterialBlock.materials[i_material].opaque);
}
