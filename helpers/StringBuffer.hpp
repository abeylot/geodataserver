#ifndef STRINGBUFFER_HPP
#define STRINGBUFFER_HPP
#define CONCATENATOR_LENGTH 0x10000
#include <cstdint>
#include <cstddef>
#include <string>


class StringBuffer
{

private:
    char buff_[CONCATENATOR_LENGTH]{};
    size_t bytesLeft_;
    std::string* pOut_;

public:
    StringBuffer(std::string& out)
    {
        pOut_ = &out;
        bytesLeft_ = CONCATENATOR_LENGTH;
    }

    StringBuffer& concat(const char* toConcat, size_t sizeToCopy);

    StringBuffer& concat(const std::string& strToConcat);

    void flush();

    virtual ~StringBuffer()
    {
    }
};

StringBuffer& operator<<(StringBuffer& sb, uint64_t l);
StringBuffer& operator<<(StringBuffer& sb, int64_t l);
StringBuffer& operator<<(StringBuffer& sb, uint32_t l);
StringBuffer& operator<<(StringBuffer& sb, int32_t l);
StringBuffer& operator<<(StringBuffer& sb, uint16_t l);
StringBuffer& operator<<(StringBuffer& sb, int16_t l);
StringBuffer& operator<<(StringBuffer& sb, uint8_t l);
StringBuffer& operator<<(StringBuffer& sb, int8_t l);
StringBuffer& operator<<(StringBuffer& sb, const char* s);
StringBuffer& operator<<(StringBuffer& sb, std::string s);
#endif
