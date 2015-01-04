/*  =========================================================================
    wap_server - wap_server

    Copyright (c) the Contributors as noted in the AUTHORS file.
                                                                
    (insert license text here)                                  
    =========================================================================
*/

/*
@header
    Description of class for man page.
@discuss
    Detailed discussion of the class, if any.
@end
*/

#include "wap_classes.h"
//  TODO: Change these to match your project's needs
#include "../include/wap_proto.h"
#include "../include/wap_server.h"

//  ---------------------------------------------------------------------------
//  Forward declarations for the two main classes we use here

typedef struct _server_t server_t;
typedef struct _client_t client_t;

//  This structure defines the context for each running server. Store
//  whatever properties and structures you need for the server.

struct _server_t {
    //  These properties must always be present in the server_t
    //  and are set by the generated engine; do not modify them!
    zsock_t *pipe;              //  Actor pipe back to caller
    zconfig_t *config;          //  Current loaded configuration
    
    //  TODO: Add any properties you need here
};

//  ---------------------------------------------------------------------------
//  This structure defines the state for each client connection. It will
//  be passed to each action in the 'self' argument.

struct _client_t {
    //  These properties must always be present in the client_t
    //  and are set by the generated engine; do not modify them!
    server_t *server;           //  Reference to parent server
    wap_proto_t *message;       //  Message in and out

    //  TODO: Add specific properties for your application
};

//  Include the generated server engine
#include "wap_server_engine.inc"

//  Allocate properties and structures for a new server instance.
//  Return 0 if OK, or -1 if there was an error.

static int
server_initialize (server_t *self)
{
    //  Construct properties here
    return 0;
}

//  Free properties and structures for a server instance

static void
server_terminate (server_t *self)
{
    //  Destroy properties here
}

//  Process server API method, return reply message if any

static zmsg_t *
server_method (server_t *self, const char *method, zmsg_t *msg)
{
    return NULL;
}


//  Allocate properties and structures for a new client connection and
//  optionally engine_set_next_event (). Return 0 if OK, or -1 on error.

static int
client_initialize (client_t *self)
{
    //  Construct properties here
    return 0;
}

//  Free properties and structures for a client connection

static void
client_terminate (client_t *self)
{
    //  Destroy properties here
}

//  ---------------------------------------------------------------------------
//  Selftest

void
wap_server_test (bool verbose)
{
    printf (" * wap_server: ");
    if (verbose)
        printf ("\n");
    
    //  @selftest
    zactor_t *server = zactor_new (wap_server, "server");
    if (verbose)
        zstr_send (server, "VERBOSE");
    zstr_sendx (server, "BIND", "ipc://@/wap_server", NULL);

    zsock_t *client = zsock_new (ZMQ_DEALER);
    assert (client);
    zsock_set_rcvtimeo (client, 2000);
    zsock_connect (client, "ipc://@/wap_server");

    //  TODO: fill this out
    wap_proto_t *request = wap_proto_new ();
    wap_proto_destroy (&request);
    
    zsock_destroy (&client);
    zactor_destroy (&server);
    //  @end
    printf ("OK\n");
}


//  ---------------------------------------------------------------------------
//  register_the_wallet
//

static void
register_the_wallet (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  retrieve_the_blocks
//

static void
retrieve_the_blocks (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  store_the_transaction
//

static void
store_the_transaction (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  retrieve_the_transaction
//

static void
retrieve_the_transaction (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  start_the_mining_process
//

static void
start_the_mining_process (client_t *self)
{
    wap_proto_set_curr_height (self->message, wap_proto_start_height (self->message));
}


//  ---------------------------------------------------------------------------
//  stop_the_mining_process
//

static void
stop_the_mining_process (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  deregister_the_wallet
//

static void
deregister_the_wallet (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  allow_time_to_settle
//

static void
allow_time_to_settle (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  register_new_client
//

static void
register_new_client (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  signal_command_not_valid
//

static void
signal_command_not_valid (client_t *self)
{
    wap_proto_set_status (self->message, WAP_PROTO_COMMAND_INVALID);
}
