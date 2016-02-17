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
		
	public:
		FlifHandler( QIODevice *device ){
			setDevice( device );
			setFormat( "flif" );
			quality = 100;
		}
		
		bool canRead() const;
		bool read( QImage *image );
		bool write( const QImage &image );
		
		bool supportsOption( ImageOption option ) const{
			switch( option ){
				case Quality: return true;
				default: return false;
			};
		}
		
		void setOption( ImageOption option, const QVariant& value ){
			if( option == Quality )
				quality = value.toInt();
		}
};

bool FlifHandler::canRead() const{
	return format() == "flif";
}


bool FlifHandler::read( QImage *img_pointer ){
	QByteArray data = device()->readAll();
	int width, height;
	
	if( !decoder.decodeMemory( data.constData(), data.size() ) )
		return false;
	
	if( decoder.imageCount() == 0 )
		return false;
	
	auto img = decoder.getImage( 0 );
	QImage out( img.getWidth(), img.getHeight(), QImage::Format_ARGB32 );
	
	auto buffer = std::make_unique<uint8_t[]>( out.width() * 4 );
 	for( unsigned iy=0; iy<out.height(); iy++ ){
		img.readRowRgba8( iy, buffer.get(), out.width() * 4 );
		auto line = (QRgb*)out.scanLine( iy );
		
		for( unsigned ix=0; ix<out.width(); ix++ )
			line[ix] = qRgba( buffer[ix*4+0], buffer[ix*4+1], buffer[ix*4+2], buffer[ix*4+3] );
	}
	
	*img_pointer = out;
	return true;
}

bool FlifHandler::write( const QImage &img ){
	QImage image = img;
	bool alpha = image.hasAlphaChannel();
	unsigned pixel_count = alpha ? 4 : 3;
	unsigned stride = pixel_count * image.width();
	/*
	uint8_t* data = new uint8_t[ stride * image.height() ];
	if( !data )
		return false;
	
	//Make sure the input is in ARGB
	if( image.format() != QImage::Format_RGB32 && image.format() != QImage::Format_ARGB32 )
		image = image.convertToFormat( QImage::Format_ARGB32 );
	
	for( int iy=0; iy<image.height(); ++iy ){
		const QRgb* in = (const QRgb*)image.constScanLine( iy );
		uint8_t* out = data + iy*stride;
		
		for( int ix=0; ix<image.width(); ++ix, ++in ){
			*(out++) = qRed( *in );
			*(out++) = qGreen( *in );
			*(out++) = qBlue( *in );
			if( alpha )
				*(out++) = qAlpha( *in );
		}
	}
	
	uint8_t* output = NULL;
	size_t size;
	
	if( quality == 100 ){
		//Lossless
		if( alpha )
			size = WebPEncodeLosslessRGBA( data, image.width(), image.height(), stride, &output );
		else
			size = WebPEncodeLosslessRGB( data, image.width(), image.height(), stride, &output );
	}
	else{
		//Lossy
		if( alpha )
			size = WebPEncodeRGBA( data, image.width(), image.height(), stride, quality+1, &output );
		else
			size = WebPEncodeRGB( data, image.width(), image.height(), stride, quality+1, &output );
	}
	
	delete[] data;
	if( !output || size == 0 )
		return false;
	
	device()->write( (char*)output, size );
	free( output );
	*/
	return true;
}


QStringList FlifPlugin::keys() const{
	return QStringList() << "flif";
}

QImageIOPlugin::Capabilities FlifPlugin::capabilities( QIODevice *device, const QByteArray &format ) const{
	if( format == "flif" )
		return Capabilities( CanRead /*| CanWrite*/ );
	else
		return 0;
}

QImageIOHandler* FlifPlugin::create( QIODevice *device, const QByteArray &format ) const{
	return new FlifHandler( device );
}


