/*
Copyright 2014 Cisco. All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, are 
permitted provided that the following conditions are met: 

   1. Redistributions of source code must retain the above copyright notice, this list of 
      conditions and the following disclaimer. 

   2. Redistributions in binary form must reproduce the above copyright notice, this list 
      of conditions and the following disclaimer in the documentation and/or other materials 
      provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY CISCO ''AS IS'' AND ANY EXPRESS OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
DAMAGE. 

The views and conclusions contained in the software and documentation are those of the 
authors and should not be interpreted as representing official policies, either expressed 
or implied, of Cisco.
*/

#ifndef STUN_INTERN_H
#define STUN_INTERN_H

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STUNCLIENT_CTX_UNKNOWN  -1
#define STUN_MAX_ERR_STRLEN    256  /* max size of string in STUN_INFO_FUNC */

/* Internal STUN signals, inputs to stun bind client. */
typedef enum {
    STUN_SIGNAL_BindReq,
    STUN_SIGNAL_BindResp,
    STUN_SIGNAL_BindRespError,
    STUN_SIGNAL_TimerTick,
    STUN_SIGNAL_TimerRetransmit,
    STUN_SIGNAL_ICMPResp,
    STUN_SIGNAL_DeAllocate,
    STUN_SIGNAL_Cancel,

    STUN_SIGNAL_Illegal = -1
} STUN_SIGNAL;


typedef struct
{
    struct sockaddr_storage srcAddr;
    StunMessage             stunRespMessage;
    uint32_t                ICMPtype;
    uint32_t                ttl;
}
StunRespStruct;



/* Internal STUN  states */
typedef enum {
    STUN_STATE_Idle = 0,
    STUN_STATE_WaitBindResp,
    STUN_STATE_Cancelled,
    STUN_STATE_End  /* must be last */
} STUN_STATE;


/* Internal message formats */
typedef struct {
    uint32_t                 threadCtx;
    void                    *userCtx;
    struct sockaddr_storage  serverAddr;
    struct sockaddr_storage  baseAddr;
    bool                     useRelay;
    char                     ufrag[300];    /* TBD  =  ICE_MAX_UFRAG_LENGTH*/
    char                     password[300]; /* TBD = ICE_MAX_PASSWD_LENGTH*/
    uint32_t                 peerPriority;
    bool                     useCandidate;
    bool                     iceControlling;
    uint64_t                 tieBreaker;
    uint8_t                  ttl;
    StunMsgId                transactionId;
    uint32_t                 sockhandle;
    STUN_SENDFUNC            sendFunc;
    STUNCB                   stunCbFunc;
    DiscussData             *discussData; /*NULL allowed if none present */
    bool addSoftware;
} StunBindReqStuct;

struct StunClientStats
{
    uint32_t InProgress;
    uint32_t BindReqSent;
    uint32_t BindReqSent_ViaRelay;
    uint32_t BindRespReceived;
    uint32_t BindRespReceived_AfterCancel;
    uint32_t BindRespReceived_InIdle;
    uint32_t BindRespReceived_ViaRelay;
    uint32_t BindRespErrReceived;
    uint32_t ICMPReceived;
    uint32_t BindReqReceived;
    uint32_t BindReqReceived_ViaRelay;
    uint32_t BindRespSent;
    uint32_t BindRespSent_ViaRelay;
    uint32_t Retransmits;
    uint32_t Failures;
};

typedef struct
{
    STUN_STATE             state;
    bool                   inUse;
    uint32_t               inst;
    StunBindReqStuct       stunBindReq;

    uint8_t      stunReqMsgBuf[STUN_MAX_PACKET_SIZE];  /* encoded STUN request    */
    int          stunReqMsgBufLen;                     /* of encoded STUN request */

    STUN_USER_CREDENTIALS userCredentials;
    bool authenticated;

    /* returned in allocate resp */
    struct sockaddr_storage rflxAddr;

    /* timers */
    int32_t TimerRetransmit;
    uint32_t     retransmits;

    /* RTT Info */
    struct timeval start;
    struct timeval stop;
    
    /* icmp */
    uint32_t ICMPtype;
    uint32_t ttl;

    /* DISCUSS */
    bool hasDiscuss;
    DiscussData discussData;

    struct StunClientStats stats;

    STUN_CLIENT_DATA *client;

    
} STUN_TRANSACTION_DATA;


struct STUN_CLIENT_DATA
{
    STUN_TRANSACTION_DATA data [MAX_STUN_TRANSACTIONS];


    /*duplicated for logging on unknown transactions etc.*/
    STUN_INFO_FUNC_PTR   Log_cb;
    void * logUserData;
    struct StunClientStats stats;
};


/********************************************/
/******  instance data ********  (internal) */
/********************************************/


/* state function */
typedef void (*STUN_STATE_FUNC)(STUN_TRANSACTION_DATA *pInst, STUN_SIGNAL sig, uint8_t *payload);

/* entry in state table */
typedef struct
{
    STUN_STATE_FUNC Statefunc;
    const char *StateStr;
}
STUN_STATE_TABLE;



#ifdef __cplusplus
}
#endif


#endif  /* STUN_INTERN_H */
