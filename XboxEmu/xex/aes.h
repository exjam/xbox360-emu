#ifndef AES_H
#define AES_H

#include "rijndael-alg-fst.h"
#include <string.h>
#include <stdint.h>

/* Rijndael / AES in CBC mode */
class Rijndael {
public:
   Rijndael()
   {
   }

   Rijndael(const uint8_t key[16])
   {
      init(key);
   }

   void init(const uint8_t key[16])
   {
      memcpy(mKey, key, 16);
      memset(mIv, 0, 16);
      mRounds = rijndaelKeySetupDec(mRoundKey, mKey, 128);
   }

   void decrypt(const uint8_t in[16], uint8_t out[16])
   {
      rijndaelDecrypt(mRoundKey, mRounds, in, out);

      for (size_t i = 0; i < 16; i++) {
         out[i] ^= mIv[i];
      }

      memcpy(mIv, in, 16);
   }

private:
   uint8_t mKey[16];
   uint8_t mIv[16];
   uint32_t mRounds;
   uint32_t mRoundKey[4 * (MAXNR + 1)];
};

#endif // AES_H
