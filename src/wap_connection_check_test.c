#include "../include/wallet.h"

int main (int argc, char *argv [])
{
    zactor_t *server = zactor_new (wap_server, NULL);

    zsock_send (server, "s", "VERBOSE");

    zsock_send (server, "ss", "BIND", "ipc://@/monero");

    zsock_send (server, "sss", "SET", "server/timeout", "5000");

    wap_client_verbose = true;

    wap_client_t *client = wap_client_new ();
    assert (client);
    int rc = wap_client_connect (client, "ipc://@/monero", 200, "wallet identity");
    assert (rc == 0);
    
    assert(wap_client_connected(client) == true);
    
    wap_client_destroy (&client);

    zactor_destroy (&server);
    return 0;
}

