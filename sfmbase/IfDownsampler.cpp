// airspy-fmradion
// Software decoder for FM broadcast radio with Airspy
//
// Copyright (C) 2015 Edouard Griffiths, F4EXB
// Copyright (C) 2019 Kenji Rikitake, JJ1BDX
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <cassert>
#include <cmath>

#include "IfDownsampler.h"

// Class IfDownsampler

IfDownsampler::IfDownsampler(unsigned int first_downsample,
                             const IQSampleCoeff &first_coeff,
                             bool enable_second_downsampler,
                             unsigned int second_downsample,
                             const IQSampleCoeff &second_coeff)

    // Initialize member fields
    : m_first_downsample(first_downsample),
      m_second_downsample(second_downsample),
      m_enable_second_downsampler(enable_second_downsampler),
      // Construct LowPassFilterFirIQ
      m_iffilter_first(first_coeff, m_first_downsample),
      m_iffilter_second(second_coeff, m_second_downsample)

{
  // do nothing
}

void IfDownsampler::process(const IQSampleVector &samples_in,
                            IQSampleVector &samples_out) {

  // First stage of the low pass filters to isolate station,
  // and perform first stage decimation.
  m_iffilter_first.process(samples_in, m_buf_iffirstout);

  if (m_enable_second_downsampler) {
    // Second stage of the low pass filters to isolate station,
    m_iffilter_second.process(m_buf_iffirstout, samples_out);
  } else {
    // No second downsampling here
    samples_out = std::move(m_buf_iffirstout);
  }
}

/* end */
