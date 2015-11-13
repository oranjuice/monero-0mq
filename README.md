# Monero 0MQ API

## Overview

This is a C library that is used by clients to connect to the Monero daemon service. It consists of:

* a protocol layer, which encodes commands into binary frames that we can send using ZeroMQ.

* a server stack, which accepts connections from clients, maintains state for each connection, and handles incoming commands accordingly.

* a client stack, that can talk to a server to send/receive commands, do heartbeating, and handle errors.

The ZeroMQ pattern is ROUTER at the server side, and DEALER for each client.

## Purpose

Writing and maintaining server and client code can be an unnecessarily complicated task without a proper system for it. This library hopes to alleviate that problem. Not to mention, 0MQ is a solid messaging framework that clearly has its place.

This library makes use of <a href="https://github.com/zeromq/zproto">zproto</a> for expressing client and server as state machine models. It generates code, so that we can declaratively aspects of the client, server and the protocol. It abstracts away a lot of internal messaging code that need not be touched during most times. This way, development of client and server communication code is much easier and efficient.

## Building and pre-requisites

First build and install this stack:

    git clone git://github.com/zeromq/libzmq.git
    cd libzmq
    ./autogen.sh && ./configure && make -j 4 check
    sudo make install && sudo ldconfig
    cd ..
    git clone git://github.com/zeromq/czmq.git
    cd czmq
    ./autogen.sh && ./configure && make -j 4 check
    sudo make install && sudo ldconfig
    cd ..

Then build the monero-0MQ library:

    ./autogen.sh && ./configure && make check

after this, the below workflow can be adopted for any changes.

Note: The library does some meta programming, or "model oriented programming", using two ZeroMQ frameworks:

    git://github.com/zeromq/zproject
    git://github.com/zeromq/zproto
    
To do any development you'll need GSL (a code generator) which lives at git://github.com/imatix/gsl.

## Broad workflow

This repository serves to generate C code that can be used elsewhere (like bitmonero).
The basic process of using it is to make changes here, generate code, copy the generated code over to the other project.
This repository isn't concerned with what goes into requests, responses and how they are processed. It is just concerned with how they are "sent". The user of the library has to fill in and read requests and responses.

## Workflow

The most typical use case of this library would be to add a new method as part of a protocol between the client an d the server.
Here are the steps to do this:

* The first step is to identify the format of the request and response objects. This goes into the protocol.

`src/wap_proto.xml` defines the protocol.
There is an element in it called `<grammar>` that defines a grammar for the types of messages being passed around.

A new method imples new types of messages. The new type of message (for the new method) has to be added here.

For example,

    <grammar>
        WAP = open ( traffic / heartbeat ) close
        open = C:OPEN ( S:OPEN-OK / S:ERROR )
        traffic = blocks / info
        blocks = C:BLOCKS ( S:BLOCKS-OK / S:ERROR )
        info = C:GET-INFO ( S:GET-INFO-OK / S:ERROR )
        heartbeat = C:PING ( S:PING-OK / S:ERROR )
        close = C:CLOSE ( S:CLOSE-OK / S:ERROR )
    </grammar>

for the above grammar, `blocks` and `info` are the two types of traffic allowed. Let's focus on `get_info` method for which `info` is the type of traffic.

`info` is nothing but a `C:GET-INFO` (client request) followed by a `S:GET-INFO-OK` (server response) or a `S:ERROR` (error).

It's also necessary to write the formats for these messages. For example,

    <message name = "GET-INFO">
        getinfo IPC
    </message>
    <message name = "GET-INFO-OK">
        <field name = "status" type = "number" size = "4" >Status</field>
        <field name = "height" type = "number" size = "8" >Height</field>
        <field name = "target_height" type = "number" size = "8" >Target Height</field>
        <field name = "difficulty" type = "number" size = "8" >Difficulty</field>
        <field name = "tx_count" type = "number" size = "8" >TX Count</field>
        <field name = "tx_pool_size" type = "number" size = "8" >TX Pool Size</field>
        <field name = "alt_blocks_count" type = "number" size = "8" >Alt Blocks Count</field>
        <field name = "outgoing_connections_count" type = "number" size = "8" >Outgoing Connections Count</field>
        <field name = "incoming_connections_count" type = "number" size = "8" >Incoming Connections Count</field>
        <field name = "white_peerlist_size" type = "number" size = "8" >White Peerlist Size</field>
        <field name = "grey_peerlist_size" type = "number" size = "8" >Grey Peerlist Size</field>
    </message>

the above elements define the formats of the `get_info` request and response messages.

* As explained, the client and server behaviour is expressed as a state machine. To incorporate our new method into the client, we need to define some extra states and actions.

`src/wap_client.xml` contains a state machine represented as XML. It's fairly self-explanatory.

When the client is "connected" it has to be able to send our new request message.
For example,

    <state name = "connected" inherit = "defaults">
        ...
        <event name = "get info" next = "expect get info ok">
            <action name = "prepare get info command" />
            <action name = "send" message = "GET INFO" />
        </event>
        ...
    </state>

makes the client go to the state `expect get info ok` when the request `get info` is made.

Hence, there is also another state to handle the response:

    <state name = "expect get info ok" inherit = "defaults">
        <event name = "GET INFO OK" next = "connected">
            <action name = "signal have get info ok" />
        </event>
    </state>
    

Note the actions here, as we'll get back to them later.

In the same file, there is a helpful method API defined. For `get_info` it has

    <method name = "get info" return = "status">
        Ask for height.
        <accept reply = "GET INFO OK" />
        <accept reply = "FAILURE" />
    </method>
    <reply name = "GET INFO OK">
        <field name = "status" type = "number" size = "4" >Status</field>
        <field name = "height" type = "number" size = "8" >Height</field>
        <field name = "target_height" type = "number" size = "8" >Target Height</field>
        <field name = "difficulty" type = "number" size = "8" >Difficulty</field>
        <field name = "tx_count" type = "number" size = "8" >TX Count</field>
        <field name = "tx_pool_size" type = "number" size = "8" >TX Pool Size</field>
        <field name = "alt_blocks_count" type = "number" size = "8" >Alt Blocks Count</field>
        <field name = "outgoing_connections_count" type = "number" size = "8" >Outgoing Connections Count</field>
        <field name = "incoming_connections_count" type = "number" size = "8" >Incoming Connections Count</field>
        <field name = "white_peerlist_size" type = "number" size = "8" >White Peerlist Size</field>
        <field name = "grey_peerlist_size" type = "number" size = "8" >Grey Peerlist Size</field>
    </reply>
    
While this seems a bit redundant, it is still necessary. (see https://github.com/oranjuice/monero-0mq/pull/4#discussion_r22440741)
Care must be taken to ensure that this matches exactly with the protocol in `src/wap_proto.xml`. 

* When the client sends a request, various fields have to be picked and packed into a 0MQ object as per the protocol.

In our example, the action `prepare get info command` is for this.
What the system does is call a C function called `prepare_get_info_command` in `src/wap_client.c`. `prepare_get_info_command` doesn't really have to do anything (because the request has no fields). For the prupose of an example, a similar one would be

    static void
    prepare_get_output_indexes_command (client_t *self)
    {
        // Set the field `tx_id` in the request
        wap_proto_set_tx_id (self->message, &self->args->tx_id);
    }

* Now when the response arrives, the response object must be again passed up the protocol layer as per the protocol. In other words, the required fields have to be picked and sent across to a higher layer.

For example,

    static void
    signal_have_get_info_ok (client_t *self)
    {
        zsock_send (self->cmdpipe, "s88888888888", "GET INFO OK",
            wap_proto_status (self->message),
            wap_proto_height (self->message),
            wap_proto_target_height (self->message),
            wap_proto_difficulty (self->message),
            wap_proto_tx_count (self->message),
            wap_proto_tx_pool_size (self->message),
            wap_proto_alt_blocks_count (self->message),
            wap_proto_outgoing_connections_count (self->message),
            wap_proto_incoming_connections_count (self->message),
            wap_proto_white_peerlist_size (self->message),
            wap_proto_grey_peerlist_size (self->message));
    }

picks all the `get_info` response fields.

Note the string "s88888888888". It is a string containing "picture"s each one describing the type of the field. See http://api.zeromq.org/czmq3-0:zsock.

Note: Certain types of fields (like frames) have two types of accessors: one that transfers ownership to the caller, and one that doesn't. For example `wap_proto_block_ids` doesn't transfer ownership, while `wap_proto_get_block_ids` does. `wap_proto_get_block_ids` has to be used here because the code here is passing the frame (or any other type) to the client API which destroys it. Hence it is necessary to claim ownership so that it is not destroyed in two places.

* Now let's look at the server side of all that we discussed. The server is again a state machine. When the server is in "connected" state, for our example, there should be an event for "get_info" request.
Here it is:

    <state name = "connected" inherit = "defaults">
        ...
        <event name = "GET INFO">
            <action name = "getinfo" />
            <action name = "send" message = "GET INFO OK" />
        </event>
        ...
    </state>

The first action is where the server is the populate the 0MQ object with respose fields. In this example, a C function called `getinfo` gets called in `src/wap_server.c`.

`getinfo` can be for example written like so:

    static void
    getinfo (client_t *self)
    {
        // Set the fields as integers from 0 through 10
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

* This completes adding a new method. The rest is simply using it in actual code and building the project.
`src/wap_tutorial` shows the basics of how to use the library in code.

After an instance of `wap_client_t` is created, to make a request, the functions in `include/wap_client.h` have to be used. That file is generated from XML. To access fields from a response, again, the declarations are in `include/wap_client.h`.

`include/wap_server.h` contains function declarations that help the server code get and set fields from objects.

## Code generation artifacts

After the xml files are edited,

    make code

uses `gsl` to generate C code for us.

`src/wap_proto.xml` -> `src/wap_proto.c`

`src/wap_server.xml` -> `src/wap_server_engine.inc` (NOT `src/wap_server.c`)

`src/wap_client.xml` -> `src/wap_client_engine.inc` (NOT `src/wap_client.c`)

These typcially have to be copied over to the other project as is after each iteration of protocol. There is no need to do this unless the XML files are edited and code generation is necessary.

## Testing in this repository

Oftentimes, it is a good idea to put dummy tests in this very repository to check if things are tightly knit and connected. A very trivial way of checking this is to do a simple end-to-end test checking the correctness of every field with `assert`ions. When the code is moved to the actual project that uses the library, these trivial checks can be replaced with actual code that sends real requests and responses.

    make check

builds `src/wap_tutorial.c` which contains a bunch of these trivial tests. Putting all this in `wap_tutorial` is probably not a very idea for a long term purpose, but it contains a complete illustration of how the whole library is to be used and also as an end-to-end test.

Note: `wap_tutorial` creates both server and client in the same process for illustration.

## Integrating somewhere else

Using this library somewhere else is simply a matter of including headers, compiling sources and linking.
<a href="https://github.com/oranjuice/bitmonero/tree/77c6e85cbbced87c2c16073e4784934742231796/src/ipc">This</a> (and <a href="https://github.com/oranjuice/bitmonero/blob/77c6e85cbbced87c2c16073e4784934742231796/src/ipc/CMakeLists.txt">this</a>) should give a good idea of how it is done. Note the separate compilation units for client and server and what goes into each one.
The names of the files from this library have not been changed there.
