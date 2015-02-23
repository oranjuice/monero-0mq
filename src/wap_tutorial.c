/*  =========================================================================
    wap_tutorial.c - Wallet access protocol tutorial

    Copyright (c) the Contributors as noted in the AUTHORS file.

    
    =========================================================================
*/

/*
@header
    This actor implements the Malamute service. The actor uses the CZMQ socket
    command interface, rather than a classic C API. You can start as many
    instances of the Malamute service as you like. Each will run in its own
    namespace, as virtual hosts. This class is wrapped as a main program via
    the malamute.c application, and can be wrapped in other languages in the
    same way as any C API.
@discuss
    This is a minimal, incomplete implementation of Malamute. It does however
    not have any known bugs.
@end
*/

//  This header file gives us the Wallet APIs plus CZMQ, and libzmq:
#include "../include/wallet.h"

int main (int argc, char *argv [])
{
    //  Let's start a new server instance; this runs as a background thread
    zactor_t *server = zactor_new (wap_server, NULL);

    //  Switch on verbose tracing... this gets a little overwhelming so you
    //  can comment or delete this when you're bored with it:
    zsock_send (server, "s", "VERBOSE");

    //  We control the server by sending it commands. It's a CZMQ actor, and
    //  we can talk to it using the zsock API (or zstr, or zframe, or zmsg).
    //  To get things started, let's tell the server to bind to an endpoint:
    zsock_send (server, "ss", "BIND", "ipc://@/monero");

    //  We can set server properties by sending it SET commands like this:
    //  (Or, configure the server via an external config file)
    zsock_send (server, "sss", "SET", "server/timeout", "5000");

    //  Switch on client tracing, we'll now see both the client and the
    //  server state machines animated:
    wap_client_verbose = true;

    //  The server is now running. Let's start a wallet client:
    wap_client_t *client = wap_client_new ();
    assert (client);
    int rc = wap_client_connect (client, "ipc://@/monero", 200, "wallet identity");
    assert (rc == 0);
    
    //  The server only supports two commands for now, START and STOP, so
    //  let's try each of these:
    int rc = wap_client_start (client, 25);
    printf("\n\n Response: %d\n", (int)wap_client_curr_height(client));
    assert (rc == 0);

    rc = wap_client_stop (client);
    assert (rc == 0);
    
    //  Great, it all works. Now to shutdown, we use the destroy method,
    //  which does a proper deconnect handshake internally:
    wap_client_destroy (&client);

    //  Finally, shut down the server by destroying the actor; this does
    //  a proper shutdown so that all memory is freed as you'd expect.
    zactor_destroy (&server);
    return 0;
}
