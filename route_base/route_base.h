#ifndef ROUTE_BASE_H_
#define ROUTE_BASE_H_

#include <sat.h>

typedef struct 
{
    void *object;
    const char *endpoint;
    const char *method;
    sat_webserver_handler_t handler;
} route_base_t;

route_base_t *route_get (void);

#endif/* ROUTE_BASE_H_ */
