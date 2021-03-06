/********************************************************************************
 *                                                                              *
 * GNORASI - The Knowlegde-Based Remote Sensing Engine                          *
 *                                                                              *
 * Language:  C++                                                               *
 *                                                                              *
 * Copyright (c) Ioannis Tsampoulatidis <itsam@iti.gr>. All rights reserved. 	*
 * Copyright (c) Informatics and Telematics Institute                           *
 *	  Centre for Research and Technology Hellas. All rights reserved.           *
 * Copyright (c) National Technical University of Athens. All rights reserved.	*
 *                                                                              *
 *                                                                              *
 * This file is part of the GNORASI software package. GNORASI is free           *
 * software: you can redistribute it and/or modify it under the terms           *
 * of the GNU General Public License version 2 as published by the              *
 * Free Software Foundation.                                                    *
 *                                                                              *
 * GNORASI is distributed in the hope that it will be useful,                   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
 * GNU General Public License for more details.                                 *
 *                                                                              *
 * You should have received a copy of the GNU General Public License            *
 * in the file "LICENSE.txt" along with this program.                           *
 * If not, see <http://www.gnu.org/licenses/>.                                  *
 *                                                                              *
 ********************************************************************************/

#ifndef ITIOTBVECTORQGLWIDGETFULLVIEW_H
#define ITIOTBVECTORQGLWIDGETFULLVIEW_H

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>

#include "../itiviewerobserver.h"

#include "../../vector_globaldefs.h"


//using namespace otb;
using namespace voreen;

namespace itiviewer{


//
// Internal classes pre-declaration.
class ImageModelRendererFullView;
class ImageViewManipulatorFullView;
class ItiOtbVectorImageViewer;


/** \class ItiOtbVectorQGLWidgetFullView
*
*   \brief This class renders an RGB bytes image buffer to the screen.
*   Rendered data can be loaded using the ReadBuffer() method.
*
*   This class will not allow zooming functionality
*   The zoom scale is being calculated in order the image size to be fit in
*   the window size.
*
*   It is also able to display a rectangle on the displayed image.
*
*  \ingroup Visualization
 */
class ItiOtbVectorQGLWidgetFullView : public QGLWidget, public ItiViewerObserver
{
    Q_OBJECT
public:
    explicit ItiOtbVectorQGLWidgetFullView(ItiOtbVectorImageViewer *parent );

    virtual ~ItiOtbVectorQGLWidgetFullView();

    //! setter getter, self explanatory
    double isotropicZoom() const { return m_IsotropicZoom; }
    void setIsotropicZoom(double z) { m_IsotropicZoom = z; }

    //! setter getter, self explanatory
    unsigned int subSamplingRate() const { return m_SubsamplingRate; }
    void setSubSamplingRate(unsigned int ss) { m_SubsamplingRate = ss; }

    //! setter getter for the focus region area
    QRect visibleRegion() const { return m_visibleRegion; }
    void setVisibleRegion(const QRect &rect) { m_visibleRegion  = rect; }

    /*!
     * \brief update , implementation from parent class
     * \param region, the region
     */
    void updateObserver(ItiViewerObservable *observable);

    /*!
     * \brief draw
     */
    void draw();


public slots:
    /*!
     * \brief onModelReady
     */
    void onModelReady();

signals:

    /*!
     * \brief sizeChanged
     */
    void sizeChanged(const QSize &);

    /*!
     * \brief currentIndexChanged
     * \param text
     */
    void currentIndexChanged(const QString &text);

    /*!
     * \brief visibleAreaTranslated
     */
    void visibleAreaTranslated(int, int);


protected:

    /*!
     * \brief paintEvent
     * \param event
     */
    void paintEvent(QPaintEvent *event);

    /*!
     * \brief initializeGL
     *  reimplemented method declared in the QGLWidget class
     */
    void initializeGL();

    /*!
     * \brief resizeGL , reimplemented method declared in the QGLWidget class
     * \param w , the width parameter
     * \param h , the height parameter
     */
    void resizeGL(int w, int h);

    /*!
     * \brief mouseMoveEvent
     * \param event
     */
    void mouseMoveEvent(QMouseEvent *event);

    /*!
     * \brief mousePressEvent
     * \param event
     */
    void mousePressEvent(QMouseEvent *event);

    /*!
     * \brief mouseReleaseEvent
     * \param event
     */
    void mouseReleaseEvent(QMouseEvent *event);


private:
    /*!
     * \brief setupViewport
     * \param width
     * \param height
     */
    void setupViewport(int w, int h);

    /** OpenGl zoom factor */
    double m_IsotropicZoom;

    /*!
     * \brief translate
     * \param previousCenter
     * \param point
     */
    void translate(const QPoint &previousCenter, const QPoint &point);

    /*!
     * \brief m_pImageViewManipulator
     *  ATM this objects holds the extent and buffer properties
     */
    ImageViewManipulatorFullView* m_pImageViewManipulator;

    /** Model Renderer pointer */
    ImageModelRendererFullView* m_pImageModelRenderer;

    /** If the image is subsampled with respect to the original image,
     * this indicates the subsampling rate */
    unsigned int m_SubsamplingRate;

    /*!
     * \brief m_pItiOtbVectorImageViewer, the parent viewer
     */
    ItiOtbVectorImageViewer *m_pItiOtbVectorImageViewer;

    /*!
     * \brief m_pen
     */
    QPen m_pen;

    /*!
     * \brief m_focusRegion
     */
    QRect m_visibleRegion;

    /*!
     * \brief m_moving, a variable used while moving the mouse
     */
    bool m_moving;

};

} // end of itiviewer

#endif // ITIOTBVECTORQGLWIDGETFULLVIEW_H
