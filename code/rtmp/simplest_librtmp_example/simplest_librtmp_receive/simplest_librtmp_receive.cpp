/**
 * Simplest Librtmp Receive
 *
 * 雷霄骅，张晖
 * leixiaohua1020@126.com
 * zhanghuicuc@gmail.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本程序用于接收RTMP流媒体并在本地保存成FLV格式的文件。
 * This program can receive rtmp live stream and save it as local flv file.
 */
#include <wchar.h>
#include <stdio.h>
#include "librtmp/rtmp_sys.h"
#include "librtmp/log.h"

int InitSockets()
{
	WORD version;
	WSADATA wsaData;
	version = MAKEWORD(1, 1);
	return (WSAStartup(version, &wsaData) == 0);
}

void CleanupSockets()
{
	WSACleanup();
}

int main(int argc, char* argv[])
{
	InitSockets();

	double duration=-1;
	int nRead;
	//is live stream ?
	bool bLiveStream=true;				

	int bufsize=1024*1024*10;
	char *buf=(char*)malloc(bufsize);
	memset(buf,0,bufsize);
	long countbufsize=0;

	FILE *pf=fopen("receive.flv","wb");
	if (!pf){
		RTMP_LogPrintf("Open File Error.\n");
		CleanupSockets();

		return -1;
	}

	/* set log level */
	//RTMP_LogLevel loglvl=RTMP_LOGDEBUG;
	//RTMP_LogSetLevel(loglvl);

	RTMP *rtmp=RTMP_Alloc();
	RTMP_Init(rtmp);
	//set connection timeout,default 30s
	rtmp->Link.timeout=10;	
	// HKS's live URL
	//z if(!RTMP_SetupURL(rtmp,"rtmp://live.hkstv.hk.lxdns.com/live/hks"))
	if(!RTMP_SetupURL(rtmp,"rtmp://127.0.0.1/live/demo"))
	{
		RTMP_Log(RTMP_LOGERROR,"SetupURL Err\n");
		RTMP_Free(rtmp);
		CleanupSockets();
		return -1;
	}
	if (bLiveStream){
		rtmp->Link.lFlags|=RTMP_LF_LIVE;
	}

	//1hour
	RTMP_SetBufferMS(rtmp, 3600*1000);		

	if(!RTMP_Connect(rtmp,NULL)){
		RTMP_Log(RTMP_LOGERROR,"Connect Err\n");
		RTMP_Free(rtmp);
		CleanupSockets();
		return -1;
	}

	if(!RTMP_ConnectStream(rtmp,0)){
		RTMP_Log(RTMP_LOGERROR,"ConnectStream Err\n");
		RTMP_Free(rtmp);
		RTMP_Close(rtmp);
		CleanupSockets();
		return -1;
	}

	/*
	while(nRead=RTMP_Read(rtmp,buf,bufsize)){
	fwrite(buf,1,nRead,pf);

	countbufsize+=nRead;
	RTMP_LogPrintf("Receive: %5dByte, Total: %5.2fkB\n",nRead,countbufsize*1.0/1024);
	}*/

	RTMPPacket pc = { 0 }, ps = { 0 };
	bool bFirst = true;
	while (RTMP_ReadPacket(rtmp, &pc))
	{
		if (RTMPPacket_IsReady(&pc))
		{
			if (pc.m_packetType == RTMP_PACKET_TYPE_VIDEO && RTMP_ClientPacket(rtmp, &pc))
			{
				bool bIsKeyFrame = false;

				//z FrameType 1 keyframe， 7 avc
				if (pc.m_body[0] == 0x17)//I frame
				{
					bIsKeyFrame = true;
					printf("Key Frame\r\n");
				}
				//z FrameType 2 interframe， 7 avc
				else if (pc.m_body[0] == 0x27)
				{
					bIsKeyFrame = false;
					printf("Inter Frame\r\n");
				}
				else if(pc.m_body[0] == 0x57)
				{
					printf("%c,video info/command Frame\r\n",pc.m_body[0]);
				}
				else
				{
					printf("%c, unknown Frame\r\n",pc.m_body[0]);
				}

				//z 先写入 start_code
				static unsigned char const start_code[4] = {0x00, 0x00, 0x00, 0x01};
				//int ret = fwrite(pc.m_body + 9, 1, pc.m_nBodySize-9, pf);

				//z 先提取 sps 以及 pps
				if( bFirst)
				{
					//AVCsequence header
					//ioBuffer.put(foredata);
					//获取sps
					//z sps 个数
					int spsnum = pc.m_body[10]&0x1f;
					int number_sps = 11;
					int count_sps = 1;

					while (count_sps<=spsnum)
					{
						int spslen =(pc.m_body[number_sps]&0x000000FF)<<8 |(pc.m_body[number_sps+1]&0x000000FF);
						number_sps += 2;

						fwrite(start_code, 1, 4, pf );
						fwrite(pc.m_body+number_sps, 1, spslen, pf );

						number_sps += spslen;
						count_sps ++;
					}
					//获取pps
					int ppsnum = pc.m_body[number_sps]&0x1f;
					int number_pps = number_sps+1;
					int count_pps = 1;

					while (count_pps<=ppsnum)
					{
						int ppslen =(pc.m_body[number_pps]&0x000000FF)<<8|pc.m_body[number_pps+1]&0x000000FF;
						number_pps += 2;

						fwrite(start_code, 1, 4, pf );
						fwrite(pc.m_body+number_pps, 1, ppslen, pf );

						number_pps += ppslen;
						count_pps ++;
					}

					bFirst =false;
				}
				else
				{
					//AVCNALU
					int len =0;
					//z video tag hdr
					int num =5;

					fwrite(start_code, 1, 4, pf );
					//ioBuffer.put(foredata);

					while(num <= pc.m_nBodySize - 4)
					{
						//z 获取 nalu 包的长度
						len =(pc.m_body[num]&0x000000FF)<<24|(pc.m_body[num+1]&0x000000FF)<<16|(pc.m_body[num+2]&0x000000FF)<<8|pc.m_body[num+3]&0x000000FF;
						static char strDbuf[256] = {0};
						sprintf(strDbuf,"nal len : %d\r\n",len);
						printf(strDbuf);

						num = num+4;

						fwrite(pc.m_body+num, 1, len, pf );
						num = num + len;
					}
				}
			}
		}
	}

	if(pf)
		fclose(pf);

	if(buf){
		free(buf);
	}

	if(rtmp){
		RTMP_Close(rtmp);
		RTMP_Free(rtmp);
		CleanupSockets();
		rtmp=NULL;
	}	
	return 0;
}
