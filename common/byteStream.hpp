#pragma once

#include <queue>
#include <string>
#include <type_traits>

template<typename T> struct is_vector : public std::false_type {};
template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

class DataReadStream {
public:
  std::vector<std::byte> data;

  DataReadStream() {}
  DataReadStream(std::vector<std::byte> data) { this->data = data; }
  ~DataReadStream() {}

  template<typename T>
  T pop() {
    if constexpr (is_vector<T>::value) {
      return popVector<typename T::value_type>();
    }
    else if constexpr (std::is_same<T, std::string>::value) {
      return popString();
    }
    else {
      T value;
      std::byte* bytes = reinterpret_cast<std::byte*>(&value);
      for (size_t i = 0; i < sizeof(T); i++) {
        bytes[i] = data[i + offset];
      }
      offset += sizeof(T);

      return value;
    }
  }

private:
  template<typename T>
  std::vector<T> popVector() {
    std::vector<T> value;
    int size = pop<int>();
    for (int i = 0; i < size; i++) {
      value.push_back(pop<T>());
    }
    return value;
  }

  std::string popString() {
    std::string value;
    while (data[offset] != std::byte('\0')) {
      value += char(data[offset]);
      offset++;
    }
    offset++;
    return value;
  }

  int offset = 0;
};

class DataWriteStream {
public:
  std::vector<std::byte> data;

  DataWriteStream() {}
  DataWriteStream(std::vector<std::byte> data) { this->data = data; }
  ~DataWriteStream() {}

  template<typename T>
  void push(T value) {
    if constexpr (is_vector<T>::value) {
      pushVector<typename T::value_type>(value);
      return;
    }
    else if constexpr (std::is_same<T, std::string>::value) {
      pushString(value);
      return;
    }
    else {
      std::byte* bytes = reinterpret_cast<std::byte*>(&value);
      for (size_t i = 0; i < sizeof(T); i++) {
        data.push_back(bytes[i]);
      }
    }
  }

private:
  template<typename T>
  void pushVector(std::vector<T>& value) {
    push<int>(value.size());
    for (auto &v : value) {
      push<T>(v);
    }
  }

  void pushString(std::string& value) {
    for (auto c : value) {
      data.push_back(std::byte(c));
    }
    data.push_back(std::byte('\0'));
  }
};
