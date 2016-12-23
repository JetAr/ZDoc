/*
 * FFmpeg for MFC
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 *
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本工程将ffmpeg工程中的ffmpeg转码器（ffmpeg.c）移植到了VC环境下。
 * 并且使用MFC做了一套简单的界面。
 * This software transplant ffmpeg to Microsoft VC++ environment.
 * And use MFC to build a simple Graphical User Interface.
 */

#include "stdafx.h"
#include "ffmpegmfc.h"
#include "TaskinfoDlg.h"
#include "ffmpegmfcDlg.h"
#include "afxdialogex.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
};

// TaskinfoDlg 对话框

IMPLEMENT_DYNAMIC(TaskinfoDlg, CDialogEx)

TaskinfoDlg::TaskinfoDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(TaskinfoDlg::IDD, pParent)
{

}

TaskinfoDlg::~TaskinfoDlg()
{
}

void TaskinfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    //  DDX_Control(pDX, IDC_TASKINFO_INFILE, m_taskinfoinfile);
    //  DDX_Control(pDX, IDC_TASKINFO_OUTFILE, m_taskinfooutfile);
    DDX_Control(pDX, IDC_TASKINFO_PARAM, m_taskinfoparam);
    DDX_Control(pDX, IDC_CODECA_CHANNELS, m_codecachannels);
    DDX_Control(pDX, IDC_CODECA_NAME, m_codecaname);
    DDX_Control(pDX, IDC_CODECA_SAMPLERATE, m_codecasamplerate);
    DDX_Control(pDX, IDC_CODECV_FRAMERATE, m_codecvframerate);
    DDX_Control(pDX, IDC_CODECV_NAME, m_codecvname);
    DDX_Control(pDX, IDC_CODECV_RESOLUTION, m_codecvresolution);
    DDX_Control(pDX, IDC_FORMAT_BITRATE, m_formatbitrate);
    DDX_Control(pDX, IDC_FORMAT_DURATION, m_formatduration);
    DDX_Control(pDX, IDC_FORMAT_INPUTFORMAT, m_formatinputformat);
    DDX_Control(pDX, IDC_TASKINFO_INFILE, m_taskinfoinfile);
    DDX_Control(pDX, IDC_TASKINFO_OUTFILE, m_taskinfooutfile);
}


BEGIN_MESSAGE_MAP(TaskinfoDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &TaskinfoDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &TaskinfoDlg::OnBnClickedCancel)
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_TASKINFO_INFILE_FDLG, &TaskinfoDlg::OnBnClickedTaskinfoInfileFdlg)
    ON_BN_CLICKED(IDC_TASKINFO_OUTFILE_FDLG, &TaskinfoDlg::OnBnClickedTaskinfoOutfileFdlg)
END_MESSAGE_MAP()


//对话框初始化
//Init Dialog
BOOL TaskinfoDlg::OnInitDialog()
{

    CDialogEx::OnInitDialog();
    //加载StringTable
    CString resloader,resloader_default;
    resloader_default.LoadString(IDS_TASKINFO_DEFAULT);


    m_taskinfoinfile.SetWindowText(task.infile);
    m_taskinfooutfile.SetWindowText(task.outfile);


    //CMFCPropertyGridProperty
    m_taskinfoparam.EnableDescriptionArea(FALSE);
    m_taskinfoparam.SetVSDotNetLook();
    m_taskinfoparam.MarkModifiedProperties();
    m_taskinfoparam.EnableHeaderCtrl(FALSE);
    //把第一列调整宽一些-----------------------
    HDITEM item;
    item.cxy=120;
    item.mask=HDI_WIDTH;
    m_taskinfoparam.GetHeaderCtrl().SetItem(0, &item);

    resloader.LoadString(IDS_TASKINFO_FORMAT);
    format=new CMFCPropertyGridProperty(resloader);
    resloader.LoadString(IDS_TASKINFO_VIDEO);
    video=new CMFCPropertyGridProperty(resloader);
    resloader.LoadString(IDS_TASKINFO_AUDIO);
    audio=new CMFCPropertyGridProperty(resloader);

    resloader.LoadString(IDS_TASKINFO_FORMAT_IF);




    format_if=new CMFCPropertyGridProperty(resloader,(_variant_t) task.ifname, _T(""));
    resloader.LoadString(IDS_TASKINFO_FORMAT_OF);
    format_of=new CMFCPropertyGridProperty(resloader, (_variant_t)task.ofname,  _T(""));
    resloader.LoadString(IDS_TASKINFO_VIDEO_BITRATE);
    video_bitrate=new CMFCPropertyGridProperty(resloader,(_variant_t) task.vbitrate, _T(""));
    resloader.LoadString(IDS_TASKINFO_VIDEO_CODEC);
    video_codec=new CMFCPropertyGridProperty(resloader,(_variant_t) task.vcname, _T(""));
    resloader.LoadString(IDS_TASKINFO_VIDEO_FRAMERATE);
    video_framerate=new CMFCPropertyGridProperty(resloader,(_variant_t) task.framerate, _T(""));
    resloader.LoadString(IDS_TASKINFO_AUDIO_BITRATE);
    audio_bitrate=new CMFCPropertyGridProperty(resloader,(_variant_t) task.abitrate, _T(""));
    resloader.LoadString(IDS_TASKINFO_AUDIO_CODEC);
    audio_codec=new CMFCPropertyGridProperty(resloader,(_variant_t) task.acname, _T(""));
    resloader.LoadString(IDS_TASKINFO_AUDIO_SAMPLERATE);
    audio_samplerate=new CMFCPropertyGridProperty(resloader,(_variant_t) task.samplefmt, _T(""));
    resloader.LoadString(IDS_TASKINFO_AUDIO_CHANNELS);
    audio_channels=new CMFCPropertyGridProperty(resloader,(_variant_t)task.channels, _T(""));


    GetNamelist();


    format_if->AddOption(resloader_default);
    for(int i=0; i<iflist.size(); i++)
        format_if->AddOption(iflist[i]);


    format_of->AddOption(resloader_default);
    for(int i=0; i<oflist.size(); i++)
        format_of->AddOption(oflist[i]);



    video_codec->AddOption(resloader_default);
    video_codec->AddOption(_T("copy"));
    for(int i=0; i<vclist.size(); i++)
        video_codec->AddOption(vclist[i]);


    audio_codec->AddOption(resloader_default);
    audio_codec->AddOption(_T("copy"));
    for(int i=0; i<aclist.size(); i++)
        audio_codec->AddOption(aclist[i]);


    UpdateData(FALSE);

    format->AddSubItem(format_if);
    format->AddSubItem(format_of);
    video->AddSubItem(video_bitrate);
    video->AddSubItem(video_codec);
    video->AddSubItem(video_framerate);
    audio->AddSubItem(audio_bitrate);
    audio->AddSubItem(audio_codec);
    audio->AddSubItem(audio_samplerate);
    audio->AddSubItem(audio_channels);

    m_taskinfoparam.AddProperty(format);
    m_taskinfoparam.AddProperty(video);
    m_taskinfoparam.AddProperty(audio);


    //标题
    if(state==TASK_DLG_EDIT)
    {
        resloader.LoadString(IDS_TASKINFO_EDIT);
        CheckInputMedia(task.infile);
    }
    else if(state==TASK_DLG_ADD)
    {
        resloader.LoadString(IDS_TASKINFO_ADD);
    }
    SetWindowText(resloader);


    video_stream=-1;
    audio_stream=-1;
    //------------------
    return true;
}


//获取支持的封装格式等信息列表
//Get support container format, video codec, audio codec etc.
void TaskinfoDlg::GetNamelist()
{

    av_register_all();

    AVCodec *first_c=NULL;
    AVInputFormat *first_if=NULL;
    AVOutputFormat *first_of=NULL;

    first_if= av_iformat_next(NULL);
    first_of= av_oformat_next(NULL);
    first_c=av_codec_next(NULL);

    AVInputFormat *if_temp=first_if;
    AVOutputFormat *of_temp=first_of;
    AVCodec *c_temp=first_c;

#ifdef _UNICODE
    USES_CONVERSION;
#endif
    //InputFormat
    while(if_temp!=NULL)
    {
        CString name;
#ifdef _UNICODE
        name.Format(_T("%s"),A2W(if_temp->name));
#else
        name.Format(_T("%s"),if_temp->name);
#endif
        iflist.push_back(name);
        if_temp=if_temp->next;
    }

    //OutputFormat
    while(of_temp!=NULL)
    {
        CString name;
#ifdef _UNICODE
        name.Format(_T("%s"),A2W(of_temp->name));
#else
        name.Format(_T("%s"),of_temp->name);
#endif
        oflist.push_back(name);
        of_temp=of_temp->next;
    }

    //Codec
    int c_index=0;
    while(c_temp!=NULL)
    {
        CString name;
        int nIndex=0;

#ifdef _UNICODE
        name.Format(_T("%s"),A2W(c_temp->name));
#else
        name.Format(_T("%s"),c_temp->name);
#endif

        switch(c_temp->type)
        {
        case AVMEDIA_TYPE_VIDEO:
        {
            vclist.push_back(name);
            break;
        }
        case AVMEDIA_TYPE_AUDIO:
        {
            aclist.push_back(name);
            break;
        }
        default:
        {
            break;
        }
        }
        c_temp=c_temp->next;
        c_index++;
    }


}
//设置Task
//Set Task
void TaskinfoDlg::SetTask()
{
    m_taskinfoinfile.GetWindowText(task.infile);
    m_taskinfooutfile.GetWindowText(task.outfile);
    //检查文件大小
    CFile mFile(task.infile,CFile::modeRead);
    task.insize=mFile.GetLength();
    mFile.Close();

    //task.insize;
    task.ifname=format_if->GetValue().bstrVal;
    task.ofname=format_of->GetValue().bstrVal;
    task.vcname=video_codec->GetValue().bstrVal;
    task.vbitrate=video_bitrate->GetValue().intVal;
    task.framerate=video_framerate->GetValue().dblVal;
    //task.reso_w;
    //task.reso_h;
    task.acname=audio_codec->GetValue().bstrVal;
    task.abitrate=audio_bitrate->GetValue().intVal;
    task.samplefmt=audio_samplerate->GetValue().intVal;
    task.channels=audio_channels->GetValue().intVal;

    task.video_stream=video_stream;
    task.audio_stream=audio_stream;
}
//加载Task
//Load Task
void TaskinfoDlg::LoadTask(TaskDlgState state_temp,int tasklistid_temp)
{

    CString resloader_default;

    resloader_default.LoadString(IDS_TASKINFO_DEFAULT);

    if(state_temp==TASK_DLG_EDIT)
    {
        state=TASK_DLG_EDIT;
        task=maindlg->tasklist[tasklistid_temp];
        tasklistid=tasklistid_temp;
    }
    else if(state_temp==TASK_DLG_ADD)
    {
        Task task_temp= {_T(""),0,_T(""),resloader_default,resloader_default,
                         resloader_default,800,25,1280,720,resloader_default,64,44100,2,TASK_PREPARE,-1,-1
                        };
        task=task_temp;
        tasklistid=-1;
        state=TASK_DLG_ADD;
    }

    audio_stream=task.audio_stream;
    video_stream=task.video_stream;
}
//确定按钮
//"OK" Button
void TaskinfoDlg::OnBnClickedOk()
{

    //再次检查
    CString inputurl;
    m_taskinfoinfile.GetWindowText(inputurl);
    if(CheckInputMedia(inputurl)!=TRUE)
    {
        AfxMessageBox(_T("Couldn't open input stream."));
        return;
    }
    SetTask();

    if(state==TASK_DLG_ADD)
    {
        maindlg->TasklistAdd(task);
    }
    else if(state==TASK_DLG_EDIT)
    {
        maindlg->TasklistEdit(task,tasklistid);
    }
    CDialogEx::OnOK();
    DestroyWindow();
    delete this;
}
//取消按钮
//"Cancel" Button
void TaskinfoDlg::OnBnClickedCancel()
{
    CDialogEx::OnCancel();
    DestroyWindow();
    delete this;
}
//拖拽文件
//Drop file
void TaskinfoDlg::OnDropFiles(HDROP hDropInfo)
{
    CDialogEx::OnDropFiles(hDropInfo);
    LPTSTR pFilePathName =(LPTSTR)malloc(MAX_PATH);
    ::DragQueryFile(hDropInfo, 0, pFilePathName,MAX_PATH);  // 获取拖放文件的完整文件名，最关键！

    SetInputandOutput(pFilePathName);

    ::DragFinish(hDropInfo);   // 注意这个不能少，它用于释放Windows 为处理文件拖放而分配的内存
    free(pFilePathName);


}
//检查输入文件信息
//Check input file information
BOOL TaskinfoDlg::CheckInputMedia(CString url)
{
    CString input_protocol,input_format,wxh,decoder_name,
            decoder_type,bitrate,extention,framerate,timelong,decoder_name_au,sample_rate_au,channels_au;
    float framerate_temp,timelong_temp,bitrate_temp;

    AVFormatContext	*pFormatCtx;
    int				i;
    AVCodecContext	*pCodecCtx,*pCodecCtx_au;
    AVCodec			*pCodec,*pCodec_au;

    //清空
    m_codecachannels.SetWindowText(_T(""));
    m_codecaname.SetWindowText(_T(""));
    m_codecasamplerate.SetWindowText(_T(""));
    m_codecvframerate.SetWindowText(_T(""));
    m_codecvname.SetWindowText(_T(""));
    m_codecvresolution.SetWindowText(_T(""));
    m_formatbitrate.SetWindowText(_T(""));
    m_formatduration.SetWindowText(_T(""));
    m_formatinputformat.SetWindowText(_T(""));





    av_register_all();
    //avformat_network_init();

    pFormatCtx = avformat_alloc_context();

#ifdef _UNICODE
    USES_CONVERSION;
#endif

#ifdef _UNICODE
    if(avformat_open_input(&pFormatCtx,W2A(url.GetBuffer()),NULL,NULL)!=0)
    {
        AfxMessageBox(_T("Couldn't open input stream."));
        return FALSE;
    }
#else
    if(avformat_open_input(&pFormatCtx,url,NULL,NULL)!=0)
    {
        AfxMessageBox(_T("Couldn't open input stream."));
        return FALSE;
    }
#endif


    if(av_find_stream_info(pFormatCtx)<0)
    {
        AfxMessageBox(_T("Couldn't find stream information."));
        return FALSE;
    }

    for(i=0; i<pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            video_stream=i;
        }
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO)
        {
            audio_stream=i;
        }
    }


    if(audio_stream==-1&&video_stream==-1)
    {
        AfxMessageBox(_T("Didn't find a video/audio stream."));
        return FALSE;
    }


    //视频解码参数，有视频的时候设置
    if(video_stream!=-1)
    {
        pCodecCtx=pFormatCtx->streams[video_stream]->codec;
        pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
        if(pCodec==NULL)
        {
            AfxMessageBox(_T("Video codec not found."));
            return FALSE;
        }
        if(avcodec_open(pCodecCtx, pCodec)<0)
        {
            AfxMessageBox(_T("Could not open video codec."));
            return FALSE;
        }

        wxh.Format(_T("%d x %d"),pCodecCtx->width,pCodecCtx->height);
        m_codecvresolution.SetWindowText(wxh);
#ifdef _UNICODE
        decoder_name.Format(_T("%s"),A2W(pCodecCtx->codec->long_name));
#else
        decoder_name.Format(_T("%s"),pCodecCtx->codec->long_name);
#endif
        m_codecvname.SetWindowText(decoder_name);
        //帧率显示还有问题
        framerate_temp=(pFormatCtx->streams[video_stream]->r_frame_rate.num)/(pFormatCtx->streams[video_stream]->r_frame_rate.den);
        framerate.Format(_T("%5.2ffps"),framerate_temp);
        m_codecvframerate.SetWindowText(framerate);
        //FIX:帧率不正常，有可能是音频里附带的图片，不对视频进行转码
        if(framerate_temp>=500)
        {
            video_stream=-1;
        }
    }
    //音频解码参数，有音频的时候设置
    if(audio_stream!=-1)
    {

        pCodecCtx_au=pFormatCtx->streams[audio_stream]->codec;
        pCodec_au=avcodec_find_decoder(pCodecCtx_au->codec_id);
        if(pCodec_au==NULL)
        {
            AfxMessageBox(_T("Audio codec not found."));
            return FALSE;
        }
        if(avcodec_open(pCodecCtx_au, pCodec_au)<0)
        {
            AfxMessageBox(_T("Could not open audio codec."));
            return FALSE;
        }

#ifdef _UNICODE
        decoder_name_au.Format(_T("%s"),A2W(pCodecCtx_au->codec->long_name));
#else
        decoder_name_au.Format(_T("%s"),pCodecCtx_au->codec->long_name);
#endif
        m_codecaname.SetWindowText(decoder_name_au);
        sample_rate_au.Format(_T("%d"),pCodecCtx_au->sample_rate);
        m_codecasamplerate.SetWindowText(sample_rate_au);
        channels_au.Format(_T("%d"),pCodecCtx_au->channels);
        m_codecachannels.SetWindowText(channels_au);
    }
    //显示成以k为单位
    bitrate_temp=((float)(pFormatCtx->bit_rate))/1000;
    bitrate.Format(_T("%5.2fkbps"),bitrate_temp);
    m_formatbitrate.SetWindowText(bitrate);
    //duration是以微秒为单位
    timelong_temp=(pFormatCtx->duration)/1000000;
    //转换成hh:mm:ss形式
    int tns, thh, tmm, tss;
    tns  = (pFormatCtx->duration)/1000000;
    thh  = tns / 3600;
    tmm  = (tns % 3600) / 60;
    tss  = (tns % 60);
    timelong.Format(_T("%02d:%02d:%02d"),thh,tmm,tss);
    m_formatduration.SetWindowText(timelong);
    //输入文件的封装格式------
#ifdef _UNICODE
    input_format.Format(_T("%s"),A2W(pFormatCtx->iformat->long_name));
#else
    input_format.Format(_T("%s"),pFormatCtx->iformat->long_name);
#endif
    m_formatinputformat.SetWindowText(input_format);
    //------------------------
    if(video_stream!=-1)
    {
        avcodec_close(pCodecCtx);
    }

    if(audio_stream!=-1)
    {
        avcodec_close(pCodecCtx_au);
    }
    avformat_close_input(&pFormatCtx);

    if(video_stream==-1)
    {
        video->Expand(FALSE);
        video_bitrate->Enable(FALSE);
        video_codec->Enable(FALSE);
        video_framerate->Enable(FALSE);
    }
    else
    {
        video->Expand(TRUE);
        video_bitrate->Enable(TRUE);
        video_codec->Enable(TRUE);
        video_framerate->Enable(TRUE);
    }
    if(audio_stream==-1)
    {
        audio->Expand(FALSE);
        audio_bitrate->Enable(FALSE);
        audio_codec->Enable(FALSE);
        audio_samplerate->Enable(FALSE);
        audio_channels->Enable(FALSE);

    }
    else
    {
        audio->Expand(TRUE);
        audio_bitrate->Enable(TRUE);
        audio_codec->Enable(TRUE);
        audio_samplerate->Enable(TRUE);
        audio_channels->Enable(TRUE);
    }

    return TRUE;
}
//“打开”文件对话框
//"Open" Dialog
void TaskinfoDlg::OnBnClickedTaskinfoInfileFdlg()
{
    CString FilePathName;
    //文件过滤字符串
    CString strfilter;
    strfilter.Append(_T("Common media formats|*.avi;*.wmv;*.wmp;*.wm;*.asf;*.rm;*.ram;*.rmvb;*.ra;*.mpg;*.mpeg;*.mpe;*.m1v;*.m2v;*.mpv2;"));
    strfilter.Append(_T("*.mp2v;*.dat;*.mp4;*.m4v;*.m4p;*.vob;*.ac3;*.dts;*.mov;*.qt;*.mr;*.3gp;*.3gpp;*.3g2;*.3gp2;*.swf;*.ogg;*.wma;*.wav;"));
    strfilter.Append(_T("*.mid;*.midi;*.mpa;*.mp2;*.mp3;*.m1a;*.m2a;*.m4a;*.aac;*.mkv;*.ogm;*.m4b;*.tp;*.ts;*.tpr;*.pva;*.pss;*.wv;*.m2ts;*.evo;"));
    strfilter.Append(_T("*.rpm;*.realpix;*.rt;*.smi;*.smil;*.scm;*.aif;*.aiff;*.aifc;*.amr;*.amv;*.au;*.acc;*.dsa;*.dsm;*.dsv;*.dss;*.pmp;*.smk;*.flic|"));
    strfilter.Append(_T("Windows Media Video(*.avi;*wmv;*wmp;*wm;*asf)|*.avi;*.wmv;*.wmp;*.wm;*.asf|"));
    strfilter.Append(_T("Windows Media Audio(*.wma;*wav;*aif;*aifc;*aiff;*mid;*midi;*rmi)|*.wma;*.wav;*.aif;*.aifc;*.aiff;*.mid;*.midi;*.rmi|"));
    strfilter.Append(_T("Real(*.rm;*ram;*rmvb;*rpm;*ra;*rt;*rp;*smi;*smil;*.scm)|*.rm;*.ram;*.rmvb;*.rpm;*.ra;*.rt;*.rp;*.smi;*.smil;*.scm|"));
    strfilter.Append(_T("MPEG Video(*.mpg;*mpeg;*mpe;*m1v;*m2v;*mpv2;*mp2v;*dat;*mp4;*m4v;*m4p;*m4b;*ts;*tp;*tpr;*pva;*pss;*.wv;)|"));
    strfilter.Append(_T("*.mpg;*.mpeg;*.mpe;*.m1v;*.m2v;*.mpv2;*.mp2v;*.dat;*.mp4;*.m4v;*.m4p;*.m4b;*.ts;*.tp;*.tpr;*.pva;*.pss;*.wv;|"));
    strfilter.Append(_T("MPEG Audio(*.mpa;*mp2;*m1a;*m2a;*m4a;*aac;*.m2ts;*.evo)|*.mpa;*.mp2;*.m1a;*.m2a;*.m4a;*.aac;*.m2ts;*.evo|"));
    strfilter.Append(_T("DVD(*.vob;*ifo;*ac3;*dts)|*.vob;*.ifo;*.ac3;*.dts|MP3(*.mp3)|*.mp3|CD Tracks(*.cda)|*.cda|"));
    strfilter.Append(_T("Quicktime(*.mov;*qt;*mr;*3gp;*3gpp;*3g2;*3gp2)|*.mov;*.qt;*.mr;*.3gp;*.3gpp;*.3g2;*.3gp2|"));
    strfilter.Append(_T("Flash Files(*.flv;*swf;*.f4v)|*.flv;*.swf;*.f4v|Playlist(*.smpl;*.asx;*m3u;*pls;*wvx;*wax;*wmx;*mpcpl)|*.smpl;*.asx;*.m3u;*.pls;*.wvx;*.wax;*.wmx;*.mpcpl|"));
    strfilter.Append(_T("Others(*.ivf;*au;*snd;*ogm;*ogg;*fli;*flc;*flic;*d2v;*mkv;*pmp;*mka;*smk;*bik;*ratdvd;*roq;*drc;*dsm;*dsv;*dsa;*dss;*mpc;*divx;*vp6;*.ape;*.flac;*.tta;*.csf)"));
    strfilter.Append(_T("|*.ivf;*.au;*.snd;*.ogm;*.ogg;*.fli;*.flc;*.flic;*.d2v;*.mkv;*.pmp;*.mka;*.smk;*.bik;*.ratdvd;*.roq;*.drc;*.dsm;*.dsv;*.dsa;*.dss;*.mpc;*.divx;*.vp6;*.ape;*.amr;*.flac;*.tta;*.csf|"));
    strfilter.Append(_T("All Files(*.*)|*.*||"));

    LPCTSTR lpszfilter=strfilter;
    CFileDialog dlg(TRUE,NULL,NULL,NULL,lpszfilter);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
    if(dlg.DoModal()==IDOK)
    {
        FilePathName=dlg.GetPathName();
        SetInputandOutput(FilePathName);
    }
}
//“保存”文件对话框
//"Save" Dialog
void TaskinfoDlg::OnBnClickedTaskinfoOutfileFdlg()
{
    CString FilePathName;

    CFileDialog dlg(FALSE,NULL,NULL,NULL,NULL);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
    if(dlg.DoModal()==IDOK)
    {
        FilePathName=dlg.GetPathName();
        m_taskinfooutfile.SetWindowText(FilePathName);
    }
}
//设置输入文件路径，并设置预设的输出文件路径
//Set input and output URL
void TaskinfoDlg::SetInputandOutput(CString inputurl)
{
    m_taskinfoinfile.SetWindowText(inputurl);

    if(CheckInputMedia(inputurl)==TRUE)
    {
        //设置输出路径
        //文件大小
        char *infilePath=(char *)alloca(500);
        char *outfilePath=(char *)alloca(500);

#ifdef _UNICODE
        USES_CONVERSION;
        wchar_t infilePathW[500]= {0};
        m_taskinfoinfile.GetWindowText(infilePathW,500);
        infilePath=W2A(infilePathW);
#else
        m_taskinfoinfile.GetWindowText(infilePath,500);
#endif

        //输出文件名
        char *suffix=(char *)strrchr(infilePath, '.');//
        *suffix='\0';
        suffix++;
        strcpy(outfilePath,infilePath);
        strcat(outfilePath,"_ffmfc.mkv");

#ifdef _UNICODE
        m_taskinfooutfile.SetWindowText(A2W(outfilePath));
#else
        m_taskinfooutfile.SetWindowText(outfilePath);
#endif

    }
}







