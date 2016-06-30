ffmpeg 2.3版本, 关于ffplay音视频同步的分析

最近学习播放器的一些东西，所以接触了ffmpeg,看源码的过程中，就想了解一下ffplay是怎么处理音视频同步的，之前只大概知道通过pts来进行同步，但对于如何实现却不甚了解，所以想借助这个机会，从最直观的代码入手，详细分析一下如何处理音视频同步。在看代码的时候，刚开始脑袋一片混乱，对于ffplay.c里面的各种时间计算完全摸不着头脑，在网上查找资料的过程中，发现关于分析ffplay音视频同步的东西比较少，要么就是ffplay版本太过于老旧，代码和现在最新版本已经不一样，要么就是简单的分析了一下，没有详细的讲清楚为什么要这么做。遂决定，在自己学习的过程中，记录下自己的分析思路，以供大家指正和参考。我用的ffmpeg版本是2.3， SDL版本为1.2.14，编译环境是windos xp下使用MinGw+msys.

一、先简单介绍下ffplay的代码结构。如下：
1.      Main函数中需要注意的有
（1）      av_register_all接口，该接口的主要作用是注册一些muxer、demuxer、coder、和decoder. 这些模块将是我们后续编解码的关键。每个demuxer和decoder都对应不同的格式，负责不同格式的demux和decode

（2） stream_open接口，该接口主要负责一些队列和时钟的初始化工作，另外一个功能就是创建read_thread线程，该线程将负责文件格式的检测，文件的打开以及frame的读取工作，文件操作的主要工作都在这个线程里面完成

（3） event_loop：事件处理，event_loop->refresh_loop_wait_event-> video_refresh,通过这个顺序进行视频的display

2．Read_thread线程
（1）  该线程主要负责文件操作，包括文件格式的检测，音视频流的打开和读取，它通过av_read_frame读取完整的音视频frame packet，并将它们放入对应的队列中，等待相应的解码线程进行解码

3. video_thread线程，该线程主要负责将packet队列中的数据取出并进行解码，然将解码完后的picture放入picture队列中，等待SDL进行渲染

4. sdl_audio_callback，这是ffplay注册给SDL的回调函数，其作用是进行音频的解码，并在SDL需要数据的时候，将解码后的音频数据写入SDL的缓冲区，SDL再调用audio驱动的接口进行播放。

5. video_refresh,该接口的作用是从picture队列中获取pic，并调用SDL进行渲染，音视频同步的关键就在这个接口中



二、音视频的同步
要想了解音视频的同步，首先得去了解一些基本的概念，video的frame_rate. Pts, audio的frequency之类的东西，这些都是比较基础的，网上资料很多，建议先搞清楚这些基本概念，这样阅读代码才会做到心中有数，好了，闲话少说，开始最直观的源码分析吧，如下：

（1）      首先来说下video和audio 的输出接口，video输出是通过调用video_refresh-> video_display-> video_image_display-> SDL_DisplayYUVOverlay来实现的。Audio是通过SDL回调sdl_audio_callback（该接口在打开音频时注册给SDL）来实现的。

（2）      音视频同步的机制，据我所知有3种，(a)以音频为基准进行同步（b）以视频为基准进行同步（c）以外部时钟为基准进行同步。Ffplay中默认以音频为基准进行同步，我们的分析也是基于此，其它两种暂不分析。

（3）      既然视频和音频的播放是独立的，那么它们是如何做到同步的，答案就是通过ffplay中音视频流各自维护的clock来实现，具体怎么做，我们还是来看代码吧。

（4）      代码分析：

（a）      先来看video_refresh的代码, 去掉了一些无关的代码，像subtitle和状态显示

static void video_refresh(void *opaque, double *remaining_time)
{
    VideoState *is = opaque;
    double time;
    SubPicture *sp, *sp2;

    if (!is->paused &&get_master_sync_type(is) == AV_SYNC_EXTERNAL_CLOCK && is->realtime)
        check_external_clock_speed(is);

    if(!display_disable && is->show_mode != SHOW_MODE_VIDEO &&is->audio_st)
    {
        time = av_gettime_relative() /1000000.0;

        if (is->force_refresh ||is->last_vis_time + rdftspeed < time)
        {
            video_display(is);
            is->last_vis_time = time;
        }

        *remaining_time =FFMIN(*remaining_time, is->last_vis_time + rdftspeed - time);
    }

    if (is->video_st)
    {
        int redisplay = 0;

        if (is->force_refresh)
            redisplay = pictq_prev_picture(is);

retry:
        if (pictq_nb_remaining(is) == 0)
        {
            // nothing to do, no picture to display in the queue
        }
        else
        {
            double last_duration, duration, delay;
            VideoPicture *vp, *lastvp;

            /* dequeue the picture */
            lastvp =&is->pictq[is->pictq_rindex];
            vp =&is->pictq[(is->pictq_rindex + is->pictq_rindex_shown) % VIDEO_PICTURE_QUEUE_SIZE];

            if (vp->serial !=is->videoq.serial)
            {
                pictq_next_picture(is);
                is->video_current_pos = -1;
                redisplay = 0;

                goto retry;
            }

            /*不管是vp的serial还是queue的serial, 在seek操作的时候才会产生变化，更准确的说，应该是packet 队列发生flush操作时*/
            if (lastvp->serial !=vp->serial && !redisplay)
            {
				//z 如果发生了 seek，那么更新 frame_timer
                is->frame_timer =av_gettime_relative() / 1000000.0;
            }

            if (is->paused)
                goto display;

            /*通过pts计算duration，duration是一个video frame的持续时间，当前帧的pts 减去上一帧的pts*/
            /* compute nominal last_duration */
            last_duration = vp_duration(is,lastvp, vp);

            if (redisplay)
            {
                delay = 0.0;
            }
            /*音视频同步的关键点*/
            else
                delay =compute_target_delay(last_duration, is);

            /*time 为系统当前时间，av_gettime_relative拿到的是1970年1月1日到现在的时间，也就是格林威治时间*/
            time=av_gettime_relative()/1000000.0;

            /*frame_timer实际上就是上一帧的播放时间，该时间是一个系统时间，而 frame_timer + delay 实际上就是当前这一帧的播放时间*/
            if (time < is->frame_timer +delay && !redisplay)
            {
                /*remaining 就是在refresh_loop_wait_event 中还需要睡眠的时间，其实就是现在还没到这一帧的播放时间，我们需要睡眠等待*/
				//z 到这一帧播放还需要多少时间。
                *remaining_time =FFMIN(is->frame_timer + delay - time,  *remaining_time);

                return;
            }

			//z 这一帧播放完的时间
            is->frame_timer += delay;

			/*如果下一帧的播放时间已经过了，并且其和当前系统时间的差值超过AV_SYNC_THRESHOLD_MAX，
			则将下一帧的播放时间改为当前系统时间，并在后续判断是否需要丢帧，其目的是立刻处理?*/
            if (delay > 0 && time -is->frame_timer > AV_SYNC_THRESHOLD_MAX)
            {
                is->frame_timer = time;
            }

            SDL_LockMutex(is->pictq_mutex);
            /*视频帧的pts一般是从0开始，按照帧率往上增加的，此处pts是一个相对值，和系统时间
			没有关系，对于固定fps，一般是按照1/frame_rate的速度往上增加，可变fps暂时没研究*/
            if (!redisplay &&!isnan(vp->pts))
                /*更新视频的clock，将当前帧的pts和当前系统的时间保存起来，这2个数据将和
			    audio  clock的pts 和系统时间一起计算delay*/
                update_video_pts(is,vp->pts, vp->pos, vp->serial);

            SDL_UnlockMutex(is->pictq_mutex);
            if (pictq_nb_remaining(is) > 1)
            {
				//z 得到下一帧数据
                VideoPicture *nextvp =&is->pictq[(is->pictq_rindex + is->pictq_rindex_shown + 1) %VIDEO_PICTURE_QUEUE_SIZE];
				//z 计算这帧和下一帧的差
                duration = vp_duration(is, vp,nextvp);

                /*如果延迟时间超过一帧，并且允许丢帧，则进行丢帧处理*/
				//z 当前时间已经超过了下一帧的播放时间了。
                if(!is->step &&(redisplay || framedrop>0 || (framedrop && get_master_sync_type(is)!= AV_SYNC_VIDEO_MASTER)) && time > is->frame_timer + duration)
                {
                    if (!redisplay)
                        is->frame_drops_late++;//z 那么将该帧丢弃

                    /*丢掉延迟的帧，取下一帧*/
					//z 丢弃该帧
                    pictq_next_picture(is);

                    redisplay = 0;

                    goto retry;
                }
            }

display:
            /* display picture */
            /* 刷新视频帧 */
            if (!display_disable &&is->show_mode == SHOW_MODE_VIDEO)
                video_display(is);

			pictq_next_picture(is);

            if (is->step &&!is->paused)
                stream_toggle_pause(is);
        }
    }
}

（b）视频的播放实际上是通过上一帧的播放时间加上一个延迟来计算下一帧的计算时间的，例如上一帧的
播放时间pre_pts是0，延迟delay为33ms，那么下一帧的播放时间则为0+33ms,第一帧的播放时间我们可以轻松
获取，那么后续帧的播放时间的计算，起关键点就在于delay，我们就是根据delay来控制视频播放的速度，从
而达到与音频同步的目的，那么如何计算delay？接着看代码，compute_target_delay接口：

static doublecompute_target_delay(double delay, VideoState *is)
{
    double sync_threshold,diff;

    /* update delay to followmaster synchronisation source */
    /*如果主同步方式不是以视频为主，默认是以audio为主进行同步*/
    if(get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER)
    {
        /* if video is slave,we try to correct big delays by duplicating ordeleting a frame */

        /*get_clock(&is->vidclk)获取到的实际上是:从处理最后一帧开始到现在的时间加上最后一帧的pts,
		具体参考set_clock_at 和get_clock的代码get_clock(&is->vidclk) ==is->vidclk.pts, 
		av_gettime_relative() / 1000000.0 -is->vidclk.last_updated  +is->vidclk.pts*/

        /*driff实际上就是已经播放的最近一个视频帧和音频帧pts的差值+ 两方系统的一个差值，用公式表达如下:
        pre_video_pts: 最近的一个视频帧的pts
        video_system_time_diff: 记录最近一个视频pts 到现在的时间，即av_gettime_relative()/ 1000000.0 - is->vidclk.last_updated

        pre_audio_pts: 音频已经播放到的时间点，即已经播放的数据所代表的时间，通过已经播放的
		samples可以计算出已经播放的时间，在sdl_audio_callback中被设置

        audio_system_time_diff: 同video_system_time_diff

        最终视频和音频的diff可以用下面的公式表示:
        diff = (pre_video_pts-pre_audio_pts) +(video_system_time_diff -  audio_system_time_diff)

        如果diff<0, 则说明视频播放太慢了，如果diff>0,
        则说明视频播放太快，此时需要通过计算delay来调整视频的播放速度如果
        diff<AV_SYNC_THRESHOLD_MIN || diff>AV_SYNC_THRESHOLD_MAX 则不用调整delay?*/

        diff =get_clock(&is->vidclk) - get_master_clock(is);

        /* skip or repeatframe. We take into account the
           delay to computethe threshold. I still don't know
           if it is the bestguess */

        sync_threshold=FFMAX(AV_SYNC_THRESHOLD_MIN,FFMIN(AV_SYNC_THRESHOLD_MAX,delay));

        if (!isnan(diff)&& fabs(diff) < is->max_frame_duration)
        {
            if (diff <=-sync_threshold)
                delay =FFMAX(0, delay + diff);
            else if (diff >= sync_threshold&& delay > AV_SYNC_FRAMEDUP_THRESHOLD)
                delay = delay+ diff;
            else if (diff>= sync_threshold)
                delay = 2 *delay;
        }
    }

    av_dlog(NULL, "video:delay=%0.3f A-V=%f\n", delay, -diff);

    return delay;
}

（c）看了以上的分析，是不是对于如何将视频同步到音频有了一个了解,那么音频clock是在哪里设置的呢？继续看代码，sdl_audio_callback 分析
static void sdl_audio_callback(void *opaque, Uint8 *stream, int len)
{
    VideoState *is = opaque;
    int audio_size, len1;

    /*当前系统时间*/
    audio_callback_time =av_gettime_relative();

    /*len为SDL中audio buffer的大小，单位是字节，该大小是我们在打开音频设备时设置*/
    while (len > 0)
    {
        /*如果audiobuffer中的数据少于SDL需要的数据，则进行解码*/
        if(is->audio_buf_index >= is->audio_buf_size)
        {
            audio_size = audio_decode_frame(is);

            if (audio_size <0)
            {
                /* if error,just output silence */
                is->audio_buf      =is->silence_buf;
                is->audio_buf_size =sizeof(is->silence_buf) / is->audio_tgt.frame_size *is->audio_tgt.frame_size;
            }
            else
            {
                if(is->show_mode != SHOW_MODE_VIDEO)
                    update_sample_display(is, (int16_t *)is->audio_buf, audio_size);

                is->audio_buf_size = audio_size;
            }

            is->audio_buf_index = 0;
        }

        /*判断解码后的数据是否满足SDL需要*/
        len1 =is->audio_buf_size - is->audio_buf_index;

        if (len1 > len)
            len1 = len;

        memcpy(stream,(uint8_t *)is->audio_buf + is->audio_buf_index, len1);
        len -= len1;
        stream += len1;
        is->audio_buf_index+= len1;
    }

    is->audio_write_buf_size = is->audio_buf_size -is->audio_buf_index;

    /* Let's assume the audiodriver that is used by SDL has two periods. */
    if(!isnan(is->audio_clock))
    {       
		/*set_clock_at第二个参数是计算音频已经播放的时间，相当于video中的上一帧的播放时间，如果不
		通过SDL，例如直接使用linux下的dsp设备进行播放，那么我们可以通过ioctl接口获取到驱动的
		audiobuffer中还有多少数据没播放，这样，我们通过音频的采样率和位深，可以很精确的算出音频
		播放到哪个点了，但是此处的计算方法有点让人看不懂*/
        set_clock_at(&is->audclk,
		is->audio_clock - (double)(2 * is->audio_hw_buf_size +is->audio_write_buf_size) / is->audio_tgt.bytes_per_sec,
		is->audio_clock_serial,
		audio_callback_time / 1000000.0);

        sync_clock_to_slave(&is->extclk, &is->audclk);
    }
}

三、总结
	音视频同步，拿以音频为基准为例，其实就是将视频当前的播放时间和音频当前的播放时间作比较，如果
	视频播放过快，则通过加大延迟或者重复播放来使速度降下来，如果慢了，则通过减小延迟或者丢帧来追
	赶音频播放的时间点，而且关键就在于音视频时间的比较以及延迟的计算。

四、还存在的问题
	关于sdl_audio_callback中 set_clock_at第二个参数的计算，为什么要那么做，还不是很明白，也有可能
	那只是一种假设的算法，只是经验，并没有什么为什么，但也有可能是其他，希望明白的人给解释一下。
	大家互相学习，互相进步。
																		邓旭光 于2015年3月17日

Ps:转摘请注明出处
