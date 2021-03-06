/* 
 * MP3 encoder and decoder
 * Copyright (c) 2003 Fabrice Bellard.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "avformat.h"
#include "mpegaudio.h"

typedef struct {
    int version;
    int lsf, mpeg25;
    int layer;
    int rate;    
    int frequency;
    int padding;
    int crc;
} MPAHeaderInfo;

#define ID3_HEADER_SIZE 10
#define ID3_TAG_SIZE 128

#define ID3_GENRE_MAX 125

static const char *id3_genre_str[ID3_GENRE_MAX + 1] = {
    [0] = "Blues",
    [1] = "Classic Rock",
    [2] = "Country",
    [3] = "Dance",
    [4] = "Disco",
    [5] = "Funk",
    [6] = "Grunge",
    [7] = "Hip-Hop",
    [8] = "Jazz",
    [9] = "Metal",
    [10] = "New Age",
    [11] = "Oldies",
    [12] = "Other",
    [13] = "Pop",
    [14] = "R&B",
    [15] = "Rap",
    [16] = "Reggae",
    [17] = "Rock",
    [18] = "Techno",
    [19] = "Industrial",
    [20] = "Alternative",
    [21] = "Ska",
    [22] = "Death Metal",
    [23] = "Pranks",
    [24] = "Soundtrack",
    [25] = "Euro-Techno",
    [26] = "Ambient",
    [27] = "Trip-Hop",
    [28] = "Vocal",
    [29] = "Jazz+Funk",
    [30] = "Fusion",
    [31] = "Trance",
    [32] = "Classical",
    [33] = "Instrumental",
    [34] = "Acid",
    [35] = "House",
    [36] = "Game",
    [37] = "Sound Clip",
    [38] = "Gospel",
    [39] = "Noise",
    [40] = "AlternRock",
    [41] = "Bass",
    [42] = "Soul",
    [43] = "Punk",
    [44] = "Space",
    [45] = "Meditative",
    [46] = "Instrumental Pop",
    [47] = "Instrumental Rock",
    [48] = "Ethnic",
    [49] = "Gothic",
    [50] = "Darkwave",
    [51] = "Techno-Industrial",
    [52] = "Electronic",
    [53] = "Pop-Folk",
    [54] = "Eurodance",
    [55] = "Dream",
    [56] = "Southern Rock",
    [57] = "Comedy",
    [58] = "Cult",
    [59] = "Gangsta",
    [60] = "Top 40",
    [61] = "Christian Rap",
    [62] = "Pop/Funk",
    [63] = "Jungle",
    [64] = "Native American",
    [65] = "Cabaret",
    [66] = "New Wave",
    [67] = "Psychadelic",
    [68] = "Rave",
    [69] = "Showtunes",
    [70] = "Trailer",
    [71] = "Lo-Fi",
    [72] = "Tribal",
    [73] = "Acid Punk",
    [74] = "Acid Jazz",
    [75] = "Polka",
    [76] = "Retro",
    [77] = "Musical",
    [78] = "Rock & Roll",
    [79] = "Hard Rock",
    [80] = "Folk",
    [81] = "Folk-Rock",
    [82] = "National Folk",
    [83] = "Swing",
    [84] = "Fast Fusion",
    [85] = "Bebob",
    [86] = "Latin",
    [87] = "Revival",
    [88] = "Celtic",
    [89] = "Bluegrass",
    [90] = "Avantgarde",
    [91] = "Gothic Rock",
    [92] = "Progressive Rock",
    [93] = "Psychedelic Rock",
    [94] = "Symphonic Rock",
    [95] = "Slow Rock",
    [96] = "Big Band",
    [97] = "Chorus",
    [98] = "Easy Listening",
    [99] = "Acoustic",
    [100] = "Humour",
    [101] = "Speech",
    [102] = "Chanson",
    [103] = "Opera",
    [104] = "Chamber Music",
    [105] = "Sonata",
    [106] = "Symphony",
    [107] = "Booty Bass",
    [108] = "Primus",
    [109] = "Porn Groove",
    [110] = "Satire",
    [111] = "Slow Jam",
    [112] = "Club",
    [113] = "Tango",
    [114] = "Samba",
    [115] = "Folklore",
    [116] = "Ballad",
    [117] = "Power Ballad",
    [118] = "Rhythmic Soul",
    [119] = "Freestyle",
    [120] = "Duet",
    [121] = "Punk Rock",
    [122] = "Drum Solo",
    [123] = "A capella",
    [124] = "Euro-House",
    [125] = "Dance Hall",
};

/* buf must be ID3_HEADER_SIZE byte long */
static int id3_match(const uint8_t *buf)
{
    return (buf[0] == 'I' &&
            buf[1] == 'D' &&
            buf[2] == '3' &&
            buf[3] != 0xff &&
            buf[4] != 0xff &&
            (buf[6] & 0x80) == 0 &&
            (buf[7] & 0x80) == 0 &&
            (buf[8] & 0x80) == 0 &&
            (buf[9] & 0x80) == 0);
}

static void id3_get_string(char *str, int str_size, 
                           const uint8_t *buf, int buf_size)
{
    int i, c;
    char *q;

    q = str;
    for(i = 0; i < buf_size; i++) {
        c = buf[i];
        if (c == '\0')
            break;
        if ((q - str) >= str_size - 1)
            break;
        *q++ = c;
    }
    *q = '\0';
}

/* 'buf' must be ID3_TAG_SIZE byte long */
static int id3_parse_tag(AVFormatContext *s, const uint8_t *buf)
{
    char str[5];
    int genre;
    
    if (!(buf[0] == 'T' &&
          buf[1] == 'A' &&
          buf[2] == 'G'))
        return -1;
    id3_get_string(s->title, sizeof(s->title), buf + 3, 30);
    id3_get_string(s->author, sizeof(s->author), buf + 33, 30);
    id3_get_string(s->album, sizeof(s->album), buf + 63, 30);
    id3_get_string(str, sizeof(str), buf + 93, 4);
    s->year = atoi(str);
    id3_get_string(s->comment, sizeof(s->comment), buf + 97, 30);
    if (buf[125] == 0 && buf[126] != 0)
        s->track = buf[126];
    genre = buf[127];
    if (genre <= ID3_GENRE_MAX)
        pstrcpy(s->genre, sizeof(s->genre), id3_genre_str[genre]);
    return 0;
}

static void id3_create_tag(AVFormatContext *s, uint8_t *buf)
{
    int v, i;

    memset(buf, 0, ID3_TAG_SIZE); /* fail safe */
    buf[0] = 'T';
    buf[1] = 'A';
    buf[2] = 'G';
    strncpy(buf + 3, s->title, 30);
    strncpy(buf + 33, s->author, 30);
    strncpy(buf + 63, s->album, 30);
    v = s->year;
    if (v > 0) {
        for(i = 0;i < 4; i++) {
            buf[96 - i] = '0' + (v % 10);
            v = v / 10;
        }
    }
    strncpy(buf + 97, s->comment, 30);
    if (s->track != 0) {
        buf[125] = 0;
        buf[126] = s->track;
    }
    for(i = 0; i <= ID3_GENRE_MAX; i++) {
        if (!strcasecmp(s->genre, id3_genre_str[i])) {
            buf[127] = i;
            break;
        }
    }
}

static inline MPAHeaderInfo decode_header( uint32_t header )
{
    MPAHeaderInfo header_info;
    
    header_info.version = (header >> 19) & 3;
    if( header & (1<<20) ) {
        header_info.lsf = (header & (1<<19)) ? 0 : 1;
        header_info.mpeg25 = 0;
    } else {
        header_info.lsf = 1;
        header_info.mpeg25 = 1; 
    }
    
    header_info.layer = 4 - ((header >> 17) & 3);
    
    header_info.frequency = mpa_freq_tab[(header >> 10) & 3] >> (header_info.lsf + header_info.mpeg25);
    
    header_info.rate = mpa_bitrate_tab[header_info.lsf][header_info.layer - 1][(header >> 12) & 0xf];
    
    header_info.padding = (header >> 9) & 1;
    
    header_info.crc = (header >> 16) & 1;
    
    return header_info;
}

static inline int calculate_frame_length( MPAHeaderInfo header_info )
{
    int frame_length;
    
    switch( header_info.layer )
    {
    case 1:
        frame_length = (header_info.rate * 12000) / header_info.frequency;
        frame_length = (frame_length + header_info.padding) * 4;
        break;
    case 2:
        frame_length = (header_info.rate * 144000) / header_info.frequency;
        frame_length += header_info.padding;
        break;
    default:
    case 3:
        frame_length = (header_info.rate * 144000) / (header_info.frequency << header_info.lsf);
        frame_length += header_info.padding;
        break;
    }
    
    return frame_length;
}

static inline void print_header_info( MPAHeaderInfo header_info )
{
    printf( "MPAHeaderInfo version %d layer %d rate %d frequency %d padding %d crc %d\n",
        header_info.version, header_info.layer, header_info.rate, header_info.frequency, header_info.padding, header_info.crc );
}

#define CONSECUTIVE_MASK 0xfffe0c00

// Compare headers using version, layer and frequency mask
static inline int compare_headers( uint32_t one, uint32_t two )
{
    return (one & CONSECUTIVE_MASK) == (two & CONSECUTIVE_MASK);
}

#define CONSECUTIVE_FRAME_THRESHOLD 3

static int mp3_probe( AVProbeData *pd )
{
    uint8_t *pb, *end;

    pb = pd->buf;
    end = pb + pd->buf_size;

    // Step through probe buffer looking for potential frames
    while( pb + 4 <= end ) {
        uint32_t header = *pb << 24 | *(pb+1) << 16 | *(pb+2) << 8 | *(pb+3);
        
        if( ff_mpa_check_header( header ) == 0 ) {
            uint8_t *ppb;
            int frame_length;
            uint32_t consecutive_header;
            int consecutive_frames = 1;
            
            // Extract header information and calculate frame length
            frame_length = calculate_frame_length( decode_header( header ) );
            ppb = pb + frame_length;
            
            // Search for consecutive frames using frame length to step through probe buffer
            while( frame_length && ppb + 4 <= end && 
                compare_headers( header, ( consecutive_header = (*ppb << 24 | *(ppb+1) << 16 | *(ppb+2) << 8 | *(ppb+3) ) ) ) ) {
                ++consecutive_frames;

                // If threshold met, return max score
                if( consecutive_frames == CONSECUTIVE_FRAME_THRESHOLD ) {
                    return AVPROBE_SCORE_MAX;
                }

                frame_length = calculate_frame_length( decode_header( consecutive_header ) );
                ppb += frame_length;
            }
        }
        
        ++pb;
    }
    
    // Fallback on matching file extension
    if( match_ext( pd->filename, "mp2,mp3" ) ) {
        return 50;
    }

    return 0;
}

/* mp3 read */
static int mp3_read_header(AVFormatContext *s,
                           AVFormatParameters *ap)
{
    AVStream *st;
    uint8_t buf[ID3_TAG_SIZE];
    int len, ret, filesize;

    st = av_new_stream(s, 0);
    if (!st)
        return AVERROR_NOMEM;

    st->codec.codec_type = CODEC_TYPE_AUDIO;
    st->codec.codec_id = CODEC_ID_MP3;
    st->need_parsing = 1;
    
    /* try to get the TAG */
    if (!url_is_streamed(&s->pb)) {
        /* XXX: change that */
        filesize = url_fsize(&s->pb);
        if (filesize > 128) {
            url_fseek(&s->pb, filesize - 128, SEEK_SET);
            ret = get_buffer(&s->pb, buf, ID3_TAG_SIZE);
            if (ret == ID3_TAG_SIZE) {
                id3_parse_tag(s, buf);
            }
            url_fseek(&s->pb, 0, SEEK_SET);
        }
    }

    /* if ID3 header found, skip it */
    ret = get_buffer(&s->pb, buf, ID3_HEADER_SIZE);
    if (ret != ID3_HEADER_SIZE)
        return -1;
    if (id3_match(buf)) {
        /* skip ID3 header */
        len = ((buf[6] & 0x7f) << 21) |
            ((buf[7] & 0x7f) << 14) |
            ((buf[8] & 0x7f) << 7) |
            (buf[9] & 0x7f);
        url_fskip(&s->pb, len);
    } else {
        url_fseek(&s->pb, 0, SEEK_SET);
    }

    /* the parameters will be extracted from the compressed bitstream */
    return 0;
}

#define MP3_PACKET_SIZE 1024

static int mp3_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    int ret, size;
    //    AVStream *st = s->streams[0];
    
    size= MP3_PACKET_SIZE;

    ret= av_get_packet(&s->pb, pkt, size);

    pkt->stream_index = 0;
    if (ret <= 0) {
        return AVERROR_IO;
    }
    /* note: we need to modify the packet size here to handle the last
       packet */
    pkt->size = ret;
    return ret;
}

static int mp3_read_close(AVFormatContext *s)
{
    return 0;
}

#ifdef CONFIG_ENCODERS
/* simple formats */
static int mp3_write_header(struct AVFormatContext *s)
{
    return 0;
}

static int mp3_write_packet(struct AVFormatContext *s, AVPacket *pkt)
{
    put_buffer(&s->pb, pkt->data, pkt->size);
    put_flush_packet(&s->pb);
    return 0;
}

static int mp3_write_trailer(struct AVFormatContext *s)
{
    uint8_t buf[ID3_TAG_SIZE];

    /* write the id3 header */
    if (s->title[0] != '\0') {
        id3_create_tag(s, buf);
        put_buffer(&s->pb, buf, ID3_TAG_SIZE);
        put_flush_packet(&s->pb);
    }
    return 0;
}
#endif //CONFIG_ENCODERS

AVInputFormat mp3_iformat = {
    "mp3",
    "MPEG audio",
    0,
    mp3_probe,
    mp3_read_header,
    mp3_read_packet,
    mp3_read_close,
    .extensions = "mp2,mp3", /* XXX: use probe */
};

#ifdef CONFIG_ENCODERS
AVOutputFormat mp2_oformat = {
    "mp2",
    "MPEG audio layer 2",
    "audio/x-mpeg",
#ifdef CONFIG_MP3LAME
    "mp2",
#else
    "mp2,mp3",
#endif
    0,
    CODEC_ID_MP2,
    0,
    mp3_write_header,
    mp3_write_packet,
    mp3_write_trailer,
};

#ifdef CONFIG_MP3LAME
AVOutputFormat mp3_oformat = {
    "mp3",
    "MPEG audio layer 3",
    "audio/x-mpeg",
    "mp3",
    0,
    CODEC_ID_MP3,
    0,
    mp3_write_header,
    mp3_write_packet,
    mp3_write_trailer,
};
#endif
#endif //CONFIG_ENCODERS

int mp3_init(void)
{
    av_register_input_format(&mp3_iformat);
#ifdef CONFIG_ENCODERS
    av_register_output_format(&mp2_oformat);
#ifdef CONFIG_MP3LAME
    av_register_output_format(&mp3_oformat);
#endif    
#endif //CONFIG_ENCODERS
    return 0;
}
