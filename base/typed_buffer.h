// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REMOTING_BASE_TYPED_BUFFER_H_
#define REMOTING_BASE_TYPED_BUFFER_H_

#include <assert.h>
#include <stdint.h>

#include <algorithm>

#include "base/logging.h"
#include "base/macros.h"

namespace remoting {

// A scoper for a variable-length structure such as SID, SECURITY_DESCRIPTOR and
// similar. These structures typically consist of a header followed by variable-
// length data, so the size may not match sizeof(T). The class supports
// move-only semantics and typed buffer getters.
template <typename T>
class TypedBuffer {
 public:
  TypedBuffer() : TypedBuffer(0) {}

  // Creates an instance of the object allocating a buffer of the given size.
  explicit TypedBuffer(uint32_t length) : buffer_(NULL), length_(length) {
    if (length_ > 0)
      buffer_ = reinterpret_cast<T*>(new uint8_t[length_]);
  }

  TypedBuffer(TypedBuffer&& rvalue) : TypedBuffer() { Swap(rvalue); }

  ~TypedBuffer() {
    if (buffer_) {
      delete[] reinterpret_cast<uint8_t*>(buffer_);
      buffer_ = NULL;
    }
  }

  TypedBuffer& operator=(TypedBuffer&& rvalue) {
    Swap(rvalue);
    return *this;
  }

  // Accessors to get the owned buffer.
  // operator* and operator-> will assert() if there is no current buffer.
  T& operator*() const {
    assert(buffer_ != NULL);
    return *buffer_;
  }
  T* operator->() const  {
    assert(buffer_ != NULL);
    return buffer_;
  }
  T* get() const { return buffer_; }

  uint32_t length() const { return length_; }

  // Helper returning a pointer to the structure starting at a specified byte
  // offset.
  T* GetAtOffset(uint32_t offset) {
    return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(buffer_) + offset);
  }

  // Allow TypedBuffer<T> to be used in boolean expressions, but not
  // implicitly convertible to a real bool (which is dangerous).
  typedef T* TypedBuffer::*Testable;
  operator Testable() const { return buffer_ ? &TypedBuffer::buffer_ : NULL; }

  // Swap two buffers.
  void Swap(TypedBuffer& other) {
    std::swap(buffer_, other.buffer_);
    std::swap(length_, other.length_);
  }

 private:
  // Points to the owned buffer.
  T* buffer_;

  // Length of the owned buffer in bytes.
  uint32_t length_;

  DISALLOW_COPY_AND_ASSIGN(TypedBuffer);
};

}  // namespace remoting

#endif  // REMOTING_BASE_TYPED_BUFFER_H_
