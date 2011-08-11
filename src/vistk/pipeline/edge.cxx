/*ckwg +5
 * Copyright 2011 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include "edge.h"
#include "edge_exception.h"

#include "stamp.h"

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

#include <queue>

/**
 * \file edge.cxx
 *
 * \brief Implementation of the \link vistk::edge edge\endlink class.
 */

namespace vistk
{

class edge::priv
{
  public:
    priv();
    ~priv();

    bool has_data() const;

    bool required;

    process_t upstream;
    process_t downstream;

    std::queue<edge_datum_t> q;

    boost::condition_variable cond_have_data;

    mutable boost::mutex mutex;
};

edge
::edge(config_t const& config)
{
  if (!config)
  {
    throw null_edge_config_exception();
  }

  d = boost::shared_ptr<priv>(new priv);
}

edge
::~edge()
{
}

bool
edge
::makes_dependency() const
{
  return true;
}

bool
edge
::has_data() const
{
  return (datum_count() != 0);
}

bool
edge
::full_of_data() const
{
  return false;
}

size_t
edge
::datum_count() const
{
  boost::mutex::scoped_lock lock(d->mutex);

  (void)lock;

  return d->q.size();
}

void
edge
::push_datum(edge_datum_t const& datum)
{
  boost::mutex::scoped_lock lock(d->mutex);

  (void)lock;

  d->q.push(datum);
}

edge_datum_t
edge
::get_datum()
{
  edge_datum_t const dat = peek_datum();

  pop_datum();

  return dat;
}

edge_datum_t
edge
::peek_datum()
{
  boost::mutex::scoped_lock lock(d->mutex, boost::defer_lock);

  while (!has_data())
  {
    d->cond_have_data.wait(lock);
  }

  lock.lock();

  return d->q.front();
}

void
edge
::pop_datum()
{
  boost::mutex::scoped_lock lock(d->mutex, boost::defer_lock);

  while (!has_data())
  {
    d->cond_have_data.wait(lock);
  }

  lock.lock();

  d->q.pop();
}

void
edge
::set_required_by_downstream(bool required)
{
  d->required = required;
}

bool
edge
::required_by_downstream() const
{
  return d->required;
}

void
edge
::set_upstream_process(process_t process)
{
  if (!process)
  {
    throw null_process_connection_exception();
  }

  if (d->upstream)
  {
    throw input_already_connected_exception(d->upstream->name(), process->name());
  }

  d->upstream = process;
}

void
edge
::set_downstream_process(process_t process)
{
  if (!process)
  {
    throw null_process_connection_exception();
  }

  if (d->downstream)
  {
    throw output_already_connected_exception(d->downstream->name(), process->name());
  }

  d->downstream = process;
}

bool
operator == (edge_datum_t const& a, edge_datum_t const& b)
{
  return (( a.get<0>() ==  b.get<0>()) &&
          (*a.get<1>() == *b.get<1>()));
}

edge::priv
::priv()
  : required(true)
{
}

edge::priv
::~priv()
{
}

bool
edge::priv
::has_data() const
{
  return (q.size() != 0);
}

}
