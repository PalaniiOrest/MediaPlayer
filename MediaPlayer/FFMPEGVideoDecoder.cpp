#include "pch.h"
#include "FFMPEGVideoDecoder.h"

FFMPEGVideoDecoder::FFMPEGVideoDecoder()
{
}

FFMPEGVideoDecoder::~FFMPEGVideoDecoder()
{
}

void FFMPEGVideoDecoder::loadMedia(const std::wstring& mediaPath)
{
}

void FFMPEGVideoDecoder::decodeFrame(VideoFrame& frame)
{
}

void FFMPEGVideoDecoder::seekToTime(uint64_t timeInTicks)
{
}

void FFMPEGVideoDecoder::initializeHWDecoder()
{
}

AVPixelFormat FFMPEGVideoDecoder::getHWFormat(AVCodecContext* ctx, const AVPixelFormat* pix_fmts)
{
	return AVPixelFormat();
}
