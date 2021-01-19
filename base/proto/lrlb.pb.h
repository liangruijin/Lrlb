// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: lrlb.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_lrlb_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_lrlb_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3012000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3012003 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_lrlb_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_lrlb_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[3]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_lrlb_2eproto;
namespace lrlb {
class GetRouteResponse;
class GetRouteResponseDefaultTypeInternal;
extern GetRouteResponseDefaultTypeInternal _GetRouteResponse_default_instance_;
class GetRouteResquest;
class GetRouteResquestDefaultTypeInternal;
extern GetRouteResquestDefaultTypeInternal _GetRouteResquest_default_instance_;
class HostInfo;
class HostInfoDefaultTypeInternal;
extern HostInfoDefaultTypeInternal _HostInfo_default_instance_;
}  // namespace lrlb
PROTOBUF_NAMESPACE_OPEN
template<> ::lrlb::GetRouteResponse* Arena::CreateMaybeMessage<::lrlb::GetRouteResponse>(Arena*);
template<> ::lrlb::GetRouteResquest* Arena::CreateMaybeMessage<::lrlb::GetRouteResquest>(Arena*);
template<> ::lrlb::HostInfo* Arena::CreateMaybeMessage<::lrlb::HostInfo>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace lrlb {

enum MessageID : int {
  ID_UNKNOW = 0,
  ID_GetRouteRequest = 1,
  ID_GetRouteResponse = 2,
  MessageID_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  MessageID_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool MessageID_IsValid(int value);
constexpr MessageID MessageID_MIN = ID_UNKNOW;
constexpr MessageID MessageID_MAX = ID_GetRouteResponse;
constexpr int MessageID_ARRAYSIZE = MessageID_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MessageID_descriptor();
template<typename T>
inline const std::string& MessageID_Name(T enum_t_value) {
  static_assert(::std::is_same<T, MessageID>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function MessageID_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    MessageID_descriptor(), enum_t_value);
}
inline bool MessageID_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, MessageID* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<MessageID>(
    MessageID_descriptor(), name, value);
}
// ===================================================================

class HostInfo PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:lrlb.HostInfo) */ {
 public:
  inline HostInfo() : HostInfo(nullptr) {}
  virtual ~HostInfo();

  HostInfo(const HostInfo& from);
  HostInfo(HostInfo&& from) noexcept
    : HostInfo() {
    *this = ::std::move(from);
  }

  inline HostInfo& operator=(const HostInfo& from) {
    CopyFrom(from);
    return *this;
  }
  inline HostInfo& operator=(HostInfo&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const HostInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const HostInfo* internal_default_instance() {
    return reinterpret_cast<const HostInfo*>(
               &_HostInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(HostInfo& a, HostInfo& b) {
    a.Swap(&b);
  }
  inline void Swap(HostInfo* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(HostInfo* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline HostInfo* New() const final {
    return CreateMaybeMessage<HostInfo>(nullptr);
  }

  HostInfo* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<HostInfo>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const HostInfo& from);
  void MergeFrom(const HostInfo& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(HostInfo* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "lrlb.HostInfo";
  }
  protected:
  explicit HostInfo(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_lrlb_2eproto);
    return ::descriptor_table_lrlb_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kIpFieldNumber = 1,
    kPortFieldNumber = 2,
  };
  // int32 ip = 1;
  void clear_ip();
  ::PROTOBUF_NAMESPACE_ID::int32 ip() const;
  void set_ip(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_ip() const;
  void _internal_set_ip(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // int32 port = 2;
  void clear_port();
  ::PROTOBUF_NAMESPACE_ID::int32 port() const;
  void set_port(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_port() const;
  void _internal_set_port(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // @@protoc_insertion_point(class_scope:lrlb.HostInfo)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::int32 ip_;
  ::PROTOBUF_NAMESPACE_ID::int32 port_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_lrlb_2eproto;
};
// -------------------------------------------------------------------

class GetRouteResquest PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:lrlb.GetRouteResquest) */ {
 public:
  inline GetRouteResquest() : GetRouteResquest(nullptr) {}
  virtual ~GetRouteResquest();

  GetRouteResquest(const GetRouteResquest& from);
  GetRouteResquest(GetRouteResquest&& from) noexcept
    : GetRouteResquest() {
    *this = ::std::move(from);
  }

  inline GetRouteResquest& operator=(const GetRouteResquest& from) {
    CopyFrom(from);
    return *this;
  }
  inline GetRouteResquest& operator=(GetRouteResquest&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const GetRouteResquest& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const GetRouteResquest* internal_default_instance() {
    return reinterpret_cast<const GetRouteResquest*>(
               &_GetRouteResquest_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(GetRouteResquest& a, GetRouteResquest& b) {
    a.Swap(&b);
  }
  inline void Swap(GetRouteResquest* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(GetRouteResquest* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline GetRouteResquest* New() const final {
    return CreateMaybeMessage<GetRouteResquest>(nullptr);
  }

  GetRouteResquest* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<GetRouteResquest>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const GetRouteResquest& from);
  void MergeFrom(const GetRouteResquest& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(GetRouteResquest* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "lrlb.GetRouteResquest";
  }
  protected:
  explicit GetRouteResquest(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_lrlb_2eproto);
    return ::descriptor_table_lrlb_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kModidFieldNumber = 1,
    kCmdidFieldNumber = 2,
  };
  // int32 modid = 1;
  void clear_modid();
  ::PROTOBUF_NAMESPACE_ID::int32 modid() const;
  void set_modid(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_modid() const;
  void _internal_set_modid(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // int32 cmdid = 2;
  void clear_cmdid();
  ::PROTOBUF_NAMESPACE_ID::int32 cmdid() const;
  void set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_cmdid() const;
  void _internal_set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // @@protoc_insertion_point(class_scope:lrlb.GetRouteResquest)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::int32 modid_;
  ::PROTOBUF_NAMESPACE_ID::int32 cmdid_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_lrlb_2eproto;
};
// -------------------------------------------------------------------

class GetRouteResponse PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:lrlb.GetRouteResponse) */ {
 public:
  inline GetRouteResponse() : GetRouteResponse(nullptr) {}
  virtual ~GetRouteResponse();

  GetRouteResponse(const GetRouteResponse& from);
  GetRouteResponse(GetRouteResponse&& from) noexcept
    : GetRouteResponse() {
    *this = ::std::move(from);
  }

  inline GetRouteResponse& operator=(const GetRouteResponse& from) {
    CopyFrom(from);
    return *this;
  }
  inline GetRouteResponse& operator=(GetRouteResponse&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const GetRouteResponse& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const GetRouteResponse* internal_default_instance() {
    return reinterpret_cast<const GetRouteResponse*>(
               &_GetRouteResponse_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  friend void swap(GetRouteResponse& a, GetRouteResponse& b) {
    a.Swap(&b);
  }
  inline void Swap(GetRouteResponse* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(GetRouteResponse* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline GetRouteResponse* New() const final {
    return CreateMaybeMessage<GetRouteResponse>(nullptr);
  }

  GetRouteResponse* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<GetRouteResponse>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const GetRouteResponse& from);
  void MergeFrom(const GetRouteResponse& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(GetRouteResponse* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "lrlb.GetRouteResponse";
  }
  protected:
  explicit GetRouteResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_lrlb_2eproto);
    return ::descriptor_table_lrlb_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kHostFieldNumber = 3,
    kModidFieldNumber = 1,
    kCmdidFieldNumber = 2,
  };
  // repeated .lrlb.HostInfo host = 3;
  int host_size() const;
  private:
  int _internal_host_size() const;
  public:
  void clear_host();
  ::lrlb::HostInfo* mutable_host(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::lrlb::HostInfo >*
      mutable_host();
  private:
  const ::lrlb::HostInfo& _internal_host(int index) const;
  ::lrlb::HostInfo* _internal_add_host();
  public:
  const ::lrlb::HostInfo& host(int index) const;
  ::lrlb::HostInfo* add_host();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::lrlb::HostInfo >&
      host() const;

  // int32 modid = 1;
  void clear_modid();
  ::PROTOBUF_NAMESPACE_ID::int32 modid() const;
  void set_modid(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_modid() const;
  void _internal_set_modid(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // int32 cmdid = 2;
  void clear_cmdid();
  ::PROTOBUF_NAMESPACE_ID::int32 cmdid() const;
  void set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_cmdid() const;
  void _internal_set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // @@protoc_insertion_point(class_scope:lrlb.GetRouteResponse)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::lrlb::HostInfo > host_;
  ::PROTOBUF_NAMESPACE_ID::int32 modid_;
  ::PROTOBUF_NAMESPACE_ID::int32 cmdid_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_lrlb_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// HostInfo

// int32 ip = 1;
inline void HostInfo::clear_ip() {
  ip_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 HostInfo::_internal_ip() const {
  return ip_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 HostInfo::ip() const {
  // @@protoc_insertion_point(field_get:lrlb.HostInfo.ip)
  return _internal_ip();
}
inline void HostInfo::_internal_set_ip(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  ip_ = value;
}
inline void HostInfo::set_ip(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_ip(value);
  // @@protoc_insertion_point(field_set:lrlb.HostInfo.ip)
}

// int32 port = 2;
inline void HostInfo::clear_port() {
  port_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 HostInfo::_internal_port() const {
  return port_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 HostInfo::port() const {
  // @@protoc_insertion_point(field_get:lrlb.HostInfo.port)
  return _internal_port();
}
inline void HostInfo::_internal_set_port(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  port_ = value;
}
inline void HostInfo::set_port(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_port(value);
  // @@protoc_insertion_point(field_set:lrlb.HostInfo.port)
}

// -------------------------------------------------------------------

// GetRouteResquest

// int32 modid = 1;
inline void GetRouteResquest::clear_modid() {
  modid_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteResquest::_internal_modid() const {
  return modid_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteResquest::modid() const {
  // @@protoc_insertion_point(field_get:lrlb.GetRouteResquest.modid)
  return _internal_modid();
}
inline void GetRouteResquest::_internal_set_modid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  modid_ = value;
}
inline void GetRouteResquest::set_modid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_modid(value);
  // @@protoc_insertion_point(field_set:lrlb.GetRouteResquest.modid)
}

// int32 cmdid = 2;
inline void GetRouteResquest::clear_cmdid() {
  cmdid_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteResquest::_internal_cmdid() const {
  return cmdid_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteResquest::cmdid() const {
  // @@protoc_insertion_point(field_get:lrlb.GetRouteResquest.cmdid)
  return _internal_cmdid();
}
inline void GetRouteResquest::_internal_set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  cmdid_ = value;
}
inline void GetRouteResquest::set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_cmdid(value);
  // @@protoc_insertion_point(field_set:lrlb.GetRouteResquest.cmdid)
}

// -------------------------------------------------------------------

// GetRouteResponse

// int32 modid = 1;
inline void GetRouteResponse::clear_modid() {
  modid_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteResponse::_internal_modid() const {
  return modid_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteResponse::modid() const {
  // @@protoc_insertion_point(field_get:lrlb.GetRouteResponse.modid)
  return _internal_modid();
}
inline void GetRouteResponse::_internal_set_modid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  modid_ = value;
}
inline void GetRouteResponse::set_modid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_modid(value);
  // @@protoc_insertion_point(field_set:lrlb.GetRouteResponse.modid)
}

// int32 cmdid = 2;
inline void GetRouteResponse::clear_cmdid() {
  cmdid_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteResponse::_internal_cmdid() const {
  return cmdid_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteResponse::cmdid() const {
  // @@protoc_insertion_point(field_get:lrlb.GetRouteResponse.cmdid)
  return _internal_cmdid();
}
inline void GetRouteResponse::_internal_set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  cmdid_ = value;
}
inline void GetRouteResponse::set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_cmdid(value);
  // @@protoc_insertion_point(field_set:lrlb.GetRouteResponse.cmdid)
}

// repeated .lrlb.HostInfo host = 3;
inline int GetRouteResponse::_internal_host_size() const {
  return host_.size();
}
inline int GetRouteResponse::host_size() const {
  return _internal_host_size();
}
inline void GetRouteResponse::clear_host() {
  host_.Clear();
}
inline ::lrlb::HostInfo* GetRouteResponse::mutable_host(int index) {
  // @@protoc_insertion_point(field_mutable:lrlb.GetRouteResponse.host)
  return host_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::lrlb::HostInfo >*
GetRouteResponse::mutable_host() {
  // @@protoc_insertion_point(field_mutable_list:lrlb.GetRouteResponse.host)
  return &host_;
}
inline const ::lrlb::HostInfo& GetRouteResponse::_internal_host(int index) const {
  return host_.Get(index);
}
inline const ::lrlb::HostInfo& GetRouteResponse::host(int index) const {
  // @@protoc_insertion_point(field_get:lrlb.GetRouteResponse.host)
  return _internal_host(index);
}
inline ::lrlb::HostInfo* GetRouteResponse::_internal_add_host() {
  return host_.Add();
}
inline ::lrlb::HostInfo* GetRouteResponse::add_host() {
  // @@protoc_insertion_point(field_add:lrlb.GetRouteResponse.host)
  return _internal_add_host();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::lrlb::HostInfo >&
GetRouteResponse::host() const {
  // @@protoc_insertion_point(field_list:lrlb.GetRouteResponse.host)
  return host_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace lrlb

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::lrlb::MessageID> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::lrlb::MessageID>() {
  return ::lrlb::MessageID_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_lrlb_2eproto