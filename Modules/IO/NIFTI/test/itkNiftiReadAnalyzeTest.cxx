/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include <iostream>
#include <fstream>
#include "itkByteSwapper.h"
#include "itkNiftiImageIO.h"
#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"
#include "itkMath.h"
#include "itkSpatialOrientationAdapter.h"

// debug
#include <map>

namespace
{
//
// Analyze 7.5 header -- this describes the data below,
// as an 6 x 6 x 8 image of float pixels
const unsigned char LittleEndian_hdr[] = {
  0x5c, 0x01, 0x00, 0x00, 0x46, 0x4c, 0x4f, 0x41, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,
  0x72, 0x00, 0x04, 0x00, 0x06, 0x00, 0x06, 0x00, 0x08, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80,
  0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
//
// float data, represented as a char stream, in little-endian
// order
const unsigned char LittleEndian_img[] = {
  0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80,
  0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00,
  0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00,
  0x00, 0x10, 0x43, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x50, 0x43,
  0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0,
  0x42, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00,
  0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00,
  0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43,
  0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x10,
  0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00,
  0x80, 0x41, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x80, 0x41, 0x00,
  0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43,
  0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50,
  0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00,
  0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00,
  0x00, 0xa0, 0x42, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x80, 0x41,
  0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10,
  0x43, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00,
  0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00,
  0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42,
  0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0xa0,
  0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00,
  0x50, 0x43, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0x10, 0x43, 0x00,
  0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41,
  0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80,
  0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00, 0x10, 0x43, 0x00, 0x00,
  0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x80, 0x41, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00,
  0x00, 0x50, 0x43, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0x50, 0x43,
  0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xa0,
  0x42, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0x50, 0x43, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00,
  0xa0, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00,
  0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43,
  0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x30,
  0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00,
  0x40, 0x42, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0xe0, 0x42, 0x00,
  0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43,
  0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70,
  0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00,
  0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00,
  0x00, 0x40, 0x42, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x40, 0x42,
  0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30,
  0x43, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00,
  0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00,
  0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42,
  0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0xe0,
  0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00,
  0x30, 0x43, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x30, 0x43, 0x00,
  0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42,
  0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40,
  0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00,
  0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00,
  0x00, 0x70, 0x43, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0x70, 0x43,
  0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0,
  0x42, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00,
  0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43, 0x00,
  0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x30, 0x43,
  0x00, 0x00, 0x30, 0x43, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x70,
  0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00,
  0xe0, 0x42, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0xe0, 0x42, 0x00,
  0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43, 0x00, 0x00, 0x70, 0x43,
  0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42, 0x00, 0x00, 0xe0, 0x42,
};

// Map between axis string labels and SpatialOrientation
std::map<itk::SpatialOrientation::ValidCoordinateOrientationFlags, std::string> codeToString = {
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP, "RIP" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LIP, "LIP" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSP, "RSP" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LSP, "LSP" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIA, "RIA" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LIA, "LIA" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSA, "RSA" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LSA, "LSA" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IRP, "IRP" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ILP, "ILP" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SRP, "SRP" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SLP, "SLP" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IRA, "IRA" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ILA, "ILA" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SRA, "SRA" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SLA, "SLA" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPI, "RPI" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPI, "LPI" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI, "RAI" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LAI, "LAI" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPS, "RPS" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPS, "LPS" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAS, "RAS" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LAS, "LAS" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PRI, "PRI" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PLI, "PLI" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ARI, "ARI" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ALI, "ALI" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PRS, "PRS" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PLS, "PLS" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ARS, "ARS" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ALS, "ALS" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IPR, "IPR" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SPR, "SPR" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IAR, "IAR" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SAR, "SAR" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IPL, "IPL" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SPL, "SPL" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IAL, "IAL" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SAL, "SAL" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIR, "PIR" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PSR, "PSR" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_AIR, "AIR" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ASR, "ASR" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIL, "PIL" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PSL, "PSL" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_AIL, "AIL" },
  { itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ASL, "ASL" }
};

/** WriteFile
 * Write out a char array as binary
 */
int
WriteFile(const std::string & name, const unsigned char * buf, size_t buflen)
{
  std::ofstream f(name.c_str(), std::ios::binary | std::ios::out);
  if (!f.is_open())
  {
    return EXIT_FAILURE;
  }
  f.write(reinterpret_cast<const char *>(buf), buflen);
  f.close();
  return EXIT_SUCCESS;
}

/** ReadFile
 * read an image from disk
 */
template <typename TImage>
typename TImage::Pointer
ReadImage(const std::string & fileName)
{
  using ReaderType = itk::ImageFileReader<TImage>;

  typename ReaderType::Pointer reader = ReaderType::New();
  {
    reader->SetFileName(fileName.c_str());
    reader->SetImageIO(itk::NiftiImageIO::New());
    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cout << "Caught an exception: " << std::endl;
      std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
      throw;
    }
    catch (...)
    {
      std::cout << "Error while reading in image  " << fileName << std::endl;
      throw;
    }
  }
  typename TImage::Pointer image = reader->GetOutput();
  return image;
}

} // namespace


int
itkNiftiAnalyzeContentsAndCoordinatesTest(char *                                                   av[],
                                          unsigned char                                            hist_orient_code,
                                          itk::SpatialOrientation::ValidCoordinateOrientationFlags expected_code)
{
  std::string hdrName(av[1]);
  hdrName += "/littleEndian_";
  hdrName += codeToString[expected_code];
  hdrName += ".hdr";
  std::string imgName(av[1]);
  imgName += "/littleEndian_";
  imgName += codeToString[expected_code];
  imgName += ".img";
  // hack the header to have proper orientation code
  unsigned char tweaked_hdr[sizeof(LittleEndian_hdr)];
  memcpy(tweaked_hdr, LittleEndian_hdr, sizeof(LittleEndian_hdr));
  tweaked_hdr[252] = hist_orient_code;

  if (WriteFile(hdrName, tweaked_hdr, sizeof(LittleEndian_hdr)) != EXIT_SUCCESS)
  {
    std::cerr << "itkNiftiAnalyzeContentsAndCoordinatesTest: failed to write " << hdrName << std::endl;
    return EXIT_FAILURE;
  }
  if (WriteFile(imgName, LittleEndian_img, sizeof(LittleEndian_img)) != EXIT_SUCCESS)
  {
    std::cerr << "itkNiftiAnalyzeContentsAndCoordinatesTest: failed to write " << imgName << std::endl;
    return EXIT_FAILURE;
  }
  //
  // read the image just written back in.
  using ImageType = itk::Image<float, 3>;
  ImageType::Pointer img;
  try
  {
    img = ReadImage<ImageType>(hdrName);
  }
  catch (...)
  {
    return EXIT_FAILURE;
  }

  const auto *                             fPtr = reinterpret_cast<const float *>(LittleEndian_img);
  itk::ImageRegionConstIterator<ImageType> it(img, img->GetLargestPossibleRegion());
  it.GoToBegin();
  for (; !it.IsAtEnd(); ++it, ++fPtr)
  {
    //
    // in the unlikely event we're testing on a big-endian machine, do
    // byte swapping on floats pulled from the little-endian array.
    float cur = *fPtr;
    itk::ByteSwapper<float>::SwapFromSystemToLittleEndian(&cur);
    if (itk::Math::NotExactlyEquals(it.Get(), cur))
    {
      std::cerr << "itkNiftiAnalyzeContentsAndCoordinatesTest: expected pixel value " << cur << " but found "
                << it.Get() << std::endl;
      return EXIT_FAILURE;
    }
  }

  itk::SpatialOrientation::ValidCoordinateOrientationFlags orientation_code =
    itk::SpatialOrientationAdapter().FromDirectionCosines(img->GetDirection());
  // verify the correct orientation :
  if (orientation_code != expected_code)
  {
    std::cerr << "Analyze orientation " << (int)hist_orient_code << std::endl;
    std::cerr << "expected orientation " << codeToString[expected_code] << " but found "
              << codeToString[orientation_code] << std::endl;
    return EXIT_FAILURE;
  }

  // TODO: check origin and spacing too
  std::cout << "Analyze orientation " << (int)hist_orient_code << std::endl
            << "Origin   :" << img->GetOrigin() << std::endl
            << "Spacing  :" << img->GetSpacing() << std::endl
            << "Code     :" << codeToString[orientation_code] << std::endl
            << "Direction:" << img->GetDirection() << std::endl;

  return EXIT_SUCCESS;
}

int
itkNiftiReadAnalyzeTest(int ac, char * av[])
{
  if (ac < 2)
  {
    std::cerr << "itkNiftiReadAnalyzeTest: Missing test directory argument" << std::endl;
    return EXIT_FAILURE;
  }

  // NOTE: according to the information from
  // https://web.archive.org/web/20121116093304/http://wideman-one.com/gw/brain/analyze/formatdoc.htm Analyze code 5
  // should have been PSR but it was revised in NIFTI somehow to PIL

  return itkNiftiAnalyzeContentsAndCoordinatesTest(av, 0, itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPI) ==
               EXIT_FAILURE ||
             itkNiftiAnalyzeContentsAndCoordinatesTest(
               av, 1, itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP) == EXIT_FAILURE ||
             itkNiftiAnalyzeContentsAndCoordinatesTest(
               av, 2, itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIR) == EXIT_FAILURE ||
             itkNiftiAnalyzeContentsAndCoordinatesTest(
               av, 3, itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI) == EXIT_FAILURE ||
             itkNiftiAnalyzeContentsAndCoordinatesTest(
               av, 4, itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSP) == EXIT_FAILURE ||
             itkNiftiAnalyzeContentsAndCoordinatesTest(
               av, 5, itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIL) == EXIT_FAILURE
           ? EXIT_FAILURE
           : EXIT_SUCCESS;
}
