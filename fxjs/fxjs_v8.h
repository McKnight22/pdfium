// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

// FXJS_V8 is a layer that makes it easier to define native objects in V8, but
// has no knowledge of PDF-specific native objects. It could in theory be used
// to implement other sets of native objects.

// PDFium code should include this file rather than including V8 headers
// directly.

#ifndef FXJS_FXJS_V8_H_
#define FXJS_FXJS_V8_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "fxjs/cjs_v8.h"
#include "v8/include/v8-util.h"
#include "v8/include/v8.h"

#ifdef PDF_ENABLE_XFA
// FXJS_V8 doesn't interpret this class, it is just passed along to XFA.
class CFXJSE_RuntimeData;
#endif  // PDF_ENABLE_XFA

class CFXJS_ObjDefinition;
class CJS_Object;

// FXJS_V8 places no restrictions on this class; it merely passes it
// on to caller-provided methods.
class IJS_EventContext;  // A description of the event that caused JS execution.

enum FXJSOBJTYPE {
  FXJSOBJTYPE_DYNAMIC = 0,  // Created by native method and returned to JS.
  FXJSOBJTYPE_STATIC,       // Created by init and hung off of global object.
  FXJSOBJTYPE_GLOBAL,       // The global object itself (may only appear once).
};

struct FXJSErr {
  const wchar_t* message;
  const wchar_t* srcline;
  unsigned linnum;
};

// Global weak map to save dynamic objects.
class V8TemplateMapTraits : public v8::StdMapTraits<void*, v8::Object> {
 public:
  typedef v8::GlobalValueMap<void*, v8::Object, V8TemplateMapTraits> MapType;
  typedef void WeakCallbackDataType;

  static WeakCallbackDataType*
  WeakCallbackParameter(MapType* map, void* key, v8::Local<v8::Object> value) {
    return key;
  }
  static MapType* MapFromWeakCallbackInfo(
      const v8::WeakCallbackInfo<WeakCallbackDataType>&);

  static void* KeyFromWeakCallbackInfo(
      const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {
    return data.GetParameter();
  }
  static const v8::PersistentContainerCallbackType kCallbackType =
      v8::kWeakWithInternalFields;
  static void DisposeWeak(
      const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {}
  static void OnWeakCallback(
      const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {}
  static void Dispose(v8::Isolate* isolate,
                      v8::Global<v8::Object> value,
                      void* key);
  static void DisposeCallbackData(WeakCallbackDataType* callbackData) {}
};

class V8TemplateMap {
 public:
  typedef v8::GlobalValueMap<void*, v8::Object, V8TemplateMapTraits> MapType;

  explicit V8TemplateMap(v8::Isolate* isolate);
  ~V8TemplateMap();

  void set(void* key, v8::Local<v8::Object> handle);

  friend class V8TemplateMapTraits;

 private:
  MapType m_map;
};

class FXJS_PerIsolateData {
 public:
  ~FXJS_PerIsolateData();

  static void SetUp(v8::Isolate* pIsolate);
  static FXJS_PerIsolateData* Get(v8::Isolate* pIsolate);

  std::vector<std::unique_ptr<CFXJS_ObjDefinition>> m_ObjectDefnArray;
#ifdef PDF_ENABLE_XFA
  std::unique_ptr<CFXJSE_RuntimeData> m_pFXJSERuntimeData;
#endif  // PDF_ENABLE_XFA
  std::unique_ptr<V8TemplateMap> m_pDynamicObjsMap;

 protected:
  explicit FXJS_PerIsolateData(v8::Isolate* pIsolate);
};

class FXJS_ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
  static const size_t kMaxAllowedBytes = 0x10000000;
  void* Allocate(size_t length) override;
  void* AllocateUninitialized(size_t length) override;
  void Free(void* data, size_t length) override;
};

void FXJS_Initialize(unsigned int embedderDataSlot, v8::Isolate* pIsolate);
void FXJS_Release();

// Gets the global isolate set by FXJS_Initialize(), or makes a new one each
// time if there is no such isolate. Returns true if a new isolate had to be
// created.
bool FXJS_GetIsolate(v8::Isolate** pResultIsolate);

// Get the global isolate's ref count.
size_t FXJS_GlobalIsolateRefCount();

class CFXJS_Engine : public CJS_V8 {
 public:
  explicit CFXJS_Engine(v8::Isolate* pIsolate);
  ~CFXJS_Engine() override;

  using Constructor = void (*)(CFXJS_Engine* pEngine,
                               v8::Local<v8::Object> obj);
  using Destructor = void (*)(CFXJS_Engine* pEngine, v8::Local<v8::Object> obj);

  static CFXJS_Engine* EngineFromIsolateCurrentContext(v8::Isolate* pIsolate);
  static CFXJS_Engine* EngineFromContext(v8::Local<v8::Context> pContext);
  static void SetEngineInContext(CFXJS_Engine* pEngine,
                                 v8::Local<v8::Context> pContext);

  static int GetObjDefnID(v8::Local<v8::Object> pObj);

  // Always returns a valid, newly-created objDefnID.
  int DefineObj(const char* sObjName,
                FXJSOBJTYPE eObjType,
                Constructor pConstructor,
                Destructor pDestructor);

  void DefineObjMethod(int nObjDefnID,
                       const char* sMethodName,
                       v8::FunctionCallback pMethodCall);
  void DefineObjProperty(int nObjDefnID,
                         const char* sPropName,
                         v8::AccessorGetterCallback pPropGet,
                         v8::AccessorSetterCallback pPropPut);
  void DefineObjAllProperties(int nObjDefnID,
                              v8::NamedPropertyQueryCallback pPropQurey,
                              v8::NamedPropertyGetterCallback pPropGet,
                              v8::NamedPropertySetterCallback pPropPut,
                              v8::NamedPropertyDeleterCallback pPropDel);
  void DefineObjConst(int nObjDefnID,
                      const char* sConstName,
                      v8::Local<v8::Value> pDefault);
  void DefineGlobalMethod(const char* sMethodName,
                          v8::FunctionCallback pMethodCall);
  void DefineGlobalConst(const wchar_t* sConstName,
                         v8::FunctionCallback pConstGetter);

  // Called after FXJS_Define* calls made.
  void InitializeEngine();
  void ReleaseEngine();

  // Called after FXJS_InitializeEngine call made.
  int Execute(const WideString& script, FXJSErr* perror);

  v8::Local<v8::Object> GetThisObj();
  v8::Local<v8::Object> NewFXJSBoundObject(int nObjDefnID,
                                           bool bStatic = false);

  // Native object binding.
  void SetObjectPrivate(v8::Local<v8::Object> pObj,
                        std::unique_ptr<CJS_Object> p);
  CJS_Object* GetObjectPrivate(v8::Local<v8::Object> pObj);
  static void FreeObjectPrivate(v8::Local<v8::Object> pObj);

  void Error(const WideString& message);

  v8::Local<v8::Context> GetV8Context() {
    return v8::Local<v8::Context>::New(GetIsolate(), m_V8Context);
  }

  v8::Local<v8::Array> GetConstArray(const WideString& name);
  void SetConstArray(const WideString& name, v8::Local<v8::Array> array);

 protected:
  CFXJS_Engine();

 private:
  v8::Global<v8::Context> m_V8Context;
  std::vector<v8::Global<v8::Object>> m_StaticObjects;
  std::map<WideString, v8::Global<v8::Array>> m_ConstArrays;
};

#endif  // FXJS_FXJS_V8_H_
