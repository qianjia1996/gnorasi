/********************************************************************************
 *                                                                    		*
 * GNORASI - The Knowlegde-Based Remote Sensing Engine                		*
 * 								      		*
 * Language:  C++						      		*
 * 										*
 * Copyright (c) Angelos Tzotsos <tzotsos@gmail.com>. All rights reserved. 	*
 * Copyright (c) National Technical University of Athens. All rights reserved.	*
 * Copyright (c) Informatics and Telematics Institute				*
 *	  Centre for Research and Technology Hellas. All rights reserved.	*
 * 										*
 *                                                                    		*
 * This file is part of the GNORASI software package. GNORASI is free  		*
 * software: you can redistribute it and/or modify it under the terms 		*
 * of the GNU General Public License version 2 as published by the    		*
 * Free Software Foundation.                                          		*
 *                                                                    		*
 * GNORASI is distributed in the hope that it will be useful,          		*
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     		*
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       		*
 * GNU General Public License for more details.                       		*
 *                                                                    		*
 * You should have received a copy of the GNU General Public License  		*
 * in the file "LICENSE.txt" along with this program.                 		*
 * If not, see <http://www.gnu.org/licenses/>.                        		*
 *                                                                    		*
 ********************************************************************************/

#include "otbimagewriterprocessor.h"
#include "voreen/core/voreenapplication.h"

namespace voreen {

const std::string OTBImageWriterProcessor::loggerCat_("voreen.OTBImageWriterProcessor");
  
OTBImageWriterProcessor::OTBImageWriterProcessor()
    : Processor(),
    inport_(Port::INPORT, "OTBImage.inport", 0),
    imageFile_("imageFile", "Output Image", "Image File", VoreenApplication::app()->getUserDataPath(), "IMG Image file (*.img)", FileDialogProperty::SAVE_FILE),
    clearImage_("clearButton", "Clear Image"),
    imageType_("outputImageType", "Output Image Type"),
    rescale_("rescaleSelection", "Rescale Output", false),
    min_("minValue","Minimum Output Value", 0.0f, 0.0f, 10000.0f),
    max_("maxValue","Maximum Output Value", 255.0f, 0.0f, 10000.0f),
    saveImageButton_("saveButton", "Save Image")
{
    imageType_.addOption("double", "64-bit double");
    imageType_.addOption("float", "32-bit float");
    imageType_.addOption("int", "16-bit integer");
    imageType_.addOption("char", "8-bit integer");
      
    // register ports and properties
    addPort(inport_);
    clearImage_.onChange(CallMemberAction<OTBImageWriterProcessor>(this, &OTBImageWriterProcessor::clearImage));
    saveImageButton_.onChange(CallMemberAction<OTBImageWriterProcessor>(this, &OTBImageWriterProcessor::saveImage));
    addProperty(imageFile_);
    addProperty(clearImage_);
    addProperty(imageType_);
    addProperty(rescale_);
    addProperty(min_);
    addProperty(max_);
    addProperty(saveImageButton_);
    
    //OTB initialization
    writer = WriterType::New();
    int_writer = IntegerWriterType::New();
    intrescaler = IntegerRescalerFilterType::New();
    float_writer = FloatWriterType::New();
    floatrescaler = FloatRescalerFilterType::New();
    byte_writer = ByteWriterType::New();
    byterescaler = ByteRescalerFilterType::New();

}

Processor* OTBImageWriterProcessor::create() const {
    return new OTBImageWriterProcessor();
}

bool OTBImageWriterProcessor::isEndProcessor() const {
    return true;
}

bool OTBImageWriterProcessor::isReady() const {
    return (inport_.isConnected());
}

std::string OTBImageWriterProcessor::getProcessorInfo() const {
    return "Saves Image Data after Performing Processing";
}

void OTBImageWriterProcessor::process() {

}

void OTBImageWriterProcessor::initialize() throw (VoreenException) {
    // call superclass function first
    Processor::initialize();
    hasImage = false;
    //getProcessorWidget()->updateFromProcessor();
}

void OTBImageWriterProcessor::deinitialize() throw (VoreenException) {
    Processor::deinitialize();
}

void OTBImageWriterProcessor::saveImage() {
    
    std::string filename = imageFile_.get();
    if (!filename.empty())
    {
	hasImage = true;
    }
  
    if(this->isReady() && hasImage)
    {
        if (imageType_.get() == "double") {
	    writer->SetFileName(filename.c_str());
	    writer->SetInput(inport_.getData());
	    try
	    {
		writer->Update();
	    }
	    catch (itk::ExceptionObject& err)
	    {
		LWARNING("ExceptionObject caught !");
		return;
	    }
	}else if (imageType_.get() == "float"){
	    floatrescaler->SetInput(inport_.getData());
	    if(rescale_.get()){ //rescale enabled
		floatrescaler->SetOutputMinimum(min_.get());
		floatrescaler->SetOutputMaximum(max_.get());
	    }
	    float_writer->SetFileName(filename.c_str());
            float_writer->SetInput(floatrescaler->GetOutput());
            try
	    {
		float_writer->Update();
	    }
	    catch (itk::ExceptionObject& err)
	    {
		LWARNING("ExceptionObject caught !");
		return;
	    }
	}else if (imageType_.get() == "int"){
	    intrescaler->SetInput(inport_.getData());
	    if(rescale_.get()){ //rescale enabled
		intrescaler->SetOutputMinimum(min_.get());
		intrescaler->SetOutputMaximum(max_.get());
	    }  
	    int_writer->SetFileName(filename.c_str());
	    int_writer->SetInput(intrescaler->GetOutput());
	    try
	    {
		int_writer->Update();
	    }
	    catch (itk::ExceptionObject& err)
	    {
		LWARNING("ExceptionObject caught !");
		return;
	    }
	}else if (imageType_.get() == "char"){
	    byterescaler->SetInput(inport_.getData());
	    if(rescale_.get()){ //rescale enabled
		byterescaler->SetOutputMinimum(min_.get());
		byterescaler->SetOutputMaximum(max_.get());
	    }  
	    byte_writer->SetFileName(filename.c_str());  
	    byte_writer->SetInput(byterescaler->GetOutput());
	    try
	    {
		byte_writer->Update();
	    }
	    catch (itk::ExceptionObject& err)
	    {
		LWARNING("ExceptionObject caught !");
		return;
	    }
	}
    }else if(!this->isReady()){
	LWARNING("Writer Inport not connected");
	return;
    }else if(!hasImage){
	LWARNING("Image Name Not Set");
	return;
    }

    LINFO("Image written succesfully!");
    
}

void OTBImageWriterProcessor::clearImage() {

    if (hasImage) {
        hasImage = false;
    }
    imageFile_.set("");
}

} // namespace
