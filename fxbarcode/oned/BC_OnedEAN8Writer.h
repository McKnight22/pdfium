// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_ONED_BC_ONEDEAN8WRITER_H_
#define FXBARCODE_ONED_BC_ONEDEAN8WRITER_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "fxbarcode/BC_Library.h"
#include "fxbarcode/oned/BC_OneDimWriter.h"

class CFX_DIBitmap;
class CFX_RenderDevice;

class CBC_OnedEAN8Writer : public CBC_OneDimWriter {
 public:
  CBC_OnedEAN8Writer();
  ~CBC_OnedEAN8Writer() override;

  // CBC_OneDimWriter
  uint8_t* EncodeWithHint(const CFX_ByteString& contents,
                          BCFORMAT format,
                          int32_t& outWidth,
                          int32_t& outHeight,
                          int32_t hints) override;
  uint8_t* EncodeImpl(const CFX_ByteString& contents,
                      int32_t& outLength) override;
  bool CheckContentValidity(const CFX_WideStringC& contents) override;
  CFX_WideString FilterContents(const CFX_WideStringC& contents) override;
  void SetDataLength(int32_t length) override;

  bool SetTextLocation(BC_TEXT_LOC location);
  int32_t CalcChecksum(const CFX_ByteString& contents);

 protected:
  bool ShowChars(const CFX_WideStringC& contents,
                 CFX_RenderDevice* device,
                 const CFX_Matrix* matrix,
                 int32_t barWidth,
                 int32_t multiple) override;

 private:
  int32_t m_codeWidth;
};

#endif  // FXBARCODE_ONED_BC_ONEDEAN8WRITER_H_
