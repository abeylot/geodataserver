#include "StringBuffer.hpp"
#define CONCATENATOR_LENGTH 0x10000

#include <string.h>

StringBuffer& StringBuffer::concat(const char* toConcat, size_t sizeToCopy)
{
    if(sizeToCopy > bytesLeft_) {
        pOut_->append(buff_, CONCATENATOR_LENGTH - bytesLeft_);
        pOut_->append(toConcat, sizeToCopy);
        bytesLeft_ = CONCATENATOR_LENGTH;
    } else {
        memcpy(buff_ + (CONCATENATOR_LENGTH - bytesLeft_), toConcat, sizeToCopy);
        bytesLeft_ -= sizeToCopy;
    }
    return *this;
}

StringBuffer& StringBuffer::concat(const std::string& strToConcat)
{
    const char* toConcat = strToConcat.c_str();
    size_t sizeToCopy = strToConcat.length();

    if(sizeToCopy > bytesLeft_) {
        pOut_->append(buff_, CONCATENATOR_LENGTH - bytesLeft_);
        pOut_->append(toConcat, sizeToCopy);
        bytesLeft_ = CONCATENATOR_LENGTH;
    } else {
        memcpy(buff_ + (CONCATENATOR_LENGTH - bytesLeft_), toConcat, sizeToCopy);
        bytesLeft_ -= sizeToCopy;
    }
    return *this;
}

void StringBuffer::flush()
{
    pOut_->append(buff_, CONCATENATOR_LENGTH - bytesLeft_);
}

StringBuffer& operator<<(StringBuffer& sb, uint64_t l)
{
    char buffer[32];
    char* p = buffer;
    int iRes = 20;

    if(l <= UINT64_C(9999999999)) {
        if(l <= UINT64_C(99999)) {
            if(l <= UINT64_C(9)) {
                iRes = 1;
                goto a1;
            }
            if(l <= UINT64_C(99)) {
                iRes = 2;
                goto a2;
            }
            if(l <= UINT64_C(999)) {
                iRes = 3;
                goto a3;
            }
            if(l <= UINT64_C(9999)) {
                iRes = 4;
                goto a4;
            }
            iRes = 5;
            goto a5;
        }

        if(l <= UINT64_C(999999)) {
            iRes = 6;
            goto a6;
        }
        if(l <= UINT64_C(9999999)) {
            iRes = 7;
            goto a7;
        }
        if(l <= UINT64_C(99999999)) {
            iRes = 8;
            goto a8;
        }
        if(l <= UINT64_C(999999999)) {
            iRes = 9;
            goto a9;
        }
        iRes = 10;
        goto a10;
    }

    if(l <= UINT64_C(999999999999999)) {
        if(l <= UINT64_C(99999999999)) {
            iRes = 11;
            goto a11;
        }
        if(l <= UINT64_C(999999999999)) {
            iRes = 12;
            goto a12;
        }
        if(l <= UINT64_C(9999999999999)) {
            iRes = 13;
            goto a13;
        }
        if(l <= UINT64_C(99999999999999)) {
            iRes = 14;
            goto a14;
        }
        iRes = 15;
        goto a15;
    }

    if(l <= UINT64_C(9999999999999999)) {
        iRes = 16;
        goto a16;
    }
    if(l <= UINT64_C(99999999999999999)) {
        iRes = 17;
        goto a17;
    }
    if(l <= UINT64_C(999999999999999999)) {
        iRes = 18;
        goto a18;
    }
    if(l <= UINT64_C(9999999999999999999)) {
        iRes = 19;
        goto a19;
    }

    // 20 chiffres

    *(p++) = '1';
    l -= UINT64_C(10000000000000000000);

    // 19 chiffres
a19:
    if(l < UINT64_C(5000000000000000000))
        goto a19_2;
    if(l > UINT64_C(8999999999999999999)) {
        *(p++) = '9';
        l -= UINT64_C(9000000000000000000);
        goto a18;
    }
    if(l > UINT64_C(7999999999999999999)) {
        *(p++) = '8';
        l -= UINT64_C(8000000000000000000);
        goto a18;
    }
    if(l > UINT64_C(6999999999999999999)) {
        *(p++) = '7';
        l -= UINT64_C(7000000000000000000);
        goto a18;
    }
    if(l > UINT64_C(5999999999999999999)) {
        *(p++) = '6';
        l -= UINT64_C(6000000000000000000);
        goto a18;
    }
    if(l > UINT64_C(4999999999999999999)) {
        *(p++) = '5';
        l -= UINT64_C(5000000000000000000);
        goto a18;
    }
a19_2:
    if(l > UINT64_C(3999999999999999999)) {
        *(p++) = '4';
        l -= UINT64_C(4000000000000000000);
        goto a18;
    }
    if(l > UINT64_C(2999999999999999999)) {
        *(p++) = '3';
        l -= UINT64_C(3000000000000000000);
        goto a18;
    }
    if(l > UINT64_C(1999999999999999999)) {
        *(p++) = '2';
        l -= UINT64_C(2000000000000000000);
        goto a18;
    }
    if(l > UINT64_C(999999999999999999)) {
        *(p++) = '1';
        l -= UINT64_C(1000000000000000000);
        goto a18;
    }
    *(p++) = '0';
    // 18 chiffres
a18:
    if(l < UINT64_C(500000000000000000))
        goto a18_2;
    if(l > UINT64_C(899999999999999999)) {
        *(p++) = '9';
        l -= UINT64_C(900000000000000000);
        goto a17;
    }
    if(l > UINT64_C(799999999999999999)) {
        *(p++) = '8';
        l -= UINT64_C(800000000000000000);
        goto a17;
    }
    if(l > UINT64_C(699999999999999999)) {
        *(p++) = '7';
        l -= UINT64_C(700000000000000000);
        goto a17;
    }
    if(l > UINT64_C(599999999999999999)) {
        *(p++) = '6';
        l -= UINT64_C(600000000000000000);
        goto a17;
    }
    if(l > UINT64_C(499999999999999999)) {
        *(p++) = '5';
        l -= UINT64_C(500000000000000000);
        goto a17;
    }
a18_2:
    if(l > UINT64_C(399999999999999999)) {
        *(p++) = '4';
        l -= UINT64_C(400000000000000000);
        goto a17;
    }
    if(l > UINT64_C(299999999999999999)) {
        *(p++) = '3';
        l -= UINT64_C(300000000000000000);
        goto a17;
    }
    if(l > UINT64_C(199999999999999999)) {
        *(p++) = '2';
        l -= UINT64_C(200000000000000000);
        goto a17;
    }
    if(l > UINT64_C(99999999999999999)) {
        *(p++) = '1';
        l -= UINT64_C(100000000000000000);
        goto a17;
    }
    *(p++) = '0';
    // 17 chiffres
a17:
    if(l < UINT64_C(50000000000000000))
        goto a17_2;
    if(l > UINT64_C(89999999999999999)) {
        *(p++) = '9';
        l -= UINT64_C(90000000000000000);
        goto a16;
    }
    if(l > UINT64_C(79999999999999999)) {
        *(p++) = '8';
        l -= UINT64_C(80000000000000000);
        goto a16;
    }
    if(l > UINT64_C(69999999999999999)) {
        *(p++) = '7';
        l -= UINT64_C(70000000000000000);
        goto a16;
    }
    if(l > UINT64_C(59999999999999999)) {
        *(p++) = '6';
        l -= UINT64_C(60000000000000000);
        goto a16;
    }
    if(l > UINT64_C(49999999999999999)) {
        *(p++) = '5';
        l -= UINT64_C(50000000000000000);
        goto a16;
    }
a17_2:
    if(l > UINT64_C(39999999999999999)) {
        *(p++) = '4';
        l -= UINT64_C(40000000000000000);
        goto a16;
    }
    if(l > UINT64_C(29999999999999999)) {
        *(p++) = '3';
        l -= UINT64_C(30000000000000000);
        goto a16;
    }
    if(l > UINT64_C(19999999999999999)) {
        *(p++) = '2';
        l -= UINT64_C(20000000000000000);
        goto a16;
    }
    if(l > UINT64_C(9999999999999999)) {
        *(p++) = '1';
        l -= UINT64_C(10000000000000000);
        goto a16;
    }
    *(p++) = '0';
    // 16 chiffres
a16:
    if(l < UINT64_C(5000000000000000))
        goto a16_2;
    if(l > UINT64_C(8999999999999999)) {
        *(p++) = '9';
        l -= UINT64_C(9000000000000000);
        goto a15;
    }
    if(l > UINT64_C(7999999999999999)) {
        *(p++) = '8';
        l -= UINT64_C(8000000000000000);
        goto a15;
    }
    if(l > UINT64_C(6999999999999999)) {
        *(p++) = '7';
        l -= UINT64_C(7000000000000000);
        goto a15;
    }
    if(l > UINT64_C(5999999999999999)) {
        *(p++) = '6';
        l -= UINT64_C(6000000000000000);
        goto a15;
    }
    if(l > UINT64_C(4999999999999999)) {
        *(p++) = '5';
        l -= UINT64_C(5000000000000000);
        goto a15;
    }
a16_2:
    if(l > UINT64_C(3999999999999999)) {
        *(p++) = '4';
        l -= UINT64_C(4000000000000000);
        goto a15;
    }
    if(l > UINT64_C(2999999999999999)) {
        *(p++) = '3';
        l -= UINT64_C(3000000000000000);
        goto a15;
    }
    if(l > UINT64_C(1999999999999999)) {
        *(p++) = '2';
        l -= UINT64_C(2000000000000000);
        goto a15;
    }
    if(l > UINT64_C(999999999999999)) {
        *(p++) = '1';
        l -= UINT64_C(1000000000000000);
        goto a15;
    }
    *(p++) = '0';
    // 15 chiffres
a15:
    if(l < UINT64_C(500000000000000))
        goto a15_2;
    if(l > UINT64_C(899999999999999)) {
        *(p++) = '9';
        l -= UINT64_C(900000000000000);
        goto a14;
    }
    if(l > UINT64_C(799999999999999)) {
        *(p++) = '8';
        l -= UINT64_C(800000000000000);
        goto a14;
    }
    if(l > UINT64_C(699999999999999)) {
        *(p++) = '7';
        l -= UINT64_C(700000000000000);
        goto a14;
    }
    if(l > UINT64_C(599999999999999)) {
        *(p++) = '6';
        l -= UINT64_C(600000000000000);
        goto a14;
    }
    if(l > UINT64_C(499999999999999)) {
        *(p++) = '5';
        l -= UINT64_C(500000000000000);
        goto a14;
    }
a15_2:
    if(l > UINT64_C(399999999999999)) {
        *(p++) = '4';
        l -= UINT64_C(400000000000000);
        goto a14;
    }
    if(l > UINT64_C(299999999999999)) {
        *(p++) = '3';
        l -= UINT64_C(300000000000000);
        goto a14;
    }
    if(l > UINT64_C(199999999999999)) {
        *(p++) = '2';
        l -= UINT64_C(200000000000000);
        goto a14;
    }
    if(l > UINT64_C(99999999999999)) {
        *(p++) = '1';
        l -= UINT64_C(100000000000000);
        goto a14;
    }
    *(p++) = '0';
    // 14 chiffres
a14:
    if(l < UINT64_C(50000000000000))
        goto a14_2;
    if(l > UINT64_C(89999999999999)) {
        *(p++) = '9';
        l -= UINT64_C(90000000000000);
        goto a13;
    }
    if(l > UINT64_C(79999999999999)) {
        *(p++) = '8';
        l -= UINT64_C(80000000000000);
        goto a13;
    }
    if(l > UINT64_C(69999999999999)) {
        *(p++) = '7';
        l -= UINT64_C(70000000000000);
        goto a13;
    }
    if(l > UINT64_C(59999999999999)) {
        *(p++) = '6';
        l -= UINT64_C(60000000000000);
        goto a13;
    }
    if(l > UINT64_C(49999999999999)) {
        *(p++) = '5';
        l -= UINT64_C(50000000000000);
        goto a13;
    }
a14_2:
    if(l > UINT64_C(39999999999999)) {
        *(p++) = '4';
        l -= UINT64_C(40000000000000);
        goto a13;
    }
    if(l > UINT64_C(29999999999999)) {
        *(p++) = '3';
        l -= UINT64_C(30000000000000);
        goto a13;
    }
    if(l > UINT64_C(19999999999999)) {
        *(p++) = '2';
        l -= UINT64_C(20000000000000);
        goto a13;
    }
    if(l > UINT64_C(9999999999999)) {
        *(p++) = '1';
        l -= UINT64_C(10000000000000);
        goto a13;
    }
    *(p++) = '0';
    // 13 chiffres
a13:
    if(l < UINT64_C(5000000000000))
        goto a13_2;
    if(l > UINT64_C(8999999999999)) {
        *(p++) = '9';
        l -= UINT64_C(9000000000000);
        goto a12;
    }
    if(l > UINT64_C(7999999999999)) {
        *(p++) = '8';
        l -= UINT64_C(8000000000000);
        goto a12;
    }
    if(l > UINT64_C(6999999999999)) {
        *(p++) = '7';
        l -= UINT64_C(7000000000000);
        goto a12;
    }
    if(l > UINT64_C(5999999999999)) {
        *(p++) = '6';
        l -= UINT64_C(6000000000000);
        goto a12;
    }
    if(l > UINT64_C(4999999999999)) {
        *(p++) = '5';
        l -= UINT64_C(5000000000000);
        goto a12;
    }
a13_2:
    if(l > UINT64_C(3999999999999)) {
        *(p++) = '4';
        l -= UINT64_C(4000000000000);
        goto a12;
    }
    if(l > UINT64_C(2999999999999)) {
        *(p++) = '3';
        l -= UINT64_C(3000000000000);
        goto a12;
    }
    if(l > UINT64_C(1999999999999)) {
        *(p++) = '2';
        l -= UINT64_C(2000000000000);
        goto a12;
    }
    if(l > UINT64_C(999999999999)) {
        *(p++) = '1';
        l -= UINT64_C(1000000000000);
        goto a12;
    }
    *(p++) = '0';
    // 12 chiffres
a12:
    if(l < UINT64_C(500000000000))
        goto a12_2;
    if(l > UINT64_C(899999999999)) {
        *(p++) = '9';
        l -= UINT64_C(900000000000);
        goto a11;
    }
    if(l > UINT64_C(799999999999)) {
        *(p++) = '8';
        l -= UINT64_C(800000000000);
        goto a11;
    }
    if(l > UINT64_C(699999999999)) {
        *(p++) = '7';
        l -= UINT64_C(700000000000);
        goto a11;
    }
    if(l > UINT64_C(599999999999)) {
        *(p++) = '6';
        l -= UINT64_C(600000000000);
        goto a11;
    }
    if(l > UINT64_C(499999999999)) {
        *(p++) = '5';
        l -= UINT64_C(500000000000);
        goto a11;
    }
a12_2:
    if(l > UINT64_C(399999999999)) {
        *(p++) = '4';
        l -= UINT64_C(400000000000);
        goto a11;
    }
    if(l > UINT64_C(299999999999)) {
        *(p++) = '3';
        l -= UINT64_C(300000000000);
        goto a11;
    }
    if(l > UINT64_C(199999999999)) {
        *(p++) = '2';
        l -= UINT64_C(200000000000);
        goto a11;
    }
    if(l > UINT64_C(99999999999)) {
        *(p++) = '1';
        l -= UINT64_C(100000000000);
        goto a11;
    }
    *(p++) = '0';
    // 11 chiffres
a11:
    if(l < UINT64_C(50000000000))
        goto a11_2;
    if(l > UINT64_C(89999999999)) {
        *(p++) = '9';
        l -= UINT64_C(90000000000);
        goto a10;
    }
    if(l > UINT64_C(79999999999)) {
        *(p++) = '8';
        l -= UINT64_C(80000000000);
        goto a10;
    }
    if(l > UINT64_C(69999999999)) {
        *(p++) = '7';
        l -= UINT64_C(70000000000);
        goto a10;
    }
    if(l > UINT64_C(59999999999)) {
        *(p++) = '6';
        l -= UINT64_C(60000000000);
        goto a10;
    }
    if(l > UINT64_C(49999999999)) {
        *(p++) = '5';
        l -= UINT64_C(50000000000);
        goto a10;
    }
a11_2:
    if(l > UINT64_C(39999999999)) {
        *(p++) = '4';
        l -= UINT64_C(40000000000);
        goto a10;
    }
    if(l > UINT64_C(29999999999)) {
        *(p++) = '3';
        l -= UINT64_C(30000000000);
        goto a10;
    }
    if(l > UINT64_C(19999999999)) {
        *(p++) = '2';
        l -= UINT64_C(20000000000);
        goto a10;
    }
    if(l > UINT64_C(9999999999)) {
        *(p++) = '1';
        l -= UINT64_C(10000000000);
        goto a10;
    }
    *(p++) = '0';
    // 10 chiffres
a10:
    if(l < UINT64_C(5000000000))
        goto a10_2;
    if(l > UINT64_C(8999999999)) {
        *(p++) = '9';
        l -= UINT64_C(9000000000);
        goto a9;
    }
    if(l > UINT64_C(7999999999)) {
        *(p++) = '8';
        l -= UINT64_C(8000000000);
        goto a9;
    }
    if(l > UINT64_C(6999999999)) {
        *(p++) = '7';
        l -= UINT64_C(7000000000);
        goto a9;
    }
    if(l > UINT64_C(5999999999)) {
        *(p++) = '6';
        l -= UINT64_C(6000000000);
        goto a9;
    }
    if(l > UINT64_C(4999999999)) {
        *(p++) = '5';
        l -= UINT64_C(5000000000);
        goto a9;
    }
a10_2:
    if(l > UINT64_C(3999999999)) {
        *(p++) = '4';
        l -= UINT64_C(4000000000);
        goto a9;
    }
    if(l > UINT64_C(2999999999)) {
        *(p++) = '3';
        l -= UINT64_C(3000000000);
        goto a9;
    }
    if(l > UINT64_C(1999999999)) {
        *(p++) = '2';
        l -= UINT64_C(2000000000);
        goto a9;
    }
    if(l > UINT64_C(999999999)) {
        *(p++) = '1';
        l -= UINT64_C(1000000000);
        goto a9;
    }
    *(p++) = '0';
    // 9 chiffres
a9:
    if(l < UINT64_C(500000000))
        goto a9_2;
    if(l > UINT64_C(899999999)) {
        *(p++) = '9';
        l -= UINT64_C(900000000);
        goto a8;
    }
    if(l > UINT64_C(799999999)) {
        *(p++) = '8';
        l -= UINT64_C(800000000);
        goto a8;
    }
    if(l > UINT64_C(699999999)) {
        *(p++) = '7';
        l -= UINT64_C(700000000);
        goto a8;
    }
    if(l > UINT64_C(599999999)) {
        *(p++) = '6';
        l -= UINT64_C(600000000);
        goto a8;
    }
    if(l > UINT64_C(499999999)) {
        *(p++) = '5';
        l -= UINT64_C(500000000);
        goto a8;
    }
a9_2:
    if(l > UINT64_C(399999999)) {
        *(p++) = '4';
        l -= UINT64_C(400000000);
        goto a8;
    }
    if(l > UINT64_C(299999999)) {
        *(p++) = '3';
        l -= UINT64_C(300000000);
        goto a8;
    }
    if(l > UINT64_C(199999999)) {
        *(p++) = '2';
        l -= UINT64_C(200000000);
        goto a8;
    }
    if(l > UINT64_C(99999999)) {
        *(p++) = '1';
        l -= UINT64_C(100000000);
        goto a8;
    }
    *(p++) = '0';
    // 8 chiffres
a8:
    if(l < UINT64_C(50000000))
        goto a8_2;
    if(l > UINT64_C(89999999)) {
        *(p++) = '9';
        l -= UINT64_C(90000000);
        goto a7;
    }
    if(l > UINT64_C(79999999)) {
        *(p++) = '8';
        l -= UINT64_C(80000000);
        goto a7;
    }
    if(l > UINT64_C(69999999)) {
        *(p++) = '7';
        l -= UINT64_C(70000000);
        goto a7;
    }
    if(l > UINT64_C(59999999)) {
        *(p++) = '6';
        l -= UINT64_C(60000000);
        goto a7;
    }
    if(l > UINT64_C(49999999)) {
        *(p++) = '5';
        l -= UINT64_C(50000000);
        goto a7;
    }
a8_2:
    if(l > UINT64_C(39999999)) {
        *(p++) = '4';
        l -= UINT64_C(40000000);
        goto a7;
    }
    if(l > UINT64_C(29999999)) {
        *(p++) = '3';
        l -= UINT64_C(30000000);
        goto a7;
    }
    if(l > UINT64_C(19999999)) {
        *(p++) = '2';
        l -= UINT64_C(20000000);
        goto a7;
    }
    if(l > UINT64_C(9999999)) {
        *(p++) = '1';
        l -= UINT64_C(10000000);
        goto a7;
    }
    *(p++) = '0';
    // 7 chiffres
a7:
    if(l < UINT64_C(5000000))
        goto a7_2;
    if(l > UINT64_C(8999999)) {
        *(p++) = '9';
        l -= UINT64_C(9000000);
        goto a6;
    }
    if(l > UINT64_C(7999999)) {
        *(p++) = '8';
        l -= UINT64_C(8000000);
        goto a6;
    }
    if(l > UINT64_C(6999999)) {
        *(p++) = '7';
        l -= UINT64_C(7000000);
        goto a6;
    }
    if(l > UINT64_C(5999999)) {
        *(p++) = '6';
        l -= UINT64_C(6000000);
        goto a6;
    }
    if(l > UINT64_C(4999999)) {
        *(p++) = '5';
        l -= UINT64_C(5000000);
        goto a6;
    }
a7_2:
    if(l > UINT64_C(3999999)) {
        *(p++) = '4';
        l -= UINT64_C(4000000);
        goto a6;
    }
    if(l > UINT64_C(2999999)) {
        *(p++) = '3';
        l -= UINT64_C(3000000);
        goto a6;
    }
    if(l > UINT64_C(1999999)) {
        *(p++) = '2';
        l -= UINT64_C(2000000);
        goto a6;
    }
    if(l > UINT64_C(999999)) {
        *(p++) = '1';
        l -= UINT64_C(1000000);
        goto a6;
    }
    *(p++) = '0';
    // 6 chiffres
a6:
    if(l < UINT64_C(500000))
        goto a6_2;
    if(l > UINT64_C(899999)) {
        *(p++) = '9';
        l -= UINT64_C(900000);
        goto a5;
    }
    if(l > UINT64_C(799999)) {
        *(p++) = '8';
        l -= UINT64_C(800000);
        goto a5;
    }
    if(l > UINT64_C(699999)) {
        *(p++) = '7';
        l -= UINT64_C(700000);
        goto a5;
    }
    if(l > UINT64_C(599999)) {
        *(p++) = '6';
        l -= UINT64_C(600000);
        goto a5;
    }
    if(l > UINT64_C(499999)) {
        *(p++) = '5';
        l -= UINT64_C(500000);
        goto a5;
    }
a6_2:
    if(l > UINT64_C(399999)) {
        *(p++) = '4';
        l -= UINT64_C(400000);
        goto a5;
    }
    if(l > UINT64_C(299999)) {
        *(p++) = '3';
        l -= UINT64_C(300000);
        goto a5;
    }
    if(l > UINT64_C(199999)) {
        *(p++) = '2';
        l -= UINT64_C(200000);
        goto a5;
    }
    if(l > UINT64_C(99999)) {
        *(p++) = '1';
        l -= UINT64_C(100000);
        goto a5;
    }
    *(p++) = '0';
    // 5 chiffres
a5:
    if(l < UINT64_C(50000))
        goto a5_2;
    if(l > UINT64_C(89999)) {
        *(p++) = '9';
        l -= UINT64_C(90000);
        goto a4;
    }
    if(l > UINT64_C(79999)) {
        *(p++) = '8';
        l -= UINT64_C(80000);
        goto a4;
    }
    if(l > UINT64_C(69999)) {
        *(p++) = '7';
        l -= UINT64_C(70000);
        goto a4;
    }
    if(l > UINT64_C(59999)) {
        *(p++) = '6';
        l -= UINT64_C(60000);
        goto a4;
    }
    if(l > UINT64_C(49999)) {
        *(p++) = '5';
        l -= UINT64_C(50000);
        goto a4;
    }
a5_2:
    if(l > UINT64_C(39999)) {
        *(p++) = '4';
        l -= UINT64_C(40000);
        goto a4;
    }
    if(l > UINT64_C(29999)) {
        *(p++) = '3';
        l -= UINT64_C(30000);
        goto a4;
    }
    if(l > UINT64_C(19999)) {
        *(p++) = '2';
        l -= UINT64_C(20000);
        goto a4;
    }
    if(l > UINT64_C(9999)) {
        *(p++) = '1';
        l -= UINT64_C(10000);
        goto a4;
    }
    *(p++) = '0';
    // 4 chiffres
a4:
    if(l < UINT64_C(5000))
        goto a4_2;
    if(l > UINT64_C(8999)) {
        *(p++) = '9';
        l -= UINT64_C(9000);
        goto a3;
    }
    if(l > UINT64_C(7999)) {
        *(p++) = '8';
        l -= UINT64_C(8000);
        goto a3;
    }
    if(l > UINT64_C(6999)) {
        *(p++) = '7';
        l -= UINT64_C(7000);
        goto a3;
    }
    if(l > UINT64_C(5999)) {
        *(p++) = '6';
        l -= UINT64_C(6000);
        goto a3;
    }
    if(l > UINT64_C(4999)) {
        *(p++) = '5';
        l -= UINT64_C(5000);
        goto a3;
    }
a4_2:
    if(l > UINT64_C(3999)) {
        *(p++) = '4';
        l -= UINT64_C(4000);
        goto a3;
    }
    if(l > UINT64_C(2999)) {
        *(p++) = '3';
        l -= UINT64_C(3000);
        goto a3;
    }
    if(l > UINT64_C(1999)) {
        *(p++) = '2';
        l -= UINT64_C(2000);
        goto a3;
    }
    if(l > UINT64_C(999)) {
        *(p++) = '1';
        l -= UINT64_C(1000);
        goto a3;
    }
    *(p++) = '0';
    // 3 chiffres
a3:
    if(l < UINT64_C(500))
        goto a3_2;
    if(l > UINT64_C(899)) {
        *(p++) = '9';
        l -= UINT64_C(900);
        goto a2;
    }
    if(l > UINT64_C(799)) {
        *(p++) = '8';
        l -= UINT64_C(800);
        goto a2;
    }
    if(l > UINT64_C(699)) {
        *(p++) = '7';
        l -= UINT64_C(700);
        goto a2;
    }
    if(l > UINT64_C(599)) {
        *(p++) = '6';
        l -= UINT64_C(600);
        goto a2;
    }
    if(l > UINT64_C(499)) {
        *(p++) = '5';
        l -= UINT64_C(500);
        goto a2;
    }
a3_2:
    if(l > UINT64_C(399)) {
        *(p++) = '4';
        l -= UINT64_C(400);
        goto a2;
    }
    if(l > UINT64_C(299)) {
        *(p++) = '3';
        l -= UINT64_C(300);
        goto a2;
    }
    if(l > UINT64_C(199)) {
        *(p++) = '2';
        l -= UINT64_C(200);
        goto a2;
    }
    if(l > UINT64_C(99)) {
        *(p++) = '1';
        l -= UINT64_C(100);
        goto a2;
    }
    *(p++) = '0';
    // 2 chiffres
a2:
    if(l < UINT64_C(50))
        goto a2_2;
    if(l > UINT64_C(89)) {
        *(p++) = '9';
        l -= UINT64_C(90);
        goto a1;
    }
    if(l > UINT64_C(79)) {
        *(p++) = '8';
        l -= UINT64_C(80);
        goto a1;
    }
    if(l > UINT64_C(69)) {
        *(p++) = '7';
        l -= UINT64_C(70);
        goto a1;
    }
    if(l > UINT64_C(59)) {
        *(p++) = '6';
        l -= UINT64_C(60);
        goto a1;
    }
    if(l > UINT64_C(49)) {
        *(p++) = '5';
        l -= UINT64_C(50);
        goto a1;
    }
a2_2:
    if(l > UINT64_C(39)) {
        *(p++) = '4';
        l -= UINT64_C(40);
        goto a1;
    }
    if(l > UINT64_C(29)) {
        *(p++) = '3';
        l -= UINT64_C(30);
        goto a1;
    }
    if(l > UINT64_C(19)) {
        *(p++) = '2';
        l -= UINT64_C(20);
        goto a1;
    }
    if(l > UINT64_C(9)) {
        *(p++) = '1';
        l -= UINT64_C(10);
        goto a1;
    }
    *(p++) = '0';
a1:
    *(p++) = '0' + l;
    *p = 0;

    return sb.concat(buffer, iRes);
}

StringBuffer& operator<<(StringBuffer& sb, int64_t l)
{
    char buffer[32];
    char* p = buffer;
    int iSign = 0;
    if(l < 0) {
        iSign = 1;
        *(p++) = '-';
        l = -l;
    }

    int iRes;

    if(l <= INT64_C(9999999999)) {
        if(l <= INT64_C(99999)) {
            if(l <= INT64_C(9)) {
                iRes = 1 + iSign;
                goto b1;
            }
            if(l <= INT64_C(99)) {
                iRes = 2 + iSign;
                goto b2;
            }
            if(l <= INT64_C(999)) {
                iRes = 3 + iSign;
                goto b3;
            }
            if(l <= INT64_C(9999)) {
                iRes = 4 + iSign;
                goto b4;
            }
            iRes = 5 + iSign;
            goto b5;
        }

        if(l <= INT64_C(999999)) {
            iRes = 6 + iSign;
            goto b6;
        }
        if(l <= INT64_C(9999999)) {
            iRes = 7 + iSign;
            goto b7;
        }
        if(l <= INT64_C(99999999)) {
            iRes = 8 + iSign;
            goto b8;
        }
        if(l <= INT64_C(999999999)) {
            iRes = 9 + iSign;
            goto b9;
        }
        iRes = 10 + iSign;
        goto b10;
    }

    if(l <= INT64_C(999999999999999)) {
        if(l <= INT64_C(99999999999)) {
            iRes = 11 + iSign;
            goto b11;
        }
        if(l <= INT64_C(999999999999)) {
            iRes = 12 + iSign;
            goto b12;
        }
        if(l <= INT64_C(9999999999999)) {
            iRes = 13 + iSign;
            goto b13;
        }
        if(l <= INT64_C(99999999999999)) {
            iRes = 14 + iSign;
            goto b14;
        }
        iRes = 15 + iSign;
        goto b15;
    }

    if(l <= INT64_C(9999999999999999)) {
        iRes = 16 + iSign;
        goto b16;
    }
    if(l <= INT64_C(99999999999999999)) {
        iRes = 17 + iSign;
        goto b17;
    }
    if(l <= INT64_C(999999999999999999)) {
        iRes = 18 + iSign;
        goto b18;
    }
    iRes = 19 + iSign;

    // 19 chiffres
    if(l < INT64_C(5000000000000000000))
        goto b19_2;
    if(l > INT64_C(8999999999999999999)) {
        *(p++) = '9';
        l -= INT64_C(9000000000000000000);
        goto b18;
    }
    if(l > INT64_C(7999999999999999999)) {
        *(p++) = '8';
        l -= INT64_C(8000000000000000000);
        goto b18;
    }
    if(l > INT64_C(6999999999999999999)) {
        *(p++) = '7';
        l -= INT64_C(7000000000000000000);
        goto b18;
    }
    if(l > INT64_C(5999999999999999999)) {
        *(p++) = '6';
        l -= INT64_C(6000000000000000000);
        goto b18;
    }
    if(l > INT64_C(4999999999999999999)) {
        *(p++) = '5';
        l -= INT64_C(5000000000000000000);
        goto b18;
    }
b19_2:
    if(l > INT64_C(3999999999999999999)) {
        *(p++) = '4';
        l -= INT64_C(4000000000000000000);
        goto b18;
    }
    if(l > INT64_C(2999999999999999999)) {
        *(p++) = '3';
        l -= INT64_C(3000000000000000000);
        goto b18;
    }
    if(l > INT64_C(1999999999999999999)) {
        *(p++) = '2';
        l -= INT64_C(2000000000000000000);
        goto b18;
    }
    if(l > INT64_C(999999999999999999)) {
        *(p++) = '1';
        l -= INT64_C(1000000000000000000);
        goto b18;
    }
    *(p++) = '0';
    // 18 chiffres
b18:
    if(l < INT64_C(500000000000000000))
        goto b18_2;
    if(l > INT64_C(899999999999999999)) {
        *(p++) = '9';
        l -= INT64_C(900000000000000000);
        goto b17;
    }
    if(l > INT64_C(799999999999999999)) {
        *(p++) = '8';
        l -= INT64_C(800000000000000000);
        goto b17;
    }
    if(l > INT64_C(699999999999999999)) {
        *(p++) = '7';
        l -= INT64_C(700000000000000000);
        goto b17;
    }
    if(l > INT64_C(599999999999999999)) {
        *(p++) = '6';
        l -= INT64_C(600000000000000000);
        goto b17;
    }
    if(l > INT64_C(499999999999999999)) {
        *(p++) = '5';
        l -= INT64_C(500000000000000000);
        goto b17;
    }
b18_2:
    if(l > INT64_C(399999999999999999)) {
        *(p++) = '4';
        l -= INT64_C(400000000000000000);
        goto b17;
    }
    if(l > INT64_C(299999999999999999)) {
        *(p++) = '3';
        l -= INT64_C(300000000000000000);
        goto b17;
    }
    if(l > INT64_C(199999999999999999)) {
        *(p++) = '2';
        l -= INT64_C(200000000000000000);
        goto b17;
    }
    if(l > INT64_C(99999999999999999)) {
        *(p++) = '1';
        l -= INT64_C(100000000000000000);
        goto b17;
    }
    *(p++) = '0';
    // 17 chiffres
b17:
    if(l < INT64_C(50000000000000000))
        goto b17_2;
    if(l > INT64_C(89999999999999999)) {
        *(p++) = '9';
        l -= INT64_C(90000000000000000);
        goto b16;
    }
    if(l > INT64_C(79999999999999999)) {
        *(p++) = '8';
        l -= INT64_C(80000000000000000);
        goto b16;
    }
    if(l > INT64_C(69999999999999999)) {
        *(p++) = '7';
        l -= INT64_C(70000000000000000);
        goto b16;
    }
    if(l > INT64_C(59999999999999999)) {
        *(p++) = '6';
        l -= INT64_C(60000000000000000);
        goto b16;
    }
    if(l > INT64_C(49999999999999999)) {
        *(p++) = '5';
        l -= INT64_C(50000000000000000);
        goto b16;
    }
b17_2:
    if(l > INT64_C(39999999999999999)) {
        *(p++) = '4';
        l -= INT64_C(40000000000000000);
        goto b16;
    }
    if(l > INT64_C(29999999999999999)) {
        *(p++) = '3';
        l -= INT64_C(30000000000000000);
        goto b16;
    }
    if(l > INT64_C(19999999999999999)) {
        *(p++) = '2';
        l -= INT64_C(20000000000000000);
        goto b16;
    }
    if(l > INT64_C(9999999999999999)) {
        *(p++) = '1';
        l -= INT64_C(10000000000000000);
        goto b16;
    }
    *(p++) = '0';
    // 16 chiffres
b16:
    if(l < INT64_C(5000000000000000))
        goto b16_2;
    if(l > INT64_C(8999999999999999)) {
        *(p++) = '9';
        l -= INT64_C(9000000000000000);
        goto b15;
    }
    if(l > INT64_C(7999999999999999)) {
        *(p++) = '8';
        l -= INT64_C(8000000000000000);
        goto b15;
    }
    if(l > INT64_C(6999999999999999)) {
        *(p++) = '7';
        l -= INT64_C(7000000000000000);
        goto b15;
    }
    if(l > INT64_C(5999999999999999)) {
        *(p++) = '6';
        l -= INT64_C(6000000000000000);
        goto b15;
    }
    if(l > INT64_C(4999999999999999)) {
        *(p++) = '5';
        l -= INT64_C(5000000000000000);
        goto b15;
    }
b16_2:
    if(l > INT64_C(3999999999999999)) {
        *(p++) = '4';
        l -= INT64_C(4000000000000000);
        goto b15;
    }
    if(l > INT64_C(2999999999999999)) {
        *(p++) = '3';
        l -= INT64_C(3000000000000000);
        goto b15;
    }
    if(l > INT64_C(1999999999999999)) {
        *(p++) = '2';
        l -= INT64_C(2000000000000000);
        goto b15;
    }
    if(l > INT64_C(999999999999999)) {
        *(p++) = '1';
        l -= INT64_C(1000000000000000);
        goto b15;
    }
    *(p++) = '0';
    // 15 chiffres
b15:
    if(l < INT64_C(500000000000000))
        goto b15_2;
    if(l > INT64_C(899999999999999)) {
        *(p++) = '9';
        l -= INT64_C(900000000000000);
        goto b14;
    }
    if(l > INT64_C(799999999999999)) {
        *(p++) = '8';
        l -= INT64_C(800000000000000);
        goto b14;
    }
    if(l > INT64_C(699999999999999)) {
        *(p++) = '7';
        l -= INT64_C(700000000000000);
        goto b14;
    }
    if(l > INT64_C(599999999999999)) {
        *(p++) = '6';
        l -= INT64_C(600000000000000);
        goto b14;
    }
    if(l > INT64_C(499999999999999)) {
        *(p++) = '5';
        l -= INT64_C(500000000000000);
        goto b14;
    }
b15_2:
    if(l > INT64_C(399999999999999)) {
        *(p++) = '4';
        l -= INT64_C(400000000000000);
        goto b14;
    }
    if(l > INT64_C(299999999999999)) {
        *(p++) = '3';
        l -= INT64_C(300000000000000);
        goto b14;
    }
    if(l > INT64_C(199999999999999)) {
        *(p++) = '2';
        l -= INT64_C(200000000000000);
        goto b14;
    }
    if(l > INT64_C(99999999999999)) {
        *(p++) = '1';
        l -= INT64_C(100000000000000);
        goto b14;
    }
    *(p++) = '0';
    // 14 chiffres
b14:
    if(l < INT64_C(50000000000000))
        goto b14_2;
    if(l > INT64_C(89999999999999)) {
        *(p++) = '9';
        l -= INT64_C(90000000000000);
        goto b13;
    }
    if(l > INT64_C(79999999999999)) {
        *(p++) = '8';
        l -= INT64_C(80000000000000);
        goto b13;
    }
    if(l > INT64_C(69999999999999)) {
        *(p++) = '7';
        l -= INT64_C(70000000000000);
        goto b13;
    }
    if(l > INT64_C(59999999999999)) {
        *(p++) = '6';
        l -= INT64_C(60000000000000);
        goto b13;
    }
    if(l > INT64_C(49999999999999)) {
        *(p++) = '5';
        l -= INT64_C(50000000000000);
        goto b13;
    }
b14_2:
    if(l > INT64_C(39999999999999)) {
        *(p++) = '4';
        l -= INT64_C(40000000000000);
        goto b13;
    }
    if(l > INT64_C(29999999999999)) {
        *(p++) = '3';
        l -= INT64_C(30000000000000);
        goto b13;
    }
    if(l > INT64_C(19999999999999)) {
        *(p++) = '2';
        l -= INT64_C(20000000000000);
        goto b13;
    }
    if(l > INT64_C(9999999999999)) {
        *(p++) = '1';
        l -= INT64_C(10000000000000);
        goto b13;
    }
    *(p++) = '0';
    // 13 chiffres
b13:
    if(l < INT64_C(5000000000000))
        goto b13_2;
    if(l > INT64_C(8999999999999)) {
        *(p++) = '9';
        l -= INT64_C(9000000000000);
        goto b12;
    }
    if(l > INT64_C(7999999999999)) {
        *(p++) = '8';
        l -= INT64_C(8000000000000);
        goto b12;
    }
    if(l > INT64_C(6999999999999)) {
        *(p++) = '7';
        l -= INT64_C(7000000000000);
        goto b12;
    }
    if(l > INT64_C(5999999999999)) {
        *(p++) = '6';
        l -= INT64_C(6000000000000);
        goto b12;
    }
    if(l > INT64_C(4999999999999)) {
        *(p++) = '5';
        l -= INT64_C(5000000000000);
        goto b12;
    }
b13_2:
    if(l > INT64_C(3999999999999)) {
        *(p++) = '4';
        l -= INT64_C(4000000000000);
        goto b12;
    }
    if(l > INT64_C(2999999999999)) {
        *(p++) = '3';
        l -= INT64_C(3000000000000);
        goto b12;
    }
    if(l > INT64_C(1999999999999)) {
        *(p++) = '2';
        l -= INT64_C(2000000000000);
        goto b12;
    }
    if(l > INT64_C(999999999999)) {
        *(p++) = '1';
        l -= INT64_C(1000000000000);
        goto b12;
    }
    *(p++) = '0';
    // 12 chiffres
b12:
    if(l < INT64_C(500000000000))
        goto b12_2;
    if(l > INT64_C(899999999999)) {
        *(p++) = '9';
        l -= INT64_C(900000000000);
        goto b11;
    }
    if(l > INT64_C(799999999999)) {
        *(p++) = '8';
        l -= INT64_C(800000000000);
        goto b11;
    }
    if(l > INT64_C(699999999999)) {
        *(p++) = '7';
        l -= INT64_C(700000000000);
        goto b11;
    }
    if(l > INT64_C(599999999999)) {
        *(p++) = '6';
        l -= INT64_C(600000000000);
        goto b11;
    }
    if(l > INT64_C(499999999999)) {
        *(p++) = '5';
        l -= INT64_C(500000000000);
        goto b11;
    }
b12_2:
    if(l > INT64_C(399999999999)) {
        *(p++) = '4';
        l -= INT64_C(400000000000);
        goto b11;
    }
    if(l > INT64_C(299999999999)) {
        *(p++) = '3';
        l -= INT64_C(300000000000);
        goto b11;
    }
    if(l > INT64_C(199999999999)) {
        *(p++) = '2';
        l -= INT64_C(200000000000);
        goto b11;
    }
    if(l > INT64_C(99999999999)) {
        *(p++) = '1';
        l -= INT64_C(100000000000);
        goto b11;
    }
    *(p++) = '0';
    // 11 chiffres
b11:
    if(l < INT64_C(50000000000))
        goto b11_2;
    if(l > INT64_C(89999999999)) {
        *(p++) = '9';
        l -= INT64_C(90000000000);
        goto b10;
    }
    if(l > INT64_C(79999999999)) {
        *(p++) = '8';
        l -= INT64_C(80000000000);
        goto b10;
    }
    if(l > INT64_C(69999999999)) {
        *(p++) = '7';
        l -= INT64_C(70000000000);
        goto b10;
    }
    if(l > INT64_C(59999999999)) {
        *(p++) = '6';
        l -= INT64_C(60000000000);
        goto b10;
    }
    if(l > INT64_C(49999999999)) {
        *(p++) = '5';
        l -= INT64_C(50000000000);
        goto b10;
    }
b11_2:
    if(l > INT64_C(39999999999)) {
        *(p++) = '4';
        l -= INT64_C(40000000000);
        goto b10;
    }
    if(l > INT64_C(29999999999)) {
        *(p++) = '3';
        l -= INT64_C(30000000000);
        goto b10;
    }
    if(l > INT64_C(19999999999)) {
        *(p++) = '2';
        l -= INT64_C(20000000000);
        goto b10;
    }
    if(l > INT64_C(9999999999)) {
        *(p++) = '1';
        l -= INT64_C(10000000000);
        goto b10;
    }
    *(p++) = '0';
    // 10 chiffres
b10:
    if(l < INT64_C(5000000000))
        goto b10_2;
    if(l > INT64_C(8999999999)) {
        *(p++) = '9';
        l -= INT64_C(9000000000);
        goto b9;
    }
    if(l > INT64_C(7999999999)) {
        *(p++) = '8';
        l -= INT64_C(8000000000);
        goto b9;
    }
    if(l > INT64_C(6999999999)) {
        *(p++) = '7';
        l -= INT64_C(7000000000);
        goto b9;
    }
    if(l > INT64_C(5999999999)) {
        *(p++) = '6';
        l -= INT64_C(6000000000);
        goto b9;
    }
    if(l > INT64_C(4999999999)) {
        *(p++) = '5';
        l -= INT64_C(5000000000);
        goto b9;
    }
b10_2:
    if(l > INT64_C(3999999999)) {
        *(p++) = '4';
        l -= INT64_C(4000000000);
        goto b9;
    }
    if(l > INT64_C(2999999999)) {
        *(p++) = '3';
        l -= INT64_C(3000000000);
        goto b9;
    }
    if(l > INT64_C(1999999999)) {
        *(p++) = '2';
        l -= INT64_C(2000000000);
        goto b9;
    }
    if(l > INT64_C(999999999)) {
        *(p++) = '1';
        l -= INT64_C(1000000000);
        goto b9;
    }
    *(p++) = '0';
    // 9 chiffres
b9:
    if(l < INT64_C(500000000))
        goto b9_2;
    if(l > INT64_C(899999999)) {
        *(p++) = '9';
        l -= INT64_C(900000000);
        goto b8;
    }
    if(l > INT64_C(799999999)) {
        *(p++) = '8';
        l -= INT64_C(800000000);
        goto b8;
    }
    if(l > INT64_C(699999999)) {
        *(p++) = '7';
        l -= INT64_C(700000000);
        goto b8;
    }
    if(l > INT64_C(599999999)) {
        *(p++) = '6';
        l -= INT64_C(600000000);
        goto b8;
    }
    if(l > INT64_C(499999999)) {
        *(p++) = '5';
        l -= INT64_C(500000000);
        goto b8;
    }
b9_2:
    if(l > INT64_C(399999999)) {
        *(p++) = '4';
        l -= INT64_C(400000000);
        goto b8;
    }
    if(l > INT64_C(299999999)) {
        *(p++) = '3';
        l -= INT64_C(300000000);
        goto b8;
    }
    if(l > INT64_C(199999999)) {
        *(p++) = '2';
        l -= INT64_C(200000000);
        goto b8;
    }
    if(l > INT64_C(99999999)) {
        *(p++) = '1';
        l -= INT64_C(100000000);
        goto b8;
    }
    *(p++) = '0';
    // 8 chiffres
b8:
    if(l < INT64_C(50000000))
        goto b8_2;
    if(l > INT64_C(89999999)) {
        *(p++) = '9';
        l -= INT64_C(90000000);
        goto b7;
    }
    if(l > INT64_C(79999999)) {
        *(p++) = '8';
        l -= INT64_C(80000000);
        goto b7;
    }
    if(l > INT64_C(69999999)) {
        *(p++) = '7';
        l -= INT64_C(70000000);
        goto b7;
    }
    if(l > INT64_C(59999999)) {
        *(p++) = '6';
        l -= INT64_C(60000000);
        goto b7;
    }
    if(l > INT64_C(49999999)) {
        *(p++) = '5';
        l -= INT64_C(50000000);
        goto b7;
    }
b8_2:
    if(l > INT64_C(39999999)) {
        *(p++) = '4';
        l -= INT64_C(40000000);
        goto b7;
    }
    if(l > INT64_C(29999999)) {
        *(p++) = '3';
        l -= INT64_C(30000000);
        goto b7;
    }
    if(l > INT64_C(19999999)) {
        *(p++) = '2';
        l -= INT64_C(20000000);
        goto b7;
    }
    if(l > INT64_C(9999999)) {
        *(p++) = '1';
        l -= INT64_C(10000000);
        goto b7;
    }
    *(p++) = '0';
    // 7 chiffres
b7:
    if(l < INT64_C(5000000))
        goto b7_2;
    if(l > INT64_C(8999999)) {
        *(p++) = '9';
        l -= INT64_C(9000000);
        goto b6;
    }
    if(l > INT64_C(7999999)) {
        *(p++) = '8';
        l -= INT64_C(8000000);
        goto b6;
    }
    if(l > INT64_C(6999999)) {
        *(p++) = '7';
        l -= INT64_C(7000000);
        goto b6;
    }
    if(l > INT64_C(5999999)) {
        *(p++) = '6';
        l -= INT64_C(6000000);
        goto b6;
    }
    if(l > INT64_C(4999999)) {
        *(p++) = '5';
        l -= INT64_C(5000000);
        goto b6;
    }
b7_2:
    if(l > INT64_C(3999999)) {
        *(p++) = '4';
        l -= INT64_C(4000000);
        goto b6;
    }
    if(l > INT64_C(2999999)) {
        *(p++) = '3';
        l -= INT64_C(3000000);
        goto b6;
    }
    if(l > INT64_C(1999999)) {
        *(p++) = '2';
        l -= INT64_C(2000000);
        goto b6;
    }
    if(l > INT64_C(999999)) {
        *(p++) = '1';
        l -= INT64_C(1000000);
        goto b6;
    }
    *(p++) = '0';
    // 6 chiffres
b6:
    if(l < INT64_C(500000))
        goto b6_2;
    if(l > INT64_C(899999)) {
        *(p++) = '9';
        l -= INT64_C(900000);
        goto b5;
    }
    if(l > INT64_C(799999)) {
        *(p++) = '8';
        l -= INT64_C(800000);
        goto b5;
    }
    if(l > INT64_C(699999)) {
        *(p++) = '7';
        l -= INT64_C(700000);
        goto b5;
    }
    if(l > INT64_C(599999)) {
        *(p++) = '6';
        l -= INT64_C(600000);
        goto b5;
    }
    if(l > INT64_C(499999)) {
        *(p++) = '5';
        l -= INT64_C(500000);
        goto b5;
    }
b6_2:
    if(l > INT64_C(399999)) {
        *(p++) = '4';
        l -= INT64_C(400000);
        goto b5;
    }
    if(l > INT64_C(299999)) {
        *(p++) = '3';
        l -= INT64_C(300000);
        goto b5;
    }
    if(l > INT64_C(199999)) {
        *(p++) = '2';
        l -= INT64_C(200000);
        goto b5;
    }
    if(l > INT64_C(99999)) {
        *(p++) = '1';
        l -= INT64_C(100000);
        goto b5;
    }
    *(p++) = '0';
    // 5 chiffres
b5:
    if(l < INT64_C(50000))
        goto b5_2;
    if(l > INT64_C(89999)) {
        *(p++) = '9';
        l -= INT64_C(90000);
        goto b4;
    }
    if(l > INT64_C(79999)) {
        *(p++) = '8';
        l -= INT64_C(80000);
        goto b4;
    }
    if(l > INT64_C(69999)) {
        *(p++) = '7';
        l -= INT64_C(70000);
        goto b4;
    }
    if(l > INT64_C(59999)) {
        *(p++) = '6';
        l -= INT64_C(60000);
        goto b4;
    }
    if(l > INT64_C(49999)) {
        *(p++) = '5';
        l -= INT64_C(50000);
        goto b4;
    }
b5_2:
    if(l > INT64_C(39999)) {
        *(p++) = '4';
        l -= INT64_C(40000);
        goto b4;
    }
    if(l > INT64_C(29999)) {
        *(p++) = '3';
        l -= INT64_C(30000);
        goto b4;
    }
    if(l > INT64_C(19999)) {
        *(p++) = '2';
        l -= INT64_C(20000);
        goto b4;
    }
    if(l > INT64_C(9999)) {
        *(p++) = '1';
        l -= INT64_C(10000);
        goto b4;
    }
    *(p++) = '0';
    // 4 chiffres
b4:
    if(l < INT64_C(5000))
        goto b4_2;
    if(l > INT64_C(8999)) {
        *(p++) = '9';
        l -= INT64_C(9000);
        goto b3;
    }
    if(l > INT64_C(7999)) {
        *(p++) = '8';
        l -= INT64_C(8000);
        goto b3;
    }
    if(l > INT64_C(6999)) {
        *(p++) = '7';
        l -= INT64_C(7000);
        goto b3;
    }
    if(l > INT64_C(5999)) {
        *(p++) = '6';
        l -= INT64_C(6000);
        goto b3;
    }
    if(l > INT64_C(4999)) {
        *(p++) = '5';
        l -= INT64_C(5000);
        goto b3;
    }
b4_2:
    if(l > INT64_C(3999)) {
        *(p++) = '4';
        l -= INT64_C(4000);
        goto b3;
    }
    if(l > INT64_C(2999)) {
        *(p++) = '3';
        l -= INT64_C(3000);
        goto b3;
    }
    if(l > INT64_C(1999)) {
        *(p++) = '2';
        l -= INT64_C(2000);
        goto b3;
    }
    if(l > INT64_C(999)) {
        *(p++) = '1';
        l -= INT64_C(1000);
        goto b3;
    }
    *(p++) = '0';
    // 3 chiffres
b3:
    if(l < INT64_C(500))
        goto b3_2;
    if(l > INT64_C(899)) {
        *(p++) = '9';
        l -= INT64_C(900);
        goto b2;
    }
    if(l > INT64_C(799)) {
        *(p++) = '8';
        l -= INT64_C(800);
        goto b2;
    }
    if(l > INT64_C(699)) {
        *(p++) = '7';
        l -= INT64_C(700);
        goto b2;
    }
    if(l > INT64_C(599)) {
        *(p++) = '6';
        l -= INT64_C(600);
        goto b2;
    }
    if(l > INT64_C(499)) {
        *(p++) = '5';
        l -= INT64_C(500);
        goto b2;
    }
b3_2:
    if(l > INT64_C(399)) {
        *(p++) = '4';
        l -= INT64_C(400);
        goto b2;
    }
    if(l > INT64_C(299)) {
        *(p++) = '3';
        l -= INT64_C(300);
        goto b2;
    }
    if(l > INT64_C(199)) {
        *(p++) = '2';
        l -= INT64_C(200);
        goto b2;
    }
    if(l > INT64_C(99)) {
        *(p++) = '1';
        l -= INT64_C(100);
        goto b2;
    }
    *(p++) = '0';
    // 2 chiffres
b2:
    if(l < INT64_C(50))
        goto b2_2;
    if(l > INT64_C(89)) {
        *(p++) = '9';
        l -= INT64_C(90);
        goto b1;
    }
    if(l > INT64_C(79)) {
        *(p++) = '8';
        l -= INT64_C(80);
        goto b1;
    }
    if(l > INT64_C(69)) {
        *(p++) = '7';
        l -= INT64_C(70);
        goto b1;
    }
    if(l > INT64_C(59)) {
        *(p++) = '6';
        l -= INT64_C(60);
        goto b1;
    }
    if(l > INT64_C(49)) {
        *(p++) = '5';
        l -= INT64_C(50);
        goto b1;
    }
b2_2:
    if(l > INT64_C(39)) {
        *(p++) = '4';
        l -= INT64_C(40);
        goto b1;
    }
    if(l > INT64_C(29)) {
        *(p++) = '3';
        l -= INT64_C(30);
        goto b1;
    }
    if(l > INT64_C(19)) {
        *(p++) = '2';
        l -= INT64_C(20);
        goto b1;
    }
    if(l > INT64_C(9)) {
        *(p++) = '1';
        l -= INT64_C(10);
        goto b1;
    }
    *(p++) = '0';
b1:
    *(p++) = '0' + l;
    *p = 0;

    return sb.concat(buffer, iRes);
}

StringBuffer& operator<<(StringBuffer& sb, uint32_t l)
{
    char buffer[32];
    char* p = buffer;
    int iRes;

    if(l <= UINT32_C(99999)) {
        if(l <= UINT32_C(9)) {
            iRes = 1;
            goto c1;
        }
        if(l <= UINT32_C(99)) {
            iRes = 2;
            goto c2;
        }
        if(l <= UINT32_C(999)) {
            iRes = 3;
            goto c3;
        }
        if(l <= UINT32_C(9999)) {
            iRes = 4;
            goto c4;
        }
        iRes = 5;
        goto c5;
    }

    if(l <= UINT32_C(999999)) {
        iRes = 6;
        goto c6;
    }
    if(l <= UINT32_C(9999999)) {
        iRes = 7;
        goto c7;
    }
    if(l <= UINT32_C(99999999)) {
        iRes = 8;
        goto c8;
    }
    if(l <= UINT32_C(999999999)) {
        iRes = 9;
        goto c9;
    }
    iRes = 10;
    

    // 10 chiffres
    if(l > UINT32_C(3999999999)) {
        *(p++) = '4';
        l -= UINT32_C(4000000000);
        goto c9;
    }
    if(l > UINT32_C(2999999999)) {
        *(p++) = '3';
        l -= UINT32_C(3000000000);
        goto c9;
    }
    if(l > UINT32_C(1999999999)) {
        *(p++) = '2';
        l -= UINT32_C(2000000000);
        goto c9;
    }
    if(l > UINT32_C(999999999)) {
        *(p++) = '1';
        l -= UINT32_C(1000000000);
        goto c9;
    }
    *(p++) = '0';
    // 9 chiffres
c9:
    if(l < UINT32_C(500000000))
        goto c9_2;
    if(l > UINT32_C(899999999)) {
        *(p++) = '9';
        l -= UINT32_C(900000000);
        goto c8;
    }
    if(l > UINT32_C(799999999)) {
        *(p++) = '8';
        l -= UINT32_C(800000000);
        goto c8;
    }
    if(l > UINT32_C(699999999)) {
        *(p++) = '7';
        l -= UINT32_C(700000000);
        goto c8;
    }
    if(l > UINT32_C(599999999)) {
        *(p++) = '6';
        l -= UINT32_C(600000000);
        goto c8;
    }
    if(l > UINT32_C(499999999)) {
        *(p++) = '5';
        l -= UINT32_C(500000000);
        goto c8;
    }
c9_2:
    if(l > UINT32_C(399999999)) {
        *(p++) = '4';
        l -= UINT32_C(400000000);
        goto c8;
    }
    if(l > UINT32_C(299999999)) {
        *(p++) = '3';
        l -= UINT32_C(300000000);
        goto c8;
    }
    if(l > UINT32_C(199999999)) {
        *(p++) = '2';
        l -= UINT32_C(200000000);
        goto c8;
    }
    if(l > UINT32_C(99999999)) {
        *(p++) = '1';
        l -= UINT32_C(100000000);
        goto c8;
    }
    *(p++) = '0';
    // 8 chiffres
c8:
    if(l < UINT32_C(50000000))
        goto c8_2;
    if(l > UINT32_C(89999999)) {
        *(p++) = '9';
        l -= UINT32_C(90000000);
        goto c7;
    }
    if(l > UINT32_C(79999999)) {
        *(p++) = '8';
        l -= UINT32_C(80000000);
        goto c7;
    }
    if(l > UINT32_C(69999999)) {
        *(p++) = '7';
        l -= UINT32_C(70000000);
        goto c7;
    }
    if(l > UINT32_C(59999999)) {
        *(p++) = '6';
        l -= UINT32_C(60000000);
        goto c7;
    }
    if(l > UINT32_C(49999999)) {
        *(p++) = '5';
        l -= UINT32_C(50000000);
        goto c7;
    }
c8_2:
    if(l > UINT32_C(39999999)) {
        *(p++) = '4';
        l -= UINT32_C(40000000);
        goto c7;
    }
    if(l > UINT32_C(29999999)) {
        *(p++) = '3';
        l -= UINT32_C(30000000);
        goto c7;
    }
    if(l > UINT32_C(19999999)) {
        *(p++) = '2';
        l -= UINT32_C(20000000);
        goto c7;
    }
    if(l > UINT32_C(9999999)) {
        *(p++) = '1';
        l -= UINT32_C(10000000);
        goto c7;
    }
    *(p++) = '0';
    // 7 chiffres
c7:
    if(l < UINT32_C(5000000))
        goto c7_2;
    if(l > UINT32_C(8999999)) {
        *(p++) = '9';
        l -= UINT32_C(9000000);
        goto c6;
    }
    if(l > UINT32_C(7999999)) {
        *(p++) = '8';
        l -= UINT32_C(8000000);
        goto c6;
    }
    if(l > UINT32_C(6999999)) {
        *(p++) = '7';
        l -= UINT32_C(7000000);
        goto c6;
    }
    if(l > UINT32_C(5999999)) {
        *(p++) = '6';
        l -= UINT32_C(6000000);
        goto c6;
    }
    if(l > UINT32_C(4999999)) {
        *(p++) = '5';
        l -= UINT32_C(5000000);
        goto c6;
    }
c7_2:
    if(l > UINT32_C(3999999)) {
        *(p++) = '4';
        l -= UINT32_C(4000000);
        goto c6;
    }
    if(l > UINT32_C(2999999)) {
        *(p++) = '3';
        l -= UINT32_C(3000000);
        goto c6;
    }
    if(l > UINT32_C(1999999)) {
        *(p++) = '2';
        l -= UINT32_C(2000000);
        goto c6;
    }
    if(l > UINT32_C(999999)) {
        *(p++) = '1';
        l -= UINT32_C(1000000);
        goto c6;
    }
    *(p++) = '0';
    // 6 chiffres
c6:
    if(l < UINT32_C(500000))
        goto c6_2;
    if(l > UINT32_C(899999)) {
        *(p++) = '9';
        l -= UINT32_C(900000);
        goto c5;
    }
    if(l > UINT32_C(799999)) {
        *(p++) = '8';
        l -= UINT32_C(800000);
        goto c5;
    }
    if(l > UINT32_C(699999)) {
        *(p++) = '7';
        l -= UINT32_C(700000);
        goto c5;
    }
    if(l > UINT32_C(599999)) {
        *(p++) = '6';
        l -= UINT32_C(600000);
        goto c5;
    }
    if(l > UINT32_C(499999)) {
        *(p++) = '5';
        l -= UINT32_C(500000);
        goto c5;
    }
c6_2:
    if(l > UINT32_C(399999)) {
        *(p++) = '4';
        l -= UINT32_C(400000);
        goto c5;
    }
    if(l > UINT32_C(299999)) {
        *(p++) = '3';
        l -= UINT32_C(300000);
        goto c5;
    }
    if(l > UINT32_C(199999)) {
        *(p++) = '2';
        l -= UINT32_C(200000);
        goto c5;
    }
    if(l > UINT32_C(99999)) {
        *(p++) = '1';
        l -= UINT32_C(100000);
        goto c5;
    }
    *(p++) = '0';
    // 5 chiffres
c5:
    if(l < UINT32_C(50000))
        goto c5_2;
    if(l > UINT32_C(89999)) {
        *(p++) = '9';
        l -= UINT32_C(90000);
        goto c4;
    }
    if(l > UINT32_C(79999)) {
        *(p++) = '8';
        l -= UINT32_C(80000);
        goto c4;
    }
    if(l > UINT32_C(69999)) {
        *(p++) = '7';
        l -= UINT32_C(70000);
        goto c4;
    }
    if(l > UINT32_C(59999)) {
        *(p++) = '6';
        l -= UINT32_C(60000);
        goto c4;
    }
    if(l > UINT32_C(49999)) {
        *(p++) = '5';
        l -= UINT32_C(50000);
        goto c4;
    }
c5_2:
    if(l > UINT32_C(39999)) {
        *(p++) = '4';
        l -= UINT32_C(40000);
        goto c4;
    }
    if(l > UINT32_C(29999)) {
        *(p++) = '3';
        l -= UINT32_C(30000);
        goto c4;
    }
    if(l > UINT32_C(19999)) {
        *(p++) = '2';
        l -= UINT32_C(20000);
        goto c4;
    }
    if(l > UINT32_C(9999)) {
        *(p++) = '1';
        l -= UINT32_C(10000);
        goto c4;
    }
    *(p++) = '0';
    // 4 chiffres
c4:
    if(l < UINT32_C(5000))
        goto c4_2;
    if(l > UINT32_C(8999)) {
        *(p++) = '9';
        l -= UINT32_C(9000);
        goto c3;
    }
    if(l > UINT32_C(7999)) {
        *(p++) = '8';
        l -= UINT32_C(8000);
        goto c3;
    }
    if(l > UINT32_C(6999)) {
        *(p++) = '7';
        l -= UINT32_C(7000);
        goto c3;
    }
    if(l > UINT32_C(5999)) {
        *(p++) = '6';
        l -= UINT32_C(6000);
        goto c3;
    }
    if(l > UINT32_C(4999)) {
        *(p++) = '5';
        l -= UINT32_C(5000);
        goto c3;
    }
c4_2:
    if(l > UINT32_C(3999)) {
        *(p++) = '4';
        l -= UINT32_C(4000);
        goto c3;
    }
    if(l > UINT32_C(2999)) {
        *(p++) = '3';
        l -= UINT32_C(3000);
        goto c3;
    }
    if(l > UINT32_C(1999)) {
        *(p++) = '2';
        l -= UINT32_C(2000);
        goto c3;
    }
    if(l > UINT32_C(999)) {
        *(p++) = '1';
        l -= UINT32_C(1000);
        goto c3;
    }
    *(p++) = '0';
    // 3 chiffres
c3:
    if(l < UINT32_C(500))
        goto c3_2;
    if(l > UINT32_C(899)) {
        *(p++) = '9';
        l -= UINT32_C(900);
        goto c2;
    }
    if(l > UINT32_C(799)) {
        *(p++) = '8';
        l -= UINT32_C(800);
        goto c2;
    }
    if(l > UINT32_C(699)) {
        *(p++) = '7';
        l -= UINT32_C(700);
        goto c2;
    }
    if(l > UINT32_C(599)) {
        *(p++) = '6';
        l -= UINT32_C(600);
        goto c2;
    }
    if(l > UINT32_C(499)) {
        *(p++) = '5';
        l -= UINT32_C(500);
        goto c2;
    }
c3_2:
    if(l > UINT32_C(399)) {
        *(p++) = '4';
        l -= UINT32_C(400);
        goto c2;
    }
    if(l > UINT32_C(299)) {
        *(p++) = '3';
        l -= UINT32_C(300);
        goto c2;
    }
    if(l > UINT32_C(199)) {
        *(p++) = '2';
        l -= UINT32_C(200);
        goto c2;
    }
    if(l > UINT32_C(99)) {
        *(p++) = '1';
        l -= UINT32_C(100);
        goto c2;
    }
    *(p++) = '0';
    // 2 chiffres
c2:
    if(l < UINT32_C(50))
        goto c2_2;
    if(l > UINT32_C(89)) {
        *(p++) = '9';
        l -= UINT32_C(90);
        goto c1;
    }
    if(l > UINT32_C(79)) {
        *(p++) = '8';
        l -= UINT32_C(80);
        goto c1;
    }
    if(l > UINT32_C(69)) {
        *(p++) = '7';
        l -= UINT32_C(70);
        goto c1;
    }
    if(l > UINT32_C(59)) {
        *(p++) = '6';
        l -= UINT32_C(60);
        goto c1;
    }
    if(l > UINT32_C(49)) {
        *(p++) = '5';
        l -= UINT32_C(50);
        goto c1;
    }
c2_2:
    if(l > UINT32_C(39)) {
        *(p++) = '4';
        l -= UINT32_C(40);
        goto c1;
    }
    if(l > UINT32_C(29)) {
        *(p++) = '3';
        l -= UINT32_C(30);
        goto c1;
    }
    if(l > UINT32_C(19)) {
        *(p++) = '2';
        l -= UINT32_C(20);
        goto c1;
    }
    if(l > UINT32_C(9)) {
        *(p++) = '1';
        l -= UINT32_C(10);
        goto c1;
    }
    *(p++) = '0';
c1:
    *(p++) = '0' + l;
    *p = 0;

    return sb.concat(buffer, iRes);
}

StringBuffer& operator<<(StringBuffer& sb, int32_t l)
{
    char buffer[32];
    char* p = buffer;
    int iSign = 0;
    if(l < 0) {
        iSign = 1;
        *(p++) = '-';
        l = -l;
    }

    int iRes;

    if(l <= INT32_C(99999)) {
        if(l <= INT32_C(9)) {
            iRes = 1 + iSign;
            goto d1;
        }
        if(l <= INT32_C(99)) {
            iRes = 2 + iSign;
            goto d2;
        }
        if(l <= INT32_C(999)) {
            iRes = 3 + iSign;
            goto d3;
        }
        if(l <= INT32_C(9999)) {
            iRes = 4 + iSign;
            goto d4;
        }
        iRes = 5 + iSign;
        goto d5;
    }

    if(l <= INT32_C(999999)) {
        iRes = 6 + iSign;
        goto d6;
    }
    if(l <= INT32_C(9999999)) {
        iRes = 7 + iSign;
        goto d7;
    }
    if(l <= INT32_C(99999999)) {
        iRes = 8 + iSign;
        goto d8;
    }
    if(l <= INT32_C(999999999)) {
        iRes = 9 + iSign;
        goto d9;
    }
    iRes = 10 + iSign;

    // 10 chiffres
    if(l > INT32_C(1999999999)) {
        *(p++) = '2';
        l -= INT32_C(2000000000);
        goto d9;
    }
    if(l > INT32_C(999999999)) {
        *(p++) = '1';
        l -= INT32_C(1000000000);
        goto d9;
    }
    *(p++) = '0';
    // 9 chiffres
d9:
    if(l < INT32_C(500000000))
        goto d9_2;
    if(l > INT32_C(899999999)) {
        *(p++) = '9';
        l -= INT32_C(900000000);
        goto d8;
    }
    if(l > INT32_C(799999999)) {
        *(p++) = '8';
        l -= INT32_C(800000000);
        goto d8;
    }
    if(l > INT32_C(699999999)) {
        *(p++) = '7';
        l -= INT32_C(700000000);
        goto d8;
    }
    if(l > INT32_C(599999999)) {
        *(p++) = '6';
        l -= INT32_C(600000000);
        goto d8;
    }
    if(l > INT32_C(499999999)) {
        *(p++) = '5';
        l -= INT32_C(500000000);
        goto d8;
    }
d9_2:
    if(l > INT32_C(399999999)) {
        *(p++) = '4';
        l -= INT32_C(400000000);
        goto d8;
    }
    if(l > INT32_C(299999999)) {
        *(p++) = '3';
        l -= INT32_C(300000000);
        goto d8;
    }
    if(l > INT32_C(199999999)) {
        *(p++) = '2';
        l -= INT32_C(200000000);
        goto d8;
    }
    if(l > INT32_C(99999999)) {
        *(p++) = '1';
        l -= INT32_C(100000000);
        goto d8;
    }
    *(p++) = '0';
    // 8 chiffres
d8:
    if(l < INT32_C(50000000))
        goto d8_2;
    if(l > INT32_C(89999999)) {
        *(p++) = '9';
        l -= INT32_C(90000000);
        goto d7;
    }
    if(l > INT32_C(79999999)) {
        *(p++) = '8';
        l -= INT32_C(80000000);
        goto d7;
    }
    if(l > INT32_C(69999999)) {
        *(p++) = '7';
        l -= INT32_C(70000000);
        goto d7;
    }
    if(l > INT32_C(59999999)) {
        *(p++) = '6';
        l -= INT32_C(60000000);
        goto d7;
    }
    if(l > INT32_C(49999999)) {
        *(p++) = '5';
        l -= INT32_C(50000000);
        goto d7;
    }
d8_2:
    if(l > INT32_C(39999999)) {
        *(p++) = '4';
        l -= INT32_C(40000000);
        goto d7;
    }
    if(l > INT32_C(29999999)) {
        *(p++) = '3';
        l -= INT32_C(30000000);
        goto d7;
    }
    if(l > INT32_C(19999999)) {
        *(p++) = '2';
        l -= INT32_C(20000000);
        goto d7;
    }
    if(l > INT32_C(9999999)) {
        *(p++) = '1';
        l -= INT32_C(10000000);
        goto d7;
    }
    *(p++) = '0';
    // 7 chiffres
d7:
    if(l < INT32_C(5000000))
        goto d7_2;
    if(l > INT32_C(8999999)) {
        *(p++) = '9';
        l -= INT32_C(9000000);
        goto d6;
    }
    if(l > INT32_C(7999999)) {
        *(p++) = '8';
        l -= INT32_C(8000000);
        goto d6;
    }
    if(l > INT32_C(6999999)) {
        *(p++) = '7';
        l -= INT32_C(7000000);
        goto d6;
    }
    if(l > INT32_C(5999999)) {
        *(p++) = '6';
        l -= INT32_C(6000000);
        goto d6;
    }
    if(l > INT32_C(4999999)) {
        *(p++) = '5';
        l -= INT32_C(5000000);
        goto d6;
    }
d7_2:
    if(l > INT32_C(3999999)) {
        *(p++) = '4';
        l -= INT32_C(4000000);
        goto d6;
    }
    if(l > INT32_C(2999999)) {
        *(p++) = '3';
        l -= INT32_C(3000000);
        goto d6;
    }
    if(l > INT32_C(1999999)) {
        *(p++) = '2';
        l -= INT32_C(2000000);
        goto d6;
    }
    if(l > INT32_C(999999)) {
        *(p++) = '1';
        l -= INT32_C(1000000);
        goto d6;
    }
    *(p++) = '0';
    // 6 chiffres
d6:
    if(l < INT32_C(500000))
        goto d6_2;
    if(l > INT32_C(899999)) {
        *(p++) = '9';
        l -= INT32_C(900000);
        goto d5;
    }
    if(l > INT32_C(799999)) {
        *(p++) = '8';
        l -= INT32_C(800000);
        goto d5;
    }
    if(l > INT32_C(699999)) {
        *(p++) = '7';
        l -= INT32_C(700000);
        goto d5;
    }
    if(l > INT32_C(599999)) {
        *(p++) = '6';
        l -= INT32_C(600000);
        goto d5;
    }
    if(l > INT32_C(499999)) {
        *(p++) = '5';
        l -= INT32_C(500000);
        goto d5;
    }
d6_2:
    if(l > INT32_C(399999)) {
        *(p++) = '4';
        l -= INT32_C(400000);
        goto d5;
    }
    if(l > INT32_C(299999)) {
        *(p++) = '3';
        l -= INT32_C(300000);
        goto d5;
    }
    if(l > INT32_C(199999)) {
        *(p++) = '2';
        l -= INT32_C(200000);
        goto d5;
    }
    if(l > INT32_C(99999)) {
        *(p++) = '1';
        l -= INT32_C(100000);
        goto d5;
    }
    *(p++) = '0';
    // 5 chiffres
d5:
    if(l < INT32_C(50000))
        goto d5_2;
    if(l > INT32_C(89999)) {
        *(p++) = '9';
        l -= INT32_C(90000);
        goto d4;
    }
    if(l > INT32_C(79999)) {
        *(p++) = '8';
        l -= INT32_C(80000);
        goto d4;
    }
    if(l > INT32_C(69999)) {
        *(p++) = '7';
        l -= INT32_C(70000);
        goto d4;
    }
    if(l > INT32_C(59999)) {
        *(p++) = '6';
        l -= INT32_C(60000);
        goto d4;
    }
    if(l > INT32_C(49999)) {
        *(p++) = '5';
        l -= INT32_C(50000);
        goto d4;
    }
d5_2:
    if(l > INT32_C(39999)) {
        *(p++) = '4';
        l -= INT32_C(40000);
        goto d4;
    }
    if(l > INT32_C(29999)) {
        *(p++) = '3';
        l -= INT32_C(30000);
        goto d4;
    }
    if(l > INT32_C(19999)) {
        *(p++) = '2';
        l -= INT32_C(20000);
        goto d4;
    }
    if(l > INT32_C(9999)) {
        *(p++) = '1';
        l -= INT32_C(10000);
        goto d4;
    }
    *(p++) = '0';
    // 4 chiffres
d4:
    if(l < INT32_C(5000))
        goto d4_2;
    if(l > INT32_C(8999)) {
        *(p++) = '9';
        l -= INT32_C(9000);
        goto d3;
    }
    if(l > INT32_C(7999)) {
        *(p++) = '8';
        l -= INT32_C(8000);
        goto d3;
    }
    if(l > INT32_C(6999)) {
        *(p++) = '7';
        l -= INT32_C(7000);
        goto d3;
    }
    if(l > INT32_C(5999)) {
        *(p++) = '6';
        l -= INT32_C(6000);
        goto d3;
    }
    if(l > INT32_C(4999)) {
        *(p++) = '5';
        l -= INT32_C(5000);
        goto d3;
    }
d4_2:
    if(l > INT32_C(3999)) {
        *(p++) = '4';
        l -= INT32_C(4000);
        goto d3;
    }
    if(l > INT32_C(2999)) {
        *(p++) = '3';
        l -= INT32_C(3000);
        goto d3;
    }
    if(l > INT32_C(1999)) {
        *(p++) = '2';
        l -= INT32_C(2000);
        goto d3;
    }
    if(l > INT32_C(999)) {
        *(p++) = '1';
        l -= INT32_C(1000);
        goto d3;
    }
    *(p++) = '0';
    // 3 chiffres
d3:
    if(l < INT32_C(500))
        goto d3_2;
    if(l > INT32_C(899)) {
        *(p++) = '9';
        l -= INT32_C(900);
        goto d2;
    }
    if(l > INT32_C(799)) {
        *(p++) = '8';
        l -= INT32_C(800);
        goto d2;
    }
    if(l > INT32_C(699)) {
        *(p++) = '7';
        l -= INT32_C(700);
        goto d2;
    }
    if(l > INT32_C(599)) {
        *(p++) = '6';
        l -= INT32_C(600);
        goto d2;
    }
    if(l > INT32_C(499)) {
        *(p++) = '5';
        l -= INT32_C(500);
        goto d2;
    }
d3_2:
    if(l > INT32_C(399)) {
        *(p++) = '4';
        l -= INT32_C(400);
        goto d2;
    }
    if(l > INT32_C(299)) {
        *(p++) = '3';
        l -= INT32_C(300);
        goto d2;
    }
    if(l > INT32_C(199)) {
        *(p++) = '2';
        l -= INT32_C(200);
        goto d2;
    }
    if(l > INT32_C(99)) {
        *(p++) = '1';
        l -= INT32_C(100);
        goto d2;
    }
    *(p++) = '0';
    // 2 chiffres
d2:
    if(l < INT32_C(50))
        goto d2_2;
    if(l > INT32_C(89)) {
        *(p++) = '9';
        l -= INT32_C(90);
        goto d1;
    }
    if(l > INT32_C(79)) {
        *(p++) = '8';
        l -= INT32_C(80);
        goto d1;
    }
    if(l > INT32_C(69)) {
        *(p++) = '7';
        l -= INT32_C(70);
        goto d1;
    }
    if(l > INT32_C(59)) {
        *(p++) = '6';
        l -= INT32_C(60);
        goto d1;
    }
    if(l > INT32_C(49)) {
        *(p++) = '5';
        l -= INT32_C(50);
        goto d1;
    }
d2_2:
    if(l > INT32_C(39)) {
        *(p++) = '4';
        l -= INT32_C(40);
        goto d1;
    }
    if(l > INT32_C(29)) {
        *(p++) = '3';
        l -= INT32_C(30);
        goto d1;
    }
    if(l > INT32_C(19)) {
        *(p++) = '2';
        l -= INT32_C(20);
        goto d1;
    }
    if(l > INT32_C(9)) {
        *(p++) = '1';
        l -= INT32_C(10);
        goto d1;
    }
    *(p++) = '0';
d1:
    *(p++) = '0' + l;
    *p = 0;

    return sb.concat(buffer, iRes);
}

StringBuffer& operator<<(StringBuffer& sb, uint16_t l)
{
    char buffer[32];
    char* p = buffer;
    int iRes;

    if(l <= UINT16_C(9)) {
        iRes = 1;
        goto e1;
    }
    if(l <= UINT16_C(99)) {
        iRes = 2;
        goto e2;
    }
    if(l <= UINT16_C(999)) {
        iRes = 3;
        goto e3;
    }
    if(l <= UINT16_C(9999)) {
        iRes = 4;
        goto e4;
    }
    iRes = 5;

    if(l < UINT16_C(50000))
        goto e5_2;
    if(l > UINT16_C(59999)) {
        *(p++) = '6';
        l -= UINT16_C(60000);
        goto e4;
    }
    if(l > UINT16_C(49999)) {
        *(p++) = '5';
        l -= UINT16_C(50000);
        goto e4;
    }
e5_2:
    if(l > UINT16_C(39999)) {
        *(p++) = '4';
        l -= UINT16_C(40000);
        goto e4;
    }
    if(l > UINT16_C(29999)) {
        *(p++) = '3';
        l -= UINT16_C(30000);
        goto e4;
    }
    if(l > UINT16_C(19999)) {
        *(p++) = '2';
        l -= UINT16_C(20000);
        goto e4;
    }
    if(l > UINT16_C(9999)) {
        *(p++) = '1';
        l -= UINT16_C(10000);
        goto e4;
    }
    *(p++) = '0';
    // 4 chiffres
e4:
    if(l < UINT16_C(5000))
        goto e4_2;
    if(l > UINT16_C(8999)) {
        *(p++) = '9';
        l -= UINT16_C(9000);
        goto e3;
    }
    if(l > UINT16_C(7999)) {
        *(p++) = '8';
        l -= UINT16_C(8000);
        goto e3;
    }
    if(l > UINT16_C(6999)) {
        *(p++) = '7';
        l -= UINT16_C(7000);
        goto e3;
    }
    if(l > UINT16_C(5999)) {
        *(p++) = '6';
        l -= UINT16_C(6000);
        goto e3;
    }
    if(l > UINT16_C(4999)) {
        *(p++) = '5';
        l -= UINT16_C(5000);
        goto e3;
    }
e4_2:
    if(l > UINT16_C(3999)) {
        *(p++) = '4';
        l -= UINT16_C(4000);
        goto e3;
    }
    if(l > UINT16_C(2999)) {
        *(p++) = '3';
        l -= UINT16_C(3000);
        goto e3;
    }
    if(l > UINT16_C(1999)) {
        *(p++) = '2';
        l -= UINT16_C(2000);
        goto e3;
    }
    if(l > UINT16_C(999)) {
        *(p++) = '1';
        l -= UINT16_C(1000);
        goto e3;
    }
    *(p++) = '0';
    // 3 chiffres
e3:
    if(l < UINT16_C(500))
        goto e3_2;
    if(l > UINT16_C(899)) {
        *(p++) = '9';
        l -= UINT16_C(900);
        goto e2;
    }
    if(l > UINT16_C(799)) {
        *(p++) = '8';
        l -= UINT16_C(800);
        goto e2;
    }
    if(l > UINT16_C(699)) {
        *(p++) = '7';
        l -= UINT16_C(700);
        goto e2;
    }
    if(l > UINT16_C(599)) {
        *(p++) = '6';
        l -= UINT16_C(600);
        goto e2;
    }
    if(l > UINT16_C(499)) {
        *(p++) = '5';
        l -= UINT16_C(500);
        goto e2;
    }
e3_2:
    if(l > UINT16_C(399)) {
        *(p++) = '4';
        l -= UINT16_C(400);
        goto e2;
    }
    if(l > UINT16_C(299)) {
        *(p++) = '3';
        l -= UINT16_C(300);
        goto e2;
    }
    if(l > UINT16_C(199)) {
        *(p++) = '2';
        l -= UINT16_C(200);
        goto e2;
    }
    if(l > UINT16_C(99)) {
        *(p++) = '1';
        l -= UINT16_C(100);
        goto e2;
    }
    *(p++) = '0';
    // 2 chiffres
e2:
    if(l < UINT16_C(50))
        goto e2_2;
    if(l > UINT16_C(89)) {
        *(p++) = '9';
        l -= UINT16_C(90);
        goto e1;
    }
    if(l > UINT16_C(79)) {
        *(p++) = '8';
        l -= UINT16_C(80);
        goto e1;
    }
    if(l > UINT16_C(69)) {
        *(p++) = '7';
        l -= UINT16_C(70);
        goto e1;
    }
    if(l > UINT16_C(59)) {
        *(p++) = '6';
        l -= UINT16_C(60);
        goto e1;
    }
    if(l > UINT16_C(49)) {
        *(p++) = '5';
        l -= UINT16_C(50);
        goto e1;
    }
e2_2:
    if(l > UINT16_C(39)) {
        *(p++) = '4';
        l -= UINT16_C(40);
        goto e1;
    }
    if(l > UINT16_C(29)) {
        *(p++) = '3';
        l -= UINT16_C(30);
        goto e1;
    }
    if(l > UINT16_C(19)) {
        *(p++) = '2';
        l -= UINT16_C(20);
        goto e1;
    }
    if(l > UINT16_C(9)) {
        *(p++) = '1';
        l -= UINT16_C(10);
        goto e1;
    }
    *(p++) = '0';
e1:
    *(p++) = '0' + l;
    *p = 0;

    return sb.concat(buffer, iRes);
}

StringBuffer& operator<<(StringBuffer& sb, int16_t l)
{
    char buffer[32];
    char* p = buffer;
    int iSign = 0;
    if(l < 0) {
        iSign = 1;
        *(p++) = '-';
        l = -l;
    }

    int iRes;

    if(l <= INT16_C(9)) {
        iRes = 1 + iSign;
        goto f1;
    }
    if(l <= INT16_C(99)) {
        iRes = 2 + iSign;
        goto f2;
    }
    if(l <= INT16_C(999)) {
        iRes = 3 + iSign;
        goto f3;
    }
    if(l <= INT16_C(9999)) {
        iRes = 4 + iSign;
        goto f4;
    }
    iRes = 5 + iSign;

    if(l > INT16_C(29999)) {
        *(p++) = '3';
        l -= INT16_C(30000);
        goto f4;
    }
    if(l > INT16_C(19999)) {
        *(p++) = '2';
        l -= INT16_C(20000);
        goto f4;
    }
    if(l > INT16_C(9999)) {
        *(p++) = '1';
        l -= INT16_C(10000);
        goto f4;
    }
    *(p++) = '0';
    // 4 chiffres
f4:
    if(l < INT16_C(5000))
        goto f4_2;
    if(l > INT16_C(8999)) {
        *(p++) = '9';
        l -= INT16_C(9000);
        goto f3;
    }
    if(l > INT16_C(7999)) {
        *(p++) = '8';
        l -= INT16_C(8000);
        goto f3;
    }
    if(l > INT16_C(6999)) {
        *(p++) = '7';
        l -= INT16_C(7000);
        goto f3;
    }
    if(l > INT16_C(5999)) {
        *(p++) = '6';
        l -= INT16_C(6000);
        goto f3;
    }
    if(l > INT16_C(4999)) {
        *(p++) = '5';
        l -= INT16_C(5000);
        goto f3;
    }
f4_2:
    if(l > INT16_C(3999)) {
        *(p++) = '4';
        l -= INT16_C(4000);
        goto f3;
    }
    if(l > INT16_C(2999)) {
        *(p++) = '3';
        l -= INT16_C(3000);
        goto f3;
    }
    if(l > INT16_C(1999)) {
        *(p++) = '2';
        l -= INT16_C(2000);
        goto f3;
    }
    if(l > INT16_C(999)) {
        *(p++) = '1';
        l -= INT16_C(1000);
        goto f3;
    }
    *(p++) = '0';
    // 3 chiffres
f3:
    if(l < INT16_C(500))
        goto f3_2;
    if(l > INT16_C(899)) {
        *(p++) = '9';
        l -= INT16_C(900);
        goto f2;
    }
    if(l > INT16_C(799)) {
        *(p++) = '8';
        l -= INT16_C(800);
        goto f2;
    }
    if(l > INT16_C(699)) {
        *(p++) = '7';
        l -= INT16_C(700);
        goto f2;
    }
    if(l > INT16_C(599)) {
        *(p++) = '6';
        l -= INT16_C(600);
        goto f2;
    }
    if(l > INT16_C(499)) {
        *(p++) = '5';
        l -= INT16_C(500);
        goto f2;
    }
f3_2:
    if(l > INT16_C(399)) {
        *(p++) = '4';
        l -= INT16_C(400);
        goto f2;
    }
    if(l > INT16_C(299)) {
        *(p++) = '3';
        l -= INT16_C(300);
        goto f2;
    }
    if(l > INT16_C(199)) {
        *(p++) = '2';
        l -= INT16_C(200);
        goto f2;
    }
    if(l > INT16_C(99)) {
        *(p++) = '1';
        l -= INT16_C(100);
        goto f2;
    }
    *(p++) = '0';
    // 2 chiffres
f2:
    if(l < INT16_C(50))
        goto f2_2;
    if(l > INT16_C(89)) {
        *(p++) = '9';
        l -= INT16_C(90);
        goto f1;
    }
    if(l > INT16_C(79)) {
        *(p++) = '8';
        l -= INT16_C(80);
        goto f1;
    }
    if(l > INT16_C(69)) {
        *(p++) = '7';
        l -= INT16_C(70);
        goto f1;
    }
    if(l > INT16_C(59)) {
        *(p++) = '6';
        l -= INT16_C(60);
        goto f1;
    }
    if(l > INT16_C(49)) {
        *(p++) = '5';
        l -= INT16_C(50);
        goto f1;
    }
f2_2:
    if(l > INT16_C(39)) {
        *(p++) = '4';
        l -= INT16_C(40);
        goto f1;
    }
    if(l > INT16_C(29)) {
        *(p++) = '3';
        l -= INT16_C(30);
        goto f1;
    }
    if(l > INT16_C(19)) {
        *(p++) = '2';
        l -= INT16_C(20);
        goto f1;
    }
    if(l > INT16_C(9)) {
        *(p++) = '1';
        l -= INT16_C(10);
        goto f1;
    }
    *(p++) = '0';
f1:
    *(p++) = '0' + l;
    *p = 0;

    return sb.concat(buffer, iRes);
}

StringBuffer& operator<<(StringBuffer& sb, uint8_t l)
{

    char buffer[32];
    char* p = buffer;
    int iRes;

    if(l <= UINT8_C(9)) {
        iRes = 1;
        goto g1;
    }
    if(l <= UINT8_C(99)) {
        iRes = 2;
        goto g2;
    }
    iRes = 3;

    if(l > UINT8_C(199)) {
        *(p++) = '2';
        l -= UINT8_C(200);
        goto g2;
    }
    if(l > UINT8_C(99)) {
        *(p++) = '1';
        l -= UINT8_C(100);
        goto g2;
    }
    *(p++) = '0';
    // 2 chiffres
g2:
    if(l < UINT8_C(50))
        goto g2_2;
    if(l > UINT8_C(89)) {
        *(p++) = '9';
        l -= UINT8_C(90);
        goto g1;
    }
    if(l > UINT8_C(79)) {
        *(p++) = '8';
        l -= UINT8_C(80);
        goto g1;
    }
    if(l > UINT8_C(69)) {
        *(p++) = '7';
        l -= UINT8_C(70);
        goto g1;
    }
    if(l > UINT8_C(59)) {
        *(p++) = '6';
        l -= UINT8_C(60);
        goto g1;
    }
    if(l > UINT8_C(49)) {
        *(p++) = '5';
        l -= UINT8_C(50);
        goto g1;
    }
g2_2:
    if(l > UINT8_C(39)) {
        *(p++) = '4';
        l -= UINT8_C(40);
        goto g1;
    }
    if(l > UINT8_C(29)) {
        *(p++) = '3';
        l -= UINT8_C(30);
        goto g1;
    }
    if(l > UINT8_C(19)) {
        *(p++) = '2';
        l -= UINT8_C(20);
        goto g1;
    }
    if(l > UINT8_C(9)) {
        *(p++) = '1';
        l -= UINT8_C(10);
        goto g1;
    }
    *(p++) = '0';
g1:
    *(p++) = '0' + l;
    *p = 0;

    return sb.concat(buffer, iRes);
}

StringBuffer& operator<<(StringBuffer& sb, int8_t l)
{
    char buffer[32];
    char* p = buffer;
    int iSign = 0;
    if(l < 0) {
        iSign = 1;
        *(p++) = '-';
        l = -l;
    }

    int iRes;

    if(l <= INT8_C(9)) {
        iRes = 1 + iSign;
        goto h1;
    }
    if(l <= INT8_C(99)) {
        iRes = 2 + iSign;
        goto h2;
    }
    iRes = 3 + iSign;

    if(l > INT8_C(99)) {
        *(p++) = '1';
        l -= INT8_C(100);
        goto h2;
    }
    *(p++) = '0';
    // 2 chiffres
h2:
    if(l < INT8_C(50))
        goto h2_2;
    if(l > INT8_C(89)) {
        *(p++) = '9';
        l -= INT8_C(90);
        goto h1;
    }
    if(l > INT8_C(79)) {
        *(p++) = '8';
        l -= INT8_C(80);
        goto h1;
    }
    if(l > INT8_C(69)) {
        *(p++) = '7';
        l -= INT8_C(70);
        goto h1;
    }
    if(l > INT8_C(59)) {
        *(p++) = '6';
        l -= INT8_C(60);
        goto h1;
    }
    if(l > INT8_C(49)) {
        *(p++) = '5';
        l -= INT8_C(50);
        goto h1;
    }
h2_2:
    if(l > INT8_C(39)) {
        *(p++) = '4';
        l -= INT8_C(40);
        goto h1;
    }
    if(l > INT8_C(29)) {
        *(p++) = '3';
        l -= INT8_C(30);
        goto h1;
    }
    if(l > INT8_C(19)) {
        *(p++) = '2';
        l -= INT8_C(20);
        goto h1;
    }
    if(l > INT8_C(9)) {
        *(p++) = '1';
        l -= INT8_C(10);
        goto h1;
    }
    *(p++) = '0';
h1:
    *(p++) = '0' + l;
    *p = 0;

    return sb.concat(buffer, iRes);
}

StringBuffer& operator<<(StringBuffer& sb, const char* s)
{
    return sb.concat(s, strlen(s));
}

StringBuffer& operator<<(StringBuffer& sb, std::string s)
{
    return sb.concat(s);
}
