/*
	This file is part of qt5-flif-plugin.

	qt-flif-plugin is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	qt-flif-plugin is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with qt-flif-plugin.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FLIFPP_HPP
#define FLIFPP_HPP

#include <FLIF/flif.h>
#include <FLIF/flif_dec.h>


class FlifImage{
	private:
		FLIF_IMAGE* image{ nullptr };
		
	public:
		FlifImage( FLIF_IMAGE* image ) : image(image) { }
		FlifImage( uint32_t width, uint32_t height )
			: image( flif_create_image( width, height ) ) {
				//TODO: throw on nullptr
			}
		
		FlifImage( const FlifImage& ) = delete;
		FlifImage( FlifImage&& img ) : image( img.image )
			{ img.image = nullptr; }
		
		~FlifImage(){ /*flif_destroy_image( image );*/ } //TODO: getImage is non-owning?
		
		uint32_t getWidth() const{ return flif_image_get_width( image ); }
		uint32_t getHeight() const{ return flif_image_get_height( image ); }
		uint8_t  getChannelCount() const{ return flif_image_get_nb_channels( image ); }
		uint32_t getFrameDelay() const{ return flif_image_get_frame_delay( image ); }
		
		void readRowRgba8( uint32_t row, void* buffer, size_t buffer_size_bytes )
			{ flif_image_read_row_RGBA8( image, row, buffer, buffer_size_bytes ); }
		
		void writeRowRgba8( uint32_t row, const void* buffer, size_t buffer_size_bytes )
			{ flif_image_write_row_RGBA8( image, row, buffer, buffer_size_bytes ); }
		
};	

class FlifDecoder{
	private:
		FLIF_DECODER* d{ nullptr };
		
	public:
		FlifDecoder() : d( flif_create_decoder() ){
			//TODO: throw out-of-memory exception
		}
		FlifDecoder( const FlifDecoder& ) = delete;
		FlifDecoder( FlifDecoder&& decoder) : d( decoder.d )
			{ decoder.d = nullptr; }
		~FlifDecoder() { flif_destroy_decoder( d ); }
		
		void setQuality( int32_t quality )
			{ flif_decoder_set_quality( d, quality ); }
		
		void setScale( uint32_t scale )
			{ flif_decoder_set_scale( d, scale ); }
		
		void setResize( uint32_t width, uint32_t height )
			{ flif_decoder_set_resize( d, width, height ); }
		
		void setCallback( uint32_t (*callback)(int32_t,int64_t) )
			{ flif_decoder_set_callback( d, callback ); }
		
		void setFirstCallbackQuality( int32_t quality )
			{ flif_decoder_set_first_callback_quality( d, quality ); }
		
		int32_t decodeFile( const char* filename )
			{ return flif_decoder_decode_file( d, filename ); }
		
		int32_t decodeMemory( const void* buffer, size_t buffer_size_bytes )
			{ return flif_decoder_decode_memory( d, buffer, buffer_size_bytes ); }
		
		size_t imageCount() const
			{ return flif_decoder_num_images( d ); }
		
		int32_t loopCount() const
			{ return flif_decoder_num_loops( d ); }
		
		FlifImage getImage( size_t index ) const //??
			{ return { flif_decoder_get_image( d, index ) }; }
};

#endif