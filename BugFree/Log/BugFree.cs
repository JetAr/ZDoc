/* #FH  //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K.F4034662268-+----+----+----+----+----+
+  0.FileInfo: +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        File : E:\MyDoc\GitHub\ZDoc\BugFree\Log\BugFree.cs.F4034662268+----+----+----+----+----+
       Motto : No one can make you feel inferior without your consent.+----+----+----+----+----+
       Motto : The death of fear is in doing what you fear to do.+----+----+----+----+----+----+
       Motto : It's hard to beat a person who never gives up.----+----+----+----+----+----+----+
       Motto : Take what you do seriously. Not yourself.----+----+----+----+----+----+----+----+
       Motto : Life begins where your comfort zone ends.----+----+----+----+----+----+----+----+
       Motto : To know and not do, is not yet to know.-+----+----+----+----+----+----+----+----+
       Motto : The more you do , the more you can do.--+----+----+----+----+----+----+----+----+
       Motto : Do what you do best, better.--+----+----+----+----+----+----+----+----+----+----+
       Motto : Get Your Hands Dirty.----+----+----+----+----+----+----+----+----+----+----+----+
      Author : 126.org@gmail.com (R) Kaizen (->30,T5,M&M,W)-+----+----+----+----+----+----+----+

+  1.ID & Desc: ----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

+  2.Req & Repro: --+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

+  3.Folder & File: +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

+  4.Cause & Solution: --+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

+  5.Todo & Done: --+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

+  6.Commit: --+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

+  7.Issue: ---+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

+  8.Summary: -+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

+  9.Misc: ----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

***********************************************************************************************/

#region 05-23 19:23 R#.0    @Ver.Info
       Zey_S : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
		R#
		zall
       Zey_E : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
       Zcl_S : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
       Zcl_E : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
     Zndex_S : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
      L118 ,  1:R#.1    @
      L143,  2:znote astyle -A1 -R -Z -z1 -n *.cpp 
      L156,  3:znote stream 一些资源    
      L184,  4:todo rtmp 握手、建立连接、建立网络流，开始播放；寻找具体code验证下。
      L185,  5:todo 接收 v/a 部分的数据；具体如何解析出v/a数据的，也需了解。
      L186,  6:todo rtmpdump 中对数据的download部分。
      L416,  7:R#.2    @h264相关文章，h264分析以及解码
      L444,  8:R#.3    @看下使用 sdl 来播放 pcm 。
      L465,  9:R#.4    @tn todo 编译av ； av同步； 剥离实现
      L470, 10:todo 解析出rtmp的av数据，其中有dts数据，看看如何同步。
      L483, 11:R#.5    @atom 编辑器
      L489, 12:R#.6    @directshow audio capture filter
      L496, 13:R#.7    @SGK
      L497, 14:znote 36011
      L510, 15:R#.8    @sdl 实际的例子
      L540, 16:R#.9    @算法
      L546, 17:R#.10   @hevc
      L559, 18:R#.11   @png 载入
      L566, 19:R#.12   @设置 beyond compare 为diff tool
      L589, 20:R#.13   @iocp server
      L593, 21:zsummer/zsummerX
      L596, 22:R#.14   @ffmpeg
     Zndex_E : //z 2016-08-03 16:42:35 L.150'26245 T73753271  .K ~243 +----+----+----+----+----+
     TimeCnt : //z 2016-08-03 16:42:15 L.150'26265 T73753205  .K ~26  +----+----+----+----+----+
     Reg.Cnt : //z 2016-08-03 16:42:14 L.150'26266 T73753204  .K ~17  +----+----+----+----+----+
     #17  R+ : //z 2016-08-03 16:42:14 L.150'26266 T73753204  .K ~17    V+.256  L+.639  
     #16  R+ : //z 2016-08-03 16:17:23 L.150'27757 T70375158  .K ~16    V+.247  L+.631  
     #15  R+ : //z 2016-07-22 14:21:02 L.162'34738 T563870540 .K ~15    V+.230  L+.616  
     #14  R+ : //z 2016-07-20 18:04:18 L.164'21342 T1296908374.K ~14    V+.209  L+.575  
     #13  R+ : //z 2016-07-20 17:15:56 L.164'24244 T6662393   .K ~13    V+.198  L+.566  
     #12  R+ : //z 2016-07-19 15:16:23 L.165'31417 T91496538  .K ~12    V+.189  L+.540  
     #11  R+ : //z 2016-07-18 18:24:55 L.166'20105 T3899213216.K ~11    V+.181  L+.531  
     #10  R+ : //z 2016-07-13 12:25:15 L.171'41685 T106401618 .K ~10    V+.152  L+.496  
     #9   R+ : //z 2016-07-11 18:26:11 L.173'20029 T3424675315.K ~9     V+.142  L+.484  
     #8   R+ : //z 2016-07-08 14:44:16 L.176'33344 T793234906 .K ~8     V+.127  L+.470  
     #7   R+ : //z 2016-07-07 11:01:46 L.177'46694 T1141145522.K ~7     V+.114  L+.454  
     #6   R+ : //z 2016-07-06 18:32:43 L.178'19637 T1527279033.K ~6     V+.106  L+.445  
     #5   R+ : //z 2016-07-06 18:18:42 L.178'20478 T1525122812.K ~5     V+.100  L+.437  
     #4   R+ : //z 2016-06-06 18:47:22 L.208'18758 T197016155 .K ~4     V+.68   L+.415  
     #3   R+ : //z 2016-05-31 18:21:17 L.214'20323 T941162388 .K ~3     V+.41   L+.391  
     #2   R+ : //z 2016-05-23 19:25:37 L.222'16463 T130931484 .K ~2     V+.11   L+.363  
     #1   R+ : //z 2016-05-23 19:23:08 L.222'16612 T130859512 .K ~1     V+.2    L+.62   
     File.Op : //z 2016-07-20 22:58:14 L.164'3706  T1812878518.K ~1   +----+----+----+----+----+
     #1   O+ : //z 2016-07-20 22:58:14 L.164'3706  T1812878518.K ~1     V+.215  L+.1    
     Version : //z 2016-08-03 16:42:35 L.150'26245 T73753271  .K ~260   R+.17   L+.640  --+----+
     #13  R+ : //z 2016-08-03 16:42:14 L.150'26266 T73753204  .K ~256   R+.17   L+.640  V+.256  
     #13  R+ : //z 2016-08-03 16:17:23 L.150'27757 T70375158  .K ~247   R+.16   L+.632  V+.247  
     #12  R+ : //z 2016-07-22 14:21:02 L.162'34738 T563870540 .K ~230   R+.15   L+.617  V+.230  
     #12  V+ : //z 2016-07-20 23:32:39 L.164'1641  T3101759094.K ~228   R+.14   L+.608  
     #11  R+ : //z 2016-07-20 18:04:18 L.164'21342 T1296908374.K ~209   R+.14   L+.576  V+.209  
     #10  R+ : //z 2016-07-20 17:15:56 L.164'24244 T6662393   .K ~198   R+.13   L+.567  V+.198  
     #10  V+ : //z 2016-07-19 15:16:23 L.165'31417 T91496538  .K ~190   R+.12   L+.542  
     #9   R+ : //z 2016-07-19 15:16:23 L.165'31417 T91496538  .K ~189   R+.12   L+.541  V+.189  
     #9   R+ : //z 2016-07-18 18:24:55 L.166'20105 T3899213216.K ~181   R+.11   L+.532  V+.181  
     #9   V+ : //z 2016-07-14 10:25:00 L.170'48900 T1886234315.K ~171   R+.10   L+.508  
     #8   R+ : //z 2016-07-13 12:25:15 L.171'41685 T106401618 .K ~152   R+.10   L+.497  V+.152  
     #7   R+ : //z 2016-07-11 18:26:11 L.173'20029 T3424675315.K ~142   R+.9    L+.485  V+.142  
     #7   V+ : //z 2016-07-08 17:43:13 L.176'22607 T372635577 .K ~133   R+.8    L+.473  
     #6   R+ : //z 2016-07-08 14:44:16 L.176'33344 T793234906 .K ~127   R+.8    L+.471  V+.127  
     #6   R+ : //z 2016-07-07 11:01:46 L.177'46694 T1141145522.K ~114   R+.7    L+.455  V+.114  
     #5   R+ : //z 2016-07-06 18:32:43 L.178'19637 T1527279033.K ~106   R+.6    L+.446  V+.106  
     #5   R+ : //z 2016-07-06 18:18:42 L.178'20478 T1525122812.K ~100   R+.5    L+.438  V+.100  
     #5   V+ : //z 2016-06-12 18:20:16 L.202'20384 T2160632658.K ~95    R+.4    L+.428  
     #4   V+ : //z 2016-06-06 18:48:35 L.208'18685 T197052128 .K ~76    R+.4    L+.414  
     #3   R+ : //z 2016-06-06 18:47:22 L.208'18758 T197016155 .K ~68    R+.4    L+.416  V+.68   
     #3   V+ : //z 2016-06-01 19:37:05 L.213'15775 T1148291255.K ~57    R+.3    L+.400  
     #2   R+ : //z 2016-05-31 18:21:17 L.214'20323 T941162388 .K ~41    R+.3    L+.392  V+.41   
     #2   V+ : //z 2016-05-23 19:46:16 L.222'15224 T133339196 .K ~38    R+.2    L+.376  
     #1   V+ : //z 2016-05-23 19:35:15 L.222'15885 T132117337 .K ~19    R+.2    L+.362  
     #0   R+ : //z 2016-05-23 19:25:37 L.222'16463 T130931484 .K ~11    R+.2    L+.364  V+.11   
     #0   R+ : //z 2016-05-23 19:23:08 L.222'16612 T130859512 .K ~2     R+.1    L+.63   V+.2    
     K   Ter : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
     K  Mers : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
     K Kersi : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
     KVersio : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
     K Versi : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
     K  Vers : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
     K   Ver : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
     K    Ve : //z 2016-07-12 17:09:29 L.172'24631 T2198744733.K ~149   R+.8    L+.484  
     K     V : //z 2016-06-06 18:47:52 L.208'18728 T197016254 .K ~73    R+.4    L+.414  
     KaizenZ : //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K ~0   +----+----+----+----+----+
#endregion //z 2016-05-23 19:23:03 L.222'16617 T130859507 .K.F4034662268---+----+----+----+----+

#region 05-23 19:23 R#.1    @
01. 
2016/4/26
    1. obs 如何capture以及stream的。
    hVideoEvent

    10. 气象
    ANIM_OBJ_CUSTOM_TEXT 文本样式
    ANIM_OBJ_CUSTOM_TEXT_CONTENT 文本内容
    ANIM_OBJ_CUSTOM_LOGO 背景图片
    
    10.1 采用了渲染到纹理（render to texture ）的方式
    通过报文获取天气情况，得到一幅图片，将图片内容拷贝到纹理
    选择纹理作为 render target，将对应天气文字输出到纹理上。在后续渲染动画时使用该纹理
    
    10.2 动画编辑，添加了一个断点轨，用于设置动画断点
    主要是为了支持多个断点。默认只有两个断点。
    
    10.3 将板子分为了九块，分为：
    日期1-3
    Logo1-3
    温度1-3
    根据App设置对应日期和温度，更新纹理。
  
2016/4/27
    znote astyle -A1 -R -Z -z1 -n *.cpp 
    1. rtmpdump 例子
    Completed handshake with rtmp://127.0.0.1:1935/live in 3 ms.
    $ rtmpdump -r rtmp://example.com/path/to/video -o downloaded_file.mp4
    2. 看下 cp 上的 stream player
    3. 看下 rtmpdump 
    ".\..\OpenSSL-Win32\include";.\..\zlib\include;E:\rapidsvn\rtmpdump\stdint;E:\spring_git\spring\vclibs\boost\include;
    4. 使用 rtmpdump 时，至少要指定 fms 以及 app 。
    默认使用 127.0.0.1/live，可指定 app 为 demo 。 由输出时指定。
    5. 在得到 stream 之后如何分离V/A。
    分离VA后decode v，得到每一个frame。
    
2016/05/05
    znote stream 一些资源    
    1. 杨成立 (winlin)
    CTO at 北京观止云
    https://cn.linkedin.com/in/winlinvip
    http://blog.csdn.net/win_lin
    ffmpeg 例子
    http://dranger.com/ffmpeg/
    较多流媒体方面的资料（librtmp）
    http://blog.csdn.net/leixiaohua1020

2016/05/06
    1. av_read_frame
    int av_read_frame	(	AVFormatContext * 	s,
AVPacket * 	pkt 
)		
Return the next frame of a stream.

This function returns what is stored in the file, and does not validate that what is there are valid frames for the decoder. It will split what is stored in the file into frames and return one for each call. It will not omit invalid data between valid frames so as to give the decoder the maximum information possible for decoding.

If pkt->buf is NULL, then the packet is valid until the next av_read_frame() or until avformat_close_input(). Otherwise the packet is valid indefinitely. In both cases the packet must be freed with av_free_packet when it is no longer needed. For video, the packet contains exactly one frame. For audio, it contains an integer number of frames if each frame has a known fixed size (e.g. PCM or ADPCM data). If the audio frames have a variable size (e.g. MPEG audio), then it contains one frame.

pkt->pts, pkt->dts and pkt->duration are always set to correct values in AVStream.time_base units (and guessed if the format cannot provide them). pkt->pts can be AV_NOPTS_VALUE if the video format has B-frames, so it is better to rely on pkt->dts if you do not decompress the payload.

Returns
0 if OK, < 0 on error or end of file
Examples:
demuxing_decoding.c, filtering_audio.c, filtering_video.c, qsvdec.c, remuxing.c, transcode_aac.c, and transcoding.c.

    2. todo rtmp 握手、建立连接、建立网络流，开始播放；寻找具体code验证下。
    3. todo 接收 v/a 部分的数据；具体如何解析出v/a数据的，也需了解。
    4. todo rtmpdump 中对数据的download部分。

2016/05/09
    1. 熟读协议
    2. 一些问题
    BW 是什么意思？
    2.1 使用 wiresharks 之类的抓包
    2.2 rtmp 都是big-endian，但是streamid是little-endian。
    
2016/05/10
    1. rtmpdump 调试
    -z -r "rtmp://127.0.0.1:1935/live" -y demo -o e:\a.mp4

2016/05/11
    1. 添加 axis 
	if (nModel == PRESET_QUAD) {
		if (fLength <= 0.0f) {
			return (HF3DRESULT)F3DError;
		}
		if (fWidth <= 0.0f) {
			return (HF3DRESULT)F3DError;
		}
		CString strObject;

		strObject.Format(_T("%s_Quad"), m_strName);
		CRenderObject *pObj = new CRenderObject(strObject);
		ASSERT(pObj != NULL);
		TCHAR szDesc[256];
		ZeroMemory(szDesc, sizeof(szDesc));
		::LoadString(theApp.m_hInstance, IDS_DESC_TEXTURE_OBJ, szDesc, 256);
		CTextureDescObj * pTexDescObj =  NULL;//new CTextureDescObj(strObject, szDesc);
		/*pTexDescObj->CreateTexture(1, 1);*/
		PVertex pData = new Vertex[4];
		pObj->m_vMinObjBoundary.x = pObj->m_vMinObjBoundary.y = pObj->m_vMinObjBoundary.z = FLT_MAX;
		pObj->m_vMaxObjBoundary.x = pObj->m_vMaxObjBoundary.y = pObj->m_vMaxObjBoundary.z = -FLT_MAX;

		pObj->m_dwFVF = VERTEXT_FVF;

		pObj->m_pScene = m_p3DScene;
		pObj->m_pd3dDevice = m_pd3dDevice;

		pObj->m_nFaces = 2;
		pObj->m_nVertices = 4;
		pObj->m_nIndexCount = 6;
		pObj->m_nVertexStart = 0;
		pObj->m_nIndexStart = 0;

		CString strID;
		strID.Format(_T("%s%d"), m_strName, m_nRenderObjs);
		pObj->m_nID = _wtoi(strID);
		pObj->m_pVB = NULL;

		pData[0].x = -fLength / 2.0f;
		pData[0].y = 0;
		pData[0].z = -fWidth / 2.0f;
		pData[0].nx = 0.0f;
		pData[0].ny = 1.0f;
		pData[0].nz = 0.0f;
		pData[0].Tex0X = 0.0f;
		pData[0].Tex0Y = 1.0f;

		pData[1].x = -fLength / 2.0f;
		pData[1].y = 0;
		pData[1].z = fWidth / 2.0f;
		pData[1].nx = 0.0f;
		pData[1].ny = 1.0f;
		pData[1].nz = 0.0f;
		pData[1].Tex0X = 0.0f;
		pData[1].Tex0Y = 0.0f;


		pData[2].x = fLength / 2.0f;
		pData[2].y = 0;
		pData[2].z = fWidth / 2.0f;
		pData[2].nx = 0.0f;
		pData[2].ny = 1.0f;
		pData[2].nz = 0.0f;
		pData[2].Tex0X = 1.0f;
		pData[2].Tex0Y = 0.0f;

		pData[3].x = fLength / 2.0f;
		pData[3].y = 0;
		pData[3].z = -fWidth / 2.0f;
		pData[3].nx = 0.0f;
		pData[3].ny = 1.0f;
		pData[3].nz = 0.0f;
		pData[3].Tex0X = 1.0f;
		pData[3].Tex0Y = 1.0f;

		for (int j = 0; j < pObj->m_nVertices; ++j) {
			
			if (pData[j].x < pObj->m_vMinObjBoundary.x) {
				pObj->m_vMinObjBoundary.x = pData[j].x;
			}
			if (pData[j].x > pObj->m_vMaxObjBoundary.x) {
				pObj->m_vMaxObjBoundary.x = pData[j].x;
			}

			if (pData[j].y < pObj->m_vMinObjBoundary.y) {
				pObj->m_vMinObjBoundary.y = pData[j].y;
			} 
			if (pData[j].y > pObj->m_vMaxObjBoundary.y) {
				pObj->m_vMaxObjBoundary.y = pData[j].y;
			}
			if (pData[j].z < pObj->m_vMinObjBoundary.z) {
				pObj->m_vMinObjBoundary.z = pData[j].z;
			} 
			if (pData[j].z > pObj->m_vMaxObjBoundary.z) {
				pObj->m_vMaxObjBoundary.z = pData[j].z;
			}
		}

		pObj->m_vCenter.x = (pObj->m_vMaxObjBoundary.x + pObj->m_vMinObjBoundary.x) / 2;
		pObj->m_vCenter.y = (pObj->m_vMaxObjBoundary.y + pObj->m_vMinObjBoundary.y) / 2;
		pObj->m_vCenter.z = (pObj->m_vMaxObjBoundary.z + pObj->m_vMinObjBoundary.z) / 2;
		m_vCenter = pObj->m_vCenter;
		m_vMaxObjBoundary = pObj->m_vMaxObjBoundary;
		m_vMinObjBoundary = pObj->m_vMinObjBoundary;

		D3D10_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(Vertex) * pObj->m_nVertices;
		cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
		cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		cbDesc.CPUAccessFlags = 0;
		cbDesc.MiscFlags = 0;

		D3D10_SUBRESOURCE_DATA vbInitData;
		ZeroMemory( &vbInitData, sizeof( D3D10_SUBRESOURCE_DATA ) );
		vbInitData.pSysMem = pData;
		vbInitData.SysMemPitch = 0;
		vbInitData.SysMemSlicePitch = 0;

		HRESULT hr = ( m_pd3dDevice->CreateBuffer( &cbDesc, &vbInitData, &pObj->m_pVB ) );

		ASSERT(hr == D3D_OK);

		pObj->m_pIB = NULL;

		DWORD wIndices[] =
		{
			0,1,2,
			0,2,3,
		};
		
		cbDesc.ByteWidth = (pObj->m_nFaces)* sizeof(DWORD) * 3;
		cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
		cbDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		cbDesc.CPUAccessFlags = 0;
		cbDesc.MiscFlags = 0;
        
		ZeroMemory( &vbInitData, sizeof( D3D10_SUBRESOURCE_DATA ) );
		vbInitData.pSysMem = wIndices;
		vbInitData.SysMemPitch = 0;
		vbInitData.SysMemSlicePitch = 0;

		hr = ( m_pd3dDevice->CreateBuffer( &cbDesc, &vbInitData, &pObj->m_pIB ) );

		ASSERT(hr == D3D_OK);

		ZeroMemory(szDesc, sizeof(szDesc));
		::LoadString(theApp.m_hInstance, IDS_DESC_RENDEROBJ, szDesc, 256);
		pObj->m_uStride = sizeof(Vertex);
		pObj->m_strDesc = szDesc;

		ZeroMemory(&pObj->m_vMaterails, sizeof(pObj->m_vMaterails));
	
		pObj->m_vMaterails.Diffuse.r = 0.5f;
		pObj->m_vMaterails.Diffuse.g = 0.5f;
		pObj->m_vMaterails.Diffuse.b = 0.5f;
		pObj->m_vMaterails.Diffuse.a = 1.0f;

		pObj->m_vMaterails.Ambient = pObj->m_vMaterails.Diffuse;
		pObj->m_vMaterails.Specular = pObj->m_vMaterails.Diffuse;

		pObj->m_vRawMaterails = pObj->m_vMaterails;
		pObj->m_pTextureDesc = pTexDescObj;
		pObj->InitDeviceObjects();

		pObj->RestoreDeviceObjects();
		D3DXMatrixIdentity(&pObj->m_matWorldDefault);
	
		pObj->m_fAlphaMap = false;

		pTexDescObj != NULL &&	pTexDescObj->m_pHostObjects.AddTail(pObj);

		int nTmp = -1;
		//m_p3DScene->AddTextureDescObj(pTexDescObj, nTmp);
		
		AddRenderObj(pObj, nTmp);
	
		delete [] pData;
		pData = NULL;

		return F3DSuccess;
	}

    Drawing a Line List
    The example in this section uses the sample vertex list created by following step 1 in the Creating Vertices procedure.
    To draw a line list
    Create an index array that indexes into the vertex buffer.
    This identifies a series of lines.
    C#
    // Initialize an array of indices of type short.
    lineListIndices = new short[(points * 2) - 2];
    
    // Populate the array with references to indices in the vertex buffer
    for (int i = 0; i < points - 1; i++)
    {
        lineListIndices[i * 2] = (short)(i);
        lineListIndices[(i * 2) + 1] = (short)(i + 1);
    }
    Render the lines by calling DrawUserIndexedPrimitives, which specifies PrimitiveType.LineList to determine how to interpret the data in the vertex array.
    C#
    GraphicsDevice.DrawUserIndexedPrimitives<VertexPositionColor>(
        PrimitiveType.LineList,
        primitiveList,
        0,  // vertex buffer offset to add to each element of the index buffer
        8,  // number of vertices in pointList
        lineListIndices,  // the index buffer
        0,  // first index element to read
        7   // number of primitives to draw
    );

2016/05/12
    1. 添加铅垂线
    2. 场景设置，增加 scale
    3. amf3 协议封装
    http://wooce.iteye.com/blog/1396278
#endregion //z 2016-05-23 19:23:08 L.222'16612 T130859512 .K.F4034662268---+----+----+----+----+

#region 05-23 19:25 R#.2    @h264相关文章，h264分析以及解码
//z 2016-05-23 19:25:39 L.222'16461 T130931486 .K[T1,L363,R2,V13]
01. 
http://blog.csdn.net/leixiaohua1020/article/details/17933821
http://blog.csdn.net/liushu1231/article/details/9203713
http://blog.csdn.net/subfate/article/details/20150017

将h.264裸码流推送到RTMP服务器
http://blog.csdn.net/win_lin/article/details/41170653
抛开flash，自己开发实现C++ RTMP直播流播放器
http://www.cnblogs.com/haibindev/p/3466094.html
Extract frames from iPhone videos using the FFMpeg libraries. http://www.codza.com
https://github.com/lajos/iFrameExtractor
socket h264 组帧
http://www.oschina.net/question/217709_34304?fromerr=ABY8Ln14

hikvision
streaming video stream from hikvision card to rtmp server
https://github.com/lookflying/hikvision

LibRtmp-Client-for-Android
https://github.com/ButterflyTV/LibRtmp-Client-for-Android

//z 2016-07-13 12:55:02 L.171'39898 T109959345 .K[T15,L407,R2,V164]
音视频同步(播放)原理
http://blog.csdn.net/zhuweigangzwg/article/details/25815851
#endregion //z 2016-05-23 19:25:37 L.222'16463 T130931484 .K.F4034662268---+----+----+----+----+

#region 05-31 18:21 R#.3    @看下使用 sdl 来播放 pcm 。
//z 2016-05-31 18:21:19 L.214'20321 T941162390 .K[T2,L391,R3,V43]
00. 编译sdl
01. 如何使用 sdl 播放pcm

//z 2016-06-01 19:28:45 L.213'16275 T1147141403.K[T3,L395,R3,V54]
02. audio video 同步
http://dranger.com/ffmpeg/tutorial05.html
https://github.com/Bilibili/ijkplayer.git

视音频同步
http://blog.csdn.net/yuanrxdu/article/details/23593035
http://blog.csdn.net/zhuweigangzwg/article/details/25815851
SDL2+FFmpeg 制作简单播放器&同步
http://www.cnblogs.com/GridScience/p/3643428.html

//z 2016-06-01 19:56:36 L.213'14604 T1150627260.K[T4,L407,R3,V64]
https://github.com/fallgold/stagefright_flv.git
http://blog.csdn.net/fallgold/article/details/12218815
#endregion //z 2016-05-31 18:21:17 L.214'20323 T941162388 .K.F4034662268---+----+----+----+----+

#region 06-06 18:47 R#.4    @tn todo 编译av ； av同步； 剥离实现
//z 2016-06-06 18:47:24 L.208'18756 T197016157 .K[T5,L415,R4,V70]
01. 

//z 2016-06-06 18:50:17 L.208'18583 T197950489 .K[T6,L419,R4,V78]
2016-06-06 18:50 todo 解析出rtmp的av数据，其中有dts数据，看看如何同步。

//z 2016-06-12 18:19:44 L.202'20416 T2159770267.K[T7,L422,R4,V89]
http://www.cnblogs.com/lihaiping/p/4018680.html
http://code.qtuba.com/article-53219.html
http://www.xuebuyuan.com/696182.html
http://wenku.baidu.com/view/2e90506baf1ffc4ffe47acac.html
http://blog.csdn.net/shaqoneal/article/details/16959671
http://blog.csdn.net/rootusers/article/details/41250273
repeat_pict 延时
https://www.google.com/#newwindow=1&q=repeat_pict+%E5%BB%B6%E6%97%B6
#endregion //z 2016-06-06 18:47:22 L.208'18758 T197016155 .K.F4034662268---+----+----+----+----+

#region 07-06 18:18 R#.5    @atom 编辑器
//z 2016-07-06 18:18:49 L.178'20471 T1525122819.K[T8,L437,R5,V102]
01. 
http://www.jeffjade.com/2015/10/19/2015-10-18-Efficacious-win-software/
#endregion //z 2016-07-06 18:18:42 L.178'20478 T1525122812.K.F4034662268---+----+----+----+----+

#region 07-06 18:32 R#.6    @directshow audio capture filter
//z 2016-07-06 18:32:44 L.178'19636 T1527279034.K[T9,L445,R6,V107]
01. 
https://github.com/rdp/screen-capture-recorder-to-video-windows-free
https://github.com/rdp/directshow-demo-audio-input-open-source
#endregion //z 2016-07-06 18:32:43 L.178'19637 T1527279033.K.F4034662268---+----+----+----+----+

#region 07-07 11:01 R#.7    @SGK
znote 36011
//z 2016-07-07 11:01:48 L.177'46692 T1141145524.K[T10,L454,R7,V116]
01. n6m5 
<nppcrypt version="101">
<encryption cipher="aes256" mode="cbc" encoding="base16" />
<random iv="0REZBbRRm/442KPkECJ+Og==" salt="/NA9eowg0v1jEZrIZ+Wzgg==" />
<key algorithm="pbkdf2" hash="md5" iterations="1000" />
</nppcrypt>
9867ca3b016cf6becf03faa545accfb07671c36263e383693f6b68e123813cdd8c8ef2130cbd8be73a5c3277e4e7c6eba4c9fd857bfa685691b3b810227e74b8
1590c4b999e8c154a4992b0b75390b93f83dc08eeee95685ac8ab68316df79c705a9973d43a474c224259bb2fa596dec3186d25e1f6724490f8a50ae6d666730
cd742abf8d660895c1f95c479d87b7a45afd9ebb885cbc42b13d9e20388093122d96b381caab372a028eb50b3a610fc4898a09b8a3d3006f75145c22a47a6c5b
#endregion //z 2016-07-07 11:01:46 L.177'46694 T1141145522.K.F4034662268---+----+----+----+----+

#region 07-08 14:44 R#.8    @sdl 实际的例子
//z 2016-07-08 14:44:17 L.176'33343 T793234907 .K[T11,L470,R8,V129]
01. 
sdl image
https://www.libsdl.org/projects/SDL_image/release
sdl pong
https://github.com/chaficnajjar/21st-century-pong
sdl 中文
http://kelvmiao.info/sdl-tutorial-cn/contents.html

//z 2016-07-12 17:09:21 L.172'24639 T2198744725.K[T13,L482,R8,V148]
oculus rift sdk with dx12
https://github.com/ClemensX/ShadedPath12.git

//z 2016-07-18 14:15:07 L.166'35093 T3027158489.K[T17,L493,R8,V172]
一些 oculus sdk 的例子
https://github.com/jimbo00000/RiftSkeleton

//z 2016-07-18 14:16:46 L.166'34994 T3027194557.K[T18,L497,R8,V175]
http://federico-mammano.github.io/Looking-Through-Oculus-Rift/

//z 2016-07-18 14:18:19 L.166'34901 T3027266335.K[T19,L500,R8,V176]
https://github.com/jherico/OculusMinimalExample

//z 2016-07-18 14:18:56 L.166'34864 T3027266464.K[T20,L503,R8,V177]
与 0.7 相关的各种资源集合
https://www.reddit.com/r/oculus/comments/3iozd5/oculus_gamesdemos_working_with_07/
https://www.reddit.com/r/oculus/wiki/sdk07_compatible
#endregion //z 2016-07-08 14:44:16 L.176'33344 T793234906 .K.F4034662268---+----+----+----+----+

#region 07-11 18:26 R#.9    @算法
//z 2016-07-11 18:26:13 L.173'20027 T3424675317.K[T12,L484,R9,V144]
01. 
https://github.com/julycoding/The-Art-Of-Programming-By-July
#endregion //z 2016-07-11 18:26:11 L.173'20029 T3424675315.K.F4034662268---+----+----+----+----+

#region 07-13 12:25 R#.10   @hevc
//z 2016-07-13 12:25:19 L.171'41681 T106401622 .K[T14,L496,R10,V154]
01. 硬编码器
http://jointwave.com/products.html
https://yq.aliyun.com/articles/31142
http://www.realpower265.com/download/index.html
http://www.strongene.com/cn/hevc/decoder/keyFeatures.jsp

//z 2016-07-14 10:24:20 L.170'48940 T1886198444.K[T16,L507,R10,V168]
三个镜头半球全景，移动，航模，大疆
http://www.tairtech.com/topics/2901.html
#endregion //z 2016-07-13 12:25:15 L.171'41685 T106401618 .K.F4034662268---+----+----+----+----+

#region 07-18 18:24 R#.11   @png 载入
//z 2016-07-18 18:25:03 L.166'20097 T3899248986.K[T21,L531,R11,V184]
01. 
https://github.com/lvandeve/lodepng
https://github.com/elanthis/upng
#endregion //z 2016-07-18 18:24:55 L.166'20105 T3899213216.K.F4034662268---+----+----+----+----+

#region 07-19 15:16 R#.12   @设置 beyond compare 为diff tool
//z 2016-07-19 15:16:25 L.165'31415 T91496540  .K[T22,L541,R12,V191]
01. 
【总结】

把TortoiseSVN中默认比较工具换成Beyond Compare的办法：

右击svn文件->Settings->Diff Viewer->选中External->设置值为

?
1
"D:\Program Files (x86)\Beyond Compare\BCompare.exe" %base %mine /title1=%bname /title2=%yname /leftreadonly
其中的含义是：

"E:\dev_install\Beyond Compare 3\BCompare.exe" 表示你的Beyond Compare的安装路径
%base 表示你的左边的要比较的文件->要比较的基础base的文件
%mine 表示要比较的右边的文件->mine，我的，我的新修改的文件
/title1=%bname 左边的标题，标题1是base file的name
/title2=%yname 右边的标题，标题2是新文件？的名字
/leftreadonly  左边是只读->不允许修改->想要运行修改左边文件，则应该是不要加上此参数
即可。
#endregion //z 2016-07-19 15:16:23 L.165'31417 T91496538  .K.F4034662268---+----+----+----+----+

#region 07-20 17:15 R#.13   @iocp server
//z 2016-07-20 17:15:58 L.164'24242 T6662395   .K[T23,L566,R13,V200]
01. 
iocp server
https://github.com/zsummer/zsummerX
push framework
https://pushframework.codeplex.com/

jp1
http://www.codeproject.com/Articles/2336/A-reusable-high-performance-socket-server-class-Pa
#endregion //z 2016-07-20 17:15:56 L.164'24244 T6662393   .K.F4034662268---+----+----+----+----+

#region 07-20 18:04 R#.14   @ffmpeg
//z 2016-07-20 18:04:20 L.164'21340 T1296908399.K[T24,L575,R14,V210]
01. vc 编译 ffmpeg
https://trac.ffmpeg.org/wiki/CompilationGuide/MSVC
https://www.ffmpeg.org/platform.html
静态库：./configure --enable-static --prefix=./vs2013_build --enable-debug --toolchain=msvc 
动态库：./configure --enable-shared --prefix=./vs2013_build --enable-debug --toolchain=msvc

$ ./configure --enable-asm --enable-yasm --disable-ffserver --disable-avdevice --disable-doc --disable-ffplay --disable-ffprobe --disable-ffmpeg --enable-shared --disable-static --disable-bzlib --disable-libopenjpeg --disable-iconv --disable-zlib --prefix=/c/ffmpeg --toolchain=msvc --arch=amd64 --extra-cflags="-MDd" --extra-ldflags="/NODEFAULTLIB:libcmt" --enable-debug

./configure --toolchain=msvc --arch=x86_64 --enable-debug --enable-shared --disable-optimizations --prefix=./vs2013_build_debug --enable-gpl --enable-nonfree --enable-cuvid --enable-cuda --extra-cflags=-I'D:/msys/local/include' --extra-cflags=/D'_DEBUG' --extra-cflags=-ID:/msys/local/inc --extra-ldflags=/LIBPATH:D:/MSYS/local/lib

./configure --toolchain=msvc --arch=x86_64 --enable-debug --enable-shared --disable-optimizations --prefix=./vs2013_build_debug --enable-gpl --enable-nonfree --enable-cuvid --enable-cuda --extra-cflags=-I'D:/msys/local/include' --extra-ldflags=/LIBPATH:D:/MSYS/local/lib

x86_64
--disable-optimizations
$ ./configure --enable-asm --enable-yasm --enable-shared --prefix=./vs2013_build --toolchain=msvc --arch=amd64 --extra-cflags="-MDd" --extra-ldflags="/NODEFAULTLIB:libcmt" --enable-debug
#endregion //z 2016-07-20 18:04:18 L.164'21342 T1296908374.K.F4034662268---+----+----+----+----+

#region 07-22 14:21 R#.15   @JetZodiac
01. namespace
#endregion //z 2016-07-22 14:21:02 L.162'34738 T563870540 .K.F4034662268---+----+----+----+----+

#region 08-03 16:17 R#.16   @glob 模式
//z 2016-08-03 16:17:24 L.150'27756 T70375159  .K[T25,L631,R16,V249]
01. 
glob 模式是指 shell 所使用的简化了的正则表达式。星号（*）匹配零个或多个任意字符；[abc] 匹配任何一个列在方括号中的字符（这个例子要么匹配一个 a，要么匹配一个 b，要么匹配一个 c）；问号（?）只匹配一个任意字符；如果在方括号中使用短划线分隔两个字符，表示所有在这两个字符范围内的都可以匹配（比如 [0-9]表示匹配所有 0 到 9 的数字）。
#endregion //z 2016-08-03 16:17:23 L.150'27757 T70375158  .K.F4034662268---+----+----+----+----+

#region 08-03 16:42 R#.17   @git用法
//z 2016-08-03 16:42:15 L.150'26265 T73753205  .K[T26,L639,R17,V258]
01. github 的官方示例
https://github.com/github/gitignore
#endregion //z 2016-08-03 16:42:14 L.150'26266 T73753204  .K.F4034662268---+----+----+----+----+
