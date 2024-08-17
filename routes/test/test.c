#include <route_base.h>
#include <string.h>

static int test_handler (struct mg_connection *connection, void *data);
static int test_response (struct mg_connection *connection, char *content, int status);

route_base_t *route_get (void)
{
    static route_base_t test = 
    {
        .endpoint = "/tests",
        .method = "GET",
        .object = "My dynamic endpoint",
        .handler = test_handler
    };

    return &test;
}

static int test_handler (struct mg_connection *connection, void *data)
{
    char *object = (char *) data;

    char *json = "{\"message\":\"%s\"}";

    char buffer [1024] = {0};

    snprintf (buffer, 1024 - 1, json, object);

    return test_response (connection, buffer, sat_webserver_http_status_ok);
}

static int test_response (struct mg_connection *connection, char *content, int status)
{
    sat_webserver_response_t response;

    memset (&response, 0, sizeof (sat_webserver_response_t));

    sat_webserver_response_set_payload (&response, content, strlen (content));
    sat_webserver_response_set_status (&response, status);

    sat_webserver_response_header_add (&response,
                                       "Content-Type",
                                       "application/json; charset=utf-8");

    sat_webserver_response_send (connection, response);

    return status;
}