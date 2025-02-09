/*
Copyright © 2010, Ismion Inc
All rights reserved.
http://www.ismion.com/

Redistribution and use in source and binary forms, with or without
modification IS NOT permitted without specific prior written
permission. Further, neither the name of the company, Ismion
Inc, nor the names of its employees may be used to endorse or promote
products derived from this software without specific prior written
permission.

THIS SOFTWARE IS PROVIDED BY THE Ismion Inc "AS IS" AND ANY
EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COMPANY BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef FASTLIB_DATA_TYPENAME_H_
#define FASTLIB_DATA_TYPENAME_H_
#include <string>
namespace fl {
namespace data {
/**
 * @brief: This is replacing typeid which is not platform independant
 *         maps a type to its name if it is more than oned words like
 *         long int it maps it to long_int
 */
template<typename T>
struct Typename {
  static std::string Name() {
    return "";
  }
};

template<>
struct Typename<bool> {
  static std::string Name() {
    return "bool";
  }
};

template<>
struct Typename<char> {
  static std::string Name() {
    return "char";
  }
};

template<>
struct Typename<int8> {
  static std::string Name() {
    return "int8";
  }
};

template<>
struct Typename<uint8> {
  static std::string Name() {
    return "uint8";
  }
};

template<>
struct Typename<int32> {
  static std::string Name() {
    return "int32";
  }
};

template<>
struct Typename<uint32> {
  static std::string Name() {
    return "uint32";
  }
};

template<>
struct Typename<int64> {
  static std::string Name() {
    return "int64";
  }
};


template<>
struct Typename<uint64> {
  static std::string Name() {
    return "uint64";
  }
};



template<>
struct Typename<float> {
  static std::string Name() {
    return "float";
  }
};

template<>
struct Typename<double> {
  static std::string Name() {
    return "double";
  }
};

template<>
struct Typename<long double> {
  static std::string Name() {
    return "long_double";
  }
};
}
}
#endif
