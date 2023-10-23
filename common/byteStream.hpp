#pragma once

#include <queue>
#include <string>
#include <type_traits>

template<typename T> struct is_vector : public std::false_type {};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

class DataStream {
public:
  std::vector<std::byte> data;

  template<typename T>
  void push(T value) {
    static_assert(!is_vector<T>::value, "DataStreams cannot push vectors. Use pushVector instead.");
    static_assert(!std::is_same<T, std::string>::value, "DataStreams cannot push strings. Use pushString instead.");


    std::byte* bytes = reinterpret_cast<std::byte*>(&value);
    for (size_t i = 0; i < sizeof(T); i++) {
      data.push_back(bytes[i]);
    }
  }

  template<typename T>
  T pop() {
    static_assert(!is_vector<T>::value, "DataStreams cannot push vectors. Use pushVector instead.");
    static_assert(!std::is_same<T, std::string>::value, "DataStreams cannot push strings. Use pushString instead.");

    T value;
    std::byte* bytes = reinterpret_cast<std::byte*>(&value);
    for (size_t i = 0; i < sizeof(T); i++) {
      bytes[i] = data[i + offset];
    }
    offset += sizeof(T);

    return value;
  }

  template<typename T>
  void pushVector(std::vector<T> value) {
    push<int>(value.size());
    for (size_t i = 0; i < value.size(); i++) {
      push<T>(value[i]);
    }
  }

  template<typename T>
  std::vector<T> popVector() {
    std::vector<T> value;
    int size = pop<int>();
    for (int i = 0; i < size; i++) {
      value.push_back(pop<T>());
    }
    return value;
  }

  void pushString(std::string value) {
    for (size_t i = 0; i < value.size(); i++) {
      data.push_back(std::byte(value[i]));
    }
    data.push_back(std::byte('\0'));
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

protected:
  DataStream() {}
  int offset = 0;
};

class DataReadStream : public DataStream {
public:
  DataReadStream() {}
  DataReadStream(std::vector<std::byte> data) { this->data = data; }
  ~DataReadStream() {}

  template<class T>
  void push(T& value) = delete;
  void pushString(std::string value) = delete;
};

class DataWriteStream : public DataStream {
public:
  DataWriteStream() {}
  DataWriteStream(std::vector<std::byte> data) { this->data = data; }
  ~DataWriteStream() {}

  template<class T>
  T pop() = delete;
  std::string popString() = delete;
};
