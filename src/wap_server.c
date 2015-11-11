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
//  register_wallet
//

static void
register_wallet (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  retrieve_blocks
//

static void
retrieve_blocks (client_t *self)
{
    wap_proto_t *message = self->message;
    char hello[] = {'h', 'i'};
    zframe_t *frame = zframe_new(hello, sizeof(char) * 2);
    zmsg_t *msg = zmsg_new();
    zmsg_prepend(msg, &frame);
    wap_proto_set_block_data(message, &msg);
    wap_proto_set_status(message, 0);
    wap_proto_set_curr_height(message, 10);
    wap_proto_set_start_height(message, 1);
}


//  ---------------------------------------------------------------------------
//  store_transaction
//

static void
send_transaction (client_t *self)
{
    zchunk_t *tx_as_hex_chunk = wap_proto_tx_as_hex(self->message);
    assert(zchunk_size(tx_as_hex_chunk) == 5);
    char *tx_as_hex = (char*)zchunk_data(tx_as_hex_chunk);
    char *x = "12045";
    assert(memcmp(tx_as_hex, x, 5) == 0);
    wap_proto_set_status(self->message, 0);
}


//  ---------------------------------------------------------------------------
//  retrieve_transaction
//

static void
retrieve_transaction (client_t *self)
{
}


//  ---------------------------------------------------------------------------
//  start_mining_process
//

static void
start_mining_process (client_t *self)
{
    zchunk_t *address_chunk = wap_proto_address(self->message);
    assert(zchunk_size(address_chunk) == 5);
    char *address = (char*)zchunk_data(address_chunk);
    char *x = "12045";
    assert(memcmp(address, x, 5) == 0);
    assert(wap_proto_thread_count(self->message) == 3);
    wap_proto_set_status(self->message, 2);
}


//  ---------------------------------------------------------------------------
//  stop_mining_process
//

static void
stop_mining_process (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  deregister_wallet
//

static void
deregister_wallet (client_t *self)
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


//  ---------------------------------------------------------------------------
//  output_indexes
//

static void
output_indexes (client_t *self)
{
    wap_proto_t *message = self->message;
    uint64_t indexes[] = {1, 2, 3, 4};
    zchunk_t *tx_id = wap_proto_tx_id(message);
    size_t size = zchunk_size(tx_id);
    assert(size == 5);
    assert(memcmp(zchunk_data(tx_id), "12045", 5) == 0);
    zframe_t *frame = zframe_new(indexes, sizeof(uint64_t) * 4);
    wap_proto_set_o_indexes(message, &frame);
    wap_proto_set_status(message, 0);
}



//  ---------------------------------------------------------------------------
//  random_outs
//

static void
random_outs (client_t *self)
{
    wap_proto_set_status (self->message, 0);
    char hello[] = {'h', 'i'};
    zframe_t *frame = zframe_new(hello, sizeof(char) * 2);
    wap_proto_set_random_outputs(self->message, &frame);
}



//  ---------------------------------------------------------------------------
//  height
//

static void
height (client_t *self)
{
    wap_proto_set_status (self->message, 0);
    wap_proto_set_height (self->message, 12);
}

//  ---------------------------------------------------------------------------
//  save_bc
//

static void
save_bc (client_t *self)
{
    wap_proto_set_status(self->message, 34);
}


//  ---------------------------------------------------------------------------
//  getinfo
//

static void
getinfo (client_t *self)
{
    wap_proto_set_status(self->message, 0);
    wap_proto_set_height(self->message, 1);
    wap_proto_set_target_height(self->message, 2);
    wap_proto_set_difficulty(self->message, 3);
    wap_proto_set_tx_count(self->message, 4);
    wap_proto_set_tx_pool_size(self->message, 5);
    wap_proto_set_alt_blocks_count(self->message, 6);
    wap_proto_set_outgoing_connections_count(self->message, 7);
    wap_proto_set_incoming_connections_count(self->message, 8);
    wap_proto_set_white_peerlist_size(self->message, 9);
    wap_proto_set_grey_peerlist_size(self->message, 10);
}

//  ---------------------------------------------------------------------------
//  get_peer_list
//

static void
get_peer_list (client_t *self)
{
    wap_proto_set_status(self->message, 0);
    char hello[] = {'h', 'i'};
    zframe_t *frame = zframe_new(hello, sizeof(char) * 2);
    wap_proto_set_white_list(self->message, &frame);
    char hello2[] = {'h', 'i'};
    zframe_t *frame2 = zframe_new(hello2, sizeof(char) * 2);
    wap_proto_set_gray_list(self->message, &frame2);
}

//  ---------------------------------------------------------------------------
//  get_mining_status
//

static void
get_mining_status (client_t *self)
{
    wap_proto_set_status(self->message, 0);
    wap_proto_set_speed(self->message, 100);
    wap_proto_set_thread_count(self->message, 2);
    wap_proto_set_active(self->message, 1);
    zchunk_t *address = zchunk_new("12045", 5);
    wap_proto_set_address(self->message, &address);
}

//  ---------------------------------------------------------------------------
//  set_log_hash_rate
//

static void
set_log_hash_rate (client_t *self)
{
		assert(wap_proto_visible(self->message) == 1);
    wap_proto_set_status(self->message, 2);
}



//  ---------------------------------------------------------------------------
//  set_log_level
//

static void
set_log_level (client_t *self)
{
		assert((int8_t)wap_proto_level(self->message) == -12);
    wap_proto_set_status(self->message, 2);
}

//  ---------------------------------------------------------------------------
//  start_save_graph
//

static void
start_save_graph (client_t *self)
{
    wap_proto_set_status(self->message, 2);
}

//  ---------------------------------------------------------------------------
//  stop_save_graph
//

static void
stop_save_graph (client_t *self)
{
    wap_proto_set_status(self->message, 2);
}

//  ---------------------------------------------------------------------------
//  get_block_hash
//

static void
get_block_hash (client_t *self)
{
		assert(wap_proto_height(self->message) == 3);
		zchunk_t *hash = zchunk_new("12045", 5);
    wap_proto_set_hash(self->message, &hash);
    wap_proto_set_status(self->message, 2);
}

//  ---------------------------------------------------------------------------
//  get_block_template
//

static void
get_block_template (client_t *self)
{
		zchunk_t *address = wap_proto_get_address(self->message);
		assert(memcmp(zchunk_data(address), "12045", 5) == 0);
		assert(wap_proto_reserve_size(self->message) == 5);
		
    wap_proto_set_status(self->message, 2);
    wap_proto_set_reserved_offset(self->message, 3);
    wap_proto_set_height(self->message, 4);
    wap_proto_set_difficulty(self->message, 5);
    zchunk_t *hash = zchunk_new("12045", 5);
    wap_proto_set_prev_hash(self->message, &hash);
    zchunk_t *blob = zchunk_new("12046", 5);
    wap_proto_set_block_template_blob(self->message, &blob);
}



//  ---------------------------------------------------------------------------
//  get_hard_fork_info
//

static void
get_hard_fork_info (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  get_connections
//

static void
get_connections (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  get_connections_list
//

static void
get_connections_list (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  stop_daemon
//

static void
stop_daemon (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  get_block_by_height
//

static void
get_block_by_height (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  get_block_by_hash
//

static void
get_block_by_hash (client_t *self)
{

}
