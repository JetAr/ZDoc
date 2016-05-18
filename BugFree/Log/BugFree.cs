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
