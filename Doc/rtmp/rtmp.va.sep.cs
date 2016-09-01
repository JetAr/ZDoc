Rtmpdump can download the RTMP flow and save as flv file. 
If the audio or video convection in a separate processing, to extract respectively based on flv protocol. 
Simple modifications to the rtmpdump code, to increase the corresponding function. 
1 audio extraction:
Rtmpdump download cycle in the Download function:
....
do
{
....
nRead = RTMP_Read(rtmp, buffer, bufferSize);
....
}while(!RTMP_ctrlC && nRead > -1 && RTMP_IsConnected(rtmp) && !RTMP_IsTimedout(rtmp));
....

The original program is written documents received, generate flv. 

Now, are extracted from the audio and video before writing, audio extraction is simple, direct analysis of the buffer (Reference method for the RTMP_Write function).

Note that, rtmpdump is used in the RTMP_Read to receive, pay attention to its parameters. For the sake of convenience, can be directly used RTMP_ReadPacket. Behind the video using RTMP_ReadPacket to receive and process. 

int RTMP_Write2(RTMP *r, const char *buf, int size)
{
  RTMPPacket *pkt = &r->m_write;
  char *pend, *enc;
  int s2 = size, ret, num;

 if (size <11) {
   /* FLV pkt too small */
   return 0;
 }

 if (buf[0] == 'F' && buf[1] == 'L' && buf[2] == 'V')
   {
     buf += 13;
     s2 -= 13;
   }

 pkt->m_packetType = *buf++;
 pkt->m_nBodySize = AMF_DecodeInt24(buf);
 buf += 3;
 pkt->m_nTimeStamp = AMF_DecodeInt24(buf);
 buf += 3;
 pkt->m_nTimeStamp |= *buf++ <<24;
 buf += 3;
 s2 -= 11;

 if (((pkt->m_packetType == RTMP_PACKET_TYPE_AUDIO
                || pkt->m_packetType == RTMP_PACKET_TYPE_VIDEO) &&
            !pkt->m_nTimeStamp) || pkt->m_packetType == RTMP_PACKET_TYPE_INFO)
   {
     pkt->m_headerType = RTMP_PACKET_SIZE_LARGE;
     if (pkt->m_packetType == RTMP_PACKET_TYPE_INFO)
      pkt->m_nBodySize += 16;
   }
 else
   {
     pkt->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
   }


BYTE outbuf2[640];
int nLen2 = 640;


AVManager::GetInstance()->Decode((BYTE*)(pkt->m_body+1), pkt->m_nBodySize-1, outbuf2, nLen2);
//The actual audio content for the pkt-> m_body+1, size is pkt-> m_nBodySize-1. Here is the voice of Speex code. 
Why to skip the first byte, can refer to: http://bbs.rosoo.net/thread-16488-1-1.html

evt_OnReceivePacket((char*)outbuf2, nLen2);//The callback.



RTMPPacket_Free(pkt);
pkt->m_nBytesRead = 0;

2 
Video processing
You can refer to the rtmpsrv.c
The nRead = RTMP_Read (RTMP, buffer, bufferSize); change: 

RTMPPacket pc = { 0 }, ps = { 0 };
 bool bFirst = true;
while (RTMP_ReadPacket(rtmp, &pc))
{
if (RTMPPacket_IsReady(&pc))
 {
     if (pc.m_packetType == RTMP_PACKET_TYPE_VIDEO && RTMP_ClientPacket(rtmp, &pc))
    {
        bool bIsKeyFrame = false;
        
    if (result == 0x17)//I frame
    {
        bIsKeyFrame = true;
    }
    else if (result == 0x27)
    {
        bIsKeyFrame = false;
    }

static unsigned char const start_code[4] = {0x00, 0x00, 0x00, 0x01};
fwrite(start_code, 1, 4, pf );
//int ret = fwrite(pc.m_body + 9, 1, pc.m_nBodySize-9, pf);

if( bFirst) {
//AVCsequence header

//ioBuffer.put(foredata);

//Access to SPS
int spsnum = data[10]&0x1f;
int number_sps = 11;
int count_sps = 1;

while (count_sps<=spsnum){

int spslen =(data[number_sps]&0x000000FF)<<8 |(data[number_sps+1]&0x000000FF);
number_sps += 2;

fwrite(data+number_sps, 1, spslen, pf );
fwrite(start_code, 1, 4, pf );

//ioBuffer.put(data,number_sps, spslen);
//ioBuffer.put(foredata);

number_sps += spslen;
count_sps ++;
}

//Get PPS

int ppsnum = data[number_sps]&0x1f;
int number_pps = number_sps+1;
int count_pps = 1;

while (count_pps<=ppsnum){


int ppslen =(data[number_pps]&0x000000FF)<<8|data[number_pps+1]&0x000000FF;


number_pps += 2;


//ioBuffer.put(data,number_pps,ppslen);


//ioBuffer.put(foredata);


fwrite(data+number_pps, 1, ppslen, pf );
fwrite(start_code, 1, 4, pf );


number_pps += ppslen;

count_pps ++;

}

bFirst =false;

} else {

//AVCNALU

int len =0;
int num =5;

//ioBuffer.put(foredata);

while(num<pc.m_nBodySize) 
{
len =(data[num]&0x000000FF)<<24|(data[num+1]&0x000000FF)<<16|(data[num+2]&0x000000FF)<<8|data[num+3]&0x000000FF;
num = num+4;

//ioBuffer.put(data,num,len);
//ioBuffer.put(foredata);

fwrite(data+num, 1, len, pf );
fwrite(start_code, 1, 4, pf );

num = num + len;
}
}       
}
}

The video analysis.