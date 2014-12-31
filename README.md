# Monero Wallet API

## Overview

This is a C library that connects wallet clients to the Monero service. It consists of:

* a protocol layer, which encodes commands such as STOP and START into binary frames that we can send using ZeroMQ.

* a server stack, which accepts connections from clients, maintains state for each connection, and handles incoming commands accordingly.

* a client stack, that can talk to a server to send/receive commands, do heartbeating, and handle errors.

The ZeroMQ pattern is ROUTER at the server side, and DEALER for each client.

## Using the Library

Examples coming...

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

