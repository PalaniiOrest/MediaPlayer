#include "pch.h"
#include "SubtitleDecoder.h"
#include <fstream>
#include <regex>
#include "Constants.h"

SubtitleDecoder::SubtitleDecoder()
{
}

SubtitleDecoder::~SubtitleDecoder()
{
	avcodec_free_context(&m_codecCtx);
	avformat_close_input(&m_formatCtx);
}
void SubtitleDecoder::loadMedia(const MediaFile& media)
{
	std::string path(media.m_filePath.begin(), media.m_filePath.end());

	if (avformat_open_input(&m_formatCtx, path.c_str(), nullptr, nullptr) < 0)
	{
		std::cerr << "Failed to open media file!" << std::endl;
		return;
	}
	if (avformat_find_stream_info(m_formatCtx, nullptr) < 0)
	{
		std::cerr << "Failed to retrieve stream info!" << std::endl;
		return;
	}

	for (unsigned i = 0; i < m_formatCtx->nb_streams; i++)
	{
		if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE)
		{
			m_subtitleStreamIndex = i;
			break;
		}
	}

	if (m_subtitleStreamIndex != -1)
	{
		const AVCodec* codec = avcodec_find_decoder(m_formatCtx->streams[m_subtitleStreamIndex]->codecpar->codec_id);
		if (!codec)
		{
			std::cerr << "Failed to find a suitable subtitle decoder!" << std::endl;
			return;
		}

		m_codecCtx = avcodec_alloc_context3(codec);
		if (!m_codecCtx)
		{
			std::cerr << "Failed to allocate codec context!" << std::endl;
			return;
		}

		if (avcodec_parameters_to_context(m_codecCtx, m_formatCtx->streams[m_subtitleStreamIndex]->codecpar) < 0)
		{
			std::cerr << "Failed to copy codec parameters to decoder context!" << std::endl;
			avcodec_free_context(&m_codecCtx);
			return;
		}

		if (avcodec_open2(m_codecCtx, codec, nullptr) < 0)
		{
			std::cerr << "Failed to open subtitle codec!" << std::endl;
			avcodec_free_context(&m_codecCtx);
			return;
		}

		if (!(m_codecCtx->codec->capabilities & AV_CODEC_CAP_DR1))
		{
			std::cerr << "Warning: This subtitle codec may not support avcodec_send_packet!" << std::endl;
		}
	}
}

std::vector<SubtitleFrame> SubtitleDecoder::getDecodedSubtitles() {
	std::vector<SubtitleFrame> subtitles;
	if (m_subtitleStreamIndex == -1 || !m_codecCtx) return subtitles;

	AVPacket packet = { 0 };

	while (av_read_frame(m_formatCtx, &packet) >= 0) {
		if (packet.stream_index == m_subtitleStreamIndex) {
			std::cout << "Reading subtitle packet: size=" << packet.size
				<< " pts=" << packet.pts << " duration=" << packet.duration << std::endl;

			AVSubtitle subtitle;
			int got_subtitle = 0;

			if (avcodec_decode_subtitle2(m_codecCtx, &subtitle, &got_subtitle, &packet) < 0) {
				std::cerr << "Error decoding subtitle!" << std::endl;
				av_packet_unref(&packet);
				continue;
			}

			if (got_subtitle) {
				std::wstring text;

				for (unsigned i = 0; i < subtitle.num_rects; i++) {
					if (subtitle.rects[i] && subtitle.rects[i]->text) {
						std::string utf8_text(subtitle.rects[i]->text);
						text += std::wstring(utf8_text.begin(), utf8_text.end()) + L"\n";
					}
				}

				int64_t start_pts = (subtitle.pts == AV_NOPTS_VALUE) ? 0 : subtitle.pts;
				int64_t end_pts = (subtitle.end_display_time == AV_NOPTS_VALUE) ? start_pts + 1000 : start_pts + subtitle.end_display_time;

				std::wcout << L"Decoded subtitle: " << text << L" | Start: " << start_pts << L" End: " << end_pts << std::endl;

				subtitles.emplace_back(text, start_pts, end_pts);
			}

			avsubtitle_free(&subtitle);
		}

		av_packet_unref(&packet);
	}

	return subtitles;
}

std::vector<SubtitleFrame> SubtitleDecoder::getDecodedSubtitlesFromeSrt(const std::string& filePath)
{
	std::vector<SubtitleFrame> subtitles;
	std::ifstream file(filePath);
	if (!file.is_open()) 
	{
		std::cerr << "Failed to open SRT file!" << std::endl;
		return subtitles;
	}

	std::string line;
	std::string text;
	int64_t startPts = 0;
	int64_t endPts = 0;
	bool readingText = false;

	std::regex timePattern(R"((\d{2}):(\d{2}):(\d{2}),(\d{3}) --> (\d{2}):(\d{2}):(\d{2}),(\d{3}))");

	while (std::getline(file, line)) 
	{
		if (line.empty()) {
			if (!text.empty()) 
			{
				subtitles.emplace_back(std::wstring(text.begin(), text.end()), startPts, endPts);
				text.clear();
			}
			continue;
		}

		if (line.find("-->") != std::string::npos) 
		{
			std::smatch match;
			if (std::regex_match(line, match, timePattern)) 
			{
				int startHours = std::stoi(match[1].str());
				int startMinutes = std::stoi(match[2].str());
				int startSeconds = std::stoi(match[3].str());
				int startMilliseconds = std::stoi(match[4].str());
				int endHours = std::stoi(match[5].str());
				int endMinutes = std::stoi(match[6].str());
				int endSeconds = std::stoi(match[7].str());
				int endMilliseconds = std::stoi(match[8].str());

				startPts = (startHours * 3600 + startMinutes * 60 + startSeconds) + startMilliseconds;
				endPts = (endHours * 3600 + endMinutes * 60 + endSeconds) + endMilliseconds;
			}
		}
		else
		{
			if (!readingText) 
			{
				readingText = true;
			}
			text += line + "\n";
		}
	}

	if (!text.empty()) 
	{
		subtitles.emplace_back(std::wstring(text.begin(), text.end()), startPts, endPts);
	}

	return subtitles;
}
