#pragma once
#include <QString>
#include <QFile>
#include <QMap>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class OptionsLoader
{
public:
	OptionsLoader(void);
	~OptionsLoader(void);
	static QMap<QString, QString> loadXmlFile(QString path);
	static void writeXml(QString path, QMap<QString, QString> list);
};

