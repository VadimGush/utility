//
// Created by Vadim Gush on 12.04.2023.
//

#ifndef UTILITIES_BITS_H
#define UTILITIES_BITS_H

#include <cstdlib>
#include "types.h"

namespace bits {

    constexpr u8 LOW_1BIT = 0b00000001;
    constexpr u8 LOW_2BIT = 0b00000011;
    constexpr u8 LOW_3BIT = 0b00000111;
    constexpr u8 LOW_4BIT = 0b00001111;

    constexpr u8 HIGH_1BIT = 0b10000000;
    constexpr u8 HIGH_2BIT = 0b11000000;
    constexpr u8 HIGH_3BIT = 0b11100000;
    constexpr u8 HIGH_4BIT = 0b11110000;

    constexpr u8 ALL_8BIT = 0b11111111;

    /**
     * Combines 8 bit values into a 16 bit value.
     *
     * @param high high bits of 16 bit value
     * @param low low bits of 16 bit value
     * @return 16 bit value
     */
    static u16 combine(const u8 high, const u8 low) {
        return (static_cast<u16>(high) << 8) | static_cast<u16>(low);
    }

    /**
     * Basically a reinterpret_cast<> but without pointer types.
     *
     * @tparam O type of resulting object
     * @tparam I type of input object
     * @param value value of type I
     * @return value of type O
     */
    template <typename O, typename I>
    O reinterpret(const I& value) {
        return *reinterpret_cast<const O*>(&value);
    }

    /**
     * Prints bits to the provided output stream
     *
     * @tparam O type of output stream
     * @param out output stream
     * @param byte 8 bits to print
     */
    template <typename O>
    void print_bits(O& out, const u8 byte) {
        for (size_t i = 0; i < 8; i++) {
            if (byte << i & HIGH_1BIT) out << "1";
            else out << "0";
        }
    }
}


#endif //UTILITIES_BITS_H
