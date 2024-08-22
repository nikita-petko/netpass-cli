#include <np/base64.h>
#include <np/log_Macros.h>

namespace {

const char s_B64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

std::size_t
GetBase64EncodeSize(std::size_t inSize)
{
	std::size_t outSize = inSize;

	if (inSize % 3 != 0)
		outSize += 3 - (inSize % 3);

	outSize = (outSize / 3) * 4;

	return outSize;
}

}  // namespace

namespace np {

nn::Result
Base64Encode(char* pOut, size_t* outSize, const void* pIn, size_t inSize)
{
	if (pOut == NULL)
	{
		*outSize = GetBase64EncodeSize(inSize) + 1;

		return nn::ResultSuccess();
	}

	pOut[*outSize - 1] = '\0';

	const unsigned char* pInBytes = static_cast<const unsigned char*>(pIn);

	for (std::size_t i = 0, j = 0; i < inSize; i += 3, j += 4)
	{
		pOut[j + 0] = s_B64Chars[(pInBytes[i + 0] & 0xFC) >> 2];
		pOut[j + 1] = s_B64Chars[((pInBytes[i + 0] & 0x03) << 4) | ((pInBytes[i + 1] & 0xF0) >> 4)];
		pOut[j + 2] = s_B64Chars[((pInBytes[i + 1] & 0x0F) << 2) | ((pInBytes[i + 2] & 0xC0) >> 6)];
		pOut[j + 3] = s_B64Chars[pInBytes[i + 2] & 0x3F];
	}

	if (inSize % 3 == 1)
	{
		pOut[*outSize - 2] = '=';
		pOut[*outSize - 3] = '=';
	}
	else if (inSize % 3 == 2)
	{
		pOut[*outSize - 2] = '=';
	}

	return nn::ResultSuccess();
}

}  // namespace np
