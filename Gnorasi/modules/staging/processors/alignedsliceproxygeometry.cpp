/***********************************************************************************
 *                                                                                 *
 * Voreen - The Volume Rendering Engine                                            *
 *                                                                                 *
 * Copyright (C) 2005-2012 University of Muenster, Germany.                        *
 * Visualization and Computer Graphics Group <http://viscg.uni-muenster.de>        *
 * For a list of authors please refer to the file "CREDITS.txt".                   *
 *                                                                                 *
 * This file is part of the Voreen software package. Voreen is free software:      *
 * you can redistribute it and/or modify it under the terms of the GNU General     *
 * Public License version 2 as published by the Free Software Foundation.          *
 *                                                                                 *
 * Voreen is distributed in the hope that it will be useful, but WITHOUT ANY       *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR   *
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.      *
 *                                                                                 *
 * You should have received a copy of the GNU General Public License in the file   *
 * "LICENSE.txt" along with this file. If not, see <http://www.gnu.org/licenses/>. *
 *                                                                                 *
 * For non-commercial academic use see the license exception specified in the file *
 * "LICENSE-academic.txt". To get information about commercial licensing please    *
 * contact the authors.                                                            *
 *                                                                                 *
 ***********************************************************************************/

#include "alignedsliceproxygeometry.h"

#include "tgt/glmath.h"

using tgt::ivec2;
using tgt::vec3;
using tgt::mat4;
using tgt::Texture;

namespace voreen {

const std::string AlignedSliceProxyGeometry::loggerCat_("voreen.AlignedSliceProxyGeometry");

AlignedSliceProxyGeometry::AlignedSliceProxyGeometry()
    : Processor()
    , sliceAlignment_("sliceAlignmentProp", "Slice Alignment")
    , sliceIndex_("sliceIndex", "Slice Number", 0, 0, 10000, Processor::VALID)
    , floatSliceIndex_("floatSliceIndex", "Slice Number (Float)", 0.0f, 0.0f, 10000.0f, Processor::VALID)
    , camera_("camera", "Camera")
    , alignCameraButton_("alignCameraButton", "Align Camera")
    , plane_("plane", "Plane", vec3(1.0f, 0.0f, 0.0f), vec3(-5.0f), vec3(5.0f))
    , planeDist_("planeDist", "Plane Distance", 0.0f, -1000.0f, 1000.0f)
    , mwheelCycleHandler_("mouseWheelHandler", "Slice Cycling", &sliceIndex_)
    , inport_(Port::INPORT, "volume")
    , geomPort_(Port::OUTPORT, "geometry")
    , textPort_(Port::OUTPORT, "text")
{
    addInteractionHandler(mwheelCycleHandler_);

    sliceAlignment_.addOption("xy-plane", "XY-Plane (axial)", XY_PLANE);
    sliceAlignment_.addOption("xz-plane", "XZ-Plane (coronal)", XZ_PLANE);
    sliceAlignment_.addOption("yz-plane", "YZ-Plane (sagittal)", YZ_PLANE);
    sliceAlignment_.onChange( CallMemberAction<AlignedSliceProxyGeometry>(this, &AlignedSliceProxyGeometry::updateSliceProperties) );
    addProperty(sliceAlignment_);

    addProperty(sliceIndex_);
    addProperty(floatSliceIndex_);

    addProperty(camera_);

    addProperty(alignCameraButton_);
    alignCameraButton_.onChange( CallMemberAction<AlignedSliceProxyGeometry>(this, &AlignedSliceProxyGeometry::alignCamera) );
    sliceIndex_.onChange( CallMemberAction<AlignedSliceProxyGeometry>(this, &AlignedSliceProxyGeometry::indexChanged) );
    floatSliceIndex_.onChange( CallMemberAction<AlignedSliceProxyGeometry>(this, &AlignedSliceProxyGeometry::floatIndexChanged) );

    addProperty(plane_);
    plane_.setInvalidationLevel(VALID); //output only properties
    addProperty(planeDist_);
    planeDist_.setInvalidationLevel(VALID);

    addPort(inport_);
    addPort(geomPort_);
    addPort(textPort_);
}

AlignedSliceProxyGeometry::~AlignedSliceProxyGeometry() {
}

std::string AlignedSliceProxyGeometry::getProcessorInfo() const {
    return "Creates slices directly from volumes without using OpenGl.";
}

bool AlignedSliceProxyGeometry::isReady() const {
    if(inport_.isReady() && geomPort_.isReady())
        return true;
    else
        return false;
}

void AlignedSliceProxyGeometry::process() {
    LGL_ERROR;

    if (inport_.hasChanged()) {
        updateSliceProperties();  // validate the currently set values and adjust them if necessary
        update();
    }

    //Generate text output:
    std::stringstream strstr;
    strstr << sliceIndex_.get() << "/" << sliceIndex_.getMaxValue();
    textPort_.setData(strstr.str());
}

void AlignedSliceProxyGeometry::update() {
    const VolumeBase* volh = inport_.getData();
    if(!volh)
        return;

    FaceGeometry slice = getSliceGeometry(volh, sliceAlignment_.getValue(), floatSliceIndex_.get(), true);

    MeshGeometry mesh;
    mesh.addFace(slice);

    //calculate plane equation:
    tgt::plane p(slice.getVertex(0).getCoords(), slice.getVertex(1).getCoords(), slice.getVertex(2).getCoords());

    tgt::vec3 test(1.0f);
    tgt::vec4 planeVec = p.toVec4();
    if(dot(test, planeVec.xyz()) < 0.0f)
       planeVec *= -1.0f;
    plane_.set(planeVec.xyz());
    planeDist_.set(-planeVec.w);

    MeshListGeometry* geometry = new MeshListGeometry();
    geometry->addMesh(mesh);
    geomPort_.setData(geometry);
}

void AlignedSliceProxyGeometry::updateSliceProperties() {
    tgt::ivec3 volumeDim(0);
    if (inport_.getData() && inport_.getData()->getRepresentation<VolumeRAM>())
        volumeDim = inport_.getData()->getRepresentation<VolumeRAM>()->getDimensions();

    tgtAssert(sliceAlignment_.getValue() >= 0 && sliceAlignment_.getValue() <= 2, "Invalid alignment value");
    int numSlices = volumeDim[sliceAlignment_.getValue()];
    if (numSlices == 0)
        return;

    sliceIndex_.setMaxValue(numSlices-1);
    floatSliceIndex_.setMaxValue(static_cast<float>(numSlices-1));
    if (sliceIndex_.get() >= static_cast<int>(numSlices))
        sliceIndex_.set(static_cast<int>(numSlices / 2));

    alignCamera();
}

void AlignedSliceProxyGeometry::alignCamera() {
    const VolumeBase* volh = inport_.getData();
    if(!volh)
        return;

    vec3 urb = volh->getURB();
    vec3 llf = volh->getLLF();

    vec3 center = (urb + llf) / 2.0f;
    vec3 xVec = vec3(llf.x, center.y, center.z);
    vec3 yVec = vec3(center.x, llf.y, center.z);
    vec3 zVec = vec3(center.x, center.y, llf.z);

    tgt::mat4 m = volh->getPhysicalToWorldMatrix();

    center = m * center;
    xVec = m * xVec;
    yVec = m * yVec;
    zVec = m * zVec;

    vec3 mainDir;
    vec3 upDir;
    vec3 strafeDir;

    switch(sliceAlignment_.getValue()) {
        case YZ_PLANE: {
                           mainDir = xVec - center;
                           upDir = -(zVec - center);
                           strafeDir = -(yVec - center);
                       }
                       break;
        case XZ_PLANE: {
                           mainDir = yVec - center;
                           upDir = -(zVec - center);
                           strafeDir = -(xVec - center);
                       }
                       break;
        case XY_PLANE: {
                           mainDir = zVec - center;
                           upDir = (yVec - center);
                           strafeDir = (xVec - center);
                       }
                       break;
        default: tgtAssert(false, "should not get here!");
    }

    //setup camera:
    vec3 pos = center + (mainDir * 1.5f);
    vec3 up = normalize(upDir);

    //setup frustum:
    float size = std::max(length(strafeDir), length(upDir));
    tgt::Frustum f(-size, +size, -size, +size, 0.01f, 5.0f);

    tgt::Camera cam(pos, center, up);
    cam.setProjectionMode(tgt::Camera::ORTHOGRAPHIC);

    cam.setFrustum(f);

    camera_.set(cam);
}

void AlignedSliceProxyGeometry::indexChanged() {
    floatSliceIndex_.set(static_cast<float>(sliceIndex_.get()));
}

void AlignedSliceProxyGeometry::floatIndexChanged() {
    update();
}

} // namespace voreen
