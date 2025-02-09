#pragma once
#include "MediaDecoder.h"
#include "MediaFile.h"

class PreviewMediaDecoder : public MediaDecoder
{
public:
	PreviewMediaDecoder();

	void getMedidaFileData(MediaFile& mediaFile);
	std::wstring GetMetadataAttribute(const GUID& key);

private:
    //resources to read meta data
};

