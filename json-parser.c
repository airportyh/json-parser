#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

struct ParseJsonValueResult parseValue(char *input, int cursor);
struct ParseWhiteSpaceResult parseWs(char *input, int cursor);
struct ParseJsonValueResult parseElement(char *input, int cursor);

struct ParseStringResult parseCharacters(char *input, int cursor) {
    struct ParseStringResult result;
    int currentCursor = cursor;
    while (true) {
        char chr = input[currentCursor];
        if (chr == '\\') {
            // TODO parse escape
            // const escape = parseEscape(input, currentCursor + 1);
        } else if (chr == '"') {
            // done with collection
            int length = currentCursor - cursor;
            char *string = malloc(sizeof(char) * length + 1);
            strncpy(string, input + cursor, length);
            string[length] = '\0';
            result.string = string;
            result.cursor = currentCursor;
            return result;
        } else {
            currentCursor++;
        }    
    }    
}

struct ParseJsonValueResult parseString(char *input, int cursor) {
    struct ParseJsonValueResult result;
    if (input[cursor] == '"') {
        struct ParseStringResult characters = parseCharacters(input, cursor + 1);
        if (characters.string != NULL) {
            if (input[characters.cursor] == '"') {
                // string complete!
                struct JsonValue *stringValue = malloc(sizeof(struct JsonValue));
                stringValue->type = JsonString;
                stringValue->data.string = characters.string;
                result.value = stringValue;
                result.cursor = characters.cursor + 1;
                return result;
            }
        }
    }
    result.value = NULL;
    return result;
}

struct ParseObjectEntryResult parseMember(char *input, int cursor) {
    struct ParseObjectEntryResult result;
    struct ParseWhiteSpaceResult ws = parseWs(input, cursor);
    struct ParseJsonValueResult string = parseString(input, ws.cursor);
    if (string.value) {
        if (input[string.cursor] == ':') {
            struct ParseJsonValueResult element = parseElement(input, string.cursor + 1);
            if (element.value != NULL) {
                // success!
                struct ObjectEntry *objectEntry = malloc(sizeof(struct ObjectEntry));
                objectEntry->key = string.value->data.string;
                free(string.value);
                objectEntry->value = element.value;
                objectEntry->next = NULL;
                result.object = objectEntry;
                result.cursor = element.cursor;
                return result;
            }
        }
    }
    
    result.object = NULL;
    return result;
}

struct ParseJsonValueResult parseMembers(char *input, int cursor) {
    struct ParseJsonValueResult result;
    struct ParseObjectEntryResult member = parseMember(input, cursor);
    if (member.object != NULL) {
        struct JsonValue *objectValue = malloc(sizeof(struct JsonValue));
        objectValue->type = JsonObject;
        objectValue->data.object = member.object;
        while (true) {
            if (input[member.cursor] == ',') {
                struct ParseObjectEntryResult nextMember = parseMember(input, member.cursor + 1);
                if (nextMember.object != NULL) {
                    member.object->next = nextMember.object;
                    cursor = nextMember.cursor;
                    member = nextMember;
                }
            } else {
                result.value = objectValue;
                result.cursor = member.cursor;
                return result;
                break;
            }
        }
    }
    result.value = NULL;
    return result;
}

struct ParseJsonValueResult parseObject(char *input, int cursor) {
    struct ParseJsonValueResult result;
    if (input[cursor] == '{') {
        struct ParseJsonValueResult members = parseMembers(input, cursor + 1);
        if (members.value != NULL) {
            if (input[members.cursor] == '}') {
                result.value = members.value;
                result.cursor = members.cursor + 1;
                return result;
            } else {
                // TODO clean up members stuff
                result.value = NULL;
                return result;
            }
        } else {
            struct ParseWhiteSpaceResult ws = parseWs(input, cursor + 1);
            if (input[ws.cursor] == '}') {
                // empty object parsed
                struct JsonValue *object = malloc(sizeof(struct JsonValue));
                object->type = JsonObject;
                object->data.object = NULL;
                result.value = object;
                result.cursor = ws.cursor + 1;
                return result;
            }
        }
    }
    result.value = NULL;
    return result;
}

struct ParseWhiteSpaceResult parseWs(char *input, int cursor) {
    struct ParseWhiteSpaceResult result;
    while (true) {
        char chr = input[cursor];
        if (chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r') {
            cursor++;
        } else {
            break;
        }
    }
    result.cursor = cursor;
    return result;
}

struct ParseJsonValueResult parseElement(char *input, int cursor) {
    struct ParseJsonValueResult result;
    struct ParseWhiteSpaceResult ws = parseWs(input, cursor);
    struct ParseJsonValueResult value = parseValue(input, ws.cursor);
    if (value.value != NULL) {
        struct ParseWhiteSpaceResult ws2 = parseWs(input, value.cursor);
        result.value = value.value;
        result.cursor = ws2.cursor;
        return result;
    }
    result.value = NULL;
    return result;
}

struct ParseJsonValueResult parseElements(char *input, int cursor) {
    struct ParseJsonValueResult result;
    
    struct ParseJsonValueResult element = parseElement(input, cursor);
    if (element.value != NULL) {
        struct ArrayEntry *arrayEntry = malloc(sizeof(struct ArrayEntry));
        struct JsonValue *arrayValue = malloc(sizeof(struct JsonValue));
        arrayEntry->value = element.value;
        arrayEntry->next = NULL;
        arrayValue->type = JsonArray;
        arrayValue->data.array = arrayEntry;
        result.value = arrayValue;
        int currentCursor = element.cursor;
    
        while (true) {
            if (input[currentCursor] == ',') {
                struct ParseJsonValueResult element = parseElement(input, currentCursor + 1);
                if (element.value != NULL) {
                    arrayEntry->next = malloc(sizeof(struct ArrayEntry));
                    arrayEntry = arrayEntry->next;
                    arrayEntry->value = element.value;
                    arrayEntry->next = NULL;
                    currentCursor = element.cursor;
                } else {
                    // TODO clean up all the allocated stuff...
                    result.value = NULL;
                    return result;
                }
            } else {
                break;
            }
        }
        
        result.cursor = currentCursor;
        return result;
    }
    result.value = NULL;
    return result;
}

struct ParseJsonValueResult parseArray(char *input, int cursor) {
    struct ParseJsonValueResult result;
    
    if (input[cursor] == '[') {
        struct ParseJsonValueResult elements = parseElements(input, cursor + 1);
        if (elements.value != NULL) {
            if (input[elements.cursor] == ']') {
                result.value = elements.value;
                result.cursor = elements.cursor + 1;
                return result;
            }
        } else {
            struct ParseWhiteSpaceResult ws = parseWs(input, cursor + 1);
            if (input[ws.cursor] == ']') {
                struct JsonValue *arrayValue = malloc(sizeof(struct JsonValue));
                arrayValue->type = JsonArray;
                arrayValue->data.array = NULL;
                result.value = arrayValue;
                result.cursor = ws.cursor + 1;
                return result;
            }
        }
    }
    result.value = NULL;
    return result;
}

struct ParseJsonValueResult parseInteger(char *input, int cursor) {
    struct ParseJsonValueResult result;
    char chr = input[cursor++];
    if (chr >= 48 && chr <= 57) {
        double number = chr - 48;
        while (true) {
            chr = input[cursor++];
            if (chr >= 48 && chr <= 57) { /* 0 - 9 */
                number = (number * 10) + (chr - 48);
            } else {
                cursor--;
                break;
            }
        }
        struct JsonValue *value = malloc(sizeof(struct JsonValue));
        value->type = JsonNumber;
        value->data.number = number;
        result.value = value;
        result.cursor = cursor;
        return result;
    }
    result.value = NULL;
    return result;
}

// TODO decimals and exponents
struct ParseJsonValueResult parseNumber(char *input, int cursor) {
    return parseInteger(input, cursor);
}

struct ParseJsonValueResult parseBoolean(char *input, int cursor) {
    struct ParseJsonValueResult result;
    if (strncmp(input + cursor, "true", 4) == 0) {
        struct JsonValue *value = malloc(sizeof(struct JsonValue));
        value->type = JsonBoolean;
        value->data.boolean = true;
        result.value = value;
        result.cursor = cursor + 4;
        return result;
    } else if (strncmp(input + cursor, "false", 5) == 0) {
        struct JsonValue *value = malloc(sizeof(struct JsonValue));
        value->type = JsonBoolean;
        value->data.boolean = false;
        result.value = value;
        result.cursor = cursor + 5;
        return result;
    }
    result.value = NULL;
    return result;
}

struct ParseJsonValueResult parseNull(char *input, int cursor) {
    struct ParseJsonValueResult result;
    if (strncmp(input + cursor, "null", 4) == 0) {
        struct JsonValue *value = malloc(sizeof(struct JsonValue));
        value->type = JsonNull;
        result.value = value;
        result.cursor = cursor + 4;
        return result;
    }
    result.value = NULL;
    return result;
}

struct ParseJsonValueResult parseValue(char *input, int cursor) {
    struct ParseJsonValueResult null = parseNull(input, cursor);
    if (null.value != NULL) {
        return null;
    }
    struct ParseJsonValueResult boolean = parseBoolean(input, cursor);
    if (boolean.value != NULL) {
        return boolean;
    }
    struct ParseJsonValueResult array = parseArray(input, cursor);
    if (array.value != NULL) {
        return array;
    }
    struct ParseJsonValueResult object = parseObject(input, cursor);
    if (object.value != NULL) {
        return object;
    }
    struct ParseJsonValueResult string = parseString(input, cursor);
    if (string.value != NULL) {
        return string;
    }
    return parseNumber(input, cursor);
}

struct ParseJsonValueResult parseJson(char *input, int cursor) {
    return parseElement(input, cursor);
}