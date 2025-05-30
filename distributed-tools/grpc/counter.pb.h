// Generated by the protocol buffer compiler.  DO NOT EDIT!
// NO CHECKED-IN PROTOBUF GENCODE
// source: counter.proto
// Protobuf C++ Version: 6.30.1

#ifndef counter_2eproto_2epb_2eh
#define counter_2eproto_2epb_2eh

#include <limits>
#include <string>
#include <type_traits>
#include <utility>

#include "google/protobuf/runtime_version.h"
#if PROTOBUF_VERSION != 6030001
#error "Protobuf C++ gencode is built with an incompatible version of"
#error "Protobuf C++ headers/runtime. See"
#error "https://protobuf.dev/support/cross-version-runtime-guarantee/#cpp"
#endif
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/arena.h"
#include "google/protobuf/arenastring.h"
#include "google/protobuf/generated_message_tctable_decl.h"
#include "google/protobuf/generated_message_util.h"
#include "google/protobuf/metadata_lite.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/message.h"
#include "google/protobuf/message_lite.h"
#include "google/protobuf/repeated_field.h"  // IWYU pragma: export
#include "google/protobuf/extension_set.h"  // IWYU pragma: export
#include "google/protobuf/unknown_field_set.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"

#define PROTOBUF_INTERNAL_EXPORT_counter_2eproto

namespace google {
namespace protobuf {
namespace internal {
template <typename T>
::absl::string_view GetAnyMessageName();
}  // namespace internal
}  // namespace protobuf
}  // namespace google

// Internal implementation detail -- do not use these members.
struct TableStruct_counter_2eproto {
  static const ::uint32_t offsets[];
};
extern "C" {
extern const ::google::protobuf::internal::DescriptorTable descriptor_table_counter_2eproto;
}  // extern "C"
class Counter;
struct CounterDefaultTypeInternal;
extern CounterDefaultTypeInternal _Counter_default_instance_;
extern const ::google::protobuf::internal::ClassDataFull Counter_class_data_;
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google


// ===================================================================


// -------------------------------------------------------------------

class Counter final : public ::google::protobuf::Message
/* @@protoc_insertion_point(class_definition:Counter) */ {
 public:
  inline Counter() : Counter(nullptr) {}
  ~Counter() PROTOBUF_FINAL;

#if defined(PROTOBUF_CUSTOM_VTABLE)
  void operator delete(Counter* PROTOBUF_NONNULL msg, std::destroying_delete_t) {
    SharedDtor(*msg);
    ::google::protobuf::internal::SizedDelete(msg, sizeof(Counter));
  }
#endif

  template <typename = void>
  explicit PROTOBUF_CONSTEXPR Counter(::google::protobuf::internal::ConstantInitialized);

  inline Counter(const Counter& from) : Counter(nullptr, from) {}
  inline Counter(Counter&& from) noexcept
      : Counter(nullptr, std::move(from)) {}
  inline Counter& operator=(const Counter& from) {
    CopyFrom(from);
    return *this;
  }
  inline Counter& operator=(Counter&& from) noexcept {
    if (this == &from) return *this;
    if (::google::protobuf::internal::CanMoveWithInternalSwap(GetArena(), from.GetArena())) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const
      ABSL_ATTRIBUTE_LIFETIME_BOUND {
    return _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance);
  }
  inline ::google::protobuf::UnknownFieldSet* PROTOBUF_NONNULL mutable_unknown_fields()
      ABSL_ATTRIBUTE_LIFETIME_BOUND {
    return _internal_metadata_.mutable_unknown_fields<::google::protobuf::UnknownFieldSet>();
  }

  static const ::google::protobuf::Descriptor* PROTOBUF_NONNULL descriptor() {
    return GetDescriptor();
  }
  static const ::google::protobuf::Descriptor* PROTOBUF_NONNULL GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::google::protobuf::Reflection* PROTOBUF_NONNULL GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Counter& default_instance() {
    return *reinterpret_cast<const Counter*>(
        &_Counter_default_instance_);
  }
  static constexpr int kIndexInFileMessages = 0;
  friend void swap(Counter& a, Counter& b) { a.Swap(&b); }
  inline void Swap(Counter* PROTOBUF_NONNULL other) {
    if (other == this) return;
    if (::google::protobuf::internal::CanUseInternalSwap(GetArena(), other->GetArena())) {
      InternalSwap(other);
    } else {
      ::google::protobuf::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Counter* PROTOBUF_NONNULL other) {
    if (other == this) return;
    ABSL_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Counter* PROTOBUF_NONNULL New(::google::protobuf::Arena* PROTOBUF_NULLABLE arena = nullptr) const {
    return ::google::protobuf::Message::DefaultConstruct<Counter>(arena);
  }
  using ::google::protobuf::Message::CopyFrom;
  void CopyFrom(const Counter& from);
  using ::google::protobuf::Message::MergeFrom;
  void MergeFrom(const Counter& from) { Counter::MergeImpl(*this, from); }

  private:
  static void MergeImpl(::google::protobuf::MessageLite& to_msg,
                        const ::google::protobuf::MessageLite& from_msg);

  public:
  bool IsInitialized() const {
    return true;
  }
  ABSL_ATTRIBUTE_REINITIALIZES void Clear() PROTOBUF_FINAL;
  #if defined(PROTOBUF_CUSTOM_VTABLE)
  private:
  static ::size_t ByteSizeLong(const ::google::protobuf::MessageLite& msg);
  static ::uint8_t* PROTOBUF_NONNULL _InternalSerialize(
      const ::google::protobuf::MessageLite& msg, ::uint8_t* PROTOBUF_NONNULL target,
      ::google::protobuf::io::EpsCopyOutputStream* PROTOBUF_NONNULL stream);

  public:
  ::size_t ByteSizeLong() const { return ByteSizeLong(*this); }
  ::uint8_t* PROTOBUF_NONNULL _InternalSerialize(
      ::uint8_t* PROTOBUF_NONNULL target,
      ::google::protobuf::io::EpsCopyOutputStream* PROTOBUF_NONNULL stream) const {
    return _InternalSerialize(*this, target, stream);
  }
  #else   // PROTOBUF_CUSTOM_VTABLE
  ::size_t ByteSizeLong() const final;
  ::uint8_t* PROTOBUF_NONNULL _InternalSerialize(
      ::uint8_t* PROTOBUF_NONNULL target,
      ::google::protobuf::io::EpsCopyOutputStream* PROTOBUF_NONNULL stream) const final;
  #endif  // PROTOBUF_CUSTOM_VTABLE
  int GetCachedSize() const { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::google::protobuf::Arena* PROTOBUF_NULLABLE arena);
  static void SharedDtor(MessageLite& self);
  void InternalSwap(Counter* PROTOBUF_NONNULL other);
 private:
  template <typename T>
  friend ::absl::string_view(::google::protobuf::internal::GetAnyMessageName)();
  static ::absl::string_view FullMessageName() { return "Counter"; }

 protected:
  explicit Counter(::google::protobuf::Arena* PROTOBUF_NULLABLE arena);
  Counter(::google::protobuf::Arena* PROTOBUF_NULLABLE arena, const Counter& from);
  Counter(
      ::google::protobuf::Arena* PROTOBUF_NULLABLE arena, Counter&& from) noexcept
      : Counter(arena) {
    *this = ::std::move(from);
  }
  const ::google::protobuf::internal::ClassData* PROTOBUF_NONNULL GetClassData() const PROTOBUF_FINAL;
  static void* PROTOBUF_NONNULL PlacementNew_(
      const void* PROTOBUF_NONNULL, void* PROTOBUF_NONNULL mem,
      ::google::protobuf::Arena* PROTOBUF_NULLABLE arena);
  static constexpr auto InternalNewImpl_();

 public:
  static constexpr auto InternalGenerateClassData_();

  ::google::protobuf::Metadata GetMetadata() const;
  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------
  enum : int {
    kValueFieldNumber = 1,
  };
  // int32 value = 1;
  void clear_value() ;
  ::int32_t value() const;
  void set_value(::int32_t value);

  private:
  ::int32_t _internal_value() const;
  void _internal_set_value(::int32_t value);

  public:
  // @@protoc_insertion_point(class_scope:Counter)
 private:
  class _Internal;
  friend class ::google::protobuf::internal::TcParser;
  static const ::google::protobuf::internal::TcParseTable<0, 1,
                                   0, 0,
                                   2>
      _table_;

  friend class ::google::protobuf::MessageLite;
  friend class ::google::protobuf::Arena;
  template <typename T>
  friend class ::google::protobuf::Arena::InternalHelper;
  using InternalArenaConstructable_ = void;
  using DestructorSkippable_ = void;
  struct Impl_ {
    inline explicit constexpr Impl_(::google::protobuf::internal::ConstantInitialized) noexcept;
    inline explicit Impl_(
        ::google::protobuf::internal::InternalVisibility visibility,
        ::google::protobuf::Arena* PROTOBUF_NULLABLE arena);
    inline explicit Impl_(
        ::google::protobuf::internal::InternalVisibility visibility,
        ::google::protobuf::Arena* PROTOBUF_NULLABLE arena, const Impl_& from,
        const Counter& from_msg);
    ::google::protobuf::internal::HasBits<1> _has_bits_;
    ::google::protobuf::internal::CachedSize _cached_size_;
    ::int32_t value_;
    PROTOBUF_TSAN_DECLARE_MEMBER
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_counter_2eproto;
};

extern const ::google::protobuf::internal::ClassDataFull Counter_class_data_;

// ===================================================================




// ===================================================================


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// -------------------------------------------------------------------

// Counter

// int32 value = 1;
inline void Counter::clear_value() {
  ::google::protobuf::internal::TSanWrite(&_impl_);
  _impl_.value_ = 0;
  _impl_._has_bits_[0] &= ~0x00000001u;
}
inline ::int32_t Counter::value() const {
  // @@protoc_insertion_point(field_get:Counter.value)
  return _internal_value();
}
inline void Counter::set_value(::int32_t value) {
  _internal_set_value(value);
  _impl_._has_bits_[0] |= 0x00000001u;
  // @@protoc_insertion_point(field_set:Counter.value)
}
inline ::int32_t Counter::_internal_value() const {
  ::google::protobuf::internal::TSanRead(&_impl_);
  return _impl_.value_;
}
inline void Counter::_internal_set_value(::int32_t value) {
  ::google::protobuf::internal::TSanWrite(&_impl_);
  _impl_.value_ = value;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include "google/protobuf/port_undef.inc"

#endif  // counter_2eproto_2epb_2eh
