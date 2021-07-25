#version 460

uniform vec3 u_olpos; // Позиция источника света
uniform vec3 u_olcol; // Цвет света
uniform vec3 u_oeye; // Позиция наблюдателя
uniform float u_odmin; // Минимально допустимый уровень освещённости объекта в точке P
uniform float u_osfoc;  // сфокусированность зеркального блика на поверхности освещаемого объекта в точке P

uniform bool u_lie; // Признак использования модели освещения (вкл. / выкл.)

uniform sampler2D u_map1; // Текстура 1
uniform sampler2D u_map2; // Текстура 2

in vec2 v_texCoord; // Текстурные координаты
flat in int v_texNum; // Используемая вершиной текстура
in vec3 v_normal; // Нормаль
in vec3 v_pos; // Позиция вершины

layout(location = 0) out vec4 o_color;

void main()
{
    vec3 l = normalize(v_pos - u_olpos); // Нормированный вектор падения света
    float cosa = dot(l, v_normal); // Косинус угла падения луча света
    float d = max(cosa, u_odmin); // Коэффициент диффузного освещения
    vec3 r = reflect(l, v_normal); // Вектор отражения
    vec3 e = normalize(u_oeye - v_pos); // Ось зрения наблюдателя
    float s = max(pow(dot(r, e), u_osfoc), 0.0) * (int(cosa >= 0.0)); // Коэффициент зеркального блика, при cosa < 0 обнуляется для устранения бликов на обратной источнику света стороне
    vec4 texColor = texture(u_map1, v_texCoord) * (1 - v_texNum) + texture(u_map2, v_texCoord) * (0 + v_texNum);
    /*vec4 texColor;
    if (v_texNum == 0) {
        texColor = texture(u_map1, v_texCoord);
    } else {
        texColor = texture(u_map2, v_texCoord);
    }*/
    o_color = int(u_lie) * vec4(u_olcol * (d * texColor.xyz + s), 1.0) + int(!u_lie) * texColor;
}
