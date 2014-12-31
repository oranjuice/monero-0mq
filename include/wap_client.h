/*  =========================================================================
    wap_client - Wallet Client API

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: wap_client.xml, or
     * The code generation script that built this file: zproto_client_c
    ************************************************************************
    Copyright (c) the Contributors as noted in the AUTHORS file.
                                                                
    (insert license text here)                                  
    =========================================================================
*/

#ifndef __WAP_CLIENT_H_INCLUDED__
#define __WAP_CLIENT_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
#ifndef WAP_CLIENT_T_DEFINED
typedef struct _wap_client_t wap_client_t;
#define WAP_CLIENT_T_DEFINED
#endif

//  @interface
//  Create a new wap_client
//  Connect to server endpoint, with specified timeout in msecs (zero means wait    
//  forever). Constructor succeeds if connection is successful. The caller may      
//  specify its address.                                                            
WAP_EXPORT wap_client_t *
    wap_client_new (const char *endpoint, int timeout, const char *identity);

//  Destroy the wap_client
WAP_EXPORT void
    wap_client_destroy (wap_client_t **self_p);

//  Enable verbose logging of client activity
WAP_EXPORT void
    wap_client_verbose (wap_client_t *self);

//  Return actor, when caller wants to work with multiple actors and/or
//  input sockets asynchronously.
WAP_EXPORT zactor_t *
    wap_client_actor (wap_client_t *self);

//  Return message pipe for asynchronous message I/O. In the high-volume case,
//  we send methods and get replies to the actor, in a synchronous manner, and
//  we send/recv high volume message data to a second pipe, the msgpipe. In
//  the low-volume case we can do everything over the actor pipe, if traffic
//  is never ambiguous.
WAP_EXPORT zsock_t *
    wap_client_msgpipe (wap_client_t *self);

//  Send start command to server.                                                   
//  Returns >= 0 if successful, -1 if interrupted.
WAP_EXPORT int
    wap_client_start (wap_client_t *self);

//  Send stop command to server.                                                    
//  Returns >= 0 if successful, -1 if interrupted.
WAP_EXPORT int
    wap_client_stop (wap_client_t *self);

//  Return last received status
WAP_EXPORT int 
    wap_client_status (wap_client_t *self);

//  Return last received reason
WAP_EXPORT const char *
    wap_client_reason (wap_client_t *self);

//  Self test of this class
WAP_EXPORT void
    wap_client_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
