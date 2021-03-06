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
#if defined(_MSC_VER)
#  pragma warning(disable : 4786)
#endif

#include "itkVideoIOBase.h"

namespace itk
{

VideoIOBase::VideoIOBase()
  : m_FrameTotal(NumericTraits<SizeValueType>::ZeroValue())
  , m_CurrentFrame(NumericTraits<SizeValueType>::ZeroValue())
  , m_IFrameInterval(NumericTraits<SizeValueType>::ZeroValue())
  , m_LastIFrame(NumericTraits<SizeValueType>::ZeroValue())

{}

VideoIOBase::~VideoIOBase() = default;

void
VideoIOBase::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

/** Print Enumerations */
std::ostream &
operator<<(std::ostream & out, const VideoIOBase::ReadFromEnum value)
{
  return out << [value] {
    switch (value)
    {
      case VideoIOBase::ReadFromEnum::ReadFromFile:
        return "VideoIOBase::ReadFromEnum::ReadFromFile";
      case VideoIOBase::ReadFromEnum::ReadFromCamera:
        return "VideoIOBase::ReadFromEnum::ReadFromCamera";
      default:
        return "INVALID VALUE FOR VideoIOBase::ReadFromEnum";
    }
  }();
}
} // namespace itk
