#include <stdlib.h>
#include <stdio.h>

#include <check.h>

#include "stunclient.h"
#include "sockaddr_util.h"

Suite * stunclient_suite (void);

#define  MAX_INSTANCES  50
#define  TEST_THREAD_CTX 1

#define  TEST_IPv4_ADDR 
#define  TEST_IPv4_PORT
#define  TEST_IPv6_ADDR 

typedef  struct
{
    uint32_t a;
    uint8_t  b;
}
AppCtx_T;

static AppCtx_T           AppCtx[MAX_INSTANCES];
static StunCallBackData_T StunCbData[MAX_INSTANCES];

static AppCtx_T CurrAppCtx;

static StunMsgId      LastTransId;
static struct sockaddr_storage LastAddress;
static bool runningAsIPv6;

static  const uint8_t StunCookie[]   = STUN_MAGIC_COOKIE_ARRAY;

StunResult_T stunResult;

struct sockaddr_storage stunServerAddr;

DiscussData discussData;

STUN_CLIENT_DATA *stunInstance;
#define STUN_TICK_INTERVAL_MS 50

/*
static void StunStatusCallBack(void *ctx, StunCallBackData_T *retData)
{
    stunResult = retData->stunResult;
    //printf("Got STUN status callback\n");// (Result (%i)\n", retData->stunResult);
}
*/
/* Callback for management info  */
/*
static void  PrintStunInfo(StunInfoCategory_T category, char *InfoStr)
{
    //fprintf(stderr, "%s\n", ErrStr);
}
*/
static void SendRawStun(int sockfd, 
                        const uint8_t *buf, 
                        int len, 
                        const struct sockaddr *addr,
                        bool useRelay,
                        const uint8_t ttl)
{
    char addr_str[SOCKADDR_MAX_STRLEN];
    /* find the transaction id  so we can use this in the simulated resp */

    memcpy(&LastTransId, &buf[8], STUN_MSG_ID_SIZE); 

    sockaddr_copy((struct sockaddr *)&LastAddress, addr);
    
    sockaddr_toString(addr, addr_str, SOCKADDR_MAX_STRLEN, true);
                      
    //printf("Sendto: '%s'\n", addr_str);

}

static void setup (void)
{
    stunResult = StunResult_Empty; 
    runningAsIPv6 = false;
    sockaddr_initFromString((struct sockaddr*)&stunServerAddr, "193.200.93.152:3478");

    StunClient_Alloc(&stunInstance);

}

static void teardown (void)
{
    stunResult = StunResult_Empty; 
    StunClient_free(stunInstance);
}


static void setupIPv6 (void)
{
    stunResult = StunResult_Empty; 
    runningAsIPv6 = true;
    sockaddr_initFromString((struct sockaddr*)&stunServerAddr, "[2001:470:dc88:2:226:18ff:fe92:6d53]:3478");
    StunClient_Alloc(&stunInstance);

}

static void teardownIPv6 (void)
{
    stunResult = StunResult_Empty; 
    StunClient_free(stunInstance);
}


START_TEST (HandleReq_Valid)
{
    STUN_INCOMING_REQ_DATA pReq;
    StunMessage stunMsg;
    stunMsg.hasUsername = true;
    stunMsg.username.sizeValue = 10;
    strncpy(stunMsg.username.value, "testPerson", stunMsg.username.sizeValue);
    stunMsg.username.value[stunMsg.username.sizeValue]= '\0';
    stunMsg.hasPriority = true;
    stunMsg.priority.value = 1;

    bool fromRelay = false;
    
    fail_unless (StunServer_HandleStunIncomingBindReqMsg(stunInstance,
                                                         &pReq,
                                                         &stunMsg,
                                                         fromRelay));

    char ufrag[STUN_MAX_STRING] = "testPerson";
    fail_unless (strcmp(pReq.ufrag, ufrag) == 0);

    //fail_unless (stunInstance->stats.BindReqReceived);
    //fail_unless (stunInstance->stats.BindReqReceived_ViaRelay == 0);
    
    fromRelay = true;
    fail_unless (StunServer_HandleStunIncomingBindReqMsg(stunInstance,
                                                         &pReq,
                                                         &stunMsg,
                                                         fromRelay));
    
    //fail_unless (stunInstance->stats.BindReqReceived == 2);
    //fail_unless (stunInstance->stats.BindReqReceived_ViaRelay == 1);
}
END_TEST


START_TEST (HandleReq_InValid)
{
    STUN_INCOMING_REQ_DATA pReq;
    StunMessage stunMsg;
    stunMsg.hasUsername = false;
    stunMsg.username.sizeValue = 10;
    strncpy(stunMsg.username.value, "testPerson", stunMsg.username.sizeValue);
    stunMsg.username.value[stunMsg.username.sizeValue]= '\0';
    stunMsg.hasPriority = true;
    stunMsg.priority.value = 1;

    bool fromRelay = false;
    
    fail_unless (!StunServer_HandleStunIncomingBindReqMsg(stunInstance,
                                                          &pReq,
                                                          &stunMsg,
                                                          fromRelay));

    //fail_unless (stunInstance->stats.BindReqReceived == 0);
    //fail_unless (stunInstance->stats.BindReqReceived_ViaRelay == 0);
    
    fromRelay = true;
    stunMsg.hasUsername = true;
    stunMsg.hasPriority = false;
    fail_unless (!StunServer_HandleStunIncomingBindReqMsg(stunInstance,
                                                          &pReq,
                                                          &stunMsg,
                                                          fromRelay));
    
    //fail_unless (stunInstance->stats.BindReqReceived == 0);
    //fail_unless (stunInstance->stats.BindReqReceived_ViaRelay == 0);
}
END_TEST


START_TEST (SendResp_Valid)
{
    bool useRelay = false;
    
    

    fail_unless (StunServer_SendConnectivityBindingResp(stunInstance,
                                                        0, // sockhandle
                                                        LastTransId,
                                                        "pem",
                                                        (struct sockaddr *)&stunServerAddr,
                                                        (struct sockaddr *)&stunServerAddr,
                                                        NULL,
                                                        SendRawStun,
                                                        useRelay,
                                                        0, // responseCode
                                                        NULL));

}
END_TEST

START_TEST (SendDiscussResp_Valid)
{
    bool useRelay = false;
    
    discussData.streamType=0x004;
    discussData.interactivity=0x01;

    discussData.networkStatus_flags = 0;
    discussData.networkStatus_nodeCnt = 0;
    discussData.networkStatus_tbd = 0;
    discussData.networkStatus_upMaxBandwidth = 0;
    discussData.networkStatus_downMaxBandwidth = 0;


    fail_unless (StunServer_SendConnectivityBindingResp(stunInstance,
                                                        0, // sockhandle
                                                        LastTransId,
                                                        "pem",
                                                        (struct sockaddr *)&stunServerAddr,
                                                        (struct sockaddr *)&stunServerAddr,
                                                        NULL,
                                                        SendRawStun,
                                                        useRelay,
                                                        0, // responseCode
                                                        &discussData));

}
END_TEST



START_TEST (SendResp_InValid)
{
    
}

END_TEST
Suite * stunserver_suite (void)
{
  Suite *s = suite_create ("Stunserver");


  {/* handle request */

      TCase *ss_handleReq = tcase_create ("Stunserver handle incomming request");

      tcase_add_checked_fixture (ss_handleReq, setup, teardown);
      
      tcase_add_test (ss_handleReq, HandleReq_Valid);
      tcase_add_test (ss_handleReq, HandleReq_InValid);
      
      suite_add_tcase (s, ss_handleReq);

  }

  {/* send response */

      TCase *ss_sendResp = tcase_create ("Stunserver send response");

      tcase_add_checked_fixture (ss_sendResp, setup, teardown);

      tcase_add_test (ss_sendResp, SendResp_Valid);
      tcase_add_test (ss_sendResp, SendResp_InValid);

      suite_add_tcase (s, ss_sendResp);

  }

  {/* send response IPv6*/

      TCase *ss_sendRespIPv6 = tcase_create ("Stunserver send response IPv6");

      tcase_add_checked_fixture (ss_sendRespIPv6, setupIPv6, teardownIPv6);

      tcase_add_test (ss_sendRespIPv6, SendResp_Valid);
      tcase_add_test (ss_sendRespIPv6, SendResp_InValid);

      suite_add_tcase (s, ss_sendRespIPv6);

  }
  {/* send Discuss response */
      
      TCase *ss_sendDiscussResp = tcase_create ("Stunserver send Discuss response");
      
      tcase_add_checked_fixture (ss_sendDiscussResp, setup, teardown);
      
      tcase_add_test (ss_sendDiscussResp, SendDiscussResp_Valid);
     
      
      suite_add_tcase (s, ss_sendDiscussResp);
      
  }
  
  return s;
}
