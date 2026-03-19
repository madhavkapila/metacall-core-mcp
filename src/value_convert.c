#include "value_convert.h"
#include <string.h>
#include <stdlib.h>

void *json_to_metacall_value(cJSON *item)
{
    if (item == NULL || cJSON_IsNull(item))
        return metacall_value_create_null();
        
    if(cJSON_IsNumber(item))
    {
        double d = item -> valuedouble;
        if(d == (double)(int)d && d >= -2147483648.0 && d <= 2147483647.0)
            return metacall_value_create_int((int)d);
        return metacall_value_create_double(d);
    }

    if(cJSON_IsBool(item))
        return metacall_value_create_bool(cJSON_IsTrue(item) ? 1 : 0);

    if(cJSON_IsString(item))
    {
        const char *s = item->valuestring;
        return metacall_value_create_string(s, strlen(s));
    }

    if(cJSON_IsArray(item))
    {
        int n   = cJSON_GetArraySize(item);
        void **data = malloc(sizeof(void *) * (n > 0 ? n : 1));
        for(int i = 0; i < n; i++)
            data[i] = json_to_metacall_value(cJSON_GetArrayItem(item, i));
        void *arr = metacall_value_create_array((const void **)data, (size_t)n);
        free(data);
        return arr;
    }

    return metacall_value_create_null();
}

cJSON *metacall_value_to_json(void *v)
{
    if(v == NULL)
        return cJSON_CreateNull();


    switch (metacall_value_id(v))
    {
        case METACALL_BOOL:
            return cJSON_CreateBool(metacall_value_to_bool(v));
        break;
        case METACALL_INT:
            return cJSON_CreateNumber((double)metacall_value_to_int(v));
        case METACALL_LONG:
            return cJSON_CreateNumber((double)metacall_value_to_long(v));
        break;
        case METACALL_FLOAT:
            return cJSON_CreateNumber((double)metacall_value_to_float(v));
        break;
        case METACALL_DOUBLE:
            return cJSON_CreateNumber(metacall_value_to_double(v));
        case METACALL_STRING:
        {
            size_t size;
            const char *s = metacall_value_to_string(v, &size);
            return cJSON_CreateString(s);
        }
        break;
        case METACALL_NULL:
            return cJSON_CreateNull();
        break;
        case METACALL_ARRAY:
        {
            void **a = metacall_value_to_array(v);
            size_t n = metacall_value_count(v);
            cJSON *arr = cJSON_CreateArray();
            for(size_t i = 0; i < n; i++)
                cJSON_AddItemToArray(arr, metacall_value_to_json(a[i]));
            return arr;

        }
        break;
        default:
            return cJSON_CreateString("[Unsupported Type]");
    }
}