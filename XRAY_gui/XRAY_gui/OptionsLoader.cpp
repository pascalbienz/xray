#include "OptionsLoader.h"



OptionsLoader::OptionsLoader(void)
{
}


OptionsLoader::~OptionsLoader(void)
{
}


QMap<QString, QString> OptionsLoader::loadXmlFile(QString path)
{
 	QMap<QString, QString> list;

	QFile file(path);
	if(file.open(QIODevice::Text | QIODevice::ReadOnly))        //the QIODevice must be open
	{
		QXmlStreamReader xml(&file);
		
		while(!xml.atEnd())
		{
			xml.readNext();
			if(xml.isStartElement())
			{
				QString sec(xml.name().toString());
				
				xml.readNext();

				if(!list.contains(sec))
					list.insert(sec, xml.text().toString());
			}
		}

	}


	return list;

}


void OptionsLoader::writeXml(QString path, QMap<QString, QString> list)
{

	QFile file(path);
	if(file.open(QIODevice::Text | QIODevice::WriteOnly)) 
	{

	
	QXmlStreamWriter stream(&file);
	stream.setAutoFormatting(true);
	stream.writeStartDocument();
	
	stream.writeStartElement("Settings");

	for(int i=0;i<list.size();i++)
	{

	
	//stream.writeStartElement(list.keys().at(i));
	//stream.writeAttribute("href", "http://qt.nokia.com/");
	stream.writeTextElement(list.keys().at(i), list.values().at(i));
	//stream.writeEndElement(); // bookmark
	
	}

	stream.writeEndElement();

	stream.writeEndDocument();


	}

}
