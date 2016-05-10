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
    