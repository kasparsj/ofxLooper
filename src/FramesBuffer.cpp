#include "FramesBuffer.h"
#include "ofMath.h"
#include "ofGraphics.h"
#include "ofLog.h"

FramesBuffer::FramesBuffer()
{}

void FramesBuffer::setCurrentBytesRate( unsigned bytesRate )
{
	this->bytesRate = int( bytesRate ) ;
}

unsigned FramesBuffer::getCurrentBytesRate() const
{
	return unsigned( bytesRate ) ;
}

void FramesBuffer::setCurrentFPS( unsigned fps )
{
	this->fps = int( fps ) ;
}

unsigned FramesBuffer::getCurrentFPS() const
{
	return unsigned( fps ) ;
}

bool FramesBuffer::enoughMemoryToStore( size_t width, size_t height ) const
{
	void * test = malloc( size_t( width * height ) * 3 + 100000000 ) ;
	if( test )
	{
		free( test ) ;
		return true ;
	}
	else
	{
		return false ;
	}
}

bool FramesBuffer::enoughMemoryToStore( ofPixels & pixels ) const
{
	return enoughMemoryToStore( pixels.getWidth(), pixels.getHeight() ) ;
}

bool FramesBuffer::addFrame( ofPixels & pixels, float timestamp )
{
	if( _isPlaying ) return false ;

	if( ! frames.empty() )
	{
		float lastTime = frames.back().time ;

		if( lastTime > timestamp )
		{
			ofLogError( "FramesBuffer::addFrame()" ) << "timestamp is inferior than the last recorded frame time !" ;
			return false ;
		}

		if( timestamp - lastTime < 1.f / float( fps ) ) return true ;
	}

	if( ! enoughMemoryToStore( pixels ) ) return false ;


	Frame frame ;
	frame.image.setUseTexture( false ) ;
	frame.image.setFromPixels( pixels ) ;
	frame.time = timestamp ;

	float w = frame.image.getWidth() ;
	float h = frame.image.getHeight() ;
	int frameBytesSize = 3 * int( w ) * int( h ) ;
	int secondBytesSize = frameBytesSize * fps ;
	float ratio = float( secondBytesSize ) / float( bytesRate ) ;
	float scaleDownFactor = sqrtf( ratio ) ;
	if( scaleDownFactor > 1.f )
	{
		frame.image.resize( int( w / scaleDownFactor ), int( h / scaleDownFactor ) ) ;
	}

	if( frames.empty() )
	{
		bounds.width = frame.image.getWidth();
		bounds.height = frame.image.getHeight();
	}

	frames.push_back( std::move( frame ) ) ;

	// Compute duration
	if( frames.size() > 1 )
	{
		auto it = frames.rbegin() ;
		float t2 = it->time ;
		float t1 = frames.front().time ;
		duration = t2 - t1 ;
	}
	else
	{
		duration = 0.f ;
	}
	duration += 1.f / float( fps ) ;

	return true ;
}

void FramesBuffer::clear()
{
	if( _isPlaying ) stop() ;
	frames.clear() ;
	duration = 0.f ;
	bounds.width = 0.f;
	bounds.height = 0.f;
}

bool FramesBuffer::empty() const
{
	return frames.empty() ;
}

const ofRectangle &FramesBuffer::getBounds() const
{
	return bounds;
}

void FramesBuffer::update()
{
	if( ! _isPlaying ) return ;

	bool newFrame = false ; // Is there a new frame to display ?

	if( isFirstPlaybackFrame )
	{
		// The playback is just starting
		isFirstPlaybackFrame = false ;
		startPlaybackTime = ofGetElapsedTimef() - position ;
		newFrame = true ;
	}
	else if( ! _isPaused )
	{
		// Make the position progress with time
		position = ofGetElapsedTimef() - startPlaybackTime ;
	}

	if( position < duration )
	{
		// Find the frame to display
		if( nextFrameIt != frames.end() )
		{
			while( nextFrameIt != frames.end() && nextFrameIt->time <= position )
			{
				currentImage = &nextFrameIt->image ;
				nextFrameIt++ ;
				newFrame = true ;
			}
		}

		// Compute opacity
		fadeRatio = 1.f ;
		if( isFadeIn && position < fadeInDuration )
		{
			fadeRatio = ofMap( position, 0.f, fadeInDuration, 0.f, 1.f ) ;
		}
		if( isFadeOut && position > duration - fadeOutDuration )
		{
			float ratio = ofMap( position, duration - fadeOutDuration, duration, 1.f, 0.f ) ;
			fadeRatio = std::min( fadeRatio, ratio ) ;
		}
	}
	else
	{
		// We have reach the end of the frames
		if( _isLooping )
		{
			// Start from the beginning
			currentImage = &frames.front().image ;
			nextFrameIt = frames.begin() ;
			nextFrameIt++ ;
			startPlaybackTime = ofGetElapsedTimef() ;
			position = 0.f ;
			newFrame = true ;
		}
		else
		{
			// Stop the playback
			currentImage = nullptr ;
			_isPlaying = false ;
			_isPaused = false ;
			_isPlaybackDone = true ;
			ofNotifyEvent( onPlaybackDone, *this ) ;
			return ;
		}
	}

	if( _isPlaying && currentImage && newFrame )
	{
		// Allocate the display texture if needed
		if(
			! playbackTexture.isAllocated()
			|| int( playbackTexture.getWidth() ) != int( currentImage->getWidth() )
			|| int( playbackTexture.getHeight() ) != int( currentImage->getHeight() )
		)
		{
			playbackTexture.allocate( currentImage->getPixels() ) ;
		}

		// Update the texture with the current frame
		playbackTexture.loadData( currentImage->getPixels() ) ;
	}
}

void FramesBuffer::draw()
{
	if( ! _isPlaying ) return ;
	if( ! currentImage ) return ;
	if( ! playbackTexture.isAllocated() ) return ;

	ofPushStyle() ;
	ofSetColor( 255, int( alpha * fadeRatio ) ) ;
	playbackTexture.draw( bounds ) ;
	ofPopStyle() ;
}

void FramesBuffer::draw( const ofRectangle & rect, ofScaleMode scaleMode )
{
	if( ! _isPlaying ) return ;
	if( ! currentImage ) return ;
	if( ! playbackTexture.isAllocated() ) return ;

	ofRectangle r( 0.f, 0.f, playbackTexture.getWidth(), playbackTexture.getHeight() ) ;
	r.scaleTo( rect, scaleMode ) ;

	ofPushStyle() ;
	ofSetColor( 255, int( alpha * fadeRatio ) ) ;
	playbackTexture.draw( r ) ;
	ofPopStyle() ;
}

bool FramesBuffer::getIsVisible() const
{
	return isVisible ;
}

void FramesBuffer::setIsVisible( bool value )
{
	isVisible = value ;
}

void FramesBuffer::play( bool fromStart )
{
	if( empty() ) return ;
	if( _isPaused && ! fromStart )
	{
		resume() ;
		return ;
	}

	if( fromStart )
	{
		position = 0.f ;
	}

	currentImage = &frames.front().image ;
	nextFrameIt = frames.begin() ;
	nextFrameIt++ ;

	_isPlaying = true ;
	_isPaused = false ;
	_isPlaybackDone = false ;
	startPlaybackTime = ofGetElapsedTimef() - position ;
	isFirstPlaybackFrame = true ;
}

void FramesBuffer::pause()
{
	if( ! _isPlaying ) return ;
	if( _isPaused ) return ;
	_isPaused = true ;
}

void FramesBuffer::resume()
{
	if( ! _isPlaying ) return ;
	if( ! _isPaused ) return ;

	_isPaused = false ;
	startPlaybackTime = ofGetElapsedTimef() - position ;
}

void FramesBuffer::togglePause()
{
	if( _isPaused ) resume() ;
	else pause() ;
}

void FramesBuffer::stop()
{
	_isPlaying = false ;
	_isPaused = false ;
	_isPlaybackDone = false ;
	position = 0.f ;
}

bool FramesBuffer::isPlaying() const
{
	return _isPlaying ;
}

bool FramesBuffer::isPaused() const
{
	return _isPaused ;
}

bool FramesBuffer::isPlaybackDone() const
{
	return _isPlaybackDone ;
}

float FramesBuffer::getDuration() const
{
	return duration ;
}

float FramesBuffer::getPosition() const
{
	return position ;
}

void FramesBuffer::setPosition( float position )
{
	if( position < 0.f ) position = 0.f ;
	else if( position > duration ) position = duration ;
	this->position = position ;
	if( _isPlaying )
	{
		isFirstPlaybackFrame = true ;
		currentImage = &frames.front().image ;
		nextFrameIt = frames.begin() ;
		nextFrameIt++ ;
	}
}

void FramesBuffer::setLoop( bool isLooping )
{
	_isLooping = isLooping ;
}

void FramesBuffer::toggleLoop()
{
	_isLooping = ! _isLooping ;
}

bool FramesBuffer::isLooping() const
{
	return _isLooping ;
}

int FramesBuffer::getAlpha() const
{
	return alpha ;
}

void FramesBuffer::setAlpha( int value )
{
	if( value < 0 ) value = 0 ;
	if( value > 255 ) value = 255 ;
	alpha = value ;
}

void FramesBuffer::setAlpha( float value )
{
	setAlpha( int( value * 255.f ) ) ;
}

bool FramesBuffer::getIsFadeIn() const
{
	return isFadeIn ;
}

void FramesBuffer::setIsFadeIn( bool value )
{
	isFadeIn = value ;
}

float FramesBuffer::getFadeInDuration() const
{
	return fadeInDuration ;
}

void FramesBuffer::setFadeInDuration( float value )
{
	fadeInDuration = value ;
}

bool FramesBuffer::getIsFadeOut() const
{
	return isFadeOut ;
}

void FramesBuffer::setIsFadeOut( bool value )
{
	isFadeOut = value ;
}

float FramesBuffer::getFadeOutDuration() const
{
	return fadeOutDuration ;
}

void FramesBuffer::setFadeOutDuration( float value )
{
	fadeOutDuration = value ;
}

