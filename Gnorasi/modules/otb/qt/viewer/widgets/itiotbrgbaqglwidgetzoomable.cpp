#include "itiotbrgbaqglwidgetzoomable.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "../utils/itiotbimagemanager.h"
#include "../observables/itiviewerobservableregion.h"

using namespace otb;
using namespace itiviewer;

ItiOtbRgbaQGLWidgetZoomable::ItiOtbRgbaQGLWidgetZoomable(QWidget *parent) :
    m_IsotropicZoom(1.0), m_OpenGlBuffer(NULL), m_OpenGlBufferedRegion(), m_Extent(), m_SubsamplingRate(1), QGLWidget(parent)
{

}

//!
void ItiOtbRgbaQGLWidgetZoomable::ReadBuffer(const RasterImageType *image, const RasterRegionType &region){
    // Before doing anything, check if region is inside the buffered
    // region of image
    if (!image->GetBufferedRegion().IsInside(region))
      {
//      itkExceptionMacro(<< "Region to read is oustside of the buffered region.");
      }
    // Delete previous buffer if needed
    this->ClearBuffer();

    // Allocate new memory
    m_OpenGlBuffer = new unsigned char[3 * region.GetNumberOfPixels()];

    // Declare the iterator
    itk::ImageRegionConstIteratorWithIndex<RasterImageType> it(image, region);

    // Go to begin
    it.GoToBegin();

    while (!it.IsAtEnd())
    {
        // Fill the buffer
        unsigned int index = 0;

        // compute the linear index (buffer is flipped around X axis
        // when gl acceleration is disabled
        index = ComputeXAxisFlippedBufferIndex(it.GetIndex(), region);

        // Fill the buffer
        m_OpenGlBuffer[index]  = it.Get()[0];
        m_OpenGlBuffer[index + 1] = it.Get()[1];
        m_OpenGlBuffer[index + 2] = it.Get()[2];
        ++it;
    }

    //! Last, updating buffer size
    m_OpenGlBufferedRegion = region;

    update();
}

//!
void ItiOtbRgbaQGLWidgetZoomable::ClearBuffer(){
    // Delete previous buffer if needed
    if (m_OpenGlBuffer != NULL)
    {
        delete[] m_OpenGlBuffer;
        m_OpenGlBuffer = NULL;
    }

    RasterRegionType region;
    RasterIndexType index;
    RasterSizeType  size;

    size.Fill(0);
    index.Fill(0);
    region.SetIndex(index);
    region.SetSize(size);

    // Last, updating buffer size
    m_OpenGlBufferedRegion = region;

    update();
}


void ItiOtbRgbaQGLWidgetZoomable::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
}


void ItiOtbRgbaQGLWidgetZoomable::resizeGL(int w, int h)
{
    setupViewport(w,h);

}

void ItiOtbRgbaQGLWidgetZoomable::setupViewport(int w, int h){
    RasterSizeType size;
    size [0] = static_cast<unsigned int>(m_IsotropicZoom * static_cast<double>(m_OpenGlBufferedRegion.GetSize()[0]));
    size [1] = static_cast<unsigned int>(m_IsotropicZoom * static_cast<double>(m_OpenGlBufferedRegion.GetSize()[1]));

    RasterRegionType::IndexType index;
    index[0] = (w - static_cast<int>(size[0])) / 2;
    index[1] = (h - static_cast<int>(size[1])) / 2;

    m_Extent.SetIndex(index);
    m_Extent.SetSize(size);

    m_W = (GLint)w;
    m_H = (GLint)h;

    int wt = qMin(static_cast<int>(m_W),static_cast<int>(m_OpenGlBufferedRegion.GetSize()[0]));
    int ht = qMin(static_cast<int>(m_H),static_cast<int>(m_OpenGlBufferedRegion.GetSize()[1]));

    QRect rect;
    rect.setWidth(wt);
    rect.setHeight(ht);
    rect.setX(index[0]);
    rect.setY(index[1]);

    emit visibleAreaChanged(rect);

    glViewport(0, 0, m_W, m_H);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_W, 0, m_H, -1, 1);
}

void ItiOtbRgbaQGLWidgetZoomable::paintGL()
{
    unsigned int nb_displayed_rows;
    unsigned int nb_displayed_cols;
    unsigned int first_displayed_row;
    unsigned int first_displayed_col;

    if( m_Extent.GetIndex()[0] >= 0 )
    {
        nb_displayed_cols = m_OpenGlBufferedRegion.GetSize()[0];
        first_displayed_col = 0;
    }
    else
    {
        nb_displayed_cols = m_W / m_IsotropicZoom;
        first_displayed_col = (m_OpenGlBufferedRegion.GetSize()[0] - nb_displayed_cols) / 2;
    }

    if( m_Extent.GetIndex()[1] >= 0 )
    {
        nb_displayed_rows = m_OpenGlBufferedRegion.GetSize()[1];
        first_displayed_row = 0;
    }
    else
    {
        nb_displayed_rows = m_H / m_IsotropicZoom;
        first_displayed_row = (m_OpenGlBufferedRegion.GetSize()[1] - nb_displayed_rows) / 2;
    }


    RasterIndexType startPosition = m_Extent.GetIndex();
    startPosition[0] = startPosition[0] < 0 ? 0 : startPosition[0];
    startPosition[1] = startPosition[1] < 0 ? 0 : startPosition[1];

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, m_OpenGlBufferedRegion.GetSize()[0]);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, first_displayed_col);
    glPixelStorei(GL_UNPACK_SKIP_ROWS,first_displayed_row);

    glClear(GL_COLOR_BUFFER_BIT);
    glPixelZoom(m_IsotropicZoom,m_IsotropicZoom);

    glRasterPos2f(startPosition[0], startPosition[1]);
    glDrawPixels(nb_displayed_cols,
                nb_displayed_rows,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                m_OpenGlBuffer);


    glFlush();
}

//!
void ItiOtbRgbaQGLWidgetZoomable::draw(){
    OTBImagePort *port = (OTBImagePort*)ITIOTBIMAGEMANAGER->port();

    if(!port)
        return;

    //!
    RasterImageType* imgType =  (RasterImageType*)port->getData();
    if(!imgType)
        return;

    RasterRegionType region = imgType->GetLargestPossibleRegion();

    //!
    ReadBuffer(imgType,region);
}

//!
void ItiOtbRgbaQGLWidgetZoomable::updateObserver(ItiViewerObservable *observable){

    ItiViewerObservableRegion *region = qobject_cast<ItiViewerObservableRegion*>(observable);
    if(!region)
        return;

    //! TODO
    //! add the functionality here
    //! calculate the isotropic zoom and set to the viewer
    //!

//    QRect rr = region->region();

//    if(!rr.width() || !rr.height())
//        return;

//    double zoom = static_cast<double>(size().width()) / static_cast<double>(rr.width());

//    setIsotropicZoom(zoom);
}

//!
void ItiOtbRgbaQGLWidgetZoomable::wheelEvent(QWheelEvent *event){
    float scale = (float)event->delta() / 960.0;

    double newSc = m_IsotropicZoom + scale;

    if(newSc <= 1.0 || newSc >= 50.0){
        event->ignore();
        return;
    }

    setIsotropicZoom(newSc);

    updateGL();

    event->accept();
}

//!
ItiOtbRgbaQGLWidgetZoomable::~ItiOtbRgbaQGLWidgetZoomable(){
    ClearBuffer();
}