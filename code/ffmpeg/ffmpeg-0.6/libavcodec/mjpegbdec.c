﻿/*
 * Apple MJPEG-B decoder
 * Copyright (c) 2002 Alex Beregszaszi
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * Apple MJPEG-B decoder.
 */

#include "../config.h"

#include "avcodec.h"
#include "mjpeg.h"
#include "mjpegdec.h"

static uint32_t read_offs(AVCodecContext *avctx, GetBitContext *gb, uint32_t size, const char *err_msg)
{
    uint32_t offs= get_bits_long(gb, 32);
    if(offs >= size)
    {
        av_log(avctx, AV_LOG_WARNING, err_msg, offs, size);
        return 0;
    }
    return offs;
}

static int mjpegb_decode_frame(AVCodecContext *avctx,
                               void *data, int *data_size,
                               AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    MJpegDecodeContext *s = avctx->priv_data;
    const uint8_t *buf_end, *buf_ptr;
    AVFrame *picture = data;
    GetBitContext hgb; /* for the header */
    uint32_t dqt_offs, dht_offs, sof_offs, sos_offs, second_field_offs;
    uint32_t field_size, sod_offs;

    buf_ptr = buf;
    buf_end = buf + buf_size;

read_header:
    /* reset on every SOI */
    s->restart_interval = 0;
    s->restart_count = 0;
    s->mjpb_skiptosod = 0;

    init_get_bits(&hgb, buf_ptr, /*buf_size*/(buf_end - buf_ptr)*8);

    skip_bits(&hgb, 32); /* reserved zeros */

    if (get_bits_long(&hgb, 32) != MKBETAG('m','j','p','g'))
    {
        av_log(avctx, AV_LOG_WARNING, "not mjpeg-b (bad fourcc)\n");
        return 0;
    }

    field_size = get_bits_long(&hgb, 32); /* field size */
    av_log(avctx, AV_LOG_DEBUG, "field size: 0x%x\n", field_size);
    skip_bits(&hgb, 32); /* padded field size */
    second_field_offs = read_offs(avctx, &hgb, buf_end - buf_ptr, "second_field_offs is %d and size is %d\n");
    av_log(avctx, AV_LOG_DEBUG, "second field offs: 0x%x\n", second_field_offs);

    dqt_offs = read_offs(avctx, &hgb, buf_end - buf_ptr, "dqt is %d and size is %d\n");
    av_log(avctx, AV_LOG_DEBUG, "dqt offs: 0x%x\n", dqt_offs);
    if (dqt_offs)
    {
        init_get_bits(&s->gb, buf_ptr+dqt_offs, (buf_end - (buf_ptr+dqt_offs))*8);
        s->start_code = DQT;
        ff_mjpeg_decode_dqt(s);
    }

    dht_offs = read_offs(avctx, &hgb, buf_end - buf_ptr, "dht is %d and size is %d\n");
    av_log(avctx, AV_LOG_DEBUG, "dht offs: 0x%x\n", dht_offs);
    if (dht_offs)
    {
        init_get_bits(&s->gb, buf_ptr+dht_offs, (buf_end - (buf_ptr+dht_offs))*8);
        s->start_code = DHT;
        ff_mjpeg_decode_dht(s);
    }

    sof_offs = read_offs(avctx, &hgb, buf_end - buf_ptr, "sof is %d and size is %d\n");
    av_log(avctx, AV_LOG_DEBUG, "sof offs: 0x%x\n", sof_offs);
    if (sof_offs)
    {
        init_get_bits(&s->gb, buf_ptr+sof_offs, (buf_end - (buf_ptr+sof_offs))*8);
        s->start_code = SOF0;
        if (ff_mjpeg_decode_sof(s) < 0)
            return -1;
    }

    sos_offs = read_offs(avctx, &hgb, buf_end - buf_ptr, "sos is %d and size is %d\n");
    av_log(avctx, AV_LOG_DEBUG, "sos offs: 0x%x\n", sos_offs);
    sod_offs = read_offs(avctx, &hgb, buf_end - buf_ptr, "sof is %d and size is %d\n");
    av_log(avctx, AV_LOG_DEBUG, "sod offs: 0x%x\n", sod_offs);
    if (sos_offs)
    {
//        init_get_bits(&s->gb, buf+sos_offs, (buf_end - (buf+sos_offs))*8);
        init_get_bits(&s->gb, buf_ptr+sos_offs, field_size*8);
        s->mjpb_skiptosod = (sod_offs - sos_offs - show_bits(&s->gb, 16));
        s->start_code = SOS;
        ff_mjpeg_decode_sos(s);
    }

    if (s->interlaced)
    {
        s->bottom_field ^= 1;
        /* if not bottom field, do not output image yet */
        if (s->bottom_field != s->interlace_polarity && second_field_offs)
        {
            buf_ptr = buf + second_field_offs;
            second_field_offs = 0;
            goto read_header;
        }
    }

    //XXX FIXME factorize, this looks very similar to the EOI code

    *picture= s->picture;
    *data_size = sizeof(AVFrame);

    if(!s->lossless)
    {
        picture->quality= FFMAX3(s->qscale[0], s->qscale[1], s->qscale[2]);
        picture->qstride= 0;
        picture->qscale_table= s->qscale_table;
        memset(picture->qscale_table, picture->quality, (s->width+15)/16);
        if(avctx->debug & FF_DEBUG_QP)
            av_log(avctx, AV_LOG_DEBUG, "QP: %d\n", picture->quality);
        picture->quality*= FF_QP2LAMBDA;
    }

    return buf_ptr - buf;
}

AVCodec mjpegb_decoder =
{
    "mjpegb",
    AVMEDIA_TYPE_VIDEO,
    CODEC_ID_MJPEGB,
    sizeof(MJpegDecodeContext),
    ff_mjpeg_decode_init,
    NULL,
    ff_mjpeg_decode_end,
    mjpegb_decode_frame,
    CODEC_CAP_DR1,
    NULL,
    .long_name = NULL_IF_CONFIG_SMALL("Apple MJPEG-B"),
};
