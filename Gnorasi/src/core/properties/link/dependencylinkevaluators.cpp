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

#include "voreen/core/properties/link/dependencylinkevaluators.h"
#include "voreen/core/datastructures/transfunc/transfuncfactory.h"
#include "voreen/core/properties/property.h"

namespace voreen {

// TODO revise or remove
/*void DependencyLinkEvaluatorVolumeHandle::eval(Property* src, Property* dst) throw (VoreenException) {
    DependencyLinkEvaluator::eval(src, dst);

    if(static_cast<VolumeHandleProperty*>(src)->get())
        static_cast<VolumeHandleProperty*>(src)->get()->addObserver(this);
}

void DependencyLinkEvaluatorVolumeHandle::propertiesChanged(Property* src, Property* dst) {
    DependencyLinkEvaluator::propertiesChanged(src, dst);

    if(static_cast<VolumeHandleProperty*>(src)->get())
        static_cast<VolumeHandleProperty*>(src)->get()->addObserver(this);
}

void DependencyLinkEvaluatorVolumeHandle::volumeHandleDelete(const VolumeBase* source) {
    tgtAssert(keys_.size() == values_.size(), "keys and values have different number of entries");

    //We create this property just to compare its serialization against the keys:
    VolumeHandleProperty vhp("test", "Test", const_cast<Volume*>(static_cast<const Volume*>((source))));
    std::string compare = serializeProperty(&vhp);

    for (size_t i = 0; i < keys_.size(); ++i) {
        if(keys_[i] == compare) {
            keys_.erase(keys_.begin() + i);
            values_.erase(values_.begin() + i);
            break;
        }
    }
    tgtAssert(keys_.size() == values_.size(), "keys and values have different number of entries");
}

void DependencyLinkEvaluatorVolumeHandle::volumeChange(const VolumeBase*) {} */

} // namespace
