# Monero 0MQ API

## Overview

This is a C library that is used by clients to the Monero daemon service. It consists of:

* a protocol layer, which encodes commands into binary frames that we can send using ZeroMQ.

* a server stack, which accepts connections from clients, maintains state for each connection, and handles incoming commands accordingly.

* a client stack, that can talk to a server to send/receive commands, do heartbeating, and handle errors.

The ZeroMQ pattern is ROUTER at the server side, and DEALER for each client.

## Purpose

Writing and maintaining server and client code can be an unnecessarily complicated task without a proper system for it. This library hopes to alleviate that problem. Not to mention, 0MQ is a solid messaging framework that clearly has its place.

This library makes use of <a href="https://github.com/zeromq/zproto">zproto</a> for expressing client and server as state machine models. It generates code, so that we can declaratively specify some of the aspects of the client, server and the protocol. It abstracts away a lot of internal messaging code that need not be touched during most times. This way, development of client and server communication code is much easier and efficient.

## Broad workflow

This repository serves to generate C code that can be used elsewhere (like bitmonero).
The basic process of using it is to make changes here, generate code, copy the generated code over to the other project.
This repository isn't concerned with what goes into requests, responses and how they are processed. It is just concerned with how they are "sent".

## Workflow

The most typical use case of this library would be to add a new method as part of a protocol between the client an d the server.
Here are the steps to do this:

1. The first step is to identify the format of the request and response objects. This goes into the protocol.

`src/wap_proto.xml` defines the protocol.
There is an element in it called `<grammar>` that defines a grammar for the types of messages being passed around.

The new type of message (for the new method) has to be added here.

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

for the above grammar, `blocks` and `info` are the two types of traffic allowed.
`info` is nothing but a `C:GET-INFO` (client request) followed by a `S:GET-INFO-OK` (server response) or a `S:ERROR` (error).

It's also necessary to write the formats for these messages. For example,

    <message name = "GET-INFO">
        getinfo IPC
    </message>
    <message name = "GET-INFO-OK">
        <field name = "status" type = "number" size = "8" >Status</field>
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

2. As explained, the client and server behaviour is expressed as a state machine. To incorporate our new method into the client, we need to define some extra states and actions.

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

makes the client go to the state `expect get info ok` when the request is made.

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
        <field name = "status" type = "number" size = "8" >Status</field>
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
    
While this seems a bit redundant, it is still necessary. Care must be taken to ensure that this matches exactly with the protocol in `src/wap_proto.xml`. (see https://github.com/oranjuice/monero-0mq/pull/4#discussion_r22440741)

3. When the client sends a request, various fields have to be picked and packed into a 0MQ object as per the protocol.

In our example, the action `prepare get info command` is for this.
What the system does is call a C function called `prepare_get_info_command` in `src/wap_client.c`. `prepare_get_info_command` doesn't really have to do anything (because the request has no fields). For the prupose of an example, a similar one would be

    static void
    prepare_get_output_indexes_command (client_t *self)
    {
        // Set the field `tx_id` in the request
        wap_proto_set_tx_id (self->message, &self->args->tx_id);
    }

4. Now when the response arrives, the response object must be again passed up the protocol layer as per the protocol. In other words, the required fields have to be picked and sent across to a higher layer.

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

5. Coming...

## Building the Library

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

## Library Development

The library does some meta programming, or "model oriented programming", using two ZeroMQ frameworks:

    git://github.com/zeromq/zproject
    git://github.com/zeromq/zproto
    
To do any development you'll need GSL (a code generator) which lives at git://github.com/imatix/gsl.

