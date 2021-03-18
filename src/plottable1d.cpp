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

#include "plottable1d.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPPlottableInterface1D
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPPlottableInterface1D
  \brief Defines an abstract interface for one-dimensional plottables

  This class contains only pure virtual methods which define a common interface to the data
  of one-dimensional plottables.

  For example, it is implemented by the template class \ref QCPAbstractPlottable1D (the preferred
  base class for one-dimensional plottables). So if you use that template class as base class of
  your one-dimensional plottable, you won't have to care about implementing the 1d interface
  yourself.

  If your plottable doesn't derive from \ref QCPAbstractPlottable1D but still wants to provide a 1d
  interface (e.g. like \ref QCPErrorBars does), you should inherit from both \ref
  QCPAbstractPlottable and \ref QCPPlottableInterface1D and accordingly reimplement the pure
  virtual methods of the 1d interface, matching your data container. Also, reimplement \ref
  QCPAbstractPlottable::interface1D to return the \c this pointer.

  If you have a \ref QCPAbstractPlottable pointer, you can check whether it implements this
  interface by calling \ref QCPAbstractPlottable::interface1D and testing it for a non-zero return
  value. If it indeed implements this interface, you may use it to access the plottable's data
  without needing to know the exact type of the plottable or its data point type.
*/

/* start documentation of pure virtual functions */

/*! \fn virtual int QCPPlottableInterface1D::dataCount() const = 0;
  
  Returns the number of data points of the plottable.
*/

/*! \fn virtual QCPDataSelection QCPPlottableInterface1D::selectTestRect(const QRectF &rect, bool onlySelectable) const = 0;
  
  Returns a data selection containing all the data points of this plottable which are contained (or
  hit by) \a rect. This is used mainly in the selection rect interaction for data selection (\ref
  dataselection "data selection mechanism").
  
  If \a onlySelectable is true, an empty QCPDataSelection is returned if this plottable is not
  selectable (i.e. if \ref QCPAbstractPlottable::setSelectable is \ref QCP::stNone).
  
  \note \a rect must be a normalized rect (positive or zero width and height). This is especially
  important when using the rect of \ref QCPSelectionRect::accepted, which is not necessarily
  normalized. Use <tt>QRect::normalized()</tt> when passing a rect which might not be normalized.
*/

/*! \fn virtual double QCPPlottableInterface1D::dataMainKey(int index) const = 0
  
  Returns the main key of the data point at the given \a index.
  
  What the main key is, is defined by the plottable's data type. See the \ref
  qcpdatacontainer-datatype "QCPDataContainer DataType" documentation for details about this naming
  convention.
*/

/*! \fn virtual double QCPPlottableInterface1D::dataSortKey(int index) const = 0
  
  Returns the sort key of the data point at the given \a index.
  
  What the sort key is, is defined by the plottable's data type. See the \ref
  qcpdatacontainer-datatype "QCPDataContainer DataType" documentation for details about this naming
  convention.
*/

/*! \fn virtual double QCPPlottableInterface1D::dataMainValue(int index) const = 0
  
  Returns the main value of the data point at the given \a index.
  
  What the main value is, is defined by the plottable's data type. See the \ref
  qcpdatacontainer-datatype "QCPDataContainer DataType" documentation for details about this naming
  convention.
*/

/*! \fn virtual QCPRange QCPPlottableInterface1D::dataValueRange(int index) const = 0
  
  Returns the value range of the data point at the given \a index.
  
  What the value range is, is defined by the plottable's data type. See the \ref
  qcpdatacontainer-datatype "QCPDataContainer DataType" documentation for details about this naming
  convention.
*/

/*! \fn virtual QPointF QCPPlottableInterface1D::dataPixelPosition(int index) const = 0

  Returns the pixel position on the widget surface at which the data point at the given \a index
  appears.

  Usually this corresponds to the point of \ref dataMainKey/\ref dataMainValue, in pixel
  coordinates. However, depending on the plottable, this might be a different apparent position
  than just a coord-to-pixel transform of those values. For example, \ref QCPBars apparent data
  values can be shifted depending on their stacking, bar grouping or configured base value.
*/

/*! \fn virtual bool QCPPlottableInterface1D::sortKeyIsMainKey() const = 0

  Returns whether the sort key (\ref dataSortKey) is identical to the main key (\ref dataMainKey).

  What the sort and main keys are, is defined by the plottable's data type. See the \ref
  qcpdatacontainer-datatype "QCPDataContainer DataType" documentation for details about this naming
  convention.
*/

/*! \fn virtual int QCPPlottableInterface1D::findBegin(double sortKey, bool expandedRange) const = 0

  Returns the index of the data point with a (sort-)key that is equal to, just below, or just above
  \a sortKey. If \a expandedRange is true, the data point just below \a sortKey will be considered,
  otherwise the one just above.

  This can be used in conjunction with \ref findEnd to iterate over data points within a given key
  range, including or excluding the bounding data points that are just beyond the specified range.

  If \a expandedRange is true but there are no data points below \a sortKey, 0 is returned.

  If the container is empty, returns 0 (in that case, \ref findEnd will also return 0, so a loop
  using these methods will not iterate over the index 0).

  \see findEnd, QCPDataContainer::findBegin
*/

/*! \fn virtual int QCPPlottableInterface1D::findEnd(double sortKey, bool expandedRange) const = 0

  Returns the index one after the data point with a (sort-)key that is equal to, just above, or
  just below \a sortKey. If \a expandedRange is true, the data point just above \a sortKey will be
  considered, otherwise the one just below.

  This can be used in conjunction with \ref findBegin to iterate over data points within a given
  key range, including the bounding data points that are just below and above the specified range.

  If \a expandedRange is true but there are no data points above \a sortKey, the index just above the
  highest data point is returned.

  If the container is empty, returns 0.

  \see findBegin, QCPDataContainer::findEnd
*/

/* end documentation of pure virtual functions */


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPAbstractPlottable1D
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPAbstractPlottable1D
  \brief A template base class for plottables with one-dimensional data

  This template class derives from \ref QCPAbstractPlottable and from the abstract interface \ref
  QCPPlottableInterface1D. It serves as a base class for all one-dimensional data (i.e. data with
  one key dimension), such as \ref QCPGraph and QCPCurve.

  The template parameter \a DataType is the type of the data points of this plottable (e.g. \ref
  QCPGraphData or \ref QCPCurveData). The main purpose of this base class is to provide the member
  \a mDataContainer (a shared pointer to a \ref QCPDataContainer "QCPDataContainer<DataType>") and
  implement the according virtual methods of the \ref QCPPlottableInterface1D, such that most
  subclassed plottables don't need to worry about this anymore.

  Further, it provides a convenience method for retrieving selected/unselected data segments via
  \ref getDataSegments. This is useful when subclasses implement their \ref draw method and need to
  draw selected segments with a different pen/brush than unselected segments (also see \ref
  QCPSelectionDecorator).

  This class implements basic functionality of \ref QCPAbstractPlottable::selectTest and \ref
  QCPPlottableInterface1D::selectTestRect, assuming point-like data points, based on the 1D data
  interface. In spite of that, most plottable subclasses will want to reimplement those methods
  again, to provide a more accurate hit test based on their specific data visualization geometry.
*/

/* start documentation of inline functions */

/*! \fn QCPPlottableInterface1D *QCPAbstractPlottable1D::interface1D()
  
  Returns a \ref QCPPlottableInterface1D pointer to this plottable, providing access to its 1D
  interface.
  
  \seebaseclassmethod
*/

/* end documentation of inline functions */
