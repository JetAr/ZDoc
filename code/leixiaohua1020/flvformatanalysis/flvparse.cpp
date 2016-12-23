/*
 *
 * FLV 格式分析器
 * FLV Format Analysis
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * FLV封装格式分析工具
 * FLV Container Format Analysis Tools
 *
 *
 */
// This app splits an FLV file based on cue points located in a data file.
// It also separates the video from the audio.
// All the reverse_bytes action stems from the file format being big-endian
// and needing an integer to hold a little-endian version (for proper calculation).

#include "stdafx.h"
#include "flvparse.h"
#pragma pack(1)

//************ Constants
#define TAG_TYPE_AUDIO 8
#define TAG_TYPE_VIDEO 9
#define TAG_TYPE_SCRIPT 18

//*********** TYPEDEFs
typedef unsigned char ui_24[3];
typedef unsigned char byte;
typedef unsigned int uint;

typedef struct
{
    byte Signature[3];
    byte Version;
    byte Flags;
    uint DataOffset;
} FLV_HEADER;

typedef struct
{
    byte TagType;
    ui_24 DataSize;
    ui_24 Timestamp;
    uint Reserved;
} TAG_HEADER;

CSpecialFFLVDlg *dlg;
int fist_time_a=0;
int fist_time_v=0;

//********* global variables
uint flags = 0;
char project_name[_MAX_PATH];



//reverse_bytes - turn a BigEndian byte array into a LittleEndian integer
uint reverse_bytes(byte *p, char c)
{
    int r = 0;
    int i;
    for (i=0; i<c; i++)
        r |= ( *(p+i) << (((c-1)*8)-8*i));
    return r;
}


//This function handles iterative file naming and opening
FILE* open_output_file(byte tag)
{
    //instantiate two buffers
    char file_name[_MAX_PATH], ext[4];
    //determine the file extension
    strcpy(ext, (tag==TAG_TYPE_AUDIO ? "mp3\0" : "flv\0"));
    //build the file name
    sprintf(file_name, "%s.%s", project_name, ext);
    //return the file pointer
    return fopen(file_name, "wb");

}


//processfile is the central function
void flvparse(LPVOID lparam,char *in_file)
{
    //---------
    fist_time_a=0;
    fist_time_v=0;
    //-------------
    FILE *ifh=NULL, *vfh=NULL, *afh = NULL;
    FLV_HEADER flv;
    TAG_HEADER tagheader;
    uint previoustagsize, previoustagsize_z=0;
    uint ts=0, ts_new=0, ts_offset=0, ptag=0;

    dlg=(CSpecialFFLVDlg *)lparam;

    int output_video=0;
    int output_audio=0;
    output_video=dlg->m_fflvoutputv.GetCheck();
    output_audio=dlg->m_fflvoutputa.GetCheck();

    //open the input file
    if ( (ifh = fopen(in_file, "rb")) == NULL)
    {
        AfxMessageBox(_T("Failed to open files!"));
        return;
    }
    //set project name
    strncpy(project_name, in_file, strstr(in_file, ".flv")-in_file);

    //capture the FLV file header
    //FLV file header
    fread((char *)&flv,1,sizeof(FLV_HEADER),ifh);

    CString temp_str;
    temp_str.Format(_T("0x %X %X %X"),flv.Signature[0],flv.Signature[1],flv.Signature[2]);
    dlg->AppendBInfo(_T("Header"),_T("Signature"),temp_str,_T("Signature"));
    temp_str.Format(_T("0x %X"),flv.Version);
    dlg->AppendBInfo(_T("Header"),_T("Version"),temp_str,_T("Version"));
    temp_str.Format(_T("0x %X"),flv.Flags);
    dlg->AppendBInfo(_T("Header"),_T("Flag"),temp_str,_T("Flag"));


    temp_str.Format(_T("0x %X"),reverse_bytes((byte *)&flv.DataOffset, sizeof(flv.DataOffset)));
    dlg->AppendBInfo(_T("Header"),_T("HeaderSize"),temp_str,_T("HeaderSize"));


    fseek(ifh, reverse_bytes((byte *)&flv.DataOffset, sizeof(flv.DataOffset)), SEEK_SET);

    //move the file pointer to the end of the header

    //process each tag in the file
    do
    {

        //capture the PreviousTagSize integer
        previoustagsize = _getw(ifh);

        int aaa=ftell(ifh);
        fread((void *)&tagheader,sizeof(TAG_HEADER),1,ifh);


        //输出Tag信息-----------------------
        int temp_datasize=tagheader.DataSize[0]*65536+tagheader.DataSize[1]*256+tagheader.DataSize[2];
        int temp_timestamp=tagheader.Timestamp[0]*65536+tagheader.Timestamp[1]*256+tagheader.Timestamp[2];


        //set the tag value to select on
        ptag = tagheader.TagType;

        //if we are not past the end of file, process the tag
        if (feof(ifh))
        {
            break;
        }

        //process tag by type
        switch (ptag)
        {

        case TAG_TYPE_AUDIO:   //we only process like this if we are separating audio into an mp3 file
        {
            //还需要获取TagData的第一个字节---------------------------------
            char tagdata_first_byte=fgetc(ifh);
            dlg->AppendTLInfo(tagheader.TagType,temp_datasize,temp_timestamp,tagheader.Reserved,tagdata_first_byte);

            //if the output file hasn't been opened, open it.
            if(output_audio&&afh == NULL)
            {
                afh = open_output_file(ptag);
            }


            int data_size=reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize))-1;
            //决定是否输出
            if(output_audio)
            {
                //dump the audio data to the output file
                for (int i=0; i<data_size; i++)
                    fputc(fgetc(ifh),afh);
            }
            else
            {
                for (int i=0; i<data_size; i++)
                    fgetc(ifh);
            }
            break;
        }
        case TAG_TYPE_VIDEO:
        {
            //还需要获取TagData的第一个字节---------------------------------

            char tagdata_first_byte=fgetc(ifh);
            dlg->AppendTLInfo(tagheader.TagType,temp_datasize,temp_timestamp,tagheader.Reserved,tagdata_first_byte);
            //倒回去，不影响下面的操作
            fseek(ifh, -1, SEEK_CUR);
            //if the output file hasn't been opened, open it.
            if (vfh == NULL)
            {

                //record the timestamp offset for this slice
                ts_offset = reverse_bytes((byte *)&tagheader.Timestamp, sizeof(tagheader.Timestamp));

                //write the flv header (reuse the original file's hdr) and first previoustagsize
                if(output_video)
                {
                    vfh = open_output_file(ptag);
                    fwrite((char *)&flv,1, sizeof(flv),vfh);
                    fwrite((char *)&previoustagsize_z,1,sizeof(previoustagsize_z),vfh);
                }
            }

#if 0
            //offset the timestamp in the tag
            ts = reverse_bytes((byte *)&tagheader.Timestamp, sizeof(tagheader.Timestamp)) - ts_offset;

            //reverse the timestamp bytes back into BigEndian
            ts_new = reverse_bytes((byte *)&ts, sizeof(ts));

            //overwrite the highest 3 bytes of the integer into the timestamp
            memcpy(&tagheader.Timestamp, ((char *)&ts_new) + 1, sizeof(tagheader.Timestamp));
#endif


            int data_size=reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize))+4;
            //决定是否输出
            if(output_video)
            {
                //dump the video data to the output file, including the previoustagsize field
                //TagHeader
                fwrite((char *)&tagheader,1, sizeof(tagheader),vfh);
                //TagData
                for (int i=0; i<data_size; i++)
                    fputc(fgetc(ifh),vfh);
            }
            else
            {
                for (int i=0; i<data_size; i++)
                    fgetc(ifh);
            }
            //rewind 4 bytes, because we need to read the previoustagsize again for the loop's sake
            fseek(ifh, -4, SEEK_CUR);

            break;
        }
        case TAG_TYPE_SCRIPT:
        {
            char tagdata_first_byte=fgetc(ifh);
            dlg->AppendTLInfo(tagheader.TagType,temp_datasize,temp_timestamp,tagheader.Reserved,tagdata_first_byte);
            fseek(ifh, -1, SEEK_CUR);

            fseek(ifh, reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize)), SEEK_CUR);
            break;
        }
        default:
        {
            fseek(ifh, reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize)), SEEK_CUR);
            break;
        }
        }


    }
    while (!feof(ifh));

    //finished...close all file pointers
    _fcloseall();

    //feedback to user
    dlg->text.LoadString(IDS_FINISH);
    AfxMessageBox(dlg->text);

}


