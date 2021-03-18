/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2018 Emanuel Eichhammer                            **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 25.06.18                                             **
**          Version: 2.0.1                                                **
****************************************************************************/
/*! \file */
#ifndef QCP_DATACONTAINER_H
#define QCP_DATACONTAINER_H

#include "global.h"
#include "axis/range.h"
#include "selection.h"

/*! \relates QCPDataContainer
  Returns whether the sort key of \a a is less than the sort key of \a b.

  \see QCPDataContainer::sort
*/
template <class DataType>
inline bool qcpLessThanSortKey(const DataType &a, const DataType &b) { return a.sortKey() < b.sortKey(); }

template <class DataType>
class QCPDataContainer // no QCP_LIB_DECL, template class ends up in header (cpp included below)
{
public:
  typedef typename QVector<DataType>::const_iterator const_iterator;
  typedef typename QVector<DataType>::iterator iterator;
  
  QCPDataContainer();
  
  // getters:
  int size() const { return mData.size()-mPreallocSize; }
  bool isEmpty() const { return size() == 0; }
  bool autoSqueeze() const { return mAutoSqueeze; }
  
  // setters:
  void setAutoSqueeze(bool enabled);
  
  // non-virtual methods:
  void set(const QCPDataContainer<DataType> &data);
  void set(const QVector<DataType> &data, bool alreadySorted=false);
  void add(const QCPDataContainer<DataType> &data);
  void add(const QVector<DataType> &data, bool alreadySorted=false);
  void add(const DataType &data);
  void removeBefore(double sortKey);
  void removeAfter(double sortKey);
  void remove(double sortKeyFrom, double sortKeyTo);
  void remove(double sortKey);
  void clear();
  void sort();
  void squeeze(bool preAllocation=true, bool postAllocation=true);
  
  const_iterator constBegin() const { return mData.constBegin()+mPreallocSize; }
  const_iterator constEnd() const { return mData.constEnd(); }
  iterator begin() { return mData.begin()+mPreallocSize; }
  iterator end() { return mData.end(); }
  const_iterator findBegin(double sortKey, bool expandedRange=true) const;
  const_iterator findEnd(double sortKey, bool expandedRange=true) const;
  const_iterator at(int index) const { return constBegin()+qBound(0, index, size()); }
  QCPRange keyRange(bool &foundRange, QCP::SignDomain signDomain=QCP::sdBoth);
  QCPRange valueRange(bool &foundRange, QCP::SignDomain signDomain=QCP::sdBoth, const QCPRange &inKeyRange=QCPRange());
  QCPDataRange dataRange() const { return QCPDataRange(0, size()); }
  void limitIteratorsToDataRange(const_iterator &begin, const_iterator &end, const QCPDataRange &dataRange) const;
  
protected:
  // property members:
  bool mAutoSqueeze;
  
  // non-property memebers:
  QVector<DataType> mData;
  int mPreallocSize;
  int mPreallocIteration;
  
  // non-virtual methods:
  void preallocateGrow(int minimumPreallocSize);
  void performAutoSqueeze();
};

/*!
  Constructs a QCPDataContainer used for plottable classes that represent a series of key-sorted
  data
*/
template <class DataType>
QCPDataContainer<DataType>::QCPDataContainer() :
  mAutoSqueeze(true),
  mPreallocSize(0),
  mPreallocIteration(0)
{
}

/*!
  Sets whether the container automatically decides when to release memory from its post- and
  preallocation pools when data points are removed. By default this is enabled and for typical
  applications shouldn't be changed.

  If auto squeeze is disabled, you can manually decide when to release pre-/postallocation with
  \ref squeeze.
*/
template <class DataType>
void QCPDataContainer<DataType>::setAutoSqueeze(bool enabled)
{
  if (mAutoSqueeze != enabled)
  {
    mAutoSqueeze = enabled;
    if (mAutoSqueeze)
      performAutoSqueeze();
  }
}

/*! \overload

  Replaces the current data in this container with the provided \a data.

  \see add, remove
*/
template <class DataType>
void QCPDataContainer<DataType>::set(const QCPDataContainer<DataType> &data)
{
  clear();
  add(data);
}

/*! \overload

  Replaces the current data in this container with the provided \a data

  If you can guarantee that the data points in \a data have ascending order with respect to the
  DataType's sort key, set \a alreadySorted to true to avoid an unnecessary sorting run.

  \see add, remove
*/
template <class DataType>
void QCPDataContainer<DataType>::set(const QVector<DataType> &data, bool alreadySorted)
{
  mData = data;
  mPreallocSize = 0;
  mPreallocIteration = 0;
  if (!alreadySorted)
    sort();
}

/*! \overload

  Adds the provided \a data to the current data in this container.

  \see set, remove
*/
template <class DataType>
void QCPDataContainer<DataType>::add(const QCPDataContainer<DataType> &data)
{
  if (data.isEmpty())
    return;

  const int n = data.size();
  const int oldSize = size();

  if (oldSize > 0 && !qcpLessThanSortKey<DataType>(*constBegin(), *(data.constEnd()-1))) // prepend if new data keys are all smaller than or equal to existing ones
  {
    if (mPreallocSize < n)
      preallocateGrow(n);
    mPreallocSize -= n;
    std::copy(data.constBegin(), data.constEnd(), begin());
  } else // don't need to prepend, so append and merge if necessary
  {
    mData.resize(mData.size()+n);
    std::copy(data.constBegin(), data.constEnd(), end()-n);
    if (oldSize > 0 && !qcpLessThanSortKey<DataType>(*(constEnd()-n-1), *(constEnd()-n))) // if appended range keys aren't all greater than existing ones, merge the two partitions
      std::inplace_merge(begin(), end()-n, end(), qcpLessThanSortKey<DataType>);
  }
}

/*!
  Adds the provided data points in \a data to the current data.

  If you can guarantee that the data points in \a data have ascending order with respect to the
  DataType's sort key, set \a alreadySorted to true to avoid an unnecessary sorting run.

  \see set, remove
*/
template <class DataType>
void QCPDataContainer<DataType>::add(const QVector<DataType> &data, bool alreadySorted)
{
  if (data.isEmpty())
    return;
  if (isEmpty())
  {
    set(data, alreadySorted);
    return;
  }

  const int n = data.size();
  const int oldSize = size();

  if (alreadySorted && oldSize > 0 && !qcpLessThanSortKey<DataType>(*constBegin(), *(data.constEnd()-1))) // prepend if new data is sorted and keys are all smaller than or equal to existing ones
  {
    if (mPreallocSize < n)
      preallocateGrow(n);
    mPreallocSize -= n;
    std::copy(data.constBegin(), data.constEnd(), begin());
  } else // don't need to prepend, so append and then sort and merge if necessary
  {
    mData.resize(mData.size()+n);
    std::copy(data.constBegin(), data.constEnd(), end()-n);
    if (!alreadySorted) // sort appended subrange if it wasn't already sorted
      std::sort(end()-n, end(), qcpLessThanSortKey<DataType>);
    if (oldSize > 0 && !qcpLessThanSortKey<DataType>(*(constEnd()-n-1), *(constEnd()-n))) // if appended range keys aren't all greater than existing ones, merge the two partitions
      std::inplace_merge(begin(), end()-n, end(), qcpLessThanSortKey<DataType>);
  }
}

/*! \overload

  Adds the provided single data point to the current data.

  \see remove
*/
template <class DataType>
void QCPDataContainer<DataType>::add(const DataType &data)
{
  if (isEmpty() || !qcpLessThanSortKey<DataType>(data, *(constEnd()-1))) // quickly handle appends if new data key is greater or equal to existing ones
  {
    mData.append(data);
  } else if (qcpLessThanSortKey<DataType>(data, *constBegin()))  // quickly handle prepends using preallocated space
  {
    if (mPreallocSize < 1)
      preallocateGrow(1);
    --mPreallocSize;
    *begin() = data;
  } else // handle inserts, maintaining sorted keys
  {
    QCPDataContainer<DataType>::iterator insertionPoint = std::lower_bound(begin(), end(), data, qcpLessThanSortKey<DataType>);
    mData.insert(insertionPoint, data);
  }
}

/*!
  Removes all data points with (sort-)keys smaller than or equal to \a sortKey.

  \see removeAfter, remove, clear
*/
template <class DataType>
void QCPDataContainer<DataType>::removeBefore(double sortKey)
{
  QCPDataContainer<DataType>::iterator it = begin();
  QCPDataContainer<DataType>::iterator itEnd = std::lower_bound(begin(), end(), DataType::fromSortKey(sortKey), qcpLessThanSortKey<DataType>);
  mPreallocSize += itEnd-it; // don't actually delete, just add it to the preallocated block (if it gets too large, squeeze will take care of it)
  if (mAutoSqueeze)
    performAutoSqueeze();
}

/*!
  Removes all data points with (sort-)keys greater than or equal to \a sortKey.

  \see removeBefore, remove, clear
*/
template <class DataType>
void QCPDataContainer<DataType>::removeAfter(double sortKey)
{
  QCPDataContainer<DataType>::iterator it = std::upper_bound(begin(), end(), DataType::fromSortKey(sortKey), qcpLessThanSortKey<DataType>);
  QCPDataContainer<DataType>::iterator itEnd = end();
  mData.erase(it, itEnd); // typically adds it to the postallocated block
  if (mAutoSqueeze)
    performAutoSqueeze();
}

/*!
  Removes all data points with (sort-)keys between \a sortKeyFrom and \a sortKeyTo. if \a
  sortKeyFrom is greater or equal to \a sortKeyTo, the function does nothing. To remove a single
  data point with known (sort-)key, use \ref remove(double sortKey).

  \see removeBefore, removeAfter, clear
*/
template <class DataType>
void QCPDataContainer<DataType>::remove(double sortKeyFrom, double sortKeyTo)
{
  if (sortKeyFrom >= sortKeyTo || isEmpty())
    return;

  QCPDataContainer<DataType>::iterator it = std::lower_bound(begin(), end(), DataType::fromSortKey(sortKeyFrom), qcpLessThanSortKey<DataType>);
  QCPDataContainer<DataType>::iterator itEnd = std::upper_bound(it, end(), DataType::fromSortKey(sortKeyTo), qcpLessThanSortKey<DataType>);
  mData.erase(it, itEnd);
  if (mAutoSqueeze)
    performAutoSqueeze();
}

/*! \overload

  Removes a single data point at \a sortKey. If the position is not known with absolute (binary)
  precision, consider using \ref remove(double sortKeyFrom, double sortKeyTo) with a small
  fuzziness interval around the suspected position, depeding on the precision with which the
  (sort-)key is known.

  \see removeBefore, removeAfter, clear
*/
template <class DataType>
void QCPDataContainer<DataType>::remove(double sortKey)
{
  QCPDataContainer::iterator it = std::lower_bound(begin(), end(), DataType::fromSortKey(sortKey), qcpLessThanSortKey<DataType>);
  if (it != end() && it->sortKey() == sortKey)
  {
    if (it == begin())
      ++mPreallocSize; // don't actually delete, just add it to the preallocated block (if it gets too large, squeeze will take care of it)
    else
      mData.erase(it);
  }
  if (mAutoSqueeze)
    performAutoSqueeze();
}

/*!
  Removes all data points.

  \see remove, removeAfter, removeBefore
*/
template <class DataType>
void QCPDataContainer<DataType>::clear()
{
  mData.clear();
  mPreallocIteration = 0;
  mPreallocSize = 0;
}

/*!
  Re-sorts all data points in the container by their sort key.

  When setting, adding or removing points using the QCPDataContainer interface (\ref set, \ref add,
  \ref remove, etc.), the container makes sure to always stay in a sorted state such that a full
  resort is never necessary. However, if you choose to directly manipulate the sort key on data
  points by accessing and modifying it through the non-const iterators (\ref begin, \ref end), it
  is your responsibility to bring the container back into a sorted state before any other methods
  are called on it. This can be achieved by calling this method immediately after finishing the
  sort key manipulation.
*/
template <class DataType>
void QCPDataContainer<DataType>::sort()
{
  std::sort(begin(), end(), qcpLessThanSortKey<DataType>);
}

/*!
  Frees all unused memory that is currently in the preallocation and postallocation pools.

  Note that QCPDataContainer automatically decides whether squeezing is necessary, if \ref
  setAutoSqueeze is left enabled. It should thus not be necessary to use this method for typical
  applications.

  The parameters \a preAllocation and \a postAllocation control whether pre- and/or post allocation
  should be freed, respectively.
*/
template <class DataType>
void QCPDataContainer<DataType>::squeeze(bool preAllocation, bool postAllocation)
{
  if (preAllocation)
  {
    if (mPreallocSize > 0)
    {
      std::copy(begin(), end(), mData.begin());
      mData.resize(size());
      mPreallocSize = 0;
    }
    mPreallocIteration = 0;
  }
  if (postAllocation)
    mData.squeeze();
}

/*!
  Returns an iterator to the data point with a (sort-)key that is equal to, just below, or just
  above \a sortKey. If \a expandedRange is true, the data point just below \a sortKey will be
  considered, otherwise the one just above.

  This can be used in conjunction with \ref findEnd to iterate over data points within a given key
  range, including or excluding the bounding data points that are just beyond the specified range.

  If \a expandedRange is true but there are no data points below \a sortKey, \ref constBegin is
  returned.

  If the container is empty, returns \ref constEnd.

  \see findEnd, QCPPlottableInterface1D::findBegin
*/
template <class DataType>
typename QCPDataContainer<DataType>::const_iterator QCPDataContainer<DataType>::findBegin(double sortKey, bool expandedRange) const
{
  if (isEmpty())
    return constEnd();

  QCPDataContainer<DataType>::const_iterator it = std::lower_bound(constBegin(), constEnd(), DataType::fromSortKey(sortKey), qcpLessThanSortKey<DataType>);
  if (expandedRange && it != constBegin()) // also covers it == constEnd case, and we know --constEnd is valid because mData isn't empty
    --it;
  return it;
}

/*!
  Returns an iterator to the element after the data point with a (sort-)key that is equal to, just
  above or just below \a sortKey. If \a expandedRange is true, the data point just above \a sortKey
  will be considered, otherwise the one just below.

  This can be used in conjunction with \ref findBegin to iterate over data points within a given
  key range, including the bounding data points that are just below and above the specified range.

  If \a expandedRange is true but there are no data points above \a sortKey, \ref constEnd is
  returned.

  If the container is empty, \ref constEnd is returned.

  \see findBegin, QCPPlottableInterface1D::findEnd
*/
template <class DataType>
typename QCPDataContainer<DataType>::const_iterator QCPDataContainer<DataType>::findEnd(double sortKey, bool expandedRange) const
{
  if (isEmpty())
    return constEnd();

  QCPDataContainer<DataType>::const_iterator it = std::upper_bound(constBegin(), constEnd(), DataType::fromSortKey(sortKey), qcpLessThanSortKey<DataType>);
  if (expandedRange && it != constEnd())
    ++it;
  return it;
}

/*!
  Returns the range encompassed by the (main-)key coordinate of all data points. The output
  parameter \a foundRange indicates whether a sensible range was found. If this is false, you
  should not use the returned QCPRange (e.g. the data container is empty or all points have the
  same key).

  Use \a signDomain to control which sign of the key coordinates should be considered. This is
  relevant e.g. for logarithmic plots which can mathematically only display one sign domain at a
  time.

  If the DataType reports that its main key is equal to the sort key (\a sortKeyIsMainKey), as is
  the case for most plottables, this method uses this fact and finds the range very quickly.

  \see valueRange
*/
template <class DataType>
QCPRange QCPDataContainer<DataType>::keyRange(bool &foundRange, QCP::SignDomain signDomain)
{
  if (isEmpty())
  {
    foundRange = false;
    return QCPRange();
  }
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  double current;

  QCPDataContainer<DataType>::const_iterator it = constBegin();
  QCPDataContainer<DataType>::const_iterator itEnd = constEnd();
  if (signDomain == QCP::sdBoth) // range may be anywhere
  {
    if (DataType::sortKeyIsMainKey()) // if DataType is sorted by main key (e.g. QCPGraph, but not QCPCurve), use faster algorithm by finding just first and last key with non-NaN value
    {
      while (it != itEnd) // find first non-nan going up from left
      {
        if (!qIsNaN(it->mainValue()))
        {
          range.lower = it->mainKey();
          haveLower = true;
          break;
        }
        ++it;
      }
      it = itEnd;
      while (it != constBegin()) // find first non-nan going down from right
      {
        --it;
        if (!qIsNaN(it->mainValue()))
        {
          range.upper = it->mainKey();
          haveUpper = true;
          break;
        }
      }
    } else // DataType is not sorted by main key, go through all data points and accordingly expand range
    {
      while (it != itEnd)
      {
        if (!qIsNaN(it->mainValue()))
        {
          current = it->mainKey();
          if (current < range.lower || !haveLower)
          {
            range.lower = current;
            haveLower = true;
          }
          if (current > range.upper || !haveUpper)
          {
            range.upper = current;
            haveUpper = true;
          }
        }
        ++it;
      }
    }
  } else if (signDomain == QCP::sdNegative) // range may only be in the negative sign domain
  {
    while (it != itEnd)
    {
      if (!qIsNaN(it->mainValue()))
      {
        current = it->mainKey();
        if ((current < range.lower || !haveLower) && current < 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current < 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  } else if (signDomain == QCP::sdPositive) // range may only be in the positive sign domain
  {
    while (it != itEnd)
    {
      if (!qIsNaN(it->mainValue()))
      {
        current = it->mainKey();
        if ((current < range.lower || !haveLower) && current > 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current > 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  }

  foundRange = haveLower && haveUpper;
  return range;
}

/*!
  Returns the range encompassed by the value coordinates of the data points in the specified key
  range (\a inKeyRange), using the full \a DataType::valueRange reported by the data points. The
  output parameter \a foundRange indicates whether a sensible range was found. If this is false,
  you should not use the returned QCPRange (e.g. the data container is empty or all points have the
  same value).

  If \a inKeyRange has both lower and upper bound set to zero (is equal to <tt>QCPRange()</tt>),
  all data points are considered, without any restriction on the keys.

  Use \a signDomain to control which sign of the value coordinates should be considered. This is
  relevant e.g. for logarithmic plots which can mathematically only display one sign domain at a
  time.

  \see keyRange
*/
template <class DataType>
QCPRange QCPDataContainer<DataType>::valueRange(bool &foundRange, QCP::SignDomain signDomain, const QCPRange &inKeyRange)
{
  if (isEmpty())
  {
    foundRange = false;
    return QCPRange();
  }
  QCPRange range;
  const bool restrictKeyRange = inKeyRange != QCPRange();
  bool haveLower = false;
  bool haveUpper = false;
  QCPRange current;
  QCPDataContainer<DataType>::const_iterator itBegin = constBegin();
  QCPDataContainer<DataType>::const_iterator itEnd = constEnd();
  if (DataType::sortKeyIsMainKey() && restrictKeyRange)
  {
    itBegin = findBegin(inKeyRange.lower, false);
    itEnd = findEnd(inKeyRange.upper, false);
  }
  if (signDomain == QCP::sdBoth) // range may be anywhere
  {
    for (QCPDataContainer<DataType>::const_iterator it = itBegin; it != itEnd; ++it)
    {
      if (restrictKeyRange && (it->mainKey() < inKeyRange.lower || it->mainKey() > inKeyRange.upper))
        continue;
      current = it->valueRange();
      if ((current.lower < range.lower || !haveLower) && !qIsNaN(current.lower))
      {
        range.lower = current.lower;
        haveLower = true;
      }
      if ((current.upper > range.upper || !haveUpper) && !qIsNaN(current.upper))
      {
        range.upper = current.upper;
        haveUpper = true;
      }
    }
  } else if (signDomain == QCP::sdNegative) // range may only be in the negative sign domain
  {
    for (QCPDataContainer<DataType>::const_iterator it = itBegin; it != itEnd; ++it)
    {
      if (restrictKeyRange && (it->mainKey() < inKeyRange.lower || it->mainKey() > inKeyRange.upper))
        continue;
      current = it->valueRange();
      if ((current.lower < range.lower || !haveLower) && current.lower < 0 && !qIsNaN(current.lower))
      {
        range.lower = current.lower;
        haveLower = true;
      }
      if ((current.upper > range.upper || !haveUpper) && current.upper < 0 && !qIsNaN(current.upper))
      {
        range.upper = current.upper;
        haveUpper = true;
      }
    }
  } else if (signDomain == QCP::sdPositive) // range may only be in the positive sign domain
  {
    for (QCPDataContainer<DataType>::const_iterator it = itBegin; it != itEnd; ++it)
    {
      if (restrictKeyRange && (it->mainKey() < inKeyRange.lower || it->mainKey() > inKeyRange.upper))
        continue;
      current = it->valueRange();
      if ((current.lower < range.lower || !haveLower) && current.lower > 0 && !qIsNaN(current.lower))
      {
        range.lower = current.lower;
        haveLower = true;
      }
      if ((current.upper > range.upper || !haveUpper) && current.upper > 0 && !qIsNaN(current.upper))
      {
        range.upper = current.upper;
        haveUpper = true;
      }
    }
  }

  foundRange = haveLower && haveUpper;
  return range;
}

/*!
  Makes sure \a begin and \a end mark a data range that is both within the bounds of this data
  container's data, as well as within the specified \a dataRange. The initial range described by
  the passed iterators \a begin and \a end is never expanded, only contracted if necessary.

  This function doesn't require for \a dataRange to be within the bounds of this data container's
  valid range.
*/
template <class DataType>
void QCPDataContainer<DataType>::limitIteratorsToDataRange(const_iterator &begin, const_iterator &end, const QCPDataRange &dataRange) const
{
  QCPDataRange iteratorRange(begin-constBegin(), end-constBegin());
  iteratorRange = iteratorRange.bounded(dataRange.bounded(this->dataRange()));
  begin = constBegin()+iteratorRange.begin();
  end = constBegin()+iteratorRange.end();
}

/*! \internal

  Increases the preallocation pool to have a size of at least \a minimumPreallocSize. Depending on
  the preallocation history, the container will grow by more than requested, to speed up future
  consecutive size increases.

  if \a minimumPreallocSize is smaller than or equal to the current preallocation pool size, this
  method does nothing.
*/
template <class DataType>
void QCPDataContainer<DataType>::preallocateGrow(int minimumPreallocSize)
{
  if (minimumPreallocSize <= mPreallocSize)
    return;

  int newPreallocSize = minimumPreallocSize;
  newPreallocSize += (1u<<qBound(4, mPreallocIteration+4, 15)) - 12; // do 4 up to 32768-12 preallocation, doubling in each intermediate iteration
  ++mPreallocIteration;

  int sizeDifference = newPreallocSize-mPreallocSize;
  mData.resize(mData.size()+sizeDifference);
  std::copy_backward(mData.begin()+mPreallocSize, mData.end()-sizeDifference, mData.end());
  mPreallocSize = newPreallocSize;
}

/*! \internal

  This method decides, depending on the total allocation size and the size of the unused pre- and
  postallocation pools, whether it is sensible to reduce the pools in order to free up unused
  memory. It then possibly calls \ref squeeze to do the deallocation.

  If \ref setAutoSqueeze is enabled, this method is called automatically each time data points are
  removed from the container (e.g. \ref remove).

  \note when changing the decision parameters, care must be taken not to cause a back-and-forth
  between squeezing and reallocation due to the growth strategy of the internal QVector and \ref
  preallocateGrow. The hysteresis between allocation and deallocation should be made high enough
  (at the expense of possibly larger unused memory from time to time).
*/
template <class DataType>
void QCPDataContainer<DataType>::performAutoSqueeze()
{
  const int totalAlloc = mData.capacity();
  const int postAllocSize = totalAlloc-mData.size();
  const int usedSize = size();
  bool shrinkPostAllocation = false;
  bool shrinkPreAllocation = false;
  if (totalAlloc > 650000) // if allocation is larger, shrink earlier with respect to total used size
  {
    shrinkPostAllocation = postAllocSize > usedSize*1.5; // QVector grow strategy is 2^n for static data. Watch out not to oscillate!
    shrinkPreAllocation = mPreallocSize*10 > usedSize;
  } else if (totalAlloc > 1000) // below 10 MiB raw data be generous with preallocated memory, below 1k points don't even bother
  {
    shrinkPostAllocation = postAllocSize > usedSize*5;
    shrinkPreAllocation = mPreallocSize > usedSize*1.5; // preallocation can grow into postallocation, so can be smaller
  }

  if (shrinkPreAllocation || shrinkPostAllocation)
    squeeze(shrinkPreAllocation, shrinkPostAllocation);
}

#endif // QCP_DATACONTAINER_H
