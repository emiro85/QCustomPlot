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

#include "datacontainer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPDataContainer
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPDataContainer
  \brief The generic data container for one-dimensional plottables

  This class template provides a fast container for data storage of one-dimensional data. The data
  type is specified as template parameter (called \a DataType in the following) and must provide
  some methods as described in the \ref qcpdatacontainer-datatype "next section".

  The data is stored in a sorted fashion, which allows very quick lookups by the sorted key as well
  as retrieval of ranges (see \ref findBegin, \ref findEnd, \ref keyRange) using binary search. The
  container uses a preallocation and a postallocation scheme, such that appending and prepending
  data (with respect to the sort key) is very fast and minimizes reallocations. If data is added
  which needs to be inserted between existing keys, the merge usually can be done quickly too,
  using the fact that existing data is always sorted. The user can further improve performance by
  specifying that added data is already itself sorted by key, if he can guarantee that this is the
  case (see for example \ref add(const QVector<DataType> &data, bool alreadySorted)).

  The data can be accessed with the provided const iterators (\ref constBegin, \ref constEnd). If
  it is necessary to alter existing data in-place, the non-const iterators can be used (\ref begin,
  \ref end). Changing data members that are not the sort key (for most data types called \a key) is
  safe from the container's perspective.

  Great care must be taken however if the sort key is modified through the non-const iterators. For
  performance reasons, the iterators don't automatically cause a re-sorting upon their
  manipulation. It is thus the responsibility of the user to leave the container in a sorted state
  when finished with the data manipulation, before calling any other methods on the container. A
  complete re-sort (e.g. after finishing all sort key manipulation) can be done by calling \ref
  sort. Failing to do so can not be detected by the container efficiently and will cause both
  rendering artifacts and potential data loss.

  Implementing one-dimensional plottables that make use of a \ref QCPDataContainer<T> is usually
  done by subclassing from \ref QCPAbstractPlottable1D "QCPAbstractPlottable1D<T>", which
  introduces an according \a mDataContainer member and some convenience methods.

  \section qcpdatacontainer-datatype Requirements for the DataType template parameter

  The template parameter <tt>DataType</tt> is the type of the stored data points. It must be
  trivially copyable and have the following public methods, preferably inline:

  \li <tt>double sortKey() const</tt>\n Returns the member variable of this data point that is the
  sort key, defining the ordering in the container. Often this variable is simply called \a key.

  \li <tt>static DataType fromSortKey(double sortKey)</tt>\n Returns a new instance of the data
  type initialized with its sort key set to \a sortKey.

  \li <tt>static bool sortKeyIsMainKey()</tt>\n Returns true if the sort key is equal to the main
  key (see method \c mainKey below). For most plottables this is the case. It is not the case for
  example for \ref QCPCurve, which uses \a t as sort key and \a key as main key. This is the reason
  why QCPCurve unlike QCPGraph can display parametric curves with loops.

  \li <tt>double mainKey() const</tt>\n Returns the variable of this data point considered the main
  key. This is commonly the variable that is used as the coordinate of this data point on the key
  axis of the plottable. This method is used for example when determining the automatic axis
  rescaling of key axes (\ref QCPAxis::rescale).

  \li <tt>double mainValue() const</tt>\n Returns the variable of this data point considered the
  main value. This is commonly the variable that is used as the coordinate of this data point on
  the value axis of the plottable.

  \li <tt>QCPRange valueRange() const</tt>\n Returns the range this data point spans in the value
  axis coordinate. If the data is single-valued (e.g. QCPGraphData), this is simply a range with
  both lower and upper set to the main data point value. However if the data points can represent
  multiple values at once (e.g QCPFinancialData with its \a high, \a low, \a open and \a close
  values at each \a key) this method should return the range those values span. This method is used
  for example when determining the automatic axis rescaling of value axes (\ref
  QCPAxis::rescale).
*/

/* start documentation of inline functions */

/*! \fn int QCPDataContainer<DataType>::size() const
  
  Returns the number of data points in the container.
*/

/*! \fn bool QCPDataContainer<DataType>::isEmpty() const
  
  Returns whether this container holds no data points.
*/

/*! \fn QCPDataContainer::const_iterator QCPDataContainer<DataType>::constBegin() const
  
  Returns a const iterator to the first data point in this container.
*/

/*! \fn QCPDataContainer::const_iterator QCPDataContainer<DataType>::constEnd() const
  
  Returns a const iterator to the element past the last data point in this container.
*/

/*! \fn QCPDataContainer::iterator QCPDataContainer<DataType>::begin() const
  
  Returns a non-const iterator to the first data point in this container.

  You can manipulate the data points in-place through the non-const iterators, but great care must
  be taken when manipulating the sort key of a data point, see \ref sort, or the detailed
  description of this class.
*/

/*! \fn QCPDataContainer::iterator QCPDataContainer<DataType>::end() const
  
  Returns a non-const iterator to the element past the last data point in this container.
  
  You can manipulate the data points in-place through the non-const iterators, but great care must
  be taken when manipulating the sort key of a data point, see \ref sort, or the detailed
  description of this class.
*/

/*! \fn QCPDataContainer::const_iterator QCPDataContainer<DataType>::at(int index) const

  Returns a const iterator to the element with the specified \a index. If \a index points beyond
  the available elements in this container, returns \ref constEnd, i.e. an iterator past the last
  valid element.

  You can use this method to easily obtain iterators from a \ref QCPDataRange, see the \ref
  dataselection-accessing "data selection page" for an example.
*/

/*! \fn QCPDataRange QCPDataContainer::dataRange() const

  Returns a \ref QCPDataRange encompassing the entire data set of this container. This means the
  begin index of the returned range is 0, and the end index is \ref size.
*/

/* end documentation of inline functions */
