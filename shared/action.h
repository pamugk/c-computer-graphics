#ifndef ACTION
#define ACTION

enum ActionTargets {
    ACTION_TARGET_UNKNOWN = -1,
    ACTION_TARGET_MODEL,
    ACTION_TARGET_CAMERA,
    ACTION_TARGET_MUSIC
};

enum Actions {
    ACTION_UNKNOWN = -1,
    ACTION_MOVE,
    ACTION_ROTATE,
    ACTION_SWITCH,
    ACTION_ZOOM,
    ACTION_PAUSE,
    ACTION_FORWARD
};

enum ActionAdditionalInfo {
    ACTION_NOTHING,
    ACTION_FORWARD_X,
    ACTION_BACKWARD_X,
    ACTION_FORWARD_Y,
    ACTION_BACKWARD_Y,
    ACTION_FORWARD_Z,
    ACTION_BACKWARD_Z,
    ACTION_IN,
    ACTION_OUT
};

struct action {
    int key;
    enum ActionTargets target;
    int idx;
    enum Actions kind;
    enum ActionAdditionalInfo additionalInfo;
};

int parseGlfwKey(const char *key);
enum ActionTargets parseActionTarget(const char *target);
enum Actions parseActionKind(const char *kind);
enum ActionAdditionalInfo parseActionAdditionalInfo(const char *info);

#endif // ACTION
