/*********************************************************************************
  *Copyright (c) 2016-2017 U9TIME(uuu9.com). All rights reserved.
  *FileName: ByteArray.hpp
  *Author: Hao Jichen
  *Date: 2016.12.12
  *Desc: C++ ByteArray like as3, base on std::vector<byte>, support big and little endianness.
**********************************************************************************/
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

typedef unsigned char byte;

template<typename T>
std::vector<T> &operator+(std::vector<T> &v1, const std::vector<T> &v2) {
    v1.insert(v1.end(), v2.begin(), v2.end());
    return v1;
}

template<typename T>
std::vector<T> &operator+(std::vector<T> &v1, const std::string &v2) {
    v1.insert(v1.end(), v2.begin(), v2.end());
    return v1;
}

enum struct Endian {
    System,
    Little,
    Big
};

class ByteArray : public std::vector<byte> {
public:
    ByteArray() = default;

    ByteArray(const char *data, size_t len) {
        this->append(data, len);
    }

    ByteArray(const byte *data, size_t len) {
        this->append(data, len);
    }

    ByteArray(const void *data, size_t len) {
        this->append((const byte *)data, len);
    }

    ByteArray(const ByteArray &data) {
        this->append(data);
    }

    explicit ByteArray(const std::string &data) {
        this->append(data);
    }

    template<size_t N>
    explicit ByteArray(const byte (&data)[N]) {
        this->append(data, N);
    }

    template<size_t N>
    explicit ByteArray(const char (&data)[N]) {
        this->append(data, N);
    }

    ByteArray &operator+(const ByteArray &v) {
        this->insert(this->end(), v.begin(), v.end());
        return *this;
    }

    ByteArray &operator+(const std::string &v) {
        this->insert(this->end(), v.begin(), v.end());
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const ByteArray &ba) {
        return os << ba.data();
    }

    ByteArray &append(const ByteArray &v) {
        this->insert(this->end(), v.begin(), v.end());
        return *this;
    }

    ByteArray &append(const std::string &v) {
        this->insert(this->end(), v.begin(), v.end());
        return *this;
    }

    ByteArray &append(const byte v) {
        this->push_back(v);
        return *this;
    }

    ByteArray &append(const byte *v, size_t len) {
        this->resize(this->size() + len);
        memcpy(this->data() + this->size() - len, v, len);
        return *this;
    }

    ByteArray &append(const char *v, size_t len) {
        this->resize(this->size() + len);
        memcpy(this->data() + this->size() - len, v, len);
        return *this;
    }

    ByteArray &appendHex(const byte *v, size_t len) {
        for(size_t i = 0; i < len; ++i) {
            if(v[i] == ' ')
                continue;
            if(i + 1 >= len)
                break;
            if(v[i + 1] == ' ')
                continue;
            this->append(hex_to_byte(v + i));
            i = i + 1;
        }
        return *this;
    }

    ByteArray &appendHex(const char *v, size_t len) {
        return appendHex((const byte *)v, len);
    }

    ByteArray &appendHex(const std::string &v) {
        appendHex(v.data(), v.size());
    }

    ByteArray &assign(const ByteArray &v) {
        this->clear();
        return append(v);
    }

    ByteArray &assign(const std::string &v) {
        this->clear();
        return append(v);
    }

    ByteArray &assign(const byte *v, size_t len) {
        this->clear();
        return append(v, len);
    }

    ByteArray &assign(const char *v, size_t len) {
        this->clear();
        return append(v, len);
    }

    ByteArray &assignHex(const std::string &v) {
        this->clear();
        return appendHex(v);
    }

    ByteArray &writeBytes(const byte *v, size_t len, size_t offset) {
        this->resize(this->size() + len);
        memcpy(this->data() + offset + len, this->data() + offset, this->size() - offset - len);
        memcpy(this->data() + offset, v, len);
        return *this;
    }

    ByteArray &writeBytes(const char *v, size_t len, size_t offset) {
        return writeBytes((const byte *) v, len, offset);
    }

    ByteArray &writeBytes(const byte *v, size_t len) {
        return append(v, len);
    }

    ByteArray &writeBytes(const char *v, size_t len) {
        return append(v, len);
    }

    ByteArray &writeString(const std::string &v) {
        return append(v);
    }

    ByteArray readBytes(size_t len, size_t offset = 0) {
        return ByteArray(this->data() + offset, len);
    }

    byte readByte(size_t offset = 0) {
        byte result = *(byte *) (this->data() + offset);
        return result;
    }

    void writeByte(byte value) {
        this->append((byte *) &value, sizeof(byte));
    }

    bool readBool(size_t offset = 0) {
        bool result = *(bool *) (this->data() + offset);
        return result;
    }

    void writeBool(bool value) {
        this->append((byte *) &value, sizeof(bool));
    }

    uint8_t readUint8(size_t offset = 0) {
        uint8_t result = *(uint8_t *) (this->data() + offset);
        return result;
    }

    void writeUint8(uint8_t value) {
        this->append((byte *) &value, sizeof(uint8_t));
    }

    uint16_t readUint16(size_t offset = 0) {
        uint16_t result = *(uint16_t *) (this->data() + offset);
        if (this->endian != Endian::System && this->endian != this->getSystemEndian()) {
            result = __builtin_bswap16(result);
        }
        return result;
    }

    void writeUint16(uint16_t value) {
        if (this->endian != Endian::System && this->endian != this->getSystemEndian()) {
            value = __builtin_bswap16(value);
        }
        this->append((byte *) &value, sizeof(uint16_t));
    }

    uint32_t readUint32(size_t offset = 0) {
        uint32_t result = *(uint32_t *) (this->data() + offset);
        if (this->endian != Endian::System && this->endian != this->getSystemEndian()) {
            result = __builtin_bswap32(result);
        }
        return result;
    }

    void writeUint32(uint32_t value) {
        if (this->endian != Endian::System && this->endian != this->getSystemEndian()) {
            value = __builtin_bswap32(value);
        }
        this->append((byte *) &value, sizeof(uint32_t));
    }

    uint64_t readUint64(size_t offset = 0) {
        uint64_t result = *(uint64_t *) (this->data() + offset);
        if (this->endian != Endian::System && this->endian != this->getSystemEndian()) {
            result = __builtin_bswap64(result);
        }
        return result;
    }

    void writeUint64(uint64_t value) {
        if (this->endian != Endian::System && this->endian != this->getSystemEndian()) {
            value = __builtin_bswap64(value);
        }
        this->append((byte *) &value, sizeof(uint64_t));
    }

    int8_t readInt8(size_t offset = 0) {
        int8_t result = *(int8_t *) (this->data() + offset);
        return result;
    }

    void writeInt8(int8_t value) {
        this->append((byte *) &value, sizeof(int8_t));
    }

    int16_t readInt16(size_t offset = 0) {
        return (int16_t) readUint16(offset);
    }

    void writeInt16(int16_t value) {
        writeUint16(*(uint16_t *) &value);
    }

    int32_t readInt32(size_t offset = 0) {
        return (int32_t) readUint32(offset);
    }

    void writeInt32(int32_t value) {
        writeUint32(*(uint32_t *) &value);
    }

    int64_t readInt64(size_t offset = 0) {
        return (int64_t) readUint64(offset);
    }

    void writeInt64(int64_t value) {
        writeUint64(*(uint64_t *) &value);
    }

    float readFloat(size_t offset = 0) {
        uint32_t value = readUint32(offset);
        return *(float *) &value;
    }

    void writeFloat(float value) {
        writeUint32(*(uint32_t *) &value);
    }

    double readDouble(size_t offset = 0) {
        uint64_t value = readUint64(offset);
        return *(double *) &value;
    }

    void writeDouble(double value) {
        writeUint64(*(uint64_t *) &value);
    }

    ByteArray &fromHex(const std::string &v) {
        assignHex(v);
    }

    std::string toHex(bool pretty = true) {
        static const char lookup[] = "0123456789ABCDEF";
        std::string result;
        size_t i = 0;
        size_t len = this->size();
        const byte *bytes = this->data();
        while (i < len) {
            char ch = bytes[i];
            result.push_back(lookup[(ch & 0xF0) >> 4]);
            result.push_back(lookup[(ch & 0x0F)]);
            if(pretty)
                result.push_back(' ');
            i = i + 1;
        }
        if(pretty)
            result.pop_back();
        return result;
    }

    Endian getSystemEndian() {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return Endian::Little;
#else
        return Endian::Big;
#endif
//        int16_t x = 1;
//        if (*(char *) &x == 1) {
//            return Endian::Little;
//        } else {
//            return Endian::Big;
//        }
    }

    void setEndian(Endian value) {
        this->endian = value;
    }

    Endian endian = Endian::System;

private:
    static char hex_to_unit(byte ch) {
        if (ch >= 0x30 && ch <= 0x39) { // 0...9
            return char(ch - 0x30);
        } else if (ch >= 0x41 && ch <= 0x46) { // ABCDEF
            return char(ch - 0x41 + 10);
        } else if (ch >= 0x61 && ch <= 0x66) { // abcdef
            return char(ch - 0x61 + 10);
        }
        return 0;
    }

    static byte hex_to_byte(const byte *src) {
        return byte(hex_to_unit(src[0]) * 16 + hex_to_unit(src[1]));
    }
};