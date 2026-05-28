#pragma once
#include <stddef.h>
#include <stdint.h>

// Convert UTF-8 string to ASCII, replacing French accented chars with their
// base letter. Needed because LVGL's built-in Montserrat fonts only cover
// printable ASCII (0x20-0x7E).
static inline void stripAccents(char *dst, const char *src, size_t maxLen) {
    size_t di = 0;
    for (size_t i = 0; src[i] && di < maxLen - 1; ) {
        uint8_t c0 = (uint8_t)src[i];

        if (c0 < 0x80) {
            dst[di++] = src[i++];
            continue;
        }

        // Two-byte UTF-8: 0xC3 covers U+00C0-U+00FF (all common Latin-1 accents)
        if (c0 == 0xC3 && src[i + 1]) {
            uint8_t c1 = (uint8_t)src[i + 1];
            char r = '\0';
            if      (c1 >= 0x80 && c1 <= 0x86) r = 'A';   // À Á Â Ã Ä Å Æ
            else if (c1 == 0x87)                r = 'C';   // Ç
            else if (c1 >= 0x88 && c1 <= 0x8B) r = 'E';   // È É Ê Ë
            else if (c1 >= 0x8C && c1 <= 0x8F) r = 'I';   // Ì Í Î Ï
            else if (c1 == 0x91)                r = 'N';   // Ñ
            else if (c1 >= 0x92 && c1 <= 0x96) r = 'O';   // Ò Ó Ô Õ Ö
            else if (c1 >= 0x99 && c1 <= 0x9C) r = 'U';   // Ù Ú Û Ü
            else if (c1 == 0x9D || c1 == 0xB8) r = 'Y';   // Ý ø
            else if (c1 >= 0xA0 && c1 <= 0xA6) r = 'a';   // à á â ã ä å æ
            else if (c1 == 0xA7)                r = 'c';   // ç
            else if (c1 >= 0xA8 && c1 <= 0xAB) r = 'e';   // è é ê ë
            else if (c1 >= 0xAC && c1 <= 0xAF) r = 'i';   // ì í î ï
            else if (c1 == 0xB1)                r = 'n';   // ñ
            else if (c1 >= 0xB2 && c1 <= 0xB6) r = 'o';   // ò ó ô õ ö
            else if (c1 >= 0xB9 && c1 <= 0xBC) r = 'u';   // ù ú û ü
            else if (c1 == 0xBD || c1 == 0xBF) r = 'y';   // ý ÿ
            if (r) dst[di++] = r;
            i += 2;
            continue;
        }

        // 0xC5: covers œ (U+0153 = 0xC5 0x93) and Œ (U+0152 = 0xC5 0x92)
        if (c0 == 0xC5 && src[i + 1]) {
            uint8_t c1 = (uint8_t)src[i + 1];
            if (c1 == 0x92 && di + 1 < maxLen - 1) { dst[di++] = 'O'; dst[di++] = 'E'; }
            else if (c1 == 0x93 && di + 1 < maxLen - 1) { dst[di++] = 'o'; dst[di++] = 'e'; }
            i += 2;
            continue;
        }

        // Skip any other multi-byte sequence
        int bytes = ((c0 & 0xE0) == 0xC0) ? 2
                  : ((c0 & 0xF0) == 0xE0) ? 3
                  : ((c0 & 0xF8) == 0xF0) ? 4 : 1;
        i += bytes;
    }
    dst[di] = '\0';
}
