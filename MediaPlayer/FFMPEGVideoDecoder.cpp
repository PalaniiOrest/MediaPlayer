#include "pch.h"
#include "FFMPEGVideoDecoder.h"
#include "Constants.h"

FFMPEGVideoDecoder::FFMPEGVideoDecoder(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
	, m_multithreadLock(deviceResources->getD3DMultithread())
{
	avformat_network_init();
}

FFMPEGVideoDecoder::~FFMPEGVideoDecoder()
{
	if (m_codecCtx) avcodec_free_context(&m_codecCtx);
	if (m_formatCtx) avformat_close_input(&m_formatCtx);
	if (m_hwDeviceCtx) av_buffer_unref(&m_hwDeviceCtx);
	if (m_swsCtx) sws_freeContext(m_swsCtx);
	if (m_hwFrame) av_frame_free(&m_hwFrame);
	if (m_swFrame) av_frame_free(&m_swFrame);
	if (m_bgraFrame) av_frame_free(&m_bgraFrame);

}

void FFMPEGVideoDecoder::loadMedia(const std::wstring& mediaPath)
{
	if (m_codecCtx) {
		avcodec_free_context(&m_codecCtx);
		m_codecCtx = nullptr;
	}

	if (m_formatCtx)
	{
		avformat_close_input(&m_formatCtx);
		m_formatCtx = nullptr;
	}

	if (m_hwDeviceCtx)
	{
		av_buffer_unref(&m_hwDeviceCtx);
		m_hwDeviceCtx = nullptr;
	}

	std::string path(mediaPath.begin(), mediaPath.end());

	if (avformat_open_input(&m_formatCtx, path.c_str(), nullptr, nullptr) < 0)
	{
		std::cerr << "Could not open file: " << path << std::endl;
		return;
	}

	if (avformat_find_stream_info(m_formatCtx, nullptr) < 0)
	{
		std::cerr << "Could not retrieve stream info." << std::endl;
		return;
	}

	for (unsigned i = 0; i < m_formatCtx->nb_streams; i++)
	{
		if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_videoStreamIndex = i;
			m_videoStream = m_formatCtx->streams[i];
			break;
		}
	}

	if (m_videoStreamIndex == -1)
	{
		std::cerr << "No video stream found." << std::endl;
		return;
	}

	m_codec = avcodec_find_decoder(m_videoStream->codecpar->codec_id);
	if (!m_codec)
	{
		std::cerr << "Codec not found." << std::endl;
		return;
	}

	m_codecCtx = avcodec_alloc_context3(m_codec);
	if (!m_codecCtx)
	{
		std::cerr << "Failed to allocate codec context." << std::endl;
		return;
	}

	if (avcodec_parameters_to_context(m_codecCtx, m_videoStream->codecpar) < 0)
	{
		std::cerr << "Failed to copy codec parameters." << std::endl;
		return;
	}

	m_codecCtx->thread_count = std::thread::hardware_concurrency();
	m_codecCtx->thread_type = FF_THREAD_FRAME | FF_THREAD_SLICE;

	initializeHWDecoder();

	if (avcodec_open2(m_codecCtx, m_codec, nullptr) < 0)
	{
		std::cerr << "Failed to open codec." << std::endl;
		return;
	}

	m_videoWidth = m_codecCtx->width;
	m_videoHeight = m_codecCtx->height;
	m_mediaDuration = m_videoStream->duration;

	AVRational frameRate = m_videoStream->avg_frame_rate;

	if (frameRate.num == 0 || frameRate.den == 0)
	{
		frameRate = m_videoStream->r_frame_rate;
	}

	if (frameRate.den != 0)
	{
		m_frameDuration = TICKS_PER_SECOND * frameRate.den / frameRate.num;
	}

	m_hwFrame = av_frame_alloc();
	m_swFrame = av_frame_alloc();
	m_bgraFrame = av_frame_alloc();

	m_swsCtx = sws_getContext(
		m_swFrame->width, m_swFrame->height, AV_PIX_FMT_NV12,
		m_swFrame->width, m_swFrame->height, AV_PIX_FMT_BGRA,
		SWS_BICUBIC | SWS_BITEXACT, nullptr, nullptr, nullptr);
	if (!m_swsCtx)
	{
		std::cerr << "Failed to create SwsContext for format conversion." << std::endl;
	}
}


void FFMPEGVideoDecoder::decodeFrame(VideoFrame& frame) {
	while (true)
	{
		if (av_read_frame(m_formatCtx, &m_packet) < 0)
		{
			m_isEndOfStream = true;
			return;
		}

		if (m_packet.stream_index == m_videoStreamIndex)
		{
			break; // Знайдено відеопакет, виходимо з циклу
		}

		av_packet_unref(&m_packet); // Пропускаємо небажані пакети
	}

	if (m_packet.stream_index == m_videoStreamIndex)
	{
		if (avcodec_send_packet(m_codecCtx, &m_packet) == 0) 
		{
			if (avcodec_receive_frame(m_codecCtx, m_hwFrame) == 0) 
			{

				if (m_hwFrame->format == AV_PIX_FMT_D3D11)
				{
					if (av_hwframe_transfer_data(m_swFrame, m_hwFrame, 0) < 0)
					{
						std::cerr << "Failed to transfer hardware frame to system memory." << std::endl;
						return;
					}

					if (!m_swsCtx || m_videoWidth != m_codecCtx->width || m_videoHeight != m_codecCtx->height) 
					{
						if (m_swsCtx)
						{
							sws_freeContext(m_swsCtx);
						}
						m_swsCtx = sws_getContext(m_videoWidth, m_videoHeight, AV_PIX_FMT_NV12,
							m_videoWidth, m_videoHeight, AV_PIX_FMT_BGRA,
							SWS_BICUBIC, nullptr, nullptr, nullptr);
						if (!m_swsCtx)
						{
							std::cerr << "Failed to create SwsContext for format conversion." << std::endl;
						}
					}

					m_bgraFrame->format = AV_PIX_FMT_BGRA;
					m_bgraFrame->width = m_swFrame->width;
					m_bgraFrame->height = m_swFrame->height;
					av_image_alloc(
						m_bgraFrame->data,
						m_bgraFrame->linesize,
						m_bgraFrame->width,
						m_bgraFrame->height,
						AV_PIX_FMT_BGRA,
						32);

					sws_scale(
						m_swsCtx,
						m_swFrame->data,
						m_swFrame->linesize,
						0,
						m_swFrame->height,
						m_bgraFrame->data,
						m_bgraFrame->linesize);

					D3D11_TEXTURE2D_DESC texDesc = {};
					texDesc.Width = m_bgraFrame->width;
					texDesc.Height = m_bgraFrame->height;
					texDesc.MipLevels = 1;
					texDesc.ArraySize = 1;
					texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
					texDesc.SampleDesc.Count = 1;
					texDesc.Usage = D3D11_USAGE_DEFAULT;
					texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

					D3D11_SUBRESOURCE_DATA initData = {};
					initData.pSysMem = m_bgraFrame->data[0];
					initData.SysMemPitch = m_bgraFrame->linesize[0];

					{
						std::lock_guard<D3D11MultithreadLock> lock(m_multithreadLock);

						winrt::com_ptr<ID3D11Texture2D> texPtr;
						HRESULT hr = m_deviceResources->getD3DDevice()->CreateTexture2D(&texDesc, &initData, texPtr.put());
						if (FAILED(hr))
						{
							std::cerr << "Failed to create D3D11Texture2D with BGRA format." << std::endl;
						}
						else
						{
							frame.setVideoData(texPtr);
						}
					}


					av_freep(&m_bgraFrame->data[0]);
				}
			}
		}
	}

	av_packet_unref(&m_packet);
}

void FFMPEGVideoDecoder::skipFrame(uint32_t numFrames)
{
	uint32_t skipped = 0;

	while (skipped < numFrames)
	{
		if (av_read_frame(m_formatCtx, &m_packet) < 0)
		{
			m_isEndOfStream = true;
			break;
		}

		if (m_packet.stream_index == m_videoStreamIndex) 
		{
			skipped++;
		}

		av_packet_unref(&m_packet);
	}

	avcodec_flush_buffers(m_codecCtx);
}

void FFMPEGVideoDecoder::seekToTime(uint64_t timeInTicks)
{
	int64_t timestamp = av_rescale_q(timeInTicks, { 1, AV_TIME_BASE }, m_videoStream->time_base);
	av_seek_frame(m_formatCtx, m_videoStreamIndex, timestamp, AVSEEK_FLAG_ANY);
	avcodec_flush_buffers(m_codecCtx);
}

void FFMPEGVideoDecoder::initializeHWDecoder()
{
	if (av_hwdevice_ctx_create(&m_hwDeviceCtx, AV_HWDEVICE_TYPE_D3D11VA, nullptr, nullptr, 0) < 0) {
		std::cerr << "Failed to create hardware decoder context." << std::endl;
		return;
	}

	m_codecCtx->hw_device_ctx = av_buffer_ref(m_hwDeviceCtx);
	m_codecCtx->get_format = [](AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts)
		{
			for (const enum AVPixelFormat* p = pix_fmts; *p != -1; p++)
			{
				if (*p == AV_PIX_FMT_D3D11)
				{
					return *p;
				}
			}
			return AV_PIX_FMT_NONE;
		};
}

