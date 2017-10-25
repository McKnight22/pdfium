// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/report.h"

#include <vector>

#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/javascript/JS_Object.h"
#include "fpdfsdk/javascript/JS_Value.h"

JSConstSpec CJS_Report::ConstSpecs[] = {{0, JSConstSpec::Number, 0, 0}};

JSPropertySpec CJS_Report::PropertySpecs[] = {{0, 0, 0}};

JSMethodSpec CJS_Report::MethodSpecs[] = {{"save", save_static},
                                          {"writeText", writeText_static},
                                          {0, 0}};

IMPLEMENT_JS_CLASS(CJS_Report, Report, Report)

Report::Report(CJS_Object* pJSObject) : CJS_EmbedObj(pJSObject) {}

Report::~Report() {}

CJS_Return Report::writeText(CJS_Runtime* pRuntime,
                             const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported.
  return CJS_Return(true);
}

CJS_Return Report::save(CJS_Runtime* pRuntime,
                        const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported.
  return CJS_Return(true);
}
