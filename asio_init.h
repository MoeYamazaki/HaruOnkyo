#ifndef __AsioInit__
#define __AsioInit__

// name of the ASIO device to be used
#if WINDOWS
//	#define ASIO_DRIVER_NAME    "ASIO Multimedia Driver"
// Ç±Ç±Ç≈égÇ§A•D,D/AÇÃñºèÃÇãLèqÇ∑ÇÈ
//#define ASIO_DRIVER_NAME    "OCTA-CAPTURE"
#define ASIO_DRIVER_NAME    "ASIO MADIface USB"

#elif MAC
//	#define ASIO_DRIVER_NAME   	"Apple Sound Manager"
#define ASIO_DRIVER_NAME   	"ASIO Sample"
#endif

#define TEST_RUN_TIME  10.0		// run for 20 seconds
#define SAMPLE_RATE 48000.0

enum {
	// number of input and outputs supported by the host application
	// you can change these to higher or lower values
	kMaxInputChannels = 32,
	kMaxOutputChannels = 32
};

// internal data storage
typedef struct DriverInfo
{
	// ASIOInit()
	ASIODriverInfo driverInfo;

	// ASIOGetChannels()
	long           inputChannels;
	long           outputChannels;

	// ASIOGetBufferSize()
	long           minSize;
	long           maxSize;
	long           preferredSize;
	long           granularity;

	// ASIOGetSampleRate()
	ASIOSampleRate sampleRate;

	// ASIOOutputReady()
	bool           postOutput;

	// ASIOGetLatencies ()
	long           inputLatency;
	long           outputLatency;

	// ASIOCreateBuffers ()
	long inputBuffers;	// becomes number of actual created input buffers
	long outputBuffers;	// becomes number of actual created output buffers
	ASIOBufferInfo bufferInfos[kMaxInputChannels + kMaxOutputChannels]; // buffer info's

	// ASIOGetChannelInfo()
	ASIOChannelInfo channelInfos[kMaxInputChannels + kMaxOutputChannels]; // channel info's
	// The above two arrays share the same indexing, as the data in them are linked together

	// Information from ASIOGetSamplePosition()
	// data is converted to double floats for easier use, however 64 bit integer can be used, too
	double         nanoSeconds;
	double         samples;
	double         tcSamples;	// time code samples

	// bufferSwitchTimeInfo()
	ASIOTime       tInfo;			// time info state
	unsigned long  sysRefTime;      // system reference time, when bufferSwitch() was called

	// Signal the end of processing in this example
	bool           stopped;
} DriverInfo;


//----------------------------------------------------------------------------------
// conversion from 64 bit ASIOSample/ASIOTimeStamp to double float
#if NATIVE_INT64
#define ASIO64toDouble(a)  (a)
#else
const double twoRaisedTo32 = 4294967296.;
#define ASIO64toDouble(a)  ((a).lo + (a).hi * twoRaisedTo32)
#endif

long init_asio_static_data(DriverInfo* asioDriverInfo);
ASIOError create_asio_buffers(DriverInfo* asioDriverInfo);
unsigned long get_sys_reference_time();
void sampleRateChanged(ASIOSampleRate sRate);
long asioMessages(long selector, long value, void* message, double* opt);
//int set_asio(float sampling_freq, long data_length, int frame_size, int number_input_channels, int number_output_channels, DriverInfo* asioDriverInfo);
//bool asio_init(char* driver_name, DriverInfo* asioDriverInfo);
#endif