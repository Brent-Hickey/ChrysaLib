#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <algorithm>
#include <iterator>

//////////////
// files utils
//////////////

void file_copy(const std::string &src, const std::string &dst)
{
	std::ifstream in(src, std::ios_base::in | std::ios_base::binary);
	std::ofstream out(dst, std::ios_base::out | std::ios_base::binary);
	char buf[4096];
	do
	{
		in.read(&buf[0], 4096);
		out.write(&buf[0], in.gcount());
	} while (in.gcount() > 0);
	in.close();
	out.close();
}

std::vector<uint8_t> gulp(const std::string &filename)
{
	std::ifstream file(filename, std::ios::binary);
	file.unsetf(std::ios::skipws);
	std::streampos file_size;
	file.seekg(0, std::ios::end);
	file_size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<uint8_t> vec;
	vec.reserve(file_size);
	vec.insert(vec.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());
	return vec;
}

///////////////
// string utils
///////////////

//split a string
std::vector<std::string> split_string(std::string str, const std::string &token)
{
	std::vector<std::string> result;
	while(str.size())
	{
		size_t index = str.find(token);
		if (index != std::string::npos)
		{
			result.push_back(str.substr(0, index));
			str = str.substr(index + token.size());
		}
		else
		{
			result.push_back(str);
			break;
		}
	}
	return result;
}

//join strings
std::string join(const std::vector<std::string> &strings, const std::string &join)
{
	auto result = std::string{};
	for (auto &s : strings)
	{
		result.append(s);
		result.append(join);
	}
	return result;
}

//trim from start (in place)
static inline void ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int32_t ch)
	{
		return !std::isspace(ch);
	}));
}

//trim from end (in place)
static inline void rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int32_t ch)
	{
		return !std::isspace(ch);
	}).base(), s.end());
}

//trim from both ends (in place)
static inline void trim(std::string &s)
{
	ltrim(s);
	rtrim(s);
}

//trim strings (in place)
std::vector<std::string> trim_strings(std::vector<std::string> &strings)
{
	for (auto &s : strings) trim(s);
	return strings;
}

std::string to_utf8(uint32_t c)
{
	std::string utf8;
	if (c >= 0x10000)
	{
		c = 0x808080f0 + (c >> 18)
			+ ((c >> 4) & 0x3f00)
			+ ((c << 10) & 0x3f0000)
			+ ((c << 24) & 0x3f000000);
		utf8.push_back((c >> 0) & 0xff);
		utf8.push_back((c >> 8) & 0xff);
		utf8.push_back((c >> 16) & 0xff);
		utf8.push_back((c >> 24) & 0xff);
		return utf8;
	}
	else if (c >= 0x800)
	{
		c = 0x8080e0 + (c >> 12)
			+ ((c << 2) & 0x3f00)
			+ ((c << 16) & 0x3f0000);
		utf8.push_back((c >> 0) & 0xff);
		utf8.push_back((c >> 8) & 0xff);
		utf8.push_back((c >> 16) & 0xff);
		return utf8;
	}
	else if (c >= 0x80)
	{
		c = 0x80c0 + (c >> 6)
			+ ((c << 8) & 0x3f00);
		utf8.push_back((c >> 0) & 0xff);
		utf8.push_back((c >> 8) & 0xff);
		return utf8;
	}
	utf8.push_back(c);
	return utf8;
}

// (defun num-to-utf8 (_)
// 	; (num-to-utf8 num) -> str
// 	(cond
// 		((>= _ 0x10000)
// 			(char (+ 0x808080f0 (>> _ 18) (logand (>> _ 4) 0x3f00)
// 				(logand (<< _ 10) 0x3f0000) (logand (<< _ 24) 0x3f000000)) 4))
// 		((>= _ 0x800)
// 			(char (+ 0x8080e0 (>> _ 12) (logand (<< _ 2) 0x3f00)
// 				(logand (<< _ 16) 0x3f0000)) 3))
// 		((>= _ 0x80)
// 			(char (+ 0x80c0 (>> _ 6) (logand (<< _ 8) 0x3f00)) 2))
// 		(t  (char _))))

uint32_t from_utf8(uint8_t **data)
{
	auto next_utf8 = [=] (uint8_t **data, int cnt, int c) -> int
	{
		c &= (0b00111111 >> cnt);
		for (auto i = 0; i != cnt; ++i)
		{
			auto m = *(*data)++;
			c <<= 6;
			m &= 0b00111111;
			c += m;
		}
		return c;
	};
	auto c = *(*data)++;
	auto m = c & 0b10000000;
	if (m != 0)
	{
		//not 1 byte
		m = c & 0b11100000;
		if (m != 0b11000000)
		{
			//not 2 byte
			m = c & 0b11110000;
			if (m != 0b11100000)
			{
				//not 3 byte
				m = c & 0b11111000;
				if (m != 0b11110000)
				{
					//not 4 byte
					return 0;
				}
				//4 byte
				return next_utf8(data, 3, c);
			}
			//3 byte
			return next_utf8(data, 2, c);
		}
		//2 byte
		return next_utf8(data, 1, c);
	}
	return c;
}

//////////
// hashing
//////////

//used for link buffer error detection
uint32_t jenkins_hash(const uint8_t *key, size_t len)
{
	uint32_t hash, i;
	for(hash = i = 0; i < len; ++i)
	{
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

//////////
// buffers
//////////

void obfuscate(uint8_t *key, size_t len)
{
	static const auto crypto_data = std::array<uint8_t, 1024>
	{
		0x52, 0x68, 0x68, 0xf5, 0xc7, 0x59, 0x23, 0x68, 0x30, 0x2e, 0x02, 0xa8, 0x43, 0x97, 0x6f, 0xb8,
		0x89, 0x97, 0x0d, 0x45, 0x81, 0x28, 0x42, 0x58, 0xf1, 0x1a, 0x5d, 0xee, 0x00, 0xb7, 0xbf, 0x41,
		0x65, 0x98, 0x57, 0x23, 0x1d, 0x2d, 0x45, 0x41, 0x2d, 0xcc, 0x31, 0x9b, 0x7b, 0x0f, 0x3b, 0xfd,
		0x9b, 0x81, 0x45, 0x50, 0xac, 0x49, 0x1c, 0xf9, 0x48, 0x48, 0x6b, 0x1d, 0x25, 0x1b, 0x32, 0x73,
		0x76, 0xb5, 0x94, 0x6b, 0xc1, 0x24, 0xe9, 0xd1, 0xfa, 0x61, 0xc4, 0xf0, 0x47, 0xba, 0xb1, 0x71,
		0x86, 0x29, 0x4e, 0x16, 0x24, 0x9a, 0x9c, 0x99, 0x38, 0x13, 0x1e, 0x9f, 0xbc, 0x81, 0x51, 0xbe,
		0x3f, 0x49, 0x99, 0xc0, 0x39, 0xef, 0xf8, 0x81, 0x0a, 0xff, 0x7f, 0x1a, 0xc6, 0xae, 0x6d, 0x94,
		0xb5, 0x1d, 0xa0, 0xee, 0x1d, 0x02, 0x4c, 0xb1, 0xa7, 0x80, 0xa7, 0x89, 0xe4, 0xd7, 0x8e, 0xfb,
		0x52, 0x3f, 0x0d, 0xba, 0x06, 0x81, 0x48, 0xe9, 0xdd, 0x83, 0x2b, 0x0c, 0xe6, 0xbb, 0xc6, 0xad,
		0x67, 0x2e, 0x21, 0x2c, 0x79, 0x0c, 0x4b, 0xf8, 0x05, 0x36, 0x39, 0x31, 0x18, 0xa1, 0x8d, 0x43,
		0x4d, 0x0d, 0x9d, 0x78, 0xa1, 0x2e, 0xb0, 0x0b, 0xd4, 0x45, 0x4f, 0x33, 0x0d, 0xda, 0xf5, 0x08,
		0x1f, 0x16, 0xd5, 0x02, 0xca, 0x6c, 0x28, 0x61, 0xf0, 0x5d, 0xfc, 0xf2, 0x24, 0x64, 0x23, 0xb7,
		0x39, 0x3a, 0x8a, 0x2d, 0xf2, 0x43, 0xac, 0x80, 0x7d, 0xd3, 0xda, 0x4e, 0x45, 0xe7, 0x21, 0x0f,
		0x2e, 0x5f, 0x6b, 0x7b, 0x1c, 0x73, 0xce, 0x3c, 0x30, 0x09, 0x91, 0x2b, 0xc0, 0xf0, 0xc4, 0x8d,
		0x7b, 0x82, 0x17, 0xb2, 0x69, 0x45, 0x9e, 0x1b, 0xfb, 0xb2, 0x8c, 0xd2, 0x54, 0x0b, 0xe0, 0x4c,
		0xa4, 0x49, 0xe4, 0x75, 0x47, 0x4a, 0xca, 0x3b, 0x35, 0x56, 0xba, 0x5e, 0xbd, 0xd9, 0xca, 0x44,
		0xb9, 0x23, 0xb0, 0x46, 0xca, 0x42, 0x13, 0x77, 0x8d, 0x01, 0x22, 0x06, 0x9c, 0x2d, 0xec, 0x89,
		0x5a, 0x87, 0xa9, 0x87, 0x25, 0x0a, 0x3f, 0x52, 0x07, 0x5b, 0xfc, 0xb8, 0x98, 0x2e, 0xa5, 0xbf,
		0xdf, 0xbc, 0x60, 0xc3, 0xef, 0xd6, 0x56, 0x65, 0xd5, 0xd5, 0x17, 0x25, 0xad, 0x64, 0x4b, 0xfc,
		0x5d, 0x7d, 0x52, 0xf8, 0x42, 0x83, 0x5a, 0xd9, 0x30, 0xa8, 0xff, 0xbb, 0xe6, 0xfb, 0xeb, 0x5e,
		0xdb, 0x87, 0xd5, 0xb5, 0x8f, 0x46, 0xf8, 0x57, 0xfb, 0x1d, 0x77, 0x2e, 0xf4, 0xaf, 0x4c, 0x40,
		0x53, 0xb5, 0x5c, 0x8f, 0x83, 0xe0, 0x24, 0x2e, 0x79, 0xb7, 0x60, 0x60, 0x99, 0x22, 0xb2, 0x4b,
		0x2c, 0xf5, 0x45, 0x03, 0x22, 0x0c, 0x17, 0x18, 0x35, 0x13, 0x97, 0x1c, 0xd0, 0x74, 0x52, 0x59,
		0x58, 0x97, 0x6e, 0xc0, 0x17, 0x11, 0xfe, 0x0d, 0x91, 0xba, 0x3f, 0x0b, 0xe8, 0x73, 0x55, 0xc5,
		0xd4, 0x59, 0x78, 0x5c, 0xa7, 0x82, 0x89, 0xe0, 0x2e, 0xab, 0xdc, 0x4a, 0xf3, 0x11, 0x13, 0x24,
		0xb1, 0xb9, 0xd3, 0x56, 0xe3, 0xba, 0x81, 0xf6, 0x52, 0xf2, 0xa6, 0xeb, 0x2a, 0x04, 0x86, 0xeb,
		0x06, 0xb5, 0xbd, 0xca, 0xd6, 0x9d, 0x42, 0x17, 0x41, 0xa5, 0xb0, 0x4b, 0xe4, 0x28, 0x2d, 0xad,
		0x12, 0xb3, 0x83, 0x9d, 0xa3, 0xb2, 0xf8, 0x04, 0xa7, 0x75, 0x8a, 0x1e, 0x3c, 0xd7, 0x9b, 0xc8,
		0x77, 0xfd, 0xf7, 0x22, 0xa6, 0x36, 0x02, 0xf5, 0xc2, 0xb3, 0x6e, 0x5e, 0xac, 0xea, 0x77, 0xa1,
		0x51, 0xcd, 0x1b, 0x08, 0x62, 0xeb, 0x69, 0xaa, 0x82, 0x00, 0x2c, 0xb0, 0x3a, 0x1c, 0x3b, 0x72,
		0xd6, 0xd0, 0xd5, 0x12, 0x93, 0xaf, 0x3d, 0xa8, 0xa5, 0xfc, 0x2f, 0x4d, 0x9b, 0x83, 0x91, 0x1a,
		0x6d, 0x91, 0x95, 0x08, 0x3b, 0x23, 0x66, 0x5b, 0x12, 0xd3, 0xcd, 0x05, 0xa2, 0xe2, 0xf4, 0xef,
		0xf2, 0x3e, 0xab, 0xa1, 0x23, 0x10, 0x3f, 0x63, 0x93, 0x97, 0xd9, 0x3d, 0xa0, 0x63, 0x41, 0x0b,
		0x12, 0x1f, 0xf7, 0x99, 0x8c, 0x8a, 0x5c, 0xb7, 0x71, 0x21, 0x4c, 0x11, 0xb5, 0x9f, 0x94, 0x9e,
		0xb8, 0x77, 0xb8, 0x73, 0x54, 0x1e, 0xa1, 0xaf, 0xbb, 0x43, 0x7e, 0x7c, 0xe6, 0x27, 0xdd, 0xc0,
		0xcd, 0x0b, 0x81, 0x72, 0x52, 0x7b, 0x9b, 0xd1, 0x9e, 0xd8, 0x89, 0x08, 0x03, 0x81, 0x0e, 0x30,
		0xad, 0xc9, 0xad, 0xcb, 0xa3, 0x0c, 0xbc, 0xa9, 0x6f, 0xf1, 0xe0, 0xec, 0x66, 0xf5, 0x17, 0xcd,
		0x03, 0x6e, 0x85, 0x56, 0xa5, 0xc1, 0x35, 0xd9, 0x7c, 0xa8, 0x5e, 0xb7, 0x40, 0x5c, 0x9a, 0xdb,
		0xa3, 0x3a, 0x86, 0x95, 0x15, 0x2e, 0x39, 0xf8, 0x24, 0xb2, 0x01, 0xdb, 0x5f, 0x79, 0x76, 0x04,
		0x00, 0xc4, 0x9b, 0x75, 0xe3, 0x79, 0x2f, 0xbc, 0xe9, 0xb5, 0x71, 0x0f, 0x5d, 0x71, 0x82, 0x02,
		0x77, 0x5a, 0xa0, 0x1f, 0x08, 0x41, 0x92, 0xef, 0x39, 0xed, 0x97, 0x8e, 0xc5, 0xda, 0xa0, 0xc7,
		0xe2, 0x94, 0xad, 0xfa, 0x3a, 0xbb, 0x7a, 0xdc, 0xe1, 0x51, 0x6f, 0x9f, 0xb0, 0x0e, 0x27, 0xf5,
		0x31, 0xff, 0x03, 0xd4, 0xdf, 0xc0, 0x74, 0xb9, 0x1e, 0x2a, 0xed, 0xea, 0x0b, 0x39, 0xa7, 0x03,
		0x8f, 0x31, 0xc5, 0x1e, 0xfc, 0x72, 0xad, 0x65, 0xe6, 0x9a, 0xe7, 0xea, 0xac, 0xf1, 0x38, 0xbb,
		0xa1, 0xea, 0x4c, 0xb6, 0x47, 0x85, 0x3d, 0x4a, 0xad, 0xc7, 0x01, 0xb1, 0x30, 0xf1, 0xc1, 0xc8,
		0xe5, 0xc3, 0x7e, 0xe6, 0xb6, 0x52, 0x33, 0xb4, 0x65, 0xef, 0xd2, 0xd4, 0xf4, 0x37, 0x16, 0xaf,
		0x92, 0x1e, 0x22, 0x6e, 0xc9, 0xde, 0x57, 0x49, 0x46, 0x2b, 0xd5, 0xd0, 0xb2, 0xce, 0xb7, 0x8b,
		0x49, 0x91, 0x86, 0x28, 0x68, 0xb7, 0x0b, 0xee, 0x46, 0x31, 0xc0, 0xb7, 0x48, 0x4b, 0xcf, 0x9d,
		0x14, 0x46, 0x7d, 0x93, 0x56, 0x3d, 0x4e, 0xcc, 0xb1, 0xfd, 0xf9, 0x16, 0x9c, 0xc8, 0xc4, 0xca,
		0xff, 0xf3, 0x9a, 0x00, 0x2d, 0x07, 0xf5, 0xe0, 0x4c, 0x48, 0x11, 0xa7, 0x72, 0xee, 0x75, 0xbe,
		0xfb, 0x32, 0xe6, 0x02, 0xf1, 0x0e, 0xa0, 0x0a, 0xbd, 0x38, 0x0f, 0x7e, 0x96, 0x60, 0x1a, 0x1f,
		0x12, 0xb7, 0x1f, 0x0d, 0xfa, 0xa3, 0x17, 0xf2, 0x3c, 0x18, 0x06, 0x22, 0xe0, 0x77, 0x08, 0xe6,
		0x62, 0x72, 0x07, 0xcf, 0x47, 0x85, 0xde, 0x38, 0x3c, 0xd3, 0x3d, 0x41, 0x25, 0xed, 0xb9, 0x43,
		0x75, 0x72, 0x57, 0xae, 0xc8, 0xa1, 0x67, 0xf3, 0x3d, 0xb0, 0xfa, 0x37, 0xef, 0x88, 0xee, 0xb6,
		0xc8, 0xe2, 0x38, 0xf2, 0x46, 0x39, 0x4b, 0x82, 0xce, 0x76, 0x8d, 0xc0, 0x64, 0x39, 0x83, 0xb7,
		0xcf, 0x3c, 0xfb, 0x77, 0x5a, 0x16, 0x0d, 0xeb, 0x95, 0x78, 0x01, 0x57, 0x00, 0x1b, 0x38, 0x77,
		0x63, 0x1c, 0x49, 0xd9, 0xd0, 0xfb, 0x37, 0x70, 0x4c, 0x44, 0xad, 0xae, 0xc1, 0x46, 0x99, 0xa1,
		0x40, 0xe4, 0x41, 0x58, 0x47, 0xda, 0xd2, 0x3a, 0xd4, 0x4f, 0x34, 0xef, 0x5b, 0x74, 0xe8, 0x74,
		0x08, 0x76, 0x44, 0xcb, 0x8a, 0xa0, 0x0f, 0xe9, 0x07, 0x16, 0x74, 0xc0, 0xbf, 0xbd, 0x86, 0xc8,
		0xb8, 0x52, 0x36, 0x12, 0x52, 0xfc, 0x9d, 0xc7, 0x3d, 0x01, 0xf2, 0x9f, 0x47, 0x29, 0x27, 0x0c,
		0xc6, 0x0e, 0x1d, 0x39, 0x6a, 0x2c, 0xb4, 0xfa, 0x34, 0x72, 0x0e, 0xfd, 0xa3, 0x62, 0x94, 0x8e,
		0x11, 0x38, 0xf2, 0xfb, 0x0a, 0x20, 0xc7, 0xd5, 0xb9, 0xa7, 0x0c, 0xcc, 0x77, 0xd7, 0xca, 0xb8,
		0x2e, 0xfa, 0x82, 0x1b, 0x28, 0x14, 0x2d, 0x55, 0x71, 0xab, 0xc9, 0x9d, 0xfe, 0xce, 0x01, 0xe7,
		0xfd, 0xb8, 0x0e, 0x39, 0x65, 0xd3, 0x8a, 0x12, 0x14, 0x9b, 0x11, 0x53, 0x1f, 0x14, 0x10, 0xe0
	};
	uint32_t i;
	for(i = 0; i < len; ++i) key[i] = key[i] ^ crypto_data[(len + i) & (sizeof(crypto_data) - 1)];
}
