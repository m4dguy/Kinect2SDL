#include "BasicKinect2Grabber.h"


BasicKinect2Grabber::BasicKinect2Grabber(bool init)
{
	_sensor = NULL;
	_cmapper = NULL;
	_depthBuffer = NULL;
	_colourBuffer = NULL;
	_irBuffer = NULL;
	_colour2DepthMap = NULL;
	_depth2ColourMap = NULL;

	_colourFrame = NULL;
	_depthFrame = NULL;
	_irFrame = NULL;
	_multiSourceFrameReader = NULL;
	_multiSourceFrame = NULL;

	if (init)
	{
		if (initialize())
			printf("Successfully connected to Kinect v2 device.\n");
		else
			printf("Unable to find Kinect v2 device.\n");
	}
}

BasicKinect2Grabber::~BasicKinect2Grabber()
{
	if (_sensor)
	{
		_sensor->Close();
		_sensor->Release();
		_sensor = NULL;
	}

	UtilsKinect::safeRelease(_cmapper);
	
	Utils::safeRelease(_depth2ColourMap);
	Utils::safeRelease(_colour2DepthMap);
	Utils::safeRelease(_depthBuffer);
	Utils::safeRelease(_colourBuffer);
	Utils::safeRelease(_irBuffer);
}


bool BasicKinect2Grabber::initialize()
{
	initialized = false;

	HRESULT hr = -1;
	bool success = true;
	GetDefaultKinectSensor(&_sensor);

	if (!_sensor)
		return false;
	
	_sensor->Open();

	hr = _sensor->OpenMultiSourceFrameReader(
		FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Infrared,
		&_multiSourceFrameReader);

	if (FAILED(hr))
		return false;

	success &= initializeColourFrameBuffer();
	success &= initializeDepthFrameBuffer();
	success &= initializeIRFrameBuffer();
	success &= initializeCoordinateMapper();

	if (!success)
		return false;

	initialized = true;
	return true;
}

bool BasicKinect2Grabber::isInitialized()
{
	return initialized;
}

bool BasicKinect2Grabber::update()
{
	bool success = true;

	if (!_multiSourceFrameReader)
		return false;

	HRESULT hr = _multiSourceFrameReader->AcquireLatestFrame(&_multiSourceFrame);
	if (FAILED(hr))
		return false;

	success &= updateColourMat();
	success &= updateDepthMat();
	success &= updateIRMat();

	UtilsKinect::safeRelease(_multiSourceFrame);

	return success;
}

bool BasicKinect2Grabber::initializeColourFrameBuffer()
{
	IFrameDescription* description;
	IColorFrameSource* colourFrameSource;
	IColorFrameReader* colourFrameReader;

	_sensor->get_ColorFrameSource(&colourFrameSource);
	colourFrameSource->OpenReader(&colourFrameReader);

	if (!colourFrameSource)
		return false;

	if (!colourFrameSource)
		return false;

	colourFrameSource->get_FrameDescription(&description);
	description->get_Width(&_colourWidth);
	description->get_Height(&_colourHeight);

	_bufferSizeColour = _colourWidth * _colourHeight;
	_colourBuffer = new RGBQUAD[_bufferSizeColour];

	return true;
}


bool BasicKinect2Grabber::updateColourMat()
{
	bool success = true;
	HRESULT hr = -1;

	IColorFrameReference* colorFrameReference = NULL;
	hr = _multiSourceFrame->get_ColorFrameReference(&colorFrameReference);
	
	if (FAILED(hr))
		return false;

	hr = colorFrameReference->AcquireFrame(&_colourFrame);
	if (SUCCEEDED(hr))
		hr = _colourFrame->CopyConvertedFrameDataToArray(_bufferSizeColour * sizeof(RGBQUAD), reinterpret_cast<BYTE*>(_colourBuffer), ColorImageFormat_Bgra);

	success = SUCCEEDED(hr);
	UtilsKinect::safeRelease(_colourFrame);

	return success;
}


bool BasicKinect2Grabber::updateDepthMat()
{
	bool success = true;
	HRESULT hr = -1;

	IDepthFrameReference* depthFrameReference = NULL;
	hr = _multiSourceFrame->get_DepthFrameReference(&depthFrameReference);
	if (FAILED(hr))
		return false;

	hr = depthFrameReference->AcquireFrame(&_depthFrame);
	if (SUCCEEDED(hr))
		hr = _depthFrame->CopyFrameDataToArray(_bufferSizeDepth, _depthBuffer);
	
	success = SUCCEEDED(hr);
	UtilsKinect::safeRelease(_depthFrame);

	return success;
}


bool BasicKinect2Grabber::updateIRMat()
{
	bool success = true;
	HRESULT hr = -1;

	IInfraredFrameReference* irFrameReference = NULL;
	hr = _multiSourceFrame->get_InfraredFrameReference(&irFrameReference);
	if (FAILED(hr))
		return false;

	hr = irFrameReference->AcquireFrame(&_irFrame);
	if (SUCCEEDED(hr))
		hr = _irFrame->CopyFrameDataToArray(_bufferSizeIR, _irBuffer);

	success = SUCCEEDED(hr);
	UtilsKinect::safeRelease(_irFrame);

	return success;
}


bool BasicKinect2Grabber::initializeDepthFrameBuffer()
{
	IFrameDescription* description;
	IDepthFrameReader* depthFrameReader;
	IDepthFrameSource* depthFrameSource;

	_sensor->get_DepthFrameSource(&depthFrameSource);
	depthFrameSource->OpenReader(&depthFrameReader);

	if (!depthFrameSource)
		return false;

	if (!depthFrameSource)
		return false;

	depthFrameSource->get_FrameDescription(&description);
	description->get_Width(&_depthWidth);
	description->get_Height(&_depthHeight);

	_bufferSizeDepth = _depthWidth * _depthHeight;
	_depthBuffer = new UINT16[_bufferSizeDepth];

	return true;
}


bool BasicKinect2Grabber::initializeIRFrameBuffer()
{
	IFrameDescription* description;
	IInfraredFrameReader* IRFrameReader;
	IInfraredFrameSource* IRFrameSource;

	_sensor->get_InfraredFrameSource(&IRFrameSource);
	IRFrameSource->OpenReader(&IRFrameReader);

	if (!IRFrameSource)
		return false;

	if (!IRFrameSource)
		return false;

	IRFrameSource->get_FrameDescription(&description);
	description->get_Width(&_irWidth);
	description->get_Height(&_irHeight);

	_bufferSizeIR = _irWidth * _irHeight;
	_irBuffer = new UINT16[_bufferSizeIR];

	return true;
}


bool BasicKinect2Grabber::initializeCoordinateMapper()
{
	HRESULT hr = -1;
	hr = _sensor->get_CoordinateMapper(&_cmapper);

	if (FAILED(hr))
		return false;

	_depth2ColourMap = new ColorSpacePoint[_bufferSizeDepth];
	_colour2DepthMap = new DepthSpacePoint[_bufferSizeColour];
	
	hr = _cmapper->MapDepthFrameToColorSpace(_bufferSizeDepth, _depthBuffer, _bufferSizeDepth, _depth2ColourMap);
	hr = _cmapper->MapColorFrameToDepthSpace(_bufferSizeDepth, _depthBuffer, _bufferSizeColour, _colour2DepthMap);

	return SUCCEEDED(hr);
}

ColorSpacePoint* BasicKinect2Grabber::getDepth2ColourMap()
{
	_sensor->get_CoordinateMapper(&_cmapper);
	_cmapper->MapDepthFrameToColorSpace(_bufferSizeDepth, _depthBuffer, _bufferSizeDepth, _depth2ColourMap);
	return _depth2ColourMap;
}

DepthSpacePoint* BasicKinect2Grabber::getColour2DepthMap()
{
	_sensor->get_CoordinateMapper(&_cmapper);
	_cmapper->MapColorFrameToDepthSpace(_bufferSizeDepth, _depthBuffer, _bufferSizeColour, _colour2DepthMap);
	return _colour2DepthMap;
}

