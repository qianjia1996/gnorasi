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

#ifndef VRN_FONTPROPERTY_H
#define VRN_FONTPROPERTY_H

#include "voreen/core/properties/templateproperty.h"
#include "tgt/font.h"

namespace voreen {

class FontProperty : public TemplateProperty<tgt::Font*> {
public:
    FontProperty(const std::string& id, const std::string& guiText, tgt::Font* value = 0,
                 int invalidationLevel = Processor::INVALID_RESULT);
    FontProperty();

    virtual Property* create() const;

    virtual std::string getClassName() const       { return "FontProperty"; }
    virtual std::string getTypeDescription() const { return "Font"; }

    /**
     * Frees the font object represented by this property.
     */
    virtual ~FontProperty();

    /**
     * @see Property::serialize
     */
    virtual void serialize(XmlSerializer& s) const;

    /**
     * @see Property::deserialize
     */
    virtual void deserialize(XmlDeserializer& s);

};

} // namespace

#endif // VRN_FONTPROPERTY_H
