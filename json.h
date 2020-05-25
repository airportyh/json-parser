#include <stdbool.h>

enum JsonType {
    JsonString, 
    JsonNumber, 
    JsonArray, 
    JsonObject, 
    JsonBoolean, 
    JsonNull
};

union JsonData {
    char *string;
    double number;
    struct ArrayEntry *array;
    struct ObjectEntry *object;
    bool boolean;
};

struct JsonValue {
    enum JsonType type;
    union JsonData data;
};

struct ArrayEntry {
    struct JsonValue *value;
    struct ArrayEntry *next;
};

struct ObjectEntry {
    char *key;
    struct JsonValue *value;
    struct ObjectEntry *next;
};

struct ParseJsonValueResult {
    struct JsonValue *value;
    int cursor;
};

struct ParseArrayEntryResult {
    struct ArrayEntry *array;
    int cursor;
};

struct ParseObjectEntryResult {
    struct ObjectEntry *object;
    int cursor;
};

struct ParseStringResult {
    char *string;
    int cursor;
};

struct ParseWhiteSpaceResult {
    int cursor;
};

void printJson(struct JsonValue *value);
struct ParseJsonValueResult parseJson(char *input, int cursor);