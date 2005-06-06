#ifndef _WAVEFORMS_H_
#define _WAVEFORMS_H_

// -- waveform infos, ScopeMeter 90 series

#define FLUKE90_WF_QUERY_NUM        11

extern wxString fluke90wfQueries[FLUKE90_WF_QUERY_NUM];
extern wxString fluke90wfNames[FLUKE90_WF_QUERY_NUM];

// -- waveform infos, ScopeMeter 190 series

#define FLUKE190_WF_QUERY_NUM       9

extern wxString fluke190wfQueries[FLUKE190_WF_QUERY_NUM];
extern wxString fluke190wfNames[FLUKE190_WF_QUERY_NUM];

#define FLUKE190_WF_UNITSCOUNT      22

extern wxString fluke190wfUnits[FLUKE190_WF_UNITSCOUNT];

struct _fluke190wfTraceAdmin {
    unsigned char sfd1;    // '#'
    unsigned char sfd2;    // '0'
    unsigned char block_header;     // 144=header only, 0=header and samples
    unsigned int  block_length;
    union _trace_result {
        unsigned char byte;
        struct _bit {
            bool isTraceAq  : 1;
            bool isTrend    : 1;
            bool isEnvelope : 1;
            bool isReference: 1;
            bool isMaths    : 1;
        } bit;
    } trace_result;
    unsigned char y_unit;
    unsigned char x_unit;
    unsigned int  y_divisions;
    unsigned int  x_divisions;
    float         y_scale;
    float         x_scale;
    unsigned char y_step;
    unsigned char x_step;
    float         y_zero;
    float         x_zero;
    float         y_resolution;
    float         x_resolution;
    float         y_at_0;
    float         x_at_0;
    char          date_stamp[8]; // date: yyyymmdd, string not 0-terminated
    char          time_stamp[6]; // time: hhmmss, string not 0-terminated
    unsigned char checksum;
};

struct _fluke190wfTraceSamples {
    unsigned char sfd1;    // '#'
    unsigned char sfd2;    // '0'
    unsigned char block_header;     // always 129
    unsigned long block_length;
    union _sample_format {
        unsigned char byte;
        struct _bit {
            unsigned char samp_octetlenth : 3; // how many octets one sample_value consists of
            bool dummy : 1;
            unsigned char samp_compination : 3; // cf PDF reference
            bool samp_isSigned : 1;
        } bit;
    } sample_format;
    // (remainder of data must be decoded manually, depends on samp_octetlenth etc)

    // for filling out manually:
    long    overload;
    long    underload;
    long    invalid;
    unsigned int num_of_samples;
    union _samples {
        long * ldata;
        unsigned long * uldata;
        int * idata;
        unsigned int * uidata;
        char * bdata;
        unsigned char * ubdata;
    } samples;
    unsigned char checksum;
};



#endif // _WAVEFORMS_H_
