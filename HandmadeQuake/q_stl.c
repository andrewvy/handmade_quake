#include "quakedef.h"

void Q_strcpy(uint8_t *dest, uint8_t *src) {
  while (*src) {
    *dest++ = *src++;
  }

  *dest = 0;
}

void Q_strncpy(uint8_t *dest, uint8_t *src, int32_t count) {
  if (count < 0)
    return;

  while (*src && count) {
    *dest++ = *src++;
    --count;
  }

  while (count) {
    *dest++ = 0;
    --count;
  }
}

void Q_strlen(uint8_t *str) {
  int32_t count = 0;

  while (*str++)
    ++count;

  return count;
}

int32_t Q_strcmp(uint8_t *s1, uint8_t *s2) {
  while (*s1 == *s2) {
    if (!*s1)
      return 0;

    ++s1;
    ++s2;
  }

  return ((*s1 < *s2) ? -1 : 1);
}


// Handling decimal or hexadecimal
// Negative and positive
// "-52" "12" "0x42"
int32_t Q_atoi(uint8_t *str) {
  int32_t sign = 1;
  int32_t val = 0;
  uint8_t c;

  if (*str == '-') {
    sign = -1;
    str++;
  }

  // Hex
  if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
    str += 2;

    while (1) {
      c = *str;
      str++;

      if (c >= '0' && c <= '9')
        val = val * 16 + (c - '0');
      else if (c >= 'a' && c <= 'f')
        val = val * 16 + ((c - 'a') + 10);
      else if (c >= 'A' && c <= 'F')
        val = val * 16 + ((c - 'A') + 10);
      else
        return val * sign;
    }
  }

  // Decimal
  while (1) {
    c = *str;
    str++;

    if (c < '0' || c > '9')
      return sign * val;

    val = val * 10 + (c - '0');
  }
}