#include "protocol.h"
#include<string.h>
#include<stdlib.h>
PDU *mkPDU(uint uiMsgLen)
{
    uint uiPDULen=sizeof(PDU)+uiMsgLen;
    //总消息长度
    PDU* pdu=(PDU*)malloc(uiPDULen);
    //申请总大小
    if(pdu==NULL){
        exit(-1);
    }//判空
    memset(pdu,0,uiPDULen);//对申请的空间进行初始化
    pdu->uiPDULen=uiPDULen;
    pdu->uiMsgLen=uiMsgLen;
    //初始化pdu
    return pdu;
}
