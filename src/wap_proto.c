/*  =========================================================================
    wap_proto - Wallet Access Protocol

    Codec class for wap_proto.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: wap_proto.xml, or
     * The code generation script that built this file: zproto_codec_c
    ************************************************************************
    Copyright (c) the Contributors as noted in the AUTHORS file.
                                                                
    (insert license text here)                                  
    =========================================================================
*/

/*
@header
    wap_proto - Wallet Access Protocol
@discuss
@end
*/

#include "../include/wap_proto.h"

//  Structure of our class

struct _wap_proto_t {
    zframe_t *routing_id;               //  Routing_id from ROUTER, if any
    int id;                             //  wap_proto message ID
    byte *needle;                       //  Read/write pointer for serialization
    byte *ceiling;                      //  Valid upper limit for read pointer
    char identity [256];                //  Wallet identity
    zlist_t *block_ids;                 //  
    uint32_t start_height;              //  
    uint32_t curr_height;               //  
    char block_status [256];            //  
    zmsg_t *block_data;                 //  Frames of block data
    zchunk_t *tx_data;                  //  Transaction data
    char tx_id [256];                   //  Transaction ID
    uint16_t status;                    //  Error status
    char reason [256];                  //  Printable explanation
};

//  --------------------------------------------------------------------------
//  Network data encoding macros

//  Put a block of octets to the frame
#define PUT_OCTETS(host,size) { \
    memcpy (self->needle, (host), size); \
    self->needle += size; \
}

//  Get a block of octets from the frame
#define GET_OCTETS(host,size) { \
    if (self->needle + size > self->ceiling) { \
        zsys_warning ("wap_proto: GET_OCTETS failed"); \
        goto malformed; \
    } \
    memcpy ((host), self->needle, size); \
    self->needle += size; \
}

//  Put a 1-byte number to the frame
#define PUT_NUMBER1(host) { \
    *(byte *) self->needle = (host); \
    self->needle++; \
}

//  Put a 2-byte number to the frame
#define PUT_NUMBER2(host) { \
    self->needle [0] = (byte) (((host) >> 8)  & 255); \
    self->needle [1] = (byte) (((host))       & 255); \
    self->needle += 2; \
}

//  Put a 4-byte number to the frame
#define PUT_NUMBER4(host) { \
    self->needle [0] = (byte) (((host) >> 24) & 255); \
    self->needle [1] = (byte) (((host) >> 16) & 255); \
    self->needle [2] = (byte) (((host) >> 8)  & 255); \
    self->needle [3] = (byte) (((host))       & 255); \
    self->needle += 4; \
}

//  Put a 8-byte number to the frame
#define PUT_NUMBER8(host) { \
    self->needle [0] = (byte) (((host) >> 56) & 255); \
    self->needle [1] = (byte) (((host) >> 48) & 255); \
    self->needle [2] = (byte) (((host) >> 40) & 255); \
    self->needle [3] = (byte) (((host) >> 32) & 255); \
    self->needle [4] = (byte) (((host) >> 24) & 255); \
    self->needle [5] = (byte) (((host) >> 16) & 255); \
    self->needle [6] = (byte) (((host) >> 8)  & 255); \
    self->needle [7] = (byte) (((host))       & 255); \
    self->needle += 8; \
}

//  Get a 1-byte number from the frame
#define GET_NUMBER1(host) { \
    if (self->needle + 1 > self->ceiling) { \
        zsys_warning ("wap_proto: GET_NUMBER1 failed"); \
        goto malformed; \
    } \
    (host) = *(byte *) self->needle; \
    self->needle++; \
}

//  Get a 2-byte number from the frame
#define GET_NUMBER2(host) { \
    if (self->needle + 2 > self->ceiling) { \
        zsys_warning ("wap_proto: GET_NUMBER2 failed"); \
        goto malformed; \
    } \
    (host) = ((uint16_t) (self->needle [0]) << 8) \
           +  (uint16_t) (self->needle [1]); \
    self->needle += 2; \
}

//  Get a 4-byte number from the frame
#define GET_NUMBER4(host) { \
    if (self->needle + 4 > self->ceiling) { \
        zsys_warning ("wap_proto: GET_NUMBER4 failed"); \
        goto malformed; \
    } \
    (host) = ((uint32_t) (self->needle [0]) << 24) \
           + ((uint32_t) (self->needle [1]) << 16) \
           + ((uint32_t) (self->needle [2]) << 8) \
           +  (uint32_t) (self->needle [3]); \
    self->needle += 4; \
}

//  Get a 8-byte number from the frame
#define GET_NUMBER8(host) { \
    if (self->needle + 8 > self->ceiling) { \
        zsys_warning ("wap_proto: GET_NUMBER8 failed"); \
        goto malformed; \
    } \
    (host) = ((uint64_t) (self->needle [0]) << 56) \
           + ((uint64_t) (self->needle [1]) << 48) \
           + ((uint64_t) (self->needle [2]) << 40) \
           + ((uint64_t) (self->needle [3]) << 32) \
           + ((uint64_t) (self->needle [4]) << 24) \
           + ((uint64_t) (self->needle [5]) << 16) \
           + ((uint64_t) (self->needle [6]) << 8) \
           +  (uint64_t) (self->needle [7]); \
    self->needle += 8; \
}

//  Put a string to the frame
#define PUT_STRING(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER1 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a string from the frame
#define GET_STRING(host) { \
    size_t string_size; \
    GET_NUMBER1 (string_size); \
    if (self->needle + string_size > (self->ceiling)) { \
        zsys_warning ("wap_proto: GET_STRING failed"); \
        goto malformed; \
    } \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}

//  Put a long string to the frame
#define PUT_LONGSTR(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER4 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a long string from the frame
#define GET_LONGSTR(host) { \
    size_t string_size; \
    GET_NUMBER4 (string_size); \
    if (self->needle + string_size > (self->ceiling)) { \
        zsys_warning ("wap_proto: GET_LONGSTR failed"); \
        goto malformed; \
    } \
    free ((host)); \
    (host) = (char *) malloc (string_size + 1); \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}


//  --------------------------------------------------------------------------
//  Create a new wap_proto

wap_proto_t *
wap_proto_new (void)
{
    wap_proto_t *self = (wap_proto_t *) zmalloc (sizeof (wap_proto_t));
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the wap_proto

void
wap_proto_destroy (wap_proto_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        wap_proto_t *self = *self_p;

        //  Free class properties
        zframe_destroy (&self->routing_id);
        if (self->block_ids)
            zlist_destroy (&self->block_ids);
        zmsg_destroy (&self->block_data);
        zchunk_destroy (&self->tx_data);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Receive a wap_proto from the socket. Returns 0 if OK, -1 if
//  there was an error. Blocks if there is no message waiting.

int
wap_proto_recv (wap_proto_t *self, zsock_t *input)
{
    assert (input);
    
    if (zsock_type (input) == ZMQ_ROUTER) {
        zframe_destroy (&self->routing_id);
        self->routing_id = zframe_recv (input);
        if (!self->routing_id || !zsock_rcvmore (input)) {
            zsys_warning ("wap_proto: no routing ID");
            return -1;          //  Interrupted or malformed
        }
    }
    zmq_msg_t frame;
    zmq_msg_init (&frame);
    int size = zmq_msg_recv (&frame, zsock_resolve (input), 0);
    if (size == -1) {
        zsys_warning ("wap_proto: interrupted");
        goto malformed;         //  Interrupted
    }
    //  Get and check protocol signature
    self->needle = (byte *) zmq_msg_data (&frame);
    self->ceiling = self->needle + zmq_msg_size (&frame);
    
    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 0)) {
        zsys_warning ("wap_proto: invalid signature");
        //  TODO: discard invalid messages and loop, and return
        //  -1 only on interrupt
        goto malformed;         //  Interrupted
    }
    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case WAP_PROTO_OPEN:
            {
                char protocol [256];
                GET_STRING (protocol);
                if (strneq (protocol, "WAP")) {
                    zsys_warning ("wap_proto: protocol is invalid");
                    goto malformed;
                }
            }
            {
                uint16_t version;
                GET_NUMBER2 (version);
                if (version != 1) {
                    zsys_warning ("wap_proto: version is invalid");
                    goto malformed;
                }
            }
            GET_STRING (self->identity);
            break;

        case WAP_PROTO_OPEN_OK:
            break;

        case WAP_PROTO_BLOCKS:
            {
                size_t list_size;
                GET_NUMBER4 (list_size);
                self->block_ids = zlist_new ();
                zlist_autofree (self->block_ids);
                while (list_size--) {
                    char *string = NULL;
                    GET_LONGSTR (string);
                    zlist_append (self->block_ids, string);
                    free (string);
                }
            }
            break;

        case WAP_PROTO_BLOCKS_OK:
            GET_NUMBER4 (self->start_height);
            GET_NUMBER4 (self->curr_height);
            GET_STRING (self->block_status);
            //  Get zero or more remaining frames
            zmsg_destroy (&self->block_data);
            if (zsock_rcvmore (input))
                self->block_data = zmsg_recv (input);
            else
                self->block_data = zmsg_new ();
            break;

        case WAP_PROTO_PUT:
            {
                size_t chunk_size;
                GET_NUMBER4 (chunk_size);
                if (self->needle + chunk_size > (self->ceiling)) {
                    zsys_warning ("wap_proto: tx_data is missing data");
                    goto malformed;
                }
                self->tx_data = zchunk_new (self->needle, chunk_size);
                self->needle += chunk_size;
            }
            break;

        case WAP_PROTO_PUT_OK:
            GET_STRING (self->tx_id);
            break;

        case WAP_PROTO_GET:
            GET_STRING (self->tx_id);
            break;

        case WAP_PROTO_GET_OK:
            {
                size_t chunk_size;
                GET_NUMBER4 (chunk_size);
                if (self->needle + chunk_size > (self->ceiling)) {
                    zsys_warning ("wap_proto: tx_data is missing data");
                    goto malformed;
                }
                self->tx_data = zchunk_new (self->needle, chunk_size);
                self->needle += chunk_size;
            }
            break;

        case WAP_PROTO_SAVE:
            break;

        case WAP_PROTO_SAVE_OK:
            break;

        case WAP_PROTO_START:
            GET_NUMBER4 (self->start_height);
            break;

        case WAP_PROTO_START_OK:
            GET_NUMBER4 (self->curr_height);
            break;

        case WAP_PROTO_STOP:
            break;

        case WAP_PROTO_STOP_OK:
            break;

        case WAP_PROTO_CLOSE:
            break;

        case WAP_PROTO_CLOSE_OK:
            break;

        case WAP_PROTO_PING:
            break;

        case WAP_PROTO_PING_OK:
            break;

        case WAP_PROTO_ERROR:
            GET_NUMBER2 (self->status);
            GET_STRING (self->reason);
            break;

        default:
            zsys_warning ("wap_proto: bad message ID");
            goto malformed;
    }
    //  Successful return
    zmq_msg_close (&frame);
    return 0;

    //  Error returns
    malformed:
        zsys_warning ("wap_proto: wap_proto malformed message, fail");
        zmq_msg_close (&frame);
        return -1;              //  Invalid message
}


//  --------------------------------------------------------------------------
//  Send the wap_proto to the socket. Does not destroy it. Returns 0 if
//  OK, else -1.

int
wap_proto_send (wap_proto_t *self, zsock_t *output)
{
    assert (self);
    assert (output);

    if (zsock_type (output) == ZMQ_ROUTER)
        zframe_send (&self->routing_id, output, ZFRAME_MORE + ZFRAME_REUSE);

    size_t frame_size = 2 + 1;          //  Signature and message ID
    switch (self->id) {
        case WAP_PROTO_OPEN:
            frame_size += 1 + strlen ("WAP");
            frame_size += 2;            //  version
            frame_size += 1 + strlen (self->identity);
            break;
        case WAP_PROTO_BLOCKS:
            frame_size += 4;            //  Size is 4 octets
            if (self->block_ids) {
                char *block_ids = (char *) zlist_first (self->block_ids);
                while (block_ids) {
                    frame_size += 4 + strlen (block_ids);
                    block_ids = (char *) zlist_next (self->block_ids);
                }
            }
            break;
        case WAP_PROTO_BLOCKS_OK:
            frame_size += 4;            //  start_height
            frame_size += 4;            //  curr_height
            frame_size += 1 + strlen (self->block_status);
            break;
        case WAP_PROTO_PUT:
            frame_size += 4;            //  Size is 4 octets
            if (self->tx_data)
                frame_size += zchunk_size (self->tx_data);
            break;
        case WAP_PROTO_PUT_OK:
            frame_size += 1 + strlen (self->tx_id);
            break;
        case WAP_PROTO_GET:
            frame_size += 1 + strlen (self->tx_id);
            break;
        case WAP_PROTO_GET_OK:
            frame_size += 4;            //  Size is 4 octets
            if (self->tx_data)
                frame_size += zchunk_size (self->tx_data);
            break;
        case WAP_PROTO_START:
            frame_size += 4;            //  start_height
            break;
        case WAP_PROTO_START_OK:
            frame_size += 4;            //  curr_height
            break;
        case WAP_PROTO_ERROR:
            frame_size += 2;            //  status
            frame_size += 1 + strlen (self->reason);
            break;
    }
    //  Now serialize message into the frame
    zmq_msg_t frame;
    zmq_msg_init_size (&frame, frame_size);
    self->needle = (byte *) zmq_msg_data (&frame);
    PUT_NUMBER2 (0xAAA0 | 0);
    PUT_NUMBER1 (self->id);
    bool send_block_data = false;
    size_t nbr_frames = 1;              //  Total number of frames to send
    
    switch (self->id) {
        case WAP_PROTO_OPEN:
            PUT_STRING ("WAP");
            PUT_NUMBER2 (1);
            PUT_STRING (self->identity);
            break;

        case WAP_PROTO_BLOCKS:
            if (self->block_ids) {
                PUT_NUMBER4 (zlist_size (self->block_ids));
                char *block_ids = (char *) zlist_first (self->block_ids);
                while (block_ids) {
                    PUT_LONGSTR (block_ids);
                    block_ids = (char *) zlist_next (self->block_ids);
                }
            }
            else
                PUT_NUMBER4 (0);    //  Empty string array
            break;

        case WAP_PROTO_BLOCKS_OK:
            PUT_NUMBER4 (self->start_height);
            PUT_NUMBER4 (self->curr_height);
            PUT_STRING (self->block_status);
            nbr_frames += self->block_data? zmsg_size (self->block_data): 1;
            send_block_data = true;
            break;

        case WAP_PROTO_PUT:
            if (self->tx_data) {
                PUT_NUMBER4 (zchunk_size (self->tx_data));
                memcpy (self->needle,
                        zchunk_data (self->tx_data),
                        zchunk_size (self->tx_data));
                self->needle += zchunk_size (self->tx_data);
            }
            else
                PUT_NUMBER4 (0);    //  Empty chunk
            break;

        case WAP_PROTO_PUT_OK:
            PUT_STRING (self->tx_id);
            break;

        case WAP_PROTO_GET:
            PUT_STRING (self->tx_id);
            break;

        case WAP_PROTO_GET_OK:
            if (self->tx_data) {
                PUT_NUMBER4 (zchunk_size (self->tx_data));
                memcpy (self->needle,
                        zchunk_data (self->tx_data),
                        zchunk_size (self->tx_data));
                self->needle += zchunk_size (self->tx_data);
            }
            else
                PUT_NUMBER4 (0);    //  Empty chunk
            break;

        case WAP_PROTO_START:
            PUT_NUMBER4 (self->start_height);
            break;

        case WAP_PROTO_START_OK:
            PUT_NUMBER4 (self->curr_height);
            break;

        case WAP_PROTO_ERROR:
            PUT_NUMBER2 (self->status);
            PUT_STRING (self->reason);
            break;

    }
    //  Now send the data frame
    zmq_msg_send (&frame, zsock_resolve (output), --nbr_frames? ZMQ_SNDMORE: 0);
    
    //  Now send the block_data if necessary
    if (send_block_data) {
        if (self->block_data) {
            zframe_t *frame = zmsg_first (self->block_data);
            while (frame) {
                zframe_send (&frame, output, ZFRAME_REUSE + (--nbr_frames? ZFRAME_MORE: 0));
                frame = zmsg_next (self->block_data);
            }
        }
        else
            zmq_send (zsock_resolve (output), NULL, 0, 0);
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Print contents of message to stdout

void
wap_proto_print (wap_proto_t *self)
{
    assert (self);
    switch (self->id) {
        case WAP_PROTO_OPEN:
            zsys_debug ("WAP_PROTO_OPEN:");
            zsys_debug ("    protocol=wap");
            zsys_debug ("    version=1");
            if (self->identity)
                zsys_debug ("    identity='%s'", self->identity);
            else
                zsys_debug ("    identity=");
            break;
            
        case WAP_PROTO_OPEN_OK:
            zsys_debug ("WAP_PROTO_OPEN_OK:");
            break;
            
        case WAP_PROTO_BLOCKS:
            zsys_debug ("WAP_PROTO_BLOCKS:");
            zsys_debug ("    block_ids=");
            if (self->block_ids) {
                char *block_ids = (char *) zlist_first (self->block_ids);
                while (block_ids) {
                    zsys_debug ("        '%s'", block_ids);
                    block_ids = (char *) zlist_next (self->block_ids);
                }
            }
            break;
            
        case WAP_PROTO_BLOCKS_OK:
            zsys_debug ("WAP_PROTO_BLOCKS_OK:");
            zsys_debug ("    start_height=%ld", (long) self->start_height);
            zsys_debug ("    curr_height=%ld", (long) self->curr_height);
            if (self->block_status)
                zsys_debug ("    block_status='%s'", self->block_status);
            else
                zsys_debug ("    block_status=");
            zsys_debug ("    block_data=");
            if (self->block_data)
                zmsg_print (self->block_data);
            else
                zsys_debug ("(NULL)");
            break;
            
        case WAP_PROTO_PUT:
            zsys_debug ("WAP_PROTO_PUT:");
            zsys_debug ("    tx_data=[ ... ]");
            break;
            
        case WAP_PROTO_PUT_OK:
            zsys_debug ("WAP_PROTO_PUT_OK:");
            if (self->tx_id)
                zsys_debug ("    tx_id='%s'", self->tx_id);
            else
                zsys_debug ("    tx_id=");
            break;
            
        case WAP_PROTO_GET:
            zsys_debug ("WAP_PROTO_GET:");
            if (self->tx_id)
                zsys_debug ("    tx_id='%s'", self->tx_id);
            else
                zsys_debug ("    tx_id=");
            break;
            
        case WAP_PROTO_GET_OK:
            zsys_debug ("WAP_PROTO_GET_OK:");
            zsys_debug ("    tx_data=[ ... ]");
            break;
            
        case WAP_PROTO_SAVE:
            zsys_debug ("WAP_PROTO_SAVE:");
            break;
            
        case WAP_PROTO_SAVE_OK:
            zsys_debug ("WAP_PROTO_SAVE_OK:");
            break;
            
        case WAP_PROTO_START:
            zsys_debug ("WAP_PROTO_START:");
            zsys_debug ("    start_height=%ld", (long) self->start_height);
            break;
            
        case WAP_PROTO_START_OK:
            zsys_debug ("WAP_PROTO_START_OK:");
            zsys_debug ("    curr_height=%ld", (long) self->curr_height);
            break;
            
        case WAP_PROTO_STOP:
            zsys_debug ("WAP_PROTO_STOP:");
            break;
            
        case WAP_PROTO_STOP_OK:
            zsys_debug ("WAP_PROTO_STOP_OK:");
            break;
            
        case WAP_PROTO_CLOSE:
            zsys_debug ("WAP_PROTO_CLOSE:");
            break;
            
        case WAP_PROTO_CLOSE_OK:
            zsys_debug ("WAP_PROTO_CLOSE_OK:");
            break;
            
        case WAP_PROTO_PING:
            zsys_debug ("WAP_PROTO_PING:");
            break;
            
        case WAP_PROTO_PING_OK:
            zsys_debug ("WAP_PROTO_PING_OK:");
            break;
            
        case WAP_PROTO_ERROR:
            zsys_debug ("WAP_PROTO_ERROR:");
            zsys_debug ("    status=%ld", (long) self->status);
            if (self->reason)
                zsys_debug ("    reason='%s'", self->reason);
            else
                zsys_debug ("    reason=");
            break;
            
    }
}


//  --------------------------------------------------------------------------
//  Get/set the message routing_id

zframe_t *
wap_proto_routing_id (wap_proto_t *self)
{
    assert (self);
    return self->routing_id;
}

void
wap_proto_set_routing_id (wap_proto_t *self, zframe_t *routing_id)
{
    if (self->routing_id)
        zframe_destroy (&self->routing_id);
    self->routing_id = zframe_dup (routing_id);
}


//  --------------------------------------------------------------------------
//  Get/set the wap_proto id

int
wap_proto_id (wap_proto_t *self)
{
    assert (self);
    return self->id;
}

void
wap_proto_set_id (wap_proto_t *self, int id)
{
    self->id = id;
}

//  --------------------------------------------------------------------------
//  Return a printable command string

const char *
wap_proto_command (wap_proto_t *self)
{
    assert (self);
    switch (self->id) {
        case WAP_PROTO_OPEN:
            return ("OPEN");
            break;
        case WAP_PROTO_OPEN_OK:
            return ("OPEN_OK");
            break;
        case WAP_PROTO_BLOCKS:
            return ("BLOCKS");
            break;
        case WAP_PROTO_BLOCKS_OK:
            return ("BLOCKS_OK");
            break;
        case WAP_PROTO_PUT:
            return ("PUT");
            break;
        case WAP_PROTO_PUT_OK:
            return ("PUT_OK");
            break;
        case WAP_PROTO_GET:
            return ("GET");
            break;
        case WAP_PROTO_GET_OK:
            return ("GET_OK");
            break;
        case WAP_PROTO_SAVE:
            return ("SAVE");
            break;
        case WAP_PROTO_SAVE_OK:
            return ("SAVE_OK");
            break;
        case WAP_PROTO_START:
            return ("START");
            break;
        case WAP_PROTO_START_OK:
            return ("START_OK");
            break;
        case WAP_PROTO_STOP:
            return ("STOP");
            break;
        case WAP_PROTO_STOP_OK:
            return ("STOP_OK");
            break;
        case WAP_PROTO_CLOSE:
            return ("CLOSE");
            break;
        case WAP_PROTO_CLOSE_OK:
            return ("CLOSE_OK");
            break;
        case WAP_PROTO_PING:
            return ("PING");
            break;
        case WAP_PROTO_PING_OK:
            return ("PING_OK");
            break;
        case WAP_PROTO_ERROR:
            return ("ERROR");
            break;
    }
    return "?";
}

//  --------------------------------------------------------------------------
//  Get/set the identity field

const char *
wap_proto_identity (wap_proto_t *self)
{
    assert (self);
    return self->identity;
}

void
wap_proto_set_identity (wap_proto_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->identity)
        return;
    strncpy (self->identity, value, 255);
    self->identity [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get the block_ids field, without transferring ownership

zlist_t *
wap_proto_block_ids (wap_proto_t *self)
{
    assert (self);
    return self->block_ids;
}

//  Get the block_ids field and transfer ownership to caller

zlist_t *
wap_proto_get_block_ids (wap_proto_t *self)
{
    assert (self);
    zlist_t *block_ids = self->block_ids;
    self->block_ids = NULL;
    return block_ids;
}

//  Set the block_ids field, transferring ownership from caller

void
wap_proto_set_block_ids (wap_proto_t *self, zlist_t **block_ids_p)
{
    assert (self);
    assert (block_ids_p);
    zlist_destroy (&self->block_ids);
    self->block_ids = *block_ids_p;
    *block_ids_p = NULL;
}



//  --------------------------------------------------------------------------
//  Get/set the start_height field

uint32_t
wap_proto_start_height (wap_proto_t *self)
{
    assert (self);
    return self->start_height;
}

void
wap_proto_set_start_height (wap_proto_t *self, uint32_t start_height)
{
    assert (self);
    self->start_height = start_height;
}


//  --------------------------------------------------------------------------
//  Get/set the curr_height field

uint32_t
wap_proto_curr_height (wap_proto_t *self)
{
    assert (self);
    return self->curr_height;
}

void
wap_proto_set_curr_height (wap_proto_t *self, uint32_t curr_height)
{
    assert (self);
    self->curr_height = curr_height;
}


//  --------------------------------------------------------------------------
//  Get/set the block_status field

const char *
wap_proto_block_status (wap_proto_t *self)
{
    assert (self);
    return self->block_status;
}

void
wap_proto_set_block_status (wap_proto_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->block_status)
        return;
    strncpy (self->block_status, value, 255);
    self->block_status [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get the block_data field without transferring ownership

zmsg_t *
wap_proto_block_data (wap_proto_t *self)
{
    assert (self);
    return self->block_data;
}

//  Get the block_data field and transfer ownership to caller

zmsg_t *
wap_proto_get_block_data (wap_proto_t *self)
{
    zmsg_t *block_data = self->block_data;
    self->block_data = NULL;
    return block_data;
}

//  Set the block_data field, transferring ownership from caller

void
wap_proto_set_block_data (wap_proto_t *self, zmsg_t **msg_p)
{
    assert (self);
    assert (msg_p);
    zmsg_destroy (&self->block_data);
    self->block_data = *msg_p;
    *msg_p = NULL;
}


//  --------------------------------------------------------------------------
//  Get the tx_data field without transferring ownership

zchunk_t *
wap_proto_tx_data (wap_proto_t *self)
{
    assert (self);
    return self->tx_data;
}

//  Get the tx_data field and transfer ownership to caller

zchunk_t *
wap_proto_get_tx_data (wap_proto_t *self)
{
    zchunk_t *tx_data = self->tx_data;
    self->tx_data = NULL;
    return tx_data;
}

//  Set the tx_data field, transferring ownership from caller

void
wap_proto_set_tx_data (wap_proto_t *self, zchunk_t **chunk_p)
{
    assert (self);
    assert (chunk_p);
    zchunk_destroy (&self->tx_data);
    self->tx_data = *chunk_p;
    *chunk_p = NULL;
}


//  --------------------------------------------------------------------------
//  Get/set the tx_id field

const char *
wap_proto_tx_id (wap_proto_t *self)
{
    assert (self);
    return self->tx_id;
}

void
wap_proto_set_tx_id (wap_proto_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->tx_id)
        return;
    strncpy (self->tx_id, value, 255);
    self->tx_id [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the status field

uint16_t
wap_proto_status (wap_proto_t *self)
{
    assert (self);
    return self->status;
}

void
wap_proto_set_status (wap_proto_t *self, uint16_t status)
{
    assert (self);
    self->status = status;
}


//  --------------------------------------------------------------------------
//  Get/set the reason field

const char *
wap_proto_reason (wap_proto_t *self)
{
    assert (self);
    return self->reason;
}

void
wap_proto_set_reason (wap_proto_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->reason)
        return;
    strncpy (self->reason, value, 255);
    self->reason [255] = 0;
}



//  --------------------------------------------------------------------------
//  Selftest

int
wap_proto_test (bool verbose)
{
    printf (" * wap_proto: ");

    //  @selftest
    //  Simple create/destroy test
    wap_proto_t *self = wap_proto_new ();
    assert (self);
    wap_proto_destroy (&self);

    //  Create pair of sockets we can send through
    zsock_t *input = zsock_new (ZMQ_ROUTER);
    assert (input);
    zsock_connect (input, "inproc://selftest-wap_proto");

    zsock_t *output = zsock_new (ZMQ_DEALER);
    assert (output);
    zsock_bind (output, "inproc://selftest-wap_proto");

    //  Encode/send/decode and verify each message type
    int instance;
    self = wap_proto_new ();
    wap_proto_set_id (self, WAP_PROTO_OPEN);

    wap_proto_set_identity (self, "Life is short but Now lasts for ever");
    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
        assert (streq (wap_proto_identity (self), "Life is short but Now lasts for ever"));
    }
    wap_proto_set_id (self, WAP_PROTO_OPEN_OK);

    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
    }
    wap_proto_set_id (self, WAP_PROTO_BLOCKS);

    zlist_t *blocks_block_ids = zlist_new ();
    zlist_append (blocks_block_ids, "Name: Brutus");
    zlist_append (blocks_block_ids, "Age: 43");
    wap_proto_set_block_ids (self, &blocks_block_ids);
    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
        zlist_t *block_ids = wap_proto_get_block_ids (self);
        assert (zlist_size (block_ids) == 2);
        assert (streq ((char *) zlist_first (block_ids), "Name: Brutus"));
        assert (streq ((char *) zlist_next (block_ids), "Age: 43"));
        zlist_destroy (&block_ids);
    }
    wap_proto_set_id (self, WAP_PROTO_BLOCKS_OK);

    wap_proto_set_start_height (self, 123);
    wap_proto_set_curr_height (self, 123);
    wap_proto_set_block_status (self, "Life is short but Now lasts for ever");
    zmsg_t *blocks_ok_block_data = zmsg_new ();
    wap_proto_set_block_data (self, &blocks_ok_block_data);
    zmsg_addstr (wap_proto_block_data (self), "Hello, World");
    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
        assert (wap_proto_start_height (self) == 123);
        assert (wap_proto_curr_height (self) == 123);
        assert (streq (wap_proto_block_status (self), "Life is short but Now lasts for ever"));
        assert (zmsg_size (wap_proto_block_data (self)) == 1);
    }
    wap_proto_set_id (self, WAP_PROTO_PUT);

    zchunk_t *put_tx_data = zchunk_new ("Captcha Diem", 12);
    wap_proto_set_tx_data (self, &put_tx_data);
    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
        assert (memcmp (zchunk_data (wap_proto_tx_data (self)), "Captcha Diem", 12) == 0);
    }
    wap_proto_set_id (self, WAP_PROTO_PUT_OK);

    wap_proto_set_tx_id (self, "Life is short but Now lasts for ever");
    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
        assert (streq (wap_proto_tx_id (self), "Life is short but Now lasts for ever"));
    }
    wap_proto_set_id (self, WAP_PROTO_GET);

    wap_proto_set_tx_id (self, "Life is short but Now lasts for ever");
    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
        assert (streq (wap_proto_tx_id (self), "Life is short but Now lasts for ever"));
    }
    wap_proto_set_id (self, WAP_PROTO_GET_OK);

    zchunk_t *get_ok_tx_data = zchunk_new ("Captcha Diem", 12);
    wap_proto_set_tx_data (self, &get_ok_tx_data);
    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
        assert (memcmp (zchunk_data (wap_proto_tx_data (self)), "Captcha Diem", 12) == 0);
    }
    wap_proto_set_id (self, WAP_PROTO_SAVE);

    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
    }
    wap_proto_set_id (self, WAP_PROTO_SAVE_OK);

    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
    }
    wap_proto_set_id (self, WAP_PROTO_START);

    wap_proto_set_start_height (self, 123);
    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
        assert (wap_proto_start_height (self) == 123);
    }
    wap_proto_set_id (self, WAP_PROTO_START_OK);

    wap_proto_set_curr_height (self, 123);
    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
        assert (wap_proto_curr_height (self) == 123);
    }
    wap_proto_set_id (self, WAP_PROTO_STOP);

    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
    }
    wap_proto_set_id (self, WAP_PROTO_STOP_OK);

    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
    }
    wap_proto_set_id (self, WAP_PROTO_CLOSE);

    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
    }
    wap_proto_set_id (self, WAP_PROTO_CLOSE_OK);

    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
    }
    wap_proto_set_id (self, WAP_PROTO_PING);

    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
    }
    wap_proto_set_id (self, WAP_PROTO_PING_OK);

    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
    }
    wap_proto_set_id (self, WAP_PROTO_ERROR);

    wap_proto_set_status (self, 123);
    wap_proto_set_reason (self, "Life is short but Now lasts for ever");
    //  Send twice
    wap_proto_send (self, output);
    wap_proto_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        wap_proto_recv (self, input);
        assert (wap_proto_routing_id (self));
        assert (wap_proto_status (self) == 123);
        assert (streq (wap_proto_reason (self), "Life is short but Now lasts for ever"));
    }

    wap_proto_destroy (&self);
    zsock_destroy (&input);
    zsock_destroy (&output);
    //  @end

    printf ("OK\n");
    return 0;
}
