/*ckwg +29
 * Copyright 2016 by Kitware, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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

#include "draw_detected_object_boxes_process.h"

#include <vital/vital_types.h>

#include <arrows/processes/kwiver_type_traits.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <arrows/algorithms/ocv/image_container.h>
#include <arrows/algorithms/ocv/image_container.h>


#include <sstream>
#include <iostream>
// 
namespace kwiver
{

create_config_trait( threshold, float, "0.6", "min probablity for output (float)" );
create_config_trait( ignore_file, std::string, "__background__", "List of classes to ingore, seperated by semi-colon." );
create_config_trait( file_string, std::string, "", "If not empty, use this as a formated string to write output (i.e. out_%5d.png)" );

class draw_detected_object_boxes_process::priv
{
public:
  priv()
  :m_count(0)
  {}

  ~priv()
  {}

  mutable size_t m_count;
  std::string m_formated_string;
  // Configuration values
  float m_threshold;
  std::vector<std::string> m_ignore_classes;

  vital::image_container_sptr draw_on_image( vital::image_container_sptr image_data,
                                             vital::detected_object_set_sptr input_set) const
  {
    char buffer[1024];
    if(image_data == NULL || input_set == NULL) return NULL;
    cv::Mat image = arrows::ocv::image_container::vital_to_ocv(image_data->get_image());
    cv::Mat overlay;
    vital::object_labels::iterator label_iter = input_set->get_labels();
    for(;!label_iter.is_end(); ++label_iter)
    {
      bool keep_going = true;
      for(size_t i = 0; i < this->m_ignore_classes.size(); ++i)
      {
        if(this->m_ignore_classes[i] == label_iter.get_label())
        {
          keep_going = false;
          break;
        }
      }
      if(!keep_going) continue;
      vital::detected_object_set::iterator class_iterator = input_set->get_iterator(label_iter.get_key(), true, this->m_threshold);
      double tmpT = (this->m_threshold -(this->m_threshold>=0.05)?0.05:0);
      for(size_t i = class_iterator.size()-1; i < class_iterator.size() && i >= 0; --i)
      {
        image.copyTo(overlay);
        vital::detected_object_sptr dos = class_iterator[i]; //Low score first
        vital::detected_object::bounding_box bbox = dos->get_bounding_box();
        cv::Rect r(bbox.min()[0], bbox.min()[1], bbox.sizes()[0], bbox.sizes()[1]);
        double prob = dos->get_classifications()->get_score(label_iter.get_key());
        std::string p = std::to_string(prob);
        std::string txt =label_iter.get_label() + " " + p;
        prob =  (prob-tmpT)/(1-tmpT);
        cv::Scalar red(255, 0, 0);
        cv::Scalar blue(0,0,255);
        cv::rectangle(overlay, r, red);
        {
          int fontface = cv::FONT_HERSHEY_SIMPLEX;
          double scale = 0.4;
          int thickness = 1;
          int baseline = 0;
          cv::Point pt(r.tl()+cv::Point(0,15));

          cv::Size text = cv::getTextSize(txt, fontface, scale, thickness, &baseline);
          cv::rectangle(overlay, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), cv::Scalar(0,0,0), CV_FILLED);
          cv::putText(overlay, txt, pt, fontface, scale, cv::Scalar(255,255,255), thickness, 8);
        }
        cv::addWeighted(overlay, prob, image, 1 - prob, 0, image);
      }
    }
    if(!m_formated_string.empty())
    {
      sprintf(buffer, m_formated_string.c_str(), m_count);
      m_count++;
      cv::imwrite(buffer, image);
    }
    return image_data;
  }
};

draw_detected_object_boxes_process
::draw_detected_object_boxes_process( vital::config_block_sptr const& config )
: process( config ),
  d( new draw_detected_object_boxes_process::priv )
{
  attach_logger( kwiver::vital::get_logger( name() ) ); // could use a better approach
  make_ports();
  make_config();
}

draw_detected_object_boxes_process
::~draw_detected_object_boxes_process()
{
}

void draw_detected_object_boxes_process::_configure()
{
  d->m_threshold = config_value_using_trait( threshold );
  d->m_formated_string = config_value_using_trait( file_string );
  std::string parsed, list = config_value_using_trait(ignore_file);
  std::stringstream ss(list);

  if(std::getline(ss,parsed,';'))
  {
    if(!parsed.empty())
    {
      d->m_ignore_classes.push_back(parsed);
    }
  }
}

void draw_detected_object_boxes_process::_step()
{
  vital::image_container_sptr img = grab_from_port_using_trait( image );
  vital::detected_object_set_sptr detections = grab_from_port_using_trait(detected_object_set);
  vital::image_container_sptr result = d->draw_on_image(img, detections);
  push_to_port_using_trait( image, result );
}

void draw_detected_object_boxes_process::make_ports()
{
  // Set up for required ports
  sprokit::process::port_flags_t required;
  sprokit::process::port_flags_t optional;

  required.insert( flag_required );

  // -- input --
  declare_input_port_using_trait( detected_object_set, required );
  declare_input_port_using_trait( image, required );

  //output
  declare_output_port_using_trait(image, optional);
}

void draw_detected_object_boxes_process::make_config()
{
  declare_config_using_trait( threshold );
  declare_config_using_trait( ignore_file ); 
  declare_config_using_trait( file_string );
}

}//end namespace
