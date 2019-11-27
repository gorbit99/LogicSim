#include "ConfigHandler.h"
#include "debugmalloc.h"

typedef struct ConfigField {
    char *name;
    char *value;
} ConfigField;

static struct Config {
    int fieldCount;
    ConfigField *fields;
} config;

static void __create_config() {
    FILE *file = fopen("config.txt", "wt");
    fprintf(file, "screen-width: 640\n");
    fprintf(file, "screen-height: 480\n");
    fprintf(file, "maximized: false\n");
    fprintf(file, "last-opened: -\n");
    fclose(file);
}

void config_init() {
    FILE *file = fopen("config.txt", "rt");
    if (file == NULL) {
        __create_config();
        file = fopen("config.txt", "rt");
    }
    
    config.fieldCount = 0;
    config.fields = NULL;
    
    char name[100];
    char value[1024];
    while (fscanf(file, "%[^:]: %[^\n]\n", name, value) == 2) {
        config.fieldCount++;
        ConfigField *newMem = realloc(config.fields, sizeof(ConfigField) * config.fieldCount);
        if (newMem == NULL) {
            log_error("Couldn't allocate new memory!\n");
            return;
        }
        config.fields = newMem;
        config.fields[config.fieldCount - 1].name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
        config.fields[config.fieldCount - 1].value = (char *)malloc(sizeof(char) * (strlen(value) + 1));
        strcpy(config.fields[config.fieldCount - 1].name, name);
        strcpy(config.fields[config.fieldCount - 1].value, value);
    }
}

static char *__get_value(char *field) {
    for (int i = 0; i < config.fieldCount; i++)
    {
        if (strcmp(config.fields[i].name, field) == 0)
            return config.fields[i].value;
    }
    return NULL;
}

bool config_get_bool(char *field) {
    return strcmp(__get_value(field), "true") == 0;
}

int config_get_int(char *field) {
    return atoi(__get_value(field));
}

char *config_get_string(char *field) {
    return __get_value(field);
}

static void __set_value(char *field, char *value) {
    for (int i = 0; i < config.fieldCount; i++)
    {
        if (strcmp(config.fields[i].name, field) == 0) {
            free(config.fields[i].value);
            config.fields[i].value = (char *)malloc(sizeof(char) * (strlen(value) + 1));
            strcpy(config.fields[i].value, value);
        }
    }
}

void config_set_bool(char *field, bool value) {
    __set_value(field, value ? "true" : "false");
}

void config_set_int(char *field, int value) {
    char temp[20];
    __set_value(field, itoa(value, temp, 10));
}

void config_set_string(char *field, char *value) {
    __set_value(field, value);
}

void config_save() {
    FILE *file = fopen("config.txt", "wt");
    if (file == NULL) {
        log_error("Couldn't open file config.txt!\n");
        return;
    }
    for (int i = 0; i < config.fieldCount; i++)
        fprintf(file, "%s: %s\n", config.fields[i].name, config.fields[i].value);
}

void config_free() {
    for (int i = 0; i < config.fieldCount; i++)
    {
        free(config.fields[i].name);
        free(config.fields[i].value);
    }
    free(config.fields);
}