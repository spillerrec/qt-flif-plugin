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

#include <QImage>
#include <QByteArray>
#include <QImageIOHandler>
#include <QImageIOPlugin>
#include <QColor>
#include <QVariant>
#include <QDebug>
#include <memory>

#include "FlifPlugin.hpp"
#include "Flif++.hpp"

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(pnp_flifplugin, FlifPlugin)
#endif


class FlifHandler: public QImageIOHandler{
	private:
		int quality;
		FlifDecoder decoder;
		int frame{ -1 };
		int lastImageDelay{ 0 };
		
	public:
		FlifHandler( QIODevice *device ){
			setDevice( device );
			setFormat( "flif" );
			quality = 100;
		}
		
		bool loaded() const{ return frame >= 0; }
		
		bool canRead() const override;
		bool read( QImage *image ) override;
		bool write( const QImage &image ) override;
		
		bool supportsOption( ImageOption option ) const override{
			switch( option ){
				case Quality: return true;
				case Animation: return true;
				default: return false;
			};
		}
		
		bool jumpToImage( int index ) override{
			if( index < imageCount() ){
				frame = index;
				return true;
			}
			return false;	
		}
		bool jumpToNextImage() override{ return jumpToImage(frame+1); }
		int imageCount() const override{ return loaded() ? decoder.imageCount() : 0; }
		int nextImageDelay() const override{ return lastImageDelay; }
		int loopCount() const override{ return /*decoder.loopCount()*/-1; } //TODO: Figure out this value
		int currentImageNumber() const override{ return frame; }
		
		void setOption( ImageOption option, const QVariant& value ) override{
			switch( option ){
				case Quality: quality = value.toInt(); break;
				default: break;
			};
		}
		QVariant option( ImageOption option ) const override{
			switch( option ){
				case Animation: return loaded() ? loopCount() != 0 : true; //TODO: is true the good default?
				default: return {};
			}
		}
};

bool FlifHandler::canRead() const{
	return format() == "flif";
}


bool FlifHandler::read( QImage *img_pointer ){
	//Read data on first frame
	if( !loaded() ){
		auto data = device()->readAll();
		if( !decoder.decodeMemory( data.constData(), data.size() ) )
			return false;
	}
	
	frame++;
	if( imageCount() <= frame )
		return false;
	
	auto img = decoder.getImage( frame );
	QImage out( img.getWidth(), img.getHeight(), QImage::Format_ARGB32 );
	
	auto buffer = std::make_unique<uint8_t[]>( out.width() * 4 );
 	for( int iy=0; iy<out.height(); iy++ ){
		img.readRowRgba8( iy, buffer.get(), out.width() * 4 );
		auto line = (QRgb*)out.scanLine( iy );
		
		for( int ix=0; ix<out.width(); ix++ )
			line[ix] = qRgba( buffer[ix*4+0], buffer[ix*4+1], buffer[ix*4+2], buffer[ix*4+3] );
	}
	
	lastImageDelay = img.getFrameDelay();
	*img_pointer = out;
	return true;
}

static void addImage( FlifEncoder& encoder, const QImage& in ){
	FlifImage img( in.width(), in.height() );
	
	auto buffer = std::make_unique<uint8_t[]>( in.width() * 4 );
 	for( int iy=0; iy<in.height(); iy++ ){
		auto line = (const QRgb*)in.constScanLine( iy );
		
		for( int ix=0; ix<in.width(); ix++ ){
			buffer[ix*4+0] = qRed( line[ix] );
			buffer[ix*4+1] = qGreen( line[ix] );
			buffer[ix*4+2] = qBlue( line[ix] );
			buffer[ix*4+3] = qAlpha( line[ix] );
		}
		
		img.writeRowRgba8( iy, buffer.get(), in.width() * 4 );
	}
	
	encoder.addImage( img ); //TODO: investigate memory model
}

bool FlifHandler::write( const QImage& image ){
	//No mention of how animation is to be handled, I believe it is not supported
	frame++;
	if( frame != 0 )
		return false;
	
	FlifEncoder encoder;
	
	if( image.format() != QImage::Format_RGB32 && image.format() != QImage::Format_ARGB32 )
		addImage( encoder, image.convertToFormat( QImage::Format_ARGB32 ) );
	else
		addImage( encoder, image );
	
	//Write to device
	char* data { nullptr };
	size_t size { 0 };
	if( !encoder.encodeMemory( (void**)&data, size ) )
		return false;
	device()->write( data, size );
	free( data );
	
	return true;
}


QStringList FlifPlugin::keys() const{
	return QStringList() << "flif";
}

QImageIOPlugin::Capabilities FlifPlugin::capabilities( QIODevice*, const QByteArray &format ) const{
	if( format == "flif" )
		return Capabilities( CanRead | CanWrite );
	else
		return 0;
}

QImageIOHandler* FlifPlugin::create( QIODevice* device, const QByteArray& ) const{
	return new FlifHandler( device );
}


