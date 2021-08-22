#include "action.h"
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdio.h>

int parseGlfwKey(const char *key) {
    if (strcmp("GLFW_KEY_SPACE", key) == 0) {
        return GLFW_KEY_SPACE;
    } else if (strcmp("GLFW_KEY_APOSTROPHE", key) == 0) {
        return GLFW_KEY_APOSTROPHE;
    } else if (strcmp("GLFW_KEY_COMMA", key) == 0) {
        return GLFW_KEY_COMMA;
    } else if (strcmp("GLFW_KEY_MINUS", key) == 0) {
        return GLFW_KEY_MINUS;
    } else if (strcmp("GLFW_KEY_PERIOD", key) == 0) {
        return GLFW_KEY_PERIOD;
    } else if (strcmp("GLFW_KEY_SLASH", key) == 0) {
        return GLFW_KEY_SLASH;
    } else if (strcmp("GLFW_KEY_0", key) == 0) {
        return GLFW_KEY_0;
    } else if (strcmp("GLFW_KEY_1", key) == 0) {
        return GLFW_KEY_1;
    } else if (strcmp("GLFW_KEY_2", key) == 0) {
        return GLFW_KEY_2;
    } else if (strcmp("GLFW_KEY_3", key) == 0) {
        return GLFW_KEY_3;
    } else if (strcmp("GLFW_KEY_4", key) == 0) {
        return GLFW_KEY_4;
    } else if (strcmp("GLFW_KEY_5", key) == 0) {
        return GLFW_KEY_5;
    } else if (strcmp("GLFW_KEY_6", key) == 0) {
        return GLFW_KEY_6;
    } else if (strcmp("GLFW_KEY_7", key) == 0) {
        return GLFW_KEY_7;
    } else if (strcmp("GLFW_KEY_8", key) == 0) {
        return GLFW_KEY_8;
    } else if (strcmp("GLFW_KEY_9", key) == 0) {
        return GLFW_KEY_9;
    } else if (strcmp("GLFW_KEY_SEMICOLON", key) == 0) {
        return GLFW_KEY_SEMICOLON;
    } else if (strcmp("GLFW_KEY_EQUAL", key) == 0) {
        return GLFW_KEY_EQUAL;
    } else if (strcmp("GLFW_KEY_A", key) == 0) {
        return GLFW_KEY_A;
    } else if (strcmp("GLFW_KEY_B", key) == 0) {
        return GLFW_KEY_B;
    } else if (strcmp("GLFW_KEY_C", key) == 0) {
        return GLFW_KEY_C;
    } else if (strcmp("GLFW_KEY_D", key) == 0) {
        return GLFW_KEY_D;
    } else if (strcmp("GLFW_KEY_E", key) == 0) {
        return GLFW_KEY_E;
    } else if (strcmp("GLFW_KEY_F", key) == 0) {
        return GLFW_KEY_F;
    } else if (strcmp("GLFW_KEY_G", key) == 0) {
        return GLFW_KEY_G;
    } else if (strcmp("GLFW_KEY_H", key) == 0) {
        return GLFW_KEY_H;
    } else if (strcmp("GLFW_KEY_I", key) == 0) {
        return GLFW_KEY_I;
    } else if (strcmp("GLFW_KEY_J", key) == 0) {
        return GLFW_KEY_J;
    } else if (strcmp("GLFW_KEY_K", key) == 0) {
        return GLFW_KEY_K;
    } else if (strcmp("GLFW_KEY_L", key) == 0) {
        return GLFW_KEY_L;
    } else if (strcmp("GLFW_KEY_M", key) == 0) {
        return GLFW_KEY_M;
    } else if (strcmp("GLFW_KEY_N", key) == 0) {
        return GLFW_KEY_N;
    } else if (strcmp("GLFW_KEY_O", key) == 0) {
        return GLFW_KEY_O;
    } else if (strcmp("GLFW_KEY_P", key) == 0) {
        return GLFW_KEY_P;
    } else if (strcmp("GLFW_KEY_Q", key) == 0) {
        return GLFW_KEY_Q;
    } else if (strcmp("GLFW_KEY_R", key) == 0) {
        return GLFW_KEY_R;
    } else if (strcmp("GLFW_KEY_S", key) == 0) {
        return GLFW_KEY_S;
    } else if (strcmp("GLFW_KEY_T", key) == 0) {
        return GLFW_KEY_T;
    } else if (strcmp("GLFW_KEY_U", key) == 0) {
        return GLFW_KEY_U;
    } else if (strcmp("GLFW_KEY_V", key) == 0) {
        return GLFW_KEY_V;
    } else if (strcmp("GLFW_KEY_W", key) == 0) {
        return GLFW_KEY_W;
    } else if (strcmp("GLFW_KEY_X", key) == 0) {
        return GLFW_KEY_X;
    } else if (strcmp("GLFW_KEY_Y", key) == 0) {
        return GLFW_KEY_Y;
    } else if (strcmp("GLFW_KEY_Z", key) == 0) {
        return GLFW_KEY_Z;
    } else if (strcmp("GLFW_KEY_LEFT_BRACKET", key) == 0) {
        return GLFW_KEY_LEFT_BRACKET;
    } else if (strcmp("GLFW_KEY_BACKSLASH", key) == 0) {
        return GLFW_KEY_BACKSLASH;
    } else if (strcmp("GLFW_KEY_RIGHT_BRACKET", key) == 0) {
        return GLFW_KEY_RIGHT_BRACKET;
    } else if (strcmp("GLFW_KEY_GRAVE_ACCENT", key) == 0) {
        return GLFW_KEY_GRAVE_ACCENT;
    } else if (strcmp("GLFW_KEY_WORLD_1", key) == 0) {
        return GLFW_KEY_WORLD_1;
    } else if (strcmp("GLFW_KEY_WORLD_2", key) == 0) {
        return GLFW_KEY_WORLD_2;
    } 
    
    else if (strcmp("GLFW_KEY_ESCAPE", key) == 0) {
        return GLFW_KEY_ESCAPE;
    } else if (strcmp("GLFW_KEY_ENTER", key) == 0) {
        return GLFW_KEY_ENTER;
    } else if (strcmp("GLFW_KEY_TAB", key) == 0) {
        return GLFW_KEY_TAB;
    } else if (strcmp("GLFW_KEY_BACKSPACE", key) == 0) {
        return GLFW_KEY_BACKSPACE;
    } else if (strcmp("GLFW_KEY_INSERT", key) == 0) {
        return GLFW_KEY_INSERT;
    } else if (strcmp("GLFW_KEY_DELETE", key) == 0) {
        return GLFW_KEY_DELETE;
    } else if (strcmp("GLFW_KEY_RIGHT", key) == 0) {
        return GLFW_KEY_RIGHT;
    } else if (strcmp("GLFW_KEY_LEFT", key) == 0) {
        return GLFW_KEY_LEFT;
    } else if (strcmp("GLFW_KEY_DOWN", key) == 0) {
        return GLFW_KEY_DOWN;
    } else if (strcmp("GLFW_KEY_UP", key) == 0) {
        return GLFW_KEY_UP;
    } else if (strcmp("GLFW_KEY_PAGE_UP", key) == 0) {
        return GLFW_KEY_PAGE_UP;
    } else if (strcmp("GLFW_KEY_PAGE_DOWN", key) == 0) {
        return GLFW_KEY_PAGE_DOWN;
    } else if (strcmp("GLFW_KEY_HOME", key) == 0) {
        return GLFW_KEY_HOME;
    } else if (strcmp("GLFW_KEY_END", key) == 0) {
        return GLFW_KEY_END;
    } else if (strcmp("GLFW_KEY_CAPS_LOCK", key) == 0) {
        return GLFW_KEY_CAPS_LOCK;
    } else if (strcmp("GLFW_KEY_SCROLL_LOCK", key) == 0) {
        return GLFW_KEY_SCROLL_LOCK;
    } else if (strcmp("GLFW_KEY_NUM_LOCK", key) == 0) {
        return GLFW_KEY_NUM_LOCK;
    } else if (strcmp("GLFW_KEY_PRINT_SCREEN", key) == 0) {
        return GLFW_KEY_PRINT_SCREEN;
    } else if (strcmp("GLFW_KEY_PAUSE", key) == 0) {
        return GLFW_KEY_PAUSE;
    } else if (strcmp("GLFW_KEY_F1", key) == 0) {
        return GLFW_KEY_F1;
    } else if (strcmp("GLFW_KEY_F2", key) == 0) {
        return GLFW_KEY_F2;
    } else if (strcmp("GLFW_KEY_F3", key) == 0) {
        return GLFW_KEY_F3;
    } else if (strcmp("GLFW_KEY_F4", key) == 0) {
        return GLFW_KEY_F4;
    } else if (strcmp("GLFW_KEY_F5", key) == 0) {
        return GLFW_KEY_F5;
    } else if (strcmp("GLFW_KEY_F6", key) == 0) {
        return GLFW_KEY_F6;
    } else if (strcmp("GLFW_KEY_F7", key) == 0) {
        return GLFW_KEY_F7;
    } else if (strcmp("GLFW_KEY_F8", key) == 0) {
        return GLFW_KEY_F8;
    } else if (strcmp("GLFW_KEY_F9", key) == 0) {
        return GLFW_KEY_F9;
    } else if (strcmp("GLFW_KEY_F10", key) == 0) {
        return GLFW_KEY_F10;
    } else if (strcmp("GLFW_KEY_F11", key) == 0) {
        return GLFW_KEY_F11;
    } else if (strcmp("GLFW_KEY_F12", key) == 0) {
        return GLFW_KEY_F12;
    } else if (strcmp("GLFW_KEY_F13", key) == 0) {
        return GLFW_KEY_F13;
    } else if (strcmp("GLFW_KEY_F14", key) == 0) {
        return GLFW_KEY_F14;
    } else if (strcmp("GLFW_KEY_F15", key) == 0) {
        return GLFW_KEY_F15;
    } else if (strcmp("GLFW_KEY_F16", key) == 0) {
        return GLFW_KEY_F16;
    } else if (strcmp("GLFW_KEY_F17", key) == 0) {
        return GLFW_KEY_F17;
    } else if (strcmp("GLFW_KEY_F18", key) == 0) {
        return GLFW_KEY_F18;
    } else if (strcmp("GLFW_KEY_F19", key) == 0) {
        return GLFW_KEY_F19;
    } else if (strcmp("GLFW_KEY_F20", key) == 0) {
        return GLFW_KEY_F20;
    } else if (strcmp("GLFW_KEY_F21", key) == 0) {
        return GLFW_KEY_F21;
    } else if (strcmp("GLFW_KEY_F22", key) == 0) {
        return GLFW_KEY_F22;
    } else if (strcmp("GLFW_KEY_F23", key) == 0) {
        return GLFW_KEY_F23;
    } else if (strcmp("GLFW_KEY_F24", key) == 0) {
        return GLFW_KEY_F24;
    } else if (strcmp("GLFW_KEY_F25", key) == 0) {
        return GLFW_KEY_F25;
    } else if (strcmp("GLFW_KEY_KP_0", key) == 0) {
        return GLFW_KEY_KP_0;
    } else if (strcmp("GLFW_KEY_KP_1", key) == 0) {
        return GLFW_KEY_KP_1;
    } else if (strcmp("GLFW_KEY_KP_2", key) == 0) {
        return GLFW_KEY_KP_2;
    } else if (strcmp("GLFW_KEY_KP_3", key) == 0) {
        return GLFW_KEY_KP_3;
    } else if (strcmp("GLFW_KEY_KP_4", key) == 0) {
        return GLFW_KEY_KP_4;
    } else if (strcmp("GLFW_KEY_KP_5", key) == 0) {
        return GLFW_KEY_KP_5;
    } else if (strcmp("GLFW_KEY_KP_6", key) == 0) {
        return GLFW_KEY_KP_6;
    } else if (strcmp("GLFW_KEY_KP_7", key) == 0) {
        return GLFW_KEY_KP_7;
    } else if (strcmp("GLFW_KEY_KP_8", key) == 0) {
        return GLFW_KEY_KP_8;
    } else if (strcmp("GLFW_KEY_KP_9", key) == 0) {
        return GLFW_KEY_KP_9;
    } else if (strcmp("GLFW_KEY_KP_DECIMAL", key) == 0) {
        return GLFW_KEY_KP_DECIMAL;
    } else if (strcmp("GLFW_KEY_KP_DIVIDE", key) == 0) {
        return GLFW_KEY_KP_DIVIDE;
    } else if (strcmp("GLFW_KEY_KP_MULTIPLY", key) == 0) {
        return GLFW_KEY_KP_MULTIPLY;
    } else if (strcmp("GLFW_KEY_KP_SUBTRACT", key) == 0) {
        return GLFW_KEY_KP_SUBTRACT;
    } else if (strcmp("GLFW_KEY_KP_ADD", key) == 0) {
        return GLFW_KEY_KP_ADD;
    } else if (strcmp("GLFW_KEY_KP_ENTER", key) == 0) {
        return GLFW_KEY_KP_ENTER;
    } else if (strcmp("GLFW_KEY_KP_EQUAL", key) == 0) {
        return GLFW_KEY_KP_EQUAL;
    } else if (strcmp("GLFW_KEY_LEFT_SHIFT", key) == 0) {
        return GLFW_KEY_LEFT_SHIFT;
    } else if (strcmp("GLFW_KEY_LEFT_CONTROL", key) == 0) {
        return GLFW_KEY_LEFT_CONTROL;
    } else if (strcmp("GLFW_KEY_LEFT_ALT", key) == 0) {
        return GLFW_KEY_LEFT_ALT;
    } else if (strcmp("GLFW_KEY_LEFT_SUPER", key) == 0) {
        return GLFW_KEY_LEFT_SUPER;
    } else if (strcmp("GLFW_KEY_RIGHT_SHIFT", key) == 0) {
        return GLFW_KEY_RIGHT_SHIFT;
    } else if (strcmp("GLFW_KEY_RIGHT_CONTROL", key) == 0) {
        return GLFW_KEY_RIGHT_CONTROL;
    } else if (strcmp("GLFW_KEY_RIGHT_ALT", key) == 0) {
        return GLFW_KEY_RIGHT_ALT;
    } else if (strcmp("GLFW_KEY_RIGHT_SUPER", key) == 0) {
        return GLFW_KEY_RIGHT_SUPER;
    } else if (strcmp("GLFW_KEY_MENU", key) == 0) {
        return GLFW_KEY_MENU;
    } else {
        printf("Unknown GLFW key code: %s\n", key);
    }
    return GLFW_KEY_UNKNOWN;
}

enum ActionTargets parseActionTarget(const char *target) {
    if (strcmp("MODEL", target) == 0) {
         return ACTION_TARGET_MODEL;
    } else if (strcmp("CAMERA", target) == 0) {
         return ACTION_TARGET_CAMERA;
    } else if (strcmp("MUSIC_PLAYER", target) == 0) {
         return ACTION_TARGET_MUSIC;
    } else {
        printf("Unknown action target: %s\n", target);
    }
    return ACTION_TARGET_UNKNOWN;
}

enum Actions parseActionKind(const char *kind) {
    if (strcmp("MOVE", kind) == 0) {
         return ACTION_MOVE;
    } else if (strcmp("ROTATE", kind) == 0) {
         return ACTION_ROTATE;
    } else if (strcmp("SWITCH", kind) == 0) {
         return ACTION_SWITCH;
    }  else if (strcmp("ZOOM", kind) == 0) {
         return ACTION_ZOOM;
    } 
    
    else if (strcmp("PAUSE", kind) == 0) {
         return ACTION_PAUSE;
    } else if (strcmp("FORWARD", kind) == 0) {
         return ACTION_FORWARD;
    } else {
        printf("Unknown action kind: %s\n", kind);
    }
    return ACTION_UNKNOWN;
}

enum ActionAdditionalInfo parseActionAdditionalInfo(const char *info) {
    if (strcmp("FORWARD_X", info) == 0) {
        return ACTION_FORWARD_X;
    } else if (strcmp("BACKWARD_X", info) == 0) {
        return ACTION_BACKWARD_X;
    } else if (strcmp("FORWARD_Y", info) == 0) {
        return ACTION_FORWARD_Y;
    } else if (strcmp("BACKWARD_Y", info) == 0) {
        return ACTION_BACKWARD_Y;
    } else if (strcmp("FORWARD_Z", info) == 0) {
        return ACTION_FORWARD_Z;
    } else if (strcmp("BACKWARD_Z", info) == 0) {
        return ACTION_BACKWARD_Z;
    } else if (strcmp("IN", info) == 0) {
        return ACTION_IN;
    } else if (strcmp("OUT", info) == 0) {
        return ACTION_OUT;
    }  else {
        printf("Unknown action additional info: %s\n", info);
    }
    return ACTION_NOTHING;
}
