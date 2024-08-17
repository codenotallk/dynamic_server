#include <sat.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <route_base.h>

typedef struct
{
    sat_webserver_t webserver;
    sat_plugin_t plugin;
} dynamic_server_t;

static int server_response (struct mg_connection *connection, char *content, int status);

static int health_handler (struct mg_connection *connection, void *data);
static int load_handler (struct mg_connection *connection, void *data);

int main (int argc, char *argv [])
{
    dynamic_server_t server;

    sat_status_t status = sat_webserver_init (&server.webserver);
    assert (sat_status_get_result (&status) == true);

    status = sat_webserver_open (&server.webserver, &(sat_webserver_args_t)
                                                    {
                                                        .endpoint_amount = 3,
                                                        .folder = ".",
                                                        .port = "1234",
                                                        .threads_amount = "1"
                                                    });
    assert (sat_status_get_result (&status) == true);

    status = sat_webserver_add_endpoint (&server.webserver,
                                         "/health",
                                         "GET",
                                         health_handler,
                                         NULL);
    assert (sat_status_get_result (&status) == true);

    status = sat_webserver_add_endpoint (&server.webserver,
                                         "/load",
                                         "GET",
                                         load_handler,
                                         &server);
    assert (sat_status_get_result (&status) == true);

    sat_webserver_run (&server.webserver);

    status = sat_webserver_close (&server.webserver);
    assert (sat_status_get_result (&status) == true);

    return EXIT_SUCCESS;
}

static int server_response (struct mg_connection *connection, char *content, int status)
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

static int health_handler (struct mg_connection *connection, void *data)
{
    (void) data;

    char *json = "{\"status\":\"running\"}";

    return server_response (connection, json, sat_webserver_http_status_ok);
}

static int load_handler (struct mg_connection *connection, void *data)
{
    dynamic_server_t *server = (dynamic_server_t *) data;

    char *response = "{\"status\":\"not loaded\"}";

    const struct mg_request_info *ri = mg_get_request_info (connection);
    char library [1024] = {0};

    int status = sat_webserver_http_status_not_found;

    if (ri->query_string != NULL &&
        mg_get_var (ri->query_string,
                    strlen (ri->query_string),
                    "library",
                    library,
                    strlen (ri->query_string)) >= 0)
    {
        if (sat_file_exists (library) == true)
        {
            route_base_t *base;
            route_base_t *(*route_get) (void);

            sat_status_t __status = sat_plugin_open (&server->plugin,
                                                     &(sat_plugin_args_t)
                                                    {
                                                        .library_name = library
                                                    });
            assert (sat_status_get_result (&__status) == true);

            __status = sat_plugin_load_method (&server->plugin,
                                               "route_get",
                                               (void *)&route_get);
            assert (sat_status_get_result (&__status) == true);

            base = route_get ();

            __status = sat_webserver_add_endpoint (&server->webserver,
                                                   base->endpoint,
                                                   base->method,
                                                   base->handler,
                                                   base->object);
            assert (sat_status_get_result (&__status) == true);

            response = "{\"status\":\"loaded\"}";

            status = sat_webserver_http_status_ok;
        }
    }

    return server_response (connection, response, status);
}