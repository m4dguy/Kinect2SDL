#ifndef BASICKINECT2GRABBER_H
#define BASICKINECT2GRABBER_H

#include <stdio.h>
#include <Kinect.h>

#include "Utils.h"
#include "UtilsKinect.h"

class BasicKinect2Grabber
{
public:

	/**
	* Constructor. Calls initialize().
	* @param init if set to true, initialize() will be called in the constructor. If not, this step has to be done maunally.
	* @see initialize()
	*
	*/
	BasicKinect2Grabber(bool init = true);
	
	/**
	* Cleans up all pointers and closes all connections safely.
	*/
	~BasicKinect2Grabber();


	/**
	* Initializes all buffers and opens up a connection to the Kinect v2 device.
	* @return true, if camera successfully initialized. 
	*/
	bool initialize();


	/**
	* Check if camera is initialized.
	* @return true, if camera is ready for use.
	*/
	bool isInitialized();

	/*
	* Get new image and point cloud data from the sensor.
	* @return true, if data retrieval successful.
	*/
	bool update();


	/**
	* Retrieves the colour image and copies it into the cv::Mat.
	* @param dst cv::Mat to copy the data into.
	*/
	RGBQUAD* getColourImage(){ return _colourBuffer; };

	/**
	* Retrieves the depth image and copies it into the cv::Mat.
	* @param dst cv::Mat to copy the data into.
	*/
	UINT16* getDepthImage(){ return _depthBuffer; };
	
	/**
	* Retrieves the infrared image and copies it into the cv::Mat.
	* @param dst cv::Mat to copy the data into.
	*/
	UINT16* getIRImage(){ return _irBuffer; };


	/**
	* Get wdith of depth image.
	* @return depth image width.
	*/
	int getDepthImageWidth() const { return _depthWidth; };

	/**
	* Get height of depth image.
	* @return depth image height.
	*/
	int getDepthImageHeight() const { return _depthHeight; };

	/**
	* Get width of infrared image.
	* @return depth image width.
	*/
	int getIRImageWidth() const { return _irWidth; };

	/**
	* Get height of infrared image.
	* @return depth image height.
	*/
	int getIRImageHeight() const { return _irHeight; };

	/**
	* Get width of colour image.
	* @return depth image width.
	*/
	int getColourImageWidth() const { return _colourWidth; };

	/**
	* Get height of colour image.
	* @return depth image height.
	*/
	int getColourImageHeight() const{ return _colourHeight; }

	/**
	* Get depth to colour coordinate conversion map.
	*/
	ColorSpacePoint* getDepth2ColourMap();

	/**
	* Get colour to depth coordinate conversion map.
	*/
	DepthSpacePoint* getColour2DepthMap();


protected:	
	/*
	* Open a connection to colourframereadaer and get image information.
	* Initialize necessary buffers and image dimensions.
	* @return true, if initialization successful.
	*/
	bool initializeColourFrameBuffer();

	/*
	* Open a connection to depthframereader and get image information.
	* Initialize necessary buffers and image dimensions.
	* @return true, if initialization successful.
	*/
	bool initializeDepthFrameBuffer();

	/*
	* Open a connection to infraredframereader and get image information.
	* Initialize necessary buffers and image dimensions.
	* @return true, if initialization successful.
	*/
	bool initializeIRFrameBuffer();

	/*
	* Create coordinate mapper and store coordinate table.
	* @return true, if initialization successful.
	*/
	bool initializeCoordinateMapper();

	/*
	* Update Mat object holding the information of the current colour frame.
	* @return true, if update successful
	* @see update()
	*/
	bool updateColourMat();

	/*
	* Update Mat object holding the information of the current depth frame.
	* @return true, if update successful
	* @see update()
	*/
	bool updateDepthMat();

	/*
	* Update Mat object holding the information of the current infrared frame.
	* @return true, if update successful
	* @see update()
	*/
	bool updateIRMat();

	IKinectSensor* _sensor;
	IMultiSourceFrameReader* _multiSourceFrameReader;
	IMultiSourceFrame* _multiSourceFrame;

	IColorFrame* _colourFrame;
	RGBQUAD* _colourBuffer;

	IDepthFrame* _depthFrame;
	UINT16* _depthBuffer;

	IInfraredFrame* _irFrame;
	UINT16* _irBuffer;

	ICoordinateMapper* _cmapper;
	ColorSpacePoint* _depth2ColourMap;
	DepthSpacePoint* _colour2DepthMap;


	bool initialized = false;

	uint _bufferSizeDepth, _bufferSizeColour, _bufferSizeIR;
	int _colourWidth, _colourHeight;
	int _depthWidth, _depthHeight;
	int _irWidth, _irHeight;

};
#endif