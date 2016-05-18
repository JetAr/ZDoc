/**
# Example to use srs-librtmp
# see: https://github.com/winlinvip/simple-rtmp-server/wiki/v2_CN_SrsLibrtmp
    gcc main.cpp srs_librtmp.cpp -g -O0 -lstdc++ -o output
*/
#include <windows.h>
#include <stdio.h>
#include "..\src\srs\srs_librtmp.h"

void parse_amf0_object(char* p, srs_amf0_t args)
{
	char opvt = 0; // object property value type.
	const char* opnp = NULL; // object property name ptr.
	const char* opvp = NULL; // object property value ptr.

	while (*p)
	{
		switch (*p++)
		{
		case 'O':
			while (*p && *p++ != ':')
			{
			}
			if (*p++ == '1')
			{
				printf("amf0 object start\n");
			}
			else
			{
				printf("amf0 object end\n");
			}
			break;
		case 'N':
			opvt = *p++;
			if (*p++ != ':')
			{
				printf("object property must split by :.\n");
				exit(-1);
			}
			opnp = p++;
			while (*p && *p++ != ':')
			{
			}
			p[-1] = 0;
			opvp = p;
			printf("amf0 %c property[%s]=%s\n", opvt, opnp, opvp);
			switch (opvt)
			{
			case 'S':
				srs_amf0_object_property_set(args, opnp, srs_amf0_create_string(opvp));
				break;
			default:
				printf("unsupported object property.\n");
				exit(-1);
			}
			*p = 0;
			break;
		default:
			printf("only supports an object arg.\n");
			exit(-1);
		}
	}
}

int main(int argc, char** argv)
{
	srs_flv_t flv = NULL;
	const char* rtmp_url = " rtmp://127.0.0.1:1935/live/demo";
	const char* output_flv = "e:\\srs_ar.flv";

    printf("suck rtmp stream like rtmpdump\n");
    printf("srs(simple-rtmp-server) client librtmp library.\n");
    printf("version: %d.%d.%d\n", srs_version_major(), srs_version_minor(), srs_version_revision());
	
    if (argc <= 1)
    {
        printf("Usage: %s <rtmp_url>\n"
               "   rtmp_url     RTMP stream url to play\n"
               "For example:\n"
               "   %s rtmp://127.0.0.1:1935/live/livestream\n"
               "   %s rtmp://ossrs.net:1935/live/livestream\n",
               argv[0], argv[0], argv[0]);
        exit(-1);
    }

    // startup socket for windows.
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(1, 1), &WSAData))
    {
        printf("WSAStartup failed.\n");
        return -1;
    }

    srs_human_trace("rtmp url: %s", argv[1]);
    srs_rtmp_t rtmp = srs_rtmp_create(argv[1]);

    if (srs_rtmp_handshake(rtmp) != 0)
    {
        srs_human_trace("simple handshake failed.");
        goto rtmp_destroy;
    }
    srs_human_trace("simple handshake success");

    if (srs_rtmp_connect_app(rtmp) != 0)
    {
        srs_human_trace("connect vhost/app failed.");
        goto rtmp_destroy;
    }
    srs_human_trace("connect vhost/app success");

    if (srs_rtmp_play_stream(rtmp) != 0)
    {
        srs_human_trace("play stream failed.");
        goto rtmp_destroy;
    }
    srs_human_trace("play stream success");


	flv = srs_flv_open_write(output_flv);

	// flv header
	char header[9];
	// 3bytes, signature, "FLV",
	header[0] = 'F';
	header[1] = 'L';
	header[2] = 'V';
	// 1bytes, version, 0x01,
	header[3] = 0x01;
	// 1bytes, flags, UB[5] 0, UB[1] audio present, UB[1] 0, UB[1] video present.
	header[4] = 0x03; // audio + video.
	// 4bytes, dataoffset
	header[5] = 0x00;
	header[6] = 0x00;
	header[7] = 0x00;
	header[8] = 0x09;
	srs_flv_write_header(flv, header);

	int64_t nb_packets = 0;
	u_int32_t pre_timestamp = 0;
	int64_t pre_now = -1;
	int64_t start_time = -1;
    for (;;)
    {
        int size;
        char type;
        char* data;
        u_int32_t timestamp, pts;

        if (srs_rtmp_read_packet(rtmp, &type, &timestamp, &data, &size) != 0)
        {
            goto rtmp_destroy;
        }

		if (pre_now == -1)
		{
			pre_now = srs_utils_time_ms();
		}
		if (start_time == -1)
		{
			start_time = srs_utils_time_ms();
		}

		if (srs_human_print_rtmp_packet4(type, timestamp, data, size, pre_timestamp, pre_now, start_time, nb_packets++) != 0)
		{
			srs_human_trace("print rtmp packet failed.");
			goto rtmp_destroy;
		}
		pre_timestamp = timestamp;
		pre_now = srs_utils_time_ms();

		// we only write some types of messages to flv file.
		int is_flv_msg = type == SRS_RTMP_TYPE_AUDIO
			|| type == SRS_RTMP_TYPE_VIDEO || type == SRS_RTMP_TYPE_SCRIPT;

		// for script data, ignore except onMetaData
		if (type == SRS_RTMP_TYPE_SCRIPT)
		{
			if (!srs_rtmp_is_onMetaData(type, data, size))
			{
				is_flv_msg = 0;
			}
		}

		if (flv)
		{
			if (is_flv_msg)
			{
				if (srs_flv_write_tag(flv, type, timestamp, data, size) != 0)
				{
					srs_human_trace("dump rtmp packet failed.");
					goto rtmp_destroy;
				}
			}
			else
			{
				srs_human_trace("drop message type=%#x, size=%dB", type, size);
			}
		}

		free(data);
    }

rtmp_destroy:
    srs_rtmp_destroy(rtmp);
	srs_flv_close(flv);
    // cleanup socket for windows.
    WSACleanup();

    return 0;
}
