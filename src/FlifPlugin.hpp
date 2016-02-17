/*
	This file is part of qt-flif-plugin.

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

#include <QImageIOHandler>
#include <QImageIOPlugin>

class FlifPlugin: public QImageIOPlugin{
	Q_OBJECT
	
#if QT_VERSION >= 0x050000
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "extensions.json")
#endif
	
	public:
		QStringList keys() const;
		Capabilities capabilities( QIODevice *device, const QByteArray &format ) const;
		QImageIOHandler* create( QIODevice *device, const QByteArray &format = QByteArray() ) const;
};