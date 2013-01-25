//
//  main.c
//  jpushdemo
//
//  Created by Xu Jiajun on 13-1-24.
//  Copyright (c) 2013年 Xu Jiajun. All rights reserved.
//

#include <stdio.h>
#include <openssl/md5.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>



#define MAX_BODY 2048
#define APP_KEY ""//AppKey
#define API_MASTER_SECRECT ""//API MasterSecert

void bytes2Hex(char *hexString, void *bytes, int len)
{
    int i;
    unsigned char *rawBytes;
    char tmpStr[3];
    
    rawBytes = (unsigned char *)bytes;
    for (i = 0, hexString[0] = '\0';i < len;i++)
    {
        sprintf(tmpStr, "%02x", rawBytes[i]);
        strcat(hexString, tmpStr);
    }
}

int main(int argc, const char * argv[])
{

    CURL *curl;
    CURLcode res;
    MD5_CTX ctx;
    unsigned char md5Value[16] = {0};
    char md5String[33] = {0};
    
    const char *pSendno = "123456";
    const char *pAppKey = APP_KEY;
    const char *pReceiverType = "4";
    const char *pReceiverValue = "FHL";
    const char *pMasterSecret = API_MASTER_SECRECT;
    char *pForm = (char *)malloc(MAX_BODY);
    char *pTemp = (char *)malloc(MAX_BODY);
    memset(pForm, 0, MAX_BODY);
    memset(pTemp, 0, MAX_BODY);
    
    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);
    
    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        /* First set the URL that is about to receive our POST. This URL can
         just as well be a https:// URL if that is what should receive the
         data. */
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.jpush.cn:443/sendmsg/v2/sendmsg");
        /* Now specify the POST data */
        //sendno
        sprintf(pTemp, "sendno=%s", pSendno);
        strcat(pForm, pTemp);
        //app_key
        sprintf(pTemp, "&app_key=%s", pAppKey);
        strcat(pForm, pTemp);
        //receiver_type
        sprintf(pTemp, "&receiver_type=%s", pReceiverType);
        strcat(pForm, pTemp);
        //receiver_value, in this sample do not need to specify
        sprintf(pTemp, "&receiver_value=%s", pReceiverValue);
        strcat(pForm, pTemp);
        //verification_code
        MD5_Init(&ctx);
        sprintf(pTemp, "%s%s%s%s", pSendno, pReceiverType, pReceiverValue, pMasterSecret);
        MD5_Update(&ctx, pTemp, strlen(pTemp));
        MD5_Final(md5Value, &ctx);
        bytes2Hex(md5String, md5Value, 16);
        sprintf(pTemp, "&verification_code=%s", md5String);
        strcat(pForm, pTemp);
        //msg_type, always 1
        strcat(pForm, "&msg_type=1");
        //msg_content, content of message
        strcat(pForm, "&msg_content={\"n_content\":\"Hello from issacsonjj\"}");
        //send_description, can be ommitted
        strcat(pForm, "&send_description=CC");
        //platform, ios,android
        strcat(pForm, "&platform=android");
        //time_to_live, in seconds
        strcat(pForm, "&time_to_live=3600");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pForm);
        
        /* Perform the request, res will get the return code */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    
    free(pForm);
    free(pTemp);
    
    return 0;
}

