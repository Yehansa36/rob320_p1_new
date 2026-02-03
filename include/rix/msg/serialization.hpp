#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "rix/msg/message.hpp"

namespace rix {
namespace msg {
namespace detail {

template <typename T>
inline uint32_t size_number(const T &src) {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    return sizeof(T);
}
inline uint32_t size_string(const std::string &src) { return 4 + src.size(); }
inline uint32_t size_message(const Message &src) { return src.size(); }
template <typename T, size_t N>
inline uint32_t size_number_array(const std::array<T, N> &src) {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    return N * sizeof(T);
}
template <size_t N>
inline uint32_t size_string_array(const std::array<std::string, N> &src) {
    uint32_t size = 0;
    for (const auto &s : src) size += size_string(s);
    return size;
}
template <typename T, size_t N>
inline uint32_t size_message_array(const std::array<T, N> &src) {
    static_assert(std::is_base_of<Message, T>::value, "T must derive from Message");
    uint32_t size = 0;
    for (const auto &m : src) size += size_message(m);
    return size;
}
template <typename T>
inline uint32_t size_number_vector(const std::vector<T> &src) {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    return 4 + src.size() * sizeof(T);
}
inline uint32_t size_string_vector(const std::vector<std::string> &src) {
    uint32_t size = 4;
    for (const auto &s : src) size += size_string(s);
    return size;
}
template <typename T>
inline uint32_t size_message_vector(const std::vector<T> &src) {
    static_assert(std::is_base_of<Message, T>::value, "T must derive from Message");
    uint32_t size = 4;
    for (const auto &m : src) size += size_message(m);
    return size;
}

/**
 * @brief Serializes a number `src` and stores it in the byte array `dst` at
 * `offset`. `offset` is incremented by the number of bytes written to `dst`.
 *
 * @tparam T The type of the source (must be an arithmetic type)
 * @param dst The destination byte array
 * @param offset The offset in the byte array at which to write (incremented by
 * number of bytes written)
 * @param src The source number to be serialized
 */
template <typename T>
inline void serialize_number(uint8_t *dst, size_t &offset, const T &src) {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    /**< TODO */
    //explanation: copy sizeof(T) bytes from the memory where src lives into the byte buffer dst starting
    //at position offset
    std::memcpy(dst + offset, &src, sizeof(T));
    offset += sizeof(T);

}

/**
 * @brief Serializes a string `src` and stores it in the byte array `dst` at
 * `offset`. `offset` is incremented by the number of bytes written to `dst`.
 *
 * @param dst The destination byte array
 * @param offset The offset in the byte array at which to write (incremented by
 * number of bytes written)
 * @param src The source string to be serialized
 */
inline void serialize_string(uint8_t *dst, size_t &offset, const std::string &src) {
    /**< TODO */
    //serialize string length
    size_t length = src.size();
    std::memcpy(dst + offset, &length, sizeof(length));
    offset += sizeof(length);

    //serialize string charactoers
    std::memcpy(dst + offset, src.data(), length);
    offset += length;
}

/**
 * @brief Serializes a message `src` and stores it in the byte array `dst` at
 * `offset`. `offset` is incremented by the number of bytes written to `dst`.
 *
 * @param dst The destination byte array
 * @param offset The offset in the byte array at which to write (incremented by
 * number of bytes written)
 * @param src The source message to be serialized
 */
inline void serialize_message(uint8_t *dst, size_t &offset, const Message &src) {
    /**< TODO */
    src.serialize(dst, offset);

}

/**
 * @brief Serializes a number array `src` and stores it in the byte array `dst`
 * at `offset`. `offset` is incremented by the number of bytes written to `dst`.
 *
 * @tparam T The type of the source array (must be an arithmetic type)
 * @tparam N The size of the source array
 * @param dst The destination byte array
 * @param offset The offset in the byte array at which to write (incremented by
 * number of bytes written)
 * @param src The source number array to be serialized
 */
template <typename T, size_t N>
inline void serialize_number_array(uint8_t *dst, size_t &offset,
                                   const std::array<T, N> &src) {
    /**< TODO */
    const auto bytes = N * sizeof(T);
    std::memcpy(dst + offset, src.data(), bytes);
    offset += bytes;

}

/**
 * @brief Serializes a string array `src` and stores it in the byte array `dst`
 * at `offset`. `offset` is incremented by the number of bytes written to `dst`.
 *
 * @tparam N The size of the source array
 * @param dst The destination byte array
 * @param offset The offset in the byte array at which to write (incremented by
 * number of bytes written)
 * @param src The source string array to be serialized
 */
template <size_t N>
inline void serialize_string_array(uint8_t *dst, size_t &offset,
                                   const std::array<std::string, N> &src) {
    /**< TODO */
    for (const auto &s : src) {
        serialize_string(dst, offset, s);
    }


}

/**
 * @brief Serializes a message array `src` and stores it in the byte array `dst`
 * at `offset`. `offset` is incremented by the number of bytes written to `dst`.
 *
 * @tparam T The type of the source array (must derive from Message)
 * @tparam N The size of the source array
 * @param dst The destination byte array
 * @param offset The offset in the byte array at which to write (incremented by
 * number of bytes written)
 * @param src The source message array to be serialized
 */
template <typename T, size_t N>
inline void serialize_message_array(uint8_t *dst, size_t &offset,
                                    const std::array<T, N> &src) {
    static_assert(std::is_base_of<Message, T>::value, "T must derive from Message");
    /**< TODO */
    for (const auto &m : src) {
        serialize_message(dst, offset, m);
    }
}

/**
 * @brief Serializes a number vector `src` and stores it in the byte array `dst`
 * at `offset`. `offset` is incremented by the number of bytes written to `dst`.
 *
 * @tparam T The type of the source array (must be an arithmetic type)
 * @param dst The destination byte array
 * @param offset The offset in the byte array at which to write (incremented by
 * number of bytes written)
 * @param src The source number vector to be serialized
 */
template <typename T>
inline void serialize_number_vector(uint8_t *dst, size_t &offset,
                                    const std::vector<T> &src) {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    /**< TODO */
    //serialize vector length
    size_t len = src.size();
    serialize_number(dst, offset, len);

    //serialize elements
    for (const auto &v : src) {
        serialize_number(dst, offset, v);
    }
}

/**
 * @brief Serializes a string vector `src` and stores it in the byte array `dst`
 * at `offset`. `offset` is incremented by the number of bytes written to `dst`.
 *
 * @param dst The destination byte array
 * @param offset The offset in the byte array at which to write (incremented by
 * number of bytes written)
 * @param src The source string vector to be serialized
 */
inline void serialize_string_vector(uint8_t *dst, size_t &offset,
                                    const std::vector<std::string> &src) {
    /**< TODO */
    uint32_t count = static_cast<uint32_t>(src.size());
    std::memcpy(dst + offset, &count, sizeof(count));
    offset += sizeof(count);

    for (const auto &s : src) {
        uint32_t len = static_cast<uint32_t>(s.size());
        std:memcpy(dst + offset, &len, sizeof(len));
        offset += sizeof(len);

        if (len > 0) {
            std::memcpy(dst + offset, s.data(), len);
            offset += len;
        }
    }
    
}

/**
 * @brief Serializes a message vector `src` and stores it in the byte array
 * `dst` at `offset`. `offset` is incremented by the number of bytes written to
 * `dst`.
 *
 * @tparam T The type of the source array (must derive from Message)
 * @param dst The destination byte array
 * @param offset The offset in the byte array at which to write (incremented by
 * number of bytes written)
 * @param src The source message vector to be serialized
 */
template <typename T>
inline void serialize_message_vector(uint8_t *dst, size_t &offset,
                                     const std::vector<T> &src) {
    static_assert(std::is_base_of<Message, T>::value, "T must derive from Message");
    /**< TODO */
}

/**
 * @brief Deserializes a number from the byte array `src` at `offset` and stores
 * it into `dst`. `src` must be at least `size` bytes long.
 *
 * @tparam T The type of the destination value (must be an arithmetic type)
 * @param dst The destination number
 * @param src The source byte array
 * @param size The size of the byte array
 * @param offset The position in the source byte array to deserialize data from
 * @return `false` if the number of bytes needed to deserialize the number is
 * greater than the number of bytes available in the source byte array. `true`
 * otherwise.
 */
template <typename T>
inline bool deserialize_number(T &dst, const uint8_t *src, size_t size, size_t &offset) {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    /**< TODO */
    return false;
}

/**
 * @brief Deserializes a string from the byte array `src` at `offset` and stores
 * it into `dst`. `src` must be at least `size` bytes long.
 *
 * @param dst The destination string
 * @param src The source byte array
 * @param size The size of the byte array
 * @param offset The position in the source byte array to deserialize data from
 * @return `false` if the number of bytes needed to deserialize the string is
 * greater than the number of bytes available in the source byte array. `true`
 * otherwise.
 */
inline bool deserialize_string(std::string &dst, const uint8_t *src, size_t size,
                               size_t &offset) {
    /**< TODO */
    return false;
}

/**
 * @brief Deserializes a message from the byte array `src` at `offset` and
 * stores it into `dst`. `src` must be at least `size` bytes long.
 *
 * @param dst The destination message
 * @param src The source byte array
 * @param size The size of the byte array
 * @param offset The position in the source byte array to deserialize data from
 * @return `false` if the number of bytes needed to deserialize the message is
 * greater than the number of bytes available in the source byte array. `true`
 * otherwise.
 */
inline bool deserialize_message(Message &dst, const uint8_t *src, size_t size,
                                size_t &offset) {
    /**< TODO */
    return false;
}

/**
 * @brief Deserializes a string from the byte array `src` at `offset` and stores
 * it into `dst`. `src` must be at least `size` bytes long.
 *
 * @tparam T The type of the destination array (must be an arithmetic type)
 * @tparam N The size of the destination array
 * @param dst The destination string
 * @param src The source byte array
 * @param size The size of the byte array
 * @param offset The position in the source byte array to deserialize data from
 * @return `false` if the number of bytes needed to deserialize the string is
 * greater than the number of bytes available in the source byte array. `true`
 * otherwise.
 */
template <typename T, size_t N>
inline bool deserialize_number_array(std::array<T, N> &dst, const uint8_t *src,
                                     size_t size, size_t &offset) {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    /**< TODO */
    return false;
}

/**
 * @brief Deserializes a string array from the byte array `src` at `offset` and
 * stores it into `dst`. `src` must be at least `size` bytes long.
 *
 * @tparam N The size of the destination array
 * @param dst The destination string array
 * @param src The source byte array
 * @param size The size of the byte array
 * @param offset The position in the source byte array to deserialize data from
 * @return `false` if the number of bytes needed to deserialize the string array
 * is greater than the number of bytes available in the source byte array.
 * `true` otherwise.
 */
template <size_t N>
inline bool deserialize_string_array(std::array<std::string, N> &dst, const uint8_t *src,
                                     size_t size, size_t &offset) {
    /**< TODO */
    return false;
}

/**
 * @brief Deserializes a message array from the byte array `src` at `offset` and
 * stores it into `dst`. `src` must be at least `size` bytes long.
 *
 * @tparam T The type of the destination array (must derive from Message)
 * @tparam N The size of the destination array
 * @param dst The destination message array
 * @param src The source byte array
 * @param size The size of the byte array
 * @param offset The position in the source byte array to deserialize data from
 * @return `false` if the number of bytes needed to deserialize the message
 * array is greater than the number of bytes available in the source byte array.
 * `true` otherwise.
 */
template <typename T, size_t N>
inline bool deserialize_message_array(std::array<T, N> &dst, const uint8_t *src,
                                      size_t size, size_t &offset) {
    static_assert(std::is_base_of<Message, T>::value, "T must derive from Message");
    /**< TODO */
    return false;
}

/**
 * @brief Deserializes a number vector from the byte array `src` at `offset` and
 * stores it into `dst`. `src` must be at least `size` bytes long.
 *
 * @tparam T The type of the destination array (must be an arithmetic type)
 * @param dst The destination number vector
 * @param src The source byte array
 * @param size The size of the byte array
 * @param offset The position in the source byte array to deserialize data from
 * @return `false` if the number of bytes needed to deserialize the number
 * vector is greater than the number of bytes available in the source byte
 * array. `true` otherwise.
 */
template <typename T>
inline bool deserialize_number_vector(std::vector<T> &dst, const uint8_t *src,
                                      size_t size, size_t &offset) {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    /**< TODO */
    return false;
}

/**
 * @brief Deserializes a string vector from the byte array `src` at `offset` and
 * stores it into `dst`. `src` must be at least `size` bytes long.
 *
 * @param dst The destination string vector
 * @param src The source byte array
 * @param size The size of the byte array
 * @param offset The position in the source byte array to deserialize data from
 * @return `false` if the number of bytes needed to deserialize the string
 * vector is greater than the number of bytes available in the source byte
 * array. `true` otherwise.
 */
inline bool deserialize_string_vector(std::vector<std::string> &dst, const uint8_t *src,
                                      size_t size, size_t &offset) {
    /**< TODO */
    return false;
}

/**
 * @brief Deserializes a message vector from the byte array `src` at `offset` and
 * stores it into `dst`. `src` must be at least `size` bytes long.
 *
 * @tparam T The type of the destination array (must derive from Message)
 * @param dst The destination message vector
 * @param src The source byte array
 * @param size The size of the byte array
 * @param offset The position in the source byte array to deserialize data from
 * @return `false` if the number of bytes needed to deserialize the message
 * vector is greater than the number of bytes available in the source byte
 * array. `true` otherwise.
 */
template <typename T>
inline bool deserialize_message_vector(std::vector<T> &dst, const uint8_t *src,
                                       size_t size, size_t &offset) {
    static_assert(std::is_base_of<Message, T>::value, "T must derive from Message");
    /**< TODO */
    return false;
}
}  // namespace detail
}  // namespace msg
}  // namespace rix