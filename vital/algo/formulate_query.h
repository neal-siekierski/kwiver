/*ckwg +29
 * Copyright 2014-2015 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef VITAL_ALGO_FORMULATE_QUERY_H_
#define VITAL_ALGO_FORMULATE_QUERY_H_

#include <vital/vital_config.h>

#include <string>
#include <memory>

#include <vital/algo/algorithm.h>
#include <vital/types/image_container.h>
#include <vital/types/track_descriptor_set.h>
#include <vital/types/descriptor_request.h>

namespace kwiver {
namespace vital {
namespace algo {

/// An abstract base class for formulating descriptors for queries
class VITAL_ALGO_EXPORT formulate_query
  : public kwiver::vital::algorithm_def<formulate_query>
{
public:
  /// Return the name of this algorithm
  static std::string static_type_name() { return "formulate_query"; }

  /// Set this algorithm's properties via a config block
  virtual void set_configuration( kwiver::vital::config_block_sptr config );
  /// Check that the algorithm's current configuration is valid
  virtual bool check_configuration( kwiver::vital::config_block_sptr config ) const;

  /// Formulate query
  virtual kwiver::vital::track_descriptor_set_sptr formulate(
    kwiver::vital::descriptor_request_sptr request ) = 0;

protected:
  formulate_query();

};

typedef std::shared_ptr<formulate_query> formulate_query_sptr;

} } } // end namespace

#endif // VITAL_ALGO_CONVERT_IMAGE_H_