// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef UNSAFE_BUFFERS_BUILD
// TODO(crbug.com/351564777): Remove this and convert code to safer constructs.
#pragma allow_unsafe_buffers
#endif

#include "crypto/encryptor.h"

#include <stddef.h>
#include <stdint.h>

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "crypto/openssl_util.h"
#include "crypto/symmetric_key.h"
#include "third_party/boringssl/src/include/openssl/aes.h"
#include "third_party/boringssl/src/include/openssl/evp.h"

#if BUILDFLAG(ARKWEB_ENCRYPT)
#define GCM_TAG_SIZE 16
#define GCM_IV_SIZE 12
#endif

namespace crypto {

namespace {

const EVP_CIPHER* GetCipherForKey(const SymmetricKey* key) {
  switch (key->key().length()) {
    case 16: return EVP_aes_128_cbc();
    case 32: return EVP_aes_256_cbc();
    default:
      return nullptr;
  }
}

#if BUILDFLAG(ARKWEB_ENCRYPT)
const EVP_CIPHER* GetCipherForKeyGCM(const SymmetricKey* key) {
  switch (key->key().length()) {
    case 16:
      return EVP_aes_128_gcm();
    case 32:
      return EVP_aes_256_gcm();
    default:
      return nullptr;
  }
}
#endif


}  // namespace

/////////////////////////////////////////////////////////////////////////////
// Encryptor Implementation.

Encryptor::Encryptor() : key_(nullptr), mode_(CBC) {}

Encryptor::~Encryptor() = default;

bool Encryptor::Init(const SymmetricKey* key, Mode mode, std::string_view iv) {
  return Init(key, mode, base::as_bytes(base::make_span(iv)));
}

bool Encryptor::Init(const SymmetricKey* key,
                     Mode mode,
                     base::span<const uint8_t> iv) {
  DCHECK(key);
#if BUILDFLAG(ARKWEB_ENCRYPT)
  DCHECK(mode == CBC || mode == CTR || mode == GCM);
#else
  DCHECK(mode == CBC || mode == CTR);
#endif

  if (mode == CBC && iv.size() != AES_BLOCK_SIZE)
    return false;
  // CTR mode passes the starting counter separately, via SetCounter().
  if (mode == CTR && !iv.empty())
    return false;

  if (GetCipherForKey(key) == nullptr)
    return false;

#if BUILDFLAG(ARKWEB_ENCRYPT)
  if (mode == GCM && iv.size() != GCM_IV_SIZE) {
    return false;
  }
#endif

  key_ = key;
  mode_ = mode;
  iv_.assign(iv.begin(), iv.end());
  return true;
}

bool Encryptor::Encrypt(std::string_view plaintext, std::string* ciphertext) {
  return CryptString(/*do_encrypt=*/true, plaintext, ciphertext);
}

bool Encryptor::Encrypt(base::span<const uint8_t> plaintext,
                        std::vector<uint8_t>* ciphertext) {
  return CryptBytes(/*do_encrypt=*/true, plaintext, ciphertext);
}

bool Encryptor::Decrypt(std::string_view ciphertext, std::string* plaintext) {
  return CryptString(/*do_encrypt=*/false, ciphertext, plaintext);
}

bool Encryptor::Decrypt(base::span<const uint8_t> ciphertext,
                        std::vector<uint8_t>* plaintext) {
  return CryptBytes(/*do_encrypt=*/false, ciphertext, plaintext);
}

bool Encryptor::SetCounter(std::string_view counter) {
  return SetCounter(base::as_bytes(base::make_span(counter)));
}

bool Encryptor::SetCounter(base::span<const uint8_t> counter) {
  if (mode_ != CTR)
    return false;
  if (counter.size() != 16u)
    return false;

  iv_.assign(counter.begin(), counter.end());
  return true;
}

bool Encryptor::CryptString(bool do_encrypt,
                            std::string_view input,
                            std::string* output) {
  std::string result(MaxOutput(do_encrypt, input.size()), '\0');

  std::optional<size_t> len;
#if BUILDFLAG(ARKWEB_ENCRYPT)
  std::string tag(GCM_TAG_SIZE, 0);
  if (mode_ == CTR) {
    len = CryptCTR(do_encrypt, base::as_bytes(base::make_span(input)),
                   base::as_writable_bytes(base::make_span(result)));
  } else if (mode_ == GCM) {
    if (do_encrypt) {
      len = EncryptGCM(base::as_bytes(base::make_span(input)),
                       base::as_writable_bytes(base::make_span(result)), &tag);
    } else {
      // Get the tag that we attached with cipher during encryption from input
      if (input.length() <= GCM_TAG_SIZE) {
        LOG(WARNING) << "input size less than gcm tag size";
        return false;
      }

      tag = std::string(
          input.substr(input.length() - GCM_TAG_SIZE, GCM_TAG_SIZE));
      // Get the cipher part only from input
      std::string ciphertext =
          std::string(input.substr(0, input.length() - GCM_TAG_SIZE));
      const size_t output_size = ciphertext.length();
      if (output_size + 1 <= ciphertext.length()) {
        LOG(WARNING) << "output size occur overflow";
        return false;
      }
      len = DecryptGCM(ciphertext,
                       base::as_writable_bytes(base::make_span(result)), &tag);
    }
  } else {
    len = Crypt(do_encrypt, base::as_bytes(base::make_span(input)),
                base::as_writable_bytes(base::make_span(result)));
  }
#else
  len = (mode_ == CTR)
            ? CryptCTR(do_encrypt, base::as_bytes(base::make_span(input)),
                       base::as_writable_bytes(base::make_span(result)))
            : Crypt(do_encrypt, base::as_bytes(base::make_span(input)),
                    base::as_writable_bytes(base::make_span(result)));
#endif
  if (!len)
    return false;

  result.resize(*len);
#if BUILDFLAG(ARKWEB_ENCRYPT)
  // concat tag with cipher at the end.
  if (mode_ == GCM && do_encrypt) {
    result += tag;
  }
#endif
  *output = std::move(result);
  return true;
}

bool Encryptor::CryptBytes(bool do_encrypt,
                           base::span<const uint8_t> input,
                           std::vector<uint8_t>* output) {
  std::vector<uint8_t> result(MaxOutput(do_encrypt, input.size()));
  std::optional<size_t> len = (mode_ == CTR)
                                  ? CryptCTR(do_encrypt, input, result)
                                  : Crypt(do_encrypt, input, result);
  if (!len)
    return false;

  result.resize(*len);
  *output = std::move(result);
  return true;
}

size_t Encryptor::MaxOutput(bool do_encrypt, size_t length) {
#if BUILDFLAG(ARKWEB_ENCRYPT)
  size_t result = length + ((do_encrypt && mode_ == CBC)   ? 16
                            : (do_encrypt && mode_ == GCM) ? 12
                                                           : 0);
#else
  size_t result = length + ((do_encrypt && mode_ == CBC) ? 16 : 0);
#endif

  CHECK_GE(result, length);  // Overflow
  return result;
}

std::optional<size_t> Encryptor::Crypt(bool do_encrypt,
                                       base::span<const uint8_t> input,
                                       base::span<uint8_t> output) {
  DCHECK(key_);  // Must call Init() before En/De-crypt.

  const EVP_CIPHER* cipher = GetCipherForKey(key_);
  DCHECK(cipher);  // Already handled in Init();

  const std::string& key = key_->key();
  DCHECK_EQ(EVP_CIPHER_iv_length(cipher), iv_.size());
  DCHECK_EQ(EVP_CIPHER_key_length(cipher), key.size());

  OpenSSLErrStackTracer err_tracer(FROM_HERE);
  bssl::ScopedEVP_CIPHER_CTX ctx;
  if (!EVP_CipherInit_ex(ctx.get(), cipher, nullptr,
                         reinterpret_cast<const uint8_t*>(key.data()),
                         iv_.data(), do_encrypt)) {
    return std::nullopt;
  }

  // Encrypting needs a block size of space to allow for any padding.
  CHECK_GE(output.size(), input.size() + (do_encrypt ? iv_.size() : 0));
  int out_len;
  if (!EVP_CipherUpdate(ctx.get(), output.data(), &out_len, input.data(),
                        input.size()))
    return std::nullopt;

  // Write out the final block plus padding (if any) to the end of the data
  // just written.
  int tail_len;
  if (!EVP_CipherFinal_ex(ctx.get(), output.data() + out_len, &tail_len))
    return std::nullopt;

  out_len += tail_len;
  DCHECK_LE(out_len, static_cast<int>(output.size()));
  return out_len;
}

std::optional<size_t> Encryptor::CryptCTR(bool do_encrypt,
                                          base::span<const uint8_t> input,
                                          base::span<uint8_t> output) {
  if (iv_.size() != AES_BLOCK_SIZE) {
    LOG(ERROR) << "Counter value not set in CTR mode.";
    return std::nullopt;
  }

  AES_KEY aes_key;
  if (AES_set_encrypt_key(reinterpret_cast<const uint8_t*>(key_->key().data()),
                          key_->key().size() * 8, &aes_key) != 0) {
    return std::nullopt;
  }

  uint8_t ecount_buf[AES_BLOCK_SIZE] = { 0 };
  unsigned int block_offset = 0;

  // |output| must have room for |input|.
  CHECK_GE(output.size(), input.size());
  // Note AES_ctr128_encrypt() will update |iv_|. However, this method discards
  // |ecount_buf| and |block_offset|, so this is not quite a streaming API.
  AES_ctr128_encrypt(input.data(), output.data(), input.size(), &aes_key,
                     iv_.data(), ecount_buf, &block_offset);
  return input.size();
}

#if BUILDFLAG(ARKWEB_ENCRYPT)
std::optional<size_t> Encryptor::EncryptGCM(base::span<const uint8_t> input,
                                             base::span<uint8_t> output,
                                             std::string* tag) {
  DCHECK(key_);
  DCHECK(output.data());

  const EVP_CIPHER* cipher = GetCipherForKeyGCM(key_);
  DCHECK(cipher);

  const std::string& key = key_->key();
  DCHECK_EQ(EVP_CIPHER_iv_length(cipher), iv_.size());
  DCHECK_EQ(EVP_CIPHER_key_length(cipher), key.size());

  OpenSSLErrStackTracer err_tracer(FROM_HERE);

  bssl::ScopedEVP_CIPHER_CTX ctx;
  /* Initialise the encryption operation */
  if (!EVP_EncryptInit_ex(ctx.get(), cipher, nullptr, nullptr, nullptr)) {
    return std::nullopt;
  }

  /* Set the IV length, default is 12 byte which is not appropriate */
  if (!EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_SET_IVLEN, iv_.size(),
                           nullptr)) {
    return std::nullopt;
  }

  /* Initialise Key and IV */
  if (!EVP_EncryptInit_ex(ctx.get(), nullptr, nullptr,
                          reinterpret_cast<const uint8_t*>(key.data()),
                          iv_.data())) {
    return std::nullopt;
  }

  const size_t output_size = input.size() + (iv_.size());
  CHECK_GT(output_size, 0u);
  CHECK_GT(output_size + 1, input.size());

  int out_len;

  /* Provide the plaintext and obtain the encrypted update
   *  EVP_EncryptUpdate may call multiples times if necessary
   */
  if (!EVP_EncryptUpdate(ctx.get(), output.data(), &out_len,
                         reinterpret_cast<const uint8_t*>(input.data()),
                         input.size())) {
    return std::nullopt;
  }

  /* Finalise the encryption. Usually ciphertext byte may be
   * written in this stage but this does not occure in GCM mode
   */
  int len;
  if (!EVP_EncryptFinal_ex(ctx.get(), output.data() + out_len, &len)) {
    return std::nullopt;
  }

  out_len += len;

  /*Get the tag */
  if (!EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_GET_TAG, GCM_TAG_SIZE,
                           (void*)tag->data())) {
    return std::nullopt;
  }

  DCHECK_LE(out_len, static_cast<int>(output_size));

  return out_len;
}

std::optional<size_t> Encryptor::DecryptGCM(const std::string& input,
                                             base::span<uint8_t> output,
                                             std::string* tag) {
  DCHECK(key_);
  DCHECK(output.data());

  const EVP_CIPHER* cipher = GetCipherForKeyGCM(key_);
  DCHECK(cipher);

  const std::string& key = key_->key();
  DCHECK_EQ(EVP_CIPHER_iv_length(cipher), iv_.size());
  DCHECK_EQ(EVP_CIPHER_key_length(cipher), key.size());

  OpenSSLErrStackTracer err_tracer(FROM_HERE);
  bssl::ScopedEVP_CIPHER_CTX ctx;

  /* Initialise the decryption process */
  if (!EVP_DecryptInit_ex(ctx.get(), cipher, nullptr, nullptr, nullptr)) {
    return std::nullopt;
  }

  /* Set the IV length */
  if (!EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_SET_IVLEN, iv_.size(),
                           nullptr)) {
    return std::nullopt;
  }

  /* Initialise Key and IV */
  if (!EVP_DecryptInit_ex(ctx.get(), nullptr, nullptr,
                          reinterpret_cast<const uint8_t*>(key.data()),
                          iv_.data())) {
    return std::nullopt;
  }

  int out_len;

  /* Provide the cipher text and get the output as plaintext */
  if (!EVP_DecryptUpdate(ctx.get(), output.data(), &out_len,
                         reinterpret_cast<const uint8_t*>(input.data()),
                         input.length())) {
    return std::nullopt;
  }

  /* Set expected tag which is used during encryption */
  if (!EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_SET_TAG, GCM_TAG_SIZE,
                           (void*)tag->data())) {
    return std::nullopt;
  }
  /* Finalise the decryption. If plaintext is not trustworthy then
   * it return as failure.
   */
  int len;
  int ret = EVP_DecryptFinal_ex(ctx.get(), output.data() + out_len, &len);
  if (ret > 0) {
    out_len += len;
  } else {
    return std::nullopt;
  }

  DCHECK_LE(out_len, static_cast<int>(input.length()));

  return out_len;
}
#endif

}  // namespace crypto
