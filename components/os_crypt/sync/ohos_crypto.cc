/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2023. All rights reserved.
 */

#include "ohos_crypto.h"

#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <random>
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "crypto/sha2.h"
#include "ohos_keystore.h"

namespace crypto {

namespace ohos {

namespace {

class KeyCache {
 public:
  KeyCache() = default;
  ~KeyCache() = default;
  // No copy construct and assignment allowed
  KeyCache(const KeyCache&) = delete;
  KeyCache& operator=(const KeyCache&) = delete;

  // Return true if the key is found, otherwise false
  bool get_key(const std::string& key_name, std::string* key_value);

  // Add a key to the map
  void add_key(const std::string& key_name, const std::string& key_value);

 private:
  struct key_map_entry_t {
    std::string value_;
    uint64_t time_;
  };

  typedef std::map<std::string, key_map_entry_t> key_map_t;

  key_map_t key_map_;
  uint64_t key_time_{0};
  std::mutex lock;
};

// Maximum key map size
const size_t MAX_KEY_MAP_SIZE = 256;

static base::LazyInstance<KeyCache>::Leaky inst = LAZY_INSTANCE_INITIALIZER;

// Return true if the key is found, otherwise false
bool KeyCache::get_key(const std::string& key_name, std::string* key_value) {
  // Make this function synchronous
  std::lock_guard<std::mutex> guard(lock);

  auto iter = key_map_.find(key_name);
  if (iter != key_map_.end()) {
    // Refresh the entry's time to the latest
    iter->second.time_ = key_time_++;

    // Return a copy of the key
    if (key_value != nullptr) {
      *key_value = iter->second.value_;
    }

    // key found
    return true;
  }

  // Key not found
  return false;
}

// Add a key to the map
void KeyCache::add_key(const std::string& key_name,
                       const std::string& key_value) {
  // Make this function synchronous
  std::lock_guard<std::mutex> guard(lock);

  // Check if total map size exceeds the limit
  if (key_map_.size() >= MAX_KEY_MAP_SIZE) {
    // Find the entry with the smallest time
    auto iter_to_drop = key_map_.begin();
    for (auto iter = key_map_.begin(); iter != key_map_.end(); ++iter) {
      if (iter->second.time_ < iter_to_drop->second.time_) {
        iter_to_drop = iter;
      }
    }

    // Drop the entry
    key_map_.erase(iter_to_drop);
  }

  // Add the new entry
  key_map_[key_name] =
      key_map_entry_t{.value_ = key_value, .time_ = key_time_++};
}

}  // namespace

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------
// Retired for the moment - s84078948 - March 1st, 2018
static std::string _hex_repr(const std::string& s) {
  static const char hex_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                     '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

  std::string h(s.size() * 2, 0);
  for (size_t i = 0; i < s.size(); ++i) {
    const unsigned char c = (unsigned char)s[i];
    h[i + i] = hex_table[(c >> 4) & 0xf];
    h[i + i + 1] = hex_table[c & 0xf];
  }

  return h;
}

static std::string _generate_key(const std::string& key_name) {
  std::vector<uint8_t> key_byte_array;

  std::string digest = crypto::SHA256HashString(key_name);
  return crypto::ohos::GetKey(_hex_repr(digest));
}

std::string _get_random(size_t sz) {
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<> dis{0, 255};

  std::string rn(sz, 0);
  for (size_t i = 0; i < sz; ++i) {
    rn[i] = (std::string::value_type)dis(gen);
  }

  return rn;
}

std::string get_symmetric_key_256(const std::string& key_name) {
  std::string key_value;

  if (inst.Get().get_key(key_name, &key_value)) {
    // Key found
    return key_value;
  }

  // Key not found. Generate it first
  key_value = _generate_key(key_name);

  inst.Get().add_key(key_name, key_value);
  // Return the generated key
  return key_value;
}

#if defined(OHOS_EX_PASSWORD)
std::string get_asset_handle_file_256(const std::string& key_name)
{
  std::string digest = crypto::SHA256HashString(key_name);
  return _hex_repr(digest);
}
#endif

std::string get_iv(size_t sz) {
  return _get_random(sz);
}

}  // namespace ohos
}  // end namespace crypto
