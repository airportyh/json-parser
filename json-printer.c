#include <stdio.h>
#include "json.h"

void display_array_items(struct ArrayEntry *array, bool first) {
    if (array == NULL) {
        return;
    }
    if (!first) {
        printf(", ");
    }
    printJson(array->value);
    display_array_items(array->next, false);
}

void display_array(struct ArrayEntry *array) {
    printf("[ ");
    display_array_items(array, true);
    printf(" ]");
}

void display_object_entries(struct ObjectEntry *object, bool first) {
    if (object == NULL) {
        return;
    }
    if (!first) {
        printf(", ");
    }
    printf("%s: ", object->key);
    printJson(object->value);
    display_object_entries(object->next, false);
}

void display_object(struct ObjectEntry *object) {
    printf("{ ");
    display_object_entries(object, true);
    printf(" }");
}

void printJson(struct JsonValue *value) {
    if (value->type == JsonNumber) {
        printf("%f", value->data.number);
    } else if (value->type == JsonBoolean) {
        if (value->data.boolean) {
            printf("true");
        } else {
            printf("false");
        }
    } else if (value->type == JsonString) {
        printf("\"%s\"", value->data.string);
    } else if (value->type == JsonArray) {
        display_array(value->data.array);
    } else if (value->type == JsonObject) {
        display_object(value->data.object);
    } else if (value->type == JsonNull) {
        printf("null");
    }
}

/*
void testDisplay() {
    // [1, 2, true]
    char stringData[512] = "Hello, world";
    char objectKey[512] = "name";
    char objectValueData[512] = "Jerry";
    char objectKey2[512] = "age";
    struct ArrayEntry arr;
    struct ArrayEntry next;
    struct JsonValue one;
    struct JsonValue two;
    struct JsonValue string;
    struct JsonValue array;
    struct JsonValue jerry;
    struct ObjectEntry objectEntry1;
    struct ObjectEntry objectEntry2;
    struct JsonValue object;
    struct JsonValue aNull;
    one.type = TYPE_NUMBER;
    one.data.number = 1;
    two.type = TYPE_NUMBER;
    two.data.number = 2;
    arr.value = &one;
    arr.next = &next;
    next.value = &two;
    next.next = NULL;
    string.type = TYPE_STRING;
    string.data.string = stringData;
    array.type = TYPE_ARRAY;
    array.data.array = &arr;
    objectEntry1.key = objectKey;
    objectEntry1.value = &jerry;
    objectEntry1.next = &objectEntry2;
    objectEntry2.key = objectKey2;
    objectEntry2.value = &array;
    objectEntry2.next = NULL;
    jerry.type = TYPE_STRING;
    jerry.data.string = objectValueData;
    object.type = TYPE_OBJECT;
    object.data.object = &objectEntry1;
    aNull.type = TYPE_NULL;
    display(&one);
    display(&two);
    display(&string);
    display(&array);
    display(&object);
}
*/
