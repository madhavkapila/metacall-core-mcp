#include "cJSON.h"
#include <metacall/metacall.h>
#include <metacall/metacall_value.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value_convert.h"

#define MAX_LINE (1024 * 256)


// JSON-RPC result response
static void respond_result(cJSON *id, cJSON *result)
{
    cJSON *resp = cJSON_CreateObject();
    cJSON_AddStringToObject(resp, "jsonrpc", "2.0");

    if (id) 
        cJSON_AddItemReferenceToObject(resp, "id", id);
    else    
        cJSON_AddNullToObject(resp, "id");

    cJSON_AddItemToObject(resp, "result", result);

    char *out = cJSON_PrintUnformatted(resp);
    fprintf(stdout, "%s\n", out);
    fflush(stdout);
    free(out);
    cJSON_Delete(resp);
}

// JSON-RPC error response
static void respond_error(cJSON *id, int code, const char *msg)
{
    cJSON *resp = cJSON_CreateObject();
    cJSON_AddStringToObject(resp, "jsonrpc", "2.0");
    if (id) 
        cJSON_AddItemReferenceToObject(resp, "id", id);
    else    
        cJSON_AddNullToObject(resp, "id");
    cJSON *err = cJSON_CreateObject();
    cJSON_AddNumberToObject(err, "code", code);
    cJSON_AddStringToObject(err, "message", msg);
    cJSON_AddItemToObject(resp, "error", err);

    char *out = cJSON_PrintUnformatted(resp);
    fprintf(stdout, "%s\n", out);
    fflush(stdout);
    free(out);
    cJSON_Delete(resp);
}

// wRAPS TEXT in MCP content format
static cJSON *mcp_text_result(const char *text, int is_error)
{
    cJSON *result = cJSON_CreateObject();
    cJSON *content = cJSON_CreateArray();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "type", "text");
    cJSON_AddStringToObject(item, "text", text);
    cJSON_AddItemToArray(content, item);
    cJSON_AddItemToObject(result, "content", content);
    if (is_error)
        cJSON_AddBoolToObject(result, "isError", 1);
    return result;
}


//MCP tool exec
static cJSON *handle_load_script(cJSON *args)
{
    cJSON *lang = cJSON_GetObjectItem(args, "language");
    cJSON *files = cJSON_GetObjectItem(args, "files");

    if (!lang || !cJSON_IsString(lang) || !files || !cJSON_IsArray(files))
        return mcp_text_result("need 'language' (string) and 'files' (array)", 1);

    int count = cJSON_GetArraySize(files);
    if (count == 0)
        return mcp_text_result("files array is empty", 1);

    const char **paths = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) 
    {
        cJSON *f = cJSON_GetArrayItem(files, i);
        paths[i] = cJSON_IsString(f) ? f->valuestring : "";
    }

    int rc = metacall_load_from_file(lang->valuestring, paths, (size_t)count, NULL);
    free(paths);

    if (rc != 0)
        return mcp_text_result("failed to load — check file paths and language tag", 1);

    //TODO: inspect : Giving error will resolve later
    // size_t sz = 0;
    // char *info = metacall_inspect(&sz, NULL);
    // cJSON *r = mcp_text_result(info ? info : "loaded ok", 0);
    // return r;

    return mcp_text_result("loaded ok", 0);
}


static cJSON *handle_call_function(cJSON *args)
{
    cJSON *fname = cJSON_GetObjectItem(args, "function");
    if (!fname || !cJSON_IsString(fname))
        return mcp_text_result("need 'function' (string)", 1);

    
    void *fn = metacall_function(fname->valuestring);
    if (!fn) 
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "function '%s' not found — did you load a script first?", fname->valuestring);
        return mcp_text_result(buf, 1);
    }

    cJSON *jargs = cJSON_GetObjectItem(args, "args");
    size_t argc = 0;
    void **mc_args = NULL;

    if (jargs && cJSON_IsArray(jargs)) 
    {
        argc = (size_t)cJSON_GetArraySize(jargs);
        if (argc > 0) 
        {
            mc_args = malloc(sizeof(void *) * argc);
            for (size_t i = 0; i < argc; i++)
                mc_args[i] = json_to_metacall_value(cJSON_GetArrayItem(jargs, (int)i));
        }
    }

    void *ret = metacallv_s(fname->valuestring, mc_args, argc);

    if (mc_args) 
    {
        for (size_t i = 0; i < argc; i++)
            metacall_value_destroy(mc_args[i]);
        free(mc_args);
    }

    if (ret == NULL)
        return mcp_text_result("null", 0);

    cJSON *jret = metacall_value_to_json(ret);
    char *ret_str = cJSON_PrintUnformatted(jret);
    cJSON *r = mcp_text_result(ret_str, 0);
    free(ret_str);
    cJSON_Delete(jret);
    metacall_value_destroy(ret);
    return r;
}

static void dispatch(const char *method, cJSON *id, cJSON *params)
{

    if (strcmp(method, "initialize") == 0) 
    {
        cJSON *r = cJSON_CreateObject();
        cJSON_AddStringToObject(r, "protocolVersion", "2024-11-05");
        cJSON *caps = cJSON_CreateObject();
        cJSON_AddItemToObject(caps, "tools", cJSON_CreateObject());
        cJSON_AddItemToObject(r, "capabilities", caps);
        cJSON *info = cJSON_CreateObject();
        cJSON_AddStringToObject(info, "name", "metacall-mcp-poc");
        cJSON_AddStringToObject(info, "version", "0.1.0");
        cJSON_AddItemToObject(r, "serverInfo", info);
        respond_result(id, r);
        return;
    }

    if (strncmp(method, "notifications/", 14) == 0)
        return;

    if (strcmp(method, "tools/list") == 0) 
    {
        cJSON *r = cJSON_CreateObject();
        cJSON *tools = cJSON_CreateArray();

        cJSON *t1 = cJSON_CreateObject();
        cJSON_AddStringToObject(t1, "name", "load");
        cJSON_AddStringToObject(t1, "description",
            "Load source files into metacall. Supports py, node, rb, ts, and others.");
        cJSON *s1 = cJSON_Parse(
            "{\"type\":\"object\","
            "\"properties\":{"
              "\"language\":{\"type\":\"string\",\"description\":\"Loader tag, e.g. py, node, rb\"},"
              "\"files\":{\"type\":\"array\",\"items\":{\"type\":\"string\"},\"description\":\"File paths\"}"
            "},"
            "\"required\":[\"language\",\"files\"]}");
        cJSON_AddItemToObject(t1, "inputSchema", s1);
        cJSON_AddItemToArray(tools, t1);

        cJSON *t2 = cJSON_CreateObject();
        cJSON_AddStringToObject(t2, "name", "call");
        cJSON_AddStringToObject(t2, "description",
            "Call a function that was previously loaded. Pass arguments as an array.");
        cJSON *s2 = cJSON_Parse(
            "{\"type\":\"object\","
            "\"properties\":{"
              "\"function\":{\"type\":\"string\",\"description\":\"Function name\"},"
              "\"args\":{\"type\":\"array\",\"items\":{},\"description\":\"Arguments to pass\"}"
            "},"
            "\"required\":[\"function\"]}");
        cJSON_AddItemToObject(t2, "inputSchema", s2);
        cJSON_AddItemToArray(tools, t2);

        cJSON_AddItemToObject(r, "tools", tools);
        respond_result(id, r);
        return;
    }

    if (strcmp(method, "tools/call") == 0) 
    {
        cJSON *name = cJSON_GetObjectItem(params, "name");
        cJSON *arguments = cJSON_GetObjectItem(params, "arguments");
        if (!name || !cJSON_IsString(name)) 
        {
            respond_error(id, -32602, "missing tool name in params");
            return;
        }
        if (!arguments) arguments = cJSON_CreateObject();

        cJSON *result = NULL;
        if (strcmp(name->valuestring, "load") == 0)
            result = handle_load_script(arguments);
        else if (strcmp(name->valuestring, "call") == 0)
            result = handle_call_function(arguments);
        else
            result = mcp_text_result("unknown tool", 1);

        respond_result(id, result);
        return;
    }

    if (strcmp(method, "ping") == 0) 
    {
        respond_result(id, cJSON_CreateObject());
        return;
    }

    respond_error(id, -32601, "method not found");
}

int main(void)
{
    if (metacall_initialize() != 0) 
    {
        fprintf(stderr, "[mcp-poc] metacall_initialize failed\n");
        return 1;
    }
    fprintf(stderr, "[mcp-poc] ready, reading from stdin\n");

    char *line = malloc(MAX_LINE);
    if (!line) 
    {
        fprintf(stderr, "[mcp-poc] out of memory\n");
        metacall_destroy();
        return 1;
    }

    while (fgets(line, MAX_LINE, stdin)) 
    {
        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';
        if (len == 0) 
            continue;

        cJSON *root = cJSON_Parse(line);
        if (!root) 
        {
            respond_error(NULL, -32700, "parse error");
            continue;
        }

        cJSON *method_j = cJSON_GetObjectItem(root, "method");
        cJSON *id_j     = cJSON_GetObjectItem(root, "id");
        cJSON *params_j = cJSON_GetObjectItem(root, "params");

        if (!method_j || !cJSON_IsString(method_j)) 
        {
            respond_error(id_j, -32600, "invalid request — missing method");
            cJSON_Delete(root);
            continue;
        }

        dispatch(method_j->valuestring, id_j, params_j);
        cJSON_Delete(root);
    }

    free(line);
    fprintf(stderr, "[mcp-poc] shutting down\n");
    metacall_destroy();
    return 0;
}