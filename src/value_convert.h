#ifndef VALUE_CONVERT_H
#define VALUE_CONVERT_H

#include <metacall/metacall.h>
#include <metacall/metacall_value.h>
#include "cJSON.h"

void *json_to_metacall_value(cJSON *item);
cJSON *metacall_value_to_json(void *v);

#endif