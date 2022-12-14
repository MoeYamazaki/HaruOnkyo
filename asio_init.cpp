#include <stdio.h>
#include <string.h>
#include "asiosys.h"
#include "asio.h"
#include "asiodrivers.h"
#include "asio_init.h"

ASIOCallbacks asioCallbacks;

//----------------------------------------------------------------------------------
long init_asio_static_data(DriverInfo* asioDriverInfo)
{	// collect the informational data of the driver
	// get the number of available channels
	if (ASIOGetChannels(&asioDriverInfo->inputChannels, &asioDriverInfo->outputChannels) == ASE_OK)
	{
		printf("ASIOGetChannels (inputs: %d, outputs: %d);\n", asioDriverInfo->inputChannels, asioDriverInfo->outputChannels);

		// get the usable buffer sizes
		if (ASIOGetBufferSize(&asioDriverInfo->minSize, &asioDriverInfo->maxSize, &asioDriverInfo->preferredSize, &asioDriverInfo->granularity) == ASE_OK)
		{
			printf("ASIOGetBufferSize (min: %d, max: %d, preferred: %d, granularity: %d);\n",
				asioDriverInfo->minSize, asioDriverInfo->maxSize,
				asioDriverInfo->preferredSize, asioDriverInfo->granularity);

			// get the currently selected sample rate
			if (ASIOGetSampleRate(&asioDriverInfo->sampleRate) == ASE_OK)
			{
				printf("ASIOGetSampleRate (sampleRate: %f);\n", asioDriverInfo->sampleRate);
				printf("ASIOcahnelInfor: %d);\n", (int)asioDriverInfo->channelInfos[1].type);
				if (asioDriverInfo->sampleRate <= 0.0 || asioDriverInfo->sampleRate > 96000.0)
				{
					// Driver does not store it's internal sample rate, so set it to a know one.
					// Usually you should check beforehand, that the selected sample rate is valid
					// with ASIOCanSampleRate().
					if (ASIOSetSampleRate(44100.0) == ASE_OK)
					{
						if (ASIOGetSampleRate(&asioDriverInfo->sampleRate) == ASE_OK)
							printf("ASIOGetSampleRate (sampleRate: %f);\n", asioDriverInfo->sampleRate);
						else
							return -6;
					}
					else
						return -5;
				}

				if (asioDriverInfo->sampleRate != SAMPLE_RATE)
				{
					// Driver does not store it's internal sample rate, so set it to a know one.
					// Usually you should check beforehand, that the selected sample rate is valid
					// with ASIOCanSampleRate().
					if (ASIOSetSampleRate(48000.0) == ASE_OK)
					{
						if (ASIOGetSampleRate(&asioDriverInfo->sampleRate) == ASE_OK) {
							printf("ASIOGetSampleRate (sampleRate: %f);\n", asioDriverInfo->sampleRate);

						}
						else
							return -6;
					}
					else
						return -5;
				}

				// check wether the driver requires the ASIOOutputReady() optimization
				// (can be used by the driver to reduce output latency by one block)
				if (ASIOOutputReady() == ASE_OK)
					asioDriverInfo->postOutput = true;
				else
					asioDriverInfo->postOutput = false;
				printf("ASIOOutputReady(); - %s\n", asioDriverInfo->postOutput ? "Supported" : "Not supported");

				return 0;
			}
			return -3;
		}
		return -2;
	}
	return -1;
}

//----------------------------------------------------------------------------------
ASIOError create_asio_buffers(DriverInfo* asioDriverInfo)
{	// create buffers for all inputs and outputs of the card with the 
	// preferredSize from ASIOGetBufferSize() as buffer size
	long i;
	ASIOError result;

	// fill the bufferInfos from the start without a gap
	ASIOBufferInfo* info = asioDriverInfo->bufferInfos;

	// prepare inputs (Though this is not necessaily required, no opened inputs will work, too
	if (asioDriverInfo->inputChannels > kMaxInputChannels)
		asioDriverInfo->inputBuffers = kMaxInputChannels;
	else
		asioDriverInfo->inputBuffers = asioDriverInfo->inputChannels;
	for (i = 0; i < asioDriverInfo->inputBuffers; i++, info++)
	{
		info->isInput = ASIOTrue;
		info->channelNum = i;
		info->buffers[0] = info->buffers[1] = 0;
	}

	// prepare outputs
	if (asioDriverInfo->outputChannels > kMaxOutputChannels)
		asioDriverInfo->outputBuffers = kMaxOutputChannels;
	else
		asioDriverInfo->outputBuffers = asioDriverInfo->outputChannels;
	for (i = 0; i < asioDriverInfo->outputBuffers; i++, info++)
	{
		info->isInput = ASIOFalse;
		info->channelNum = i;
		info->buffers[0] = info->buffers[1] = 0;
	}

	// create and activate buffers
	result = ASIOCreateBuffers(asioDriverInfo->bufferInfos,
		asioDriverInfo->inputBuffers + asioDriverInfo->outputBuffers,
		asioDriverInfo->preferredSize, &asioCallbacks);
	if (result == ASE_OK)
	{
		// now get all the buffer details, sample word length, name, word clock group and activation
		for (i = 0; i < asioDriverInfo->inputBuffers + asioDriverInfo->outputBuffers; i++)
		{
			asioDriverInfo->channelInfos[i].channel = asioDriverInfo->bufferInfos[i].channelNum;
			asioDriverInfo->channelInfos[i].isInput = asioDriverInfo->bufferInfos[i].isInput;
			result = ASIOGetChannelInfo(&asioDriverInfo->channelInfos[i]);
			if (result != ASE_OK)
				break;
		}

		if (result == ASE_OK)
		{
			// get the input and output latencies
			// Latencies often are only valid after ASIOCreateBuffers()
			// (input latency is the age of the first sample in the currently returned audio block)
			// (output latency is the time the first sample in the currently returned audio block requires to get to the output)
			result = ASIOGetLatencies(&asioDriverInfo->inputLatency, &asioDriverInfo->outputLatency);
			if (result == ASE_OK)
				printf("ASIOGetLatencies (input: %d, output: %d);\n", asioDriverInfo->inputLatency, asioDriverInfo->outputLatency);
			printf("ASIOcahnelInfor: %d);\n", (int)asioDriverInfo->channelInfos[1].type);
		}
	}
	return result;
}


unsigned long get_sys_reference_time()
{	// get the system reference time
#if WINDOWS
	return timeGetTime();
#elif MAC
	static const double twoRaisedTo32 = 4294967296.;
	UnsignedWide ys;
	Microseconds(&ys);
	double r = ((double)ys.hi * twoRaisedTo32 + (double)ys.lo);
	return (unsigned long)(r / 1000.);
#endif
}


void sampleRateChanged(ASIOSampleRate sRate)
{
	// do whatever you need to do if the sample rate changed
	// usually this only happens during external sync.
	// Audio processing is not stopped by the driver, actual sample rate
	// might not have even changed, maybe only the sample rate status of an
	// AES/EBU or S/PDIF digital input at the audio device.
	// You might have to update time/sample related conversion routines, etc.
}

//----------------------------------------------------------------------------------
long asioMessages(long selector, long value, void* message, double* opt)
{
	// currently the parameters "value", "message" and "opt" are not used.
	long ret = 0;
	switch (selector)
	{
	case kAsioSelectorSupported:
		if (value == kAsioResetRequest
			|| value == kAsioEngineVersion
			|| value == kAsioResyncRequest
			|| value == kAsioLatenciesChanged
			// the following three were added for ASIO 2.0, you don't necessarily have to support them
			|| value == kAsioSupportsTimeInfo
			|| value == kAsioSupportsTimeCode
			|| value == kAsioSupportsInputMonitor)
			ret = 1L;
		break;
	case kAsioResetRequest:
		// defer the task and perform the reset of the driver during the next "safe" situation
		// You cannot reset the driver right now, as this code is called from the driver.
		// Reset the driver is done by completely destruct is. I.e. ASIOStop(), ASIODisposeBuffers(), Destruction
		// Afterwards you initialize the driver again.
		// /*haneda del*/ asioDriverInfo.stopped;  // In this sample the processing will just stop
		ret = 1L;
		break;
	case kAsioResyncRequest:
		// This informs the application, that the driver encountered some non fatal data loss.
		// It is used for synchronization purposes of different media.
		// Added mainly to work around the Win16Mutex problems in Windows 95/98 with the
		// Windows Multimedia system, which could loose data because the Mutex was hold too long
		// by another thread.
		// However a driver can issue it in other situations, too.
		ret = 1L;
		break;
	case kAsioLatenciesChanged:
		// This will inform the host application that the drivers were latencies changed.
		// Beware, it this does not mean that the buffer sizes have changed!
		// You might need to update internal delay data.
		ret = 1L;
		break;
	case kAsioEngineVersion:
		// return the supported ASIO version of the host application
		// If a host applications does not implement this selector, ASIO 1.0 is assumed
		// by the driver
		ret = 2L;
		break;
	case kAsioSupportsTimeInfo:
		// informs the driver wether the asioCallbacks.bufferSwitchTimeInfo() callback
		// is supported.
		// For compatibility with ASIO 1.0 drivers the host application should always support
		// the "old" bufferSwitch method, too.
		ret = 1;
		break;
	case kAsioSupportsTimeCode:
		// informs the driver wether application is interested in time code info.
		// If an application does not need to know about time code, the driver has less work
		// to do.
		ret = 0;
		break;
	}
	return ret;
}

/*
bool asio_init(char* driver_name, DriverInfo* asioDriverInfo)
{
	
	if (ASIOInit(&asioDriverInfo->driverInfo) == ASE_OK)
	{

		printf("asioVersion:   %d\n"
			"driverVersion: %d\n"
			"Name:          %s\n"
			"ErrorMessage:  %s\n",
			asioDriverInfo->driverInfo.asioVersion, asioDriverInfo->driverInfo.driverVersion,
			asioDriverInfo->driverInfo.name, asioDriverInfo->driverInfo.errorMessage);
		if (init_asio_static_data(asioDriverInfo) == 0)
		{
			// ASIOControlPanel(); you might want to check wether the ASIOControlPanel() can open



			// set up the asioCallback structure and create the ASIO data buffer
			asioCallbacks.bufferSwitch = &bufferSwitch;
			//asioCallbacks.sampleRateDidChange = &sampleRateChanged;
			asioCallbacks.asioMessage = &asioMessages;
			asioCallbacks.bufferSwitchTimeInfo = &bufferSwitchTimeInfo;
			if (create_asio_buffers(asioDriverInfo) == ASE_OK) {
				fprintf(stdout, "\n Init ok !!!.\n\n");
				return true;
			}

		}
				ASIOStop();
		ASIODisposeBuffers();
		ASIOExit();
		fprintf(stdout, "\nASIO exit??? !!!.\n\n");
	}
	//asioDrivers->removeCurrentDriver();

return false;
	



}
*/
/*
int set_asio(float sampling_freq, long data_length, int frame_size, int number_input_channels, int number_output_channels, DriverInfo* asioDriverInfo)
{
	int i;
	ASIOBufferInfo* info = asioDriverInfo->bufferInfos;

	asioDriverInfo->sampleRate = (ASIOSampleRate)sampling_freq;
	asioDriverInfo->preferredSize = (long)frame_size;
	asioDriverInfo->inputChannels = (long)number_input_channels;
	asioDriverInfo->outputChannels = (long)number_output_channels;
	asioDriverInfo->inputBuffers = asioDriverInfo->inputChannels;
	asioDriverInfo->outputBuffers = asioDriverInfo->outputChannels;
	asioDriverInfo->stopped = false;
	//asioDriverInfo->dataLength = data_length;

	for (i = 0; i < asioDriverInfo->inputBuffers; i++, info++)
	{
		info->isInput = ASIOTrue;
		info->channelNum = i;
	}
	for (i = 0; i < asioDriverInfo->outputBuffers; i++, info++)
	{
		info->isInput = ASIOFalse;
		info->channelNum = i;
	}

	
	//if (asio_init(ASIO_DRIVER_NAME, asioDriverInfo) != true) {
	//	MessageBox(NULL, "can not initialize audio driver", "Error", MB_OK);
	//	return(0);
	//}
	

	return(1);
}
*/