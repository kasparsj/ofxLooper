#pragma once
#include <deque>
#include "ofImage.h"
#include "ofEvents.h"
#include "ofRectangle.h"

/// \brief A RAM recorder/player of images sequence.
///
/// Usage:
/// - Instanciate
/// - Set the fps and the bytesrate : setCurrentFPS() and setCurrentBytesRate()
/// - Add some frames : addFrame()
/// - Play the recorded frames like a video stream.
///
class FramesBuffer
{

	public:

		FramesBuffer() ;

		// Record methods /////////////////////////////////////////////////////

		/// \brief Number of bytes per second for the record.
		void setCurrentBytesRate( unsigned bytesRate ) ;
		/// \brief Number of bytes per second for the record.
		unsigned getCurrentBytesRate() const;

		/// \brief Framerate for the record.
		void setCurrentFPS( unsigned fps ) ;
		/// \brief Framerate for the record.
		unsigned getCurrentFPS() const;

		/// \brief Check if there is enough RAM to record another frame.
		///
		/// \param width The width of the frame.
		/// \param height The height of the frame.
		/// \return true if there is enough memory.
		///
		bool enoughMemoryToStore( size_t width, size_t height ) const;

		/// \brief Check if there is enough RAM to record another frame.
		///
		/// \param pixels The frame to record.
		/// \return true if there is enough memory.
		///
		bool enoughMemoryToStore( ofPixels & pixels ) const;

		/// \brief Add a frame to the buffer.
		///
		/// The frame can be resized to fit the bytes rate.
		///
		/// \param pixels The pixels of the frame to store.
		/// \param timestamp The time of this frame. Must be superior than the last
		///                  recorded frame.
		/// \return false if there is not enough RAM or if the timestamp is incorrect.
		///
		bool addFrame( ofPixels & pixels, float timestamp ) ;

		/// \brief Clear the buffer.
		void clear() ;

		/// \brief Does the buffer empty ?
		bool empty() const;

		const ofRectangle & getBounds() const;

		// Playback methods ///////////////////////////////////////////////////

		/// \brief Update the playback process.
		void update() ;
		void draw() ;
		void draw( const ofRectangle & rect, ofScaleMode scaleMode ) ;

		bool getIsVisible() const ;
		void setIsVisible( bool value ) ;

		void play( bool fromStart = false ) ;
		void pause() ;
		void resume() ;
		void togglePause() ;
		void stop() ;

		bool isPlaying() const ;
		bool isPaused() const ;
		bool isPlaybackDone() const ;
		ofEvent< FramesBuffer > onPlaybackDone ;

		float getDuration() const ;
		float getPosition() const ;
		void setPosition( float position ) ;
		void setLoop( bool isLooping ) ;
		void toggleLoop() ;
		bool isLooping() const ;

		int getAlpha() const ;
		void setAlpha( int value ) ;
		void setAlpha( float value ) ;

		bool getIsFadeIn() const ;
		void setIsFadeIn( bool value ) ;
		float getFadeInDuration() const ;
		void setFadeInDuration( float value ) ;
		bool getIsFadeOut() const ;
		void setIsFadeOut( bool value ) ;
		float getFadeOutDuration() const ;
		void setFadeOutDuration( float value ) ;
    int getNumFrames() const { return frames.size(); }

	protected:

		/// \brief A recorded frame with its timestamp.
		struct Frame
		{
			float time ;
			ofImage image ;
		} ;
		std::deque< Frame > frames ;        ///< The recorded frames

		float duration = 0.f ;           ///< Total duration of the record.
		int bytesRate = 2000000 ;        ///< Number of bytes per second for the record.
		int fps = 25 ;                   ///< Framerate for the record.

		bool _isPlaying = false ;           ///< Are we currently playing the recorded frames ?
		bool _isPaused = false ;            ///< When playing, this flag indicate the pause.
		bool _isPlaybackDone = false ;      ///< Does the playback end ?
		bool _isLooping = false ;           ///< Does the playback start over when done ?
		float position = 0.f ;              ///< the position of the playback in seconds.
		float startPlaybackTime = 0.f ;     ///< App time when playback start.
		bool isFirstPlaybackFrame ;         ///< Does the playback just begin ?

		ofImage * currentImage = nullptr ;          ///< Frame to show during playback.
		std::deque< Frame >::iterator nextFrameIt ; ///< Next frame to show during playback
		ofTexture playbackTexture ;                 ///< A texture to receive the pixels of the currentImage.
		ofRectangle bounds ;

		bool isVisible = true ;
		int alpha = 255 ;
		float fadeRatio = 1.f ;
		bool isFadeIn = false ;
		float fadeInDuration = 1.f ;
		bool isFadeOut = false ;
		float fadeOutDuration = 1.f ;
} ;
