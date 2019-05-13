/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This application is open source and may be redistributed and/or modified
 * freely and without restriction, both in commercial and non commercial
 * applications, as long as this copyright notice is maintained.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
*/

/* file:    src/osgPlugins/rot/ReaderWriterROT.cpp
 * author:    Mike Weiblen http://mew.cx/ 2005-06-06
 * copyright:    (C) 2005 Michael Weiblen
 * license:    OpenSceneGraph Public License (OSGPL)
*/

#include <osg/Notify>
#include <osg/Matrix>
#include <osg/MatrixTransform>

#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/XmlParser>

#include <ogr_geometry.h>
#include <stdio.h>

#define EXTENSION_NAME "kml"

///////////////////////////////////////////////////////////////////////////

/**
 * An OSG reader plugin for the ".kml" file format, which inserts a
 * transform above the loaded geometry.
 */

class ReaderWriterKML : public osgDB::ReaderWriter
{
public:
    ReaderWriterKML()
    {
        supportsExtension(EXTENSION_NAME,"KML loader");
    }
    virtual const char* className() const { return "KML loader"; }
	virtual bool acceptsExtension(const std::string& extension) const
	{
		if (osgDB::equalCaseInsensitive(extension, EXTENSION_NAME)) return true;
		return false;
	}
    bool match(const std::string& lhs, const std::string& rhs) const
    {
        // check for perfect match
        // if (lhs==rhs) return true;

        // OSG_NOTICE<<"comparing "<<lhs<<" and "<<rhs<<std::endl;

        std::string::const_iterator lhs_itr = lhs.begin();
        std::string::const_iterator rhs_itr = rhs.begin();
        while ((lhs_itr != lhs.end()) && (rhs_itr != rhs.end()))
        {
            char l = *(lhs_itr);
            char r = *(rhs_itr);

            // make sure character is upper case
            if (l >= 'a' && l <= 'z') l = (l - 'a') + 'A';
            if (r >= 'a' && r <= 'z') r = (r - 'a') + 'A';

            // if both characters are equal then move to the next character
            if (l == r)
            {
                lhs_itr++;
                rhs_itr++;
                continue;
            }

            // if space, underscore or hyphon exist then stop over that particular character
            if (l == ' ' || l == '_' || l == '-')
            {
                lhs_itr++;
                continue;
            }
            if (r == ' ' || r == '_' || r == '-')
            {
                rhs_itr++;
                continue;
            }

            break;
        }
        bool matched = (lhs_itr == lhs.end()) && (rhs_itr == rhs.end());
        // OSG_NOTICE<<"  matched "<<matched<<std::endl;
        return matched;
    }
	
	virtual ReadResult readObject(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
	{
		return readNode(fileName, options);
	}
	virtual ReadResult readNode(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
	{
        std::string ext = osgDB::getLowerCaseFileExtension(fileName);
        if (!acceptsExtension(ext))
            return ReadResult::FILE_NOT_HANDLED;
        osgDB::XmlNode::Input input;
        input.open(fileName);
        input.readAllDataIntoBuffer();

        return readXmlNode(input, options);
    }
    osgDB::ReaderWriter::ReadResult ReaderWriterKML::readNode(std::istream& fin, const Options* options) const
    {
        osgDB::XmlNode::Input input;
        input.attach(fin);
        input.readAllDataIntoBuffer();
        return readXmlNode(input, options);
    }
    osgDB::ReaderWriter::ReadResult ReaderWriterKML::readXmlNode(osgDB::XmlNode::Input& input, const osgDB::ReaderWriter::Options* options) const
    {
        osg::ref_ptr<osgDB::XmlNode> doc = new osgDB::XmlNode;
        osgDB::XmlNode* root = 0;

        doc->read(input);
        if (doc == NULL)
        {
            fprintf(stderr, "Document not parsed successfully. \n");
            return ReadResult::FILE_NOT_HANDLED;
        }

        for (osgDB::XmlNode::Children::iterator itr = doc->children.begin();
            itr != doc->children.end() && !root;
            ++itr)
        {
            if (match((*itr)->name, "kml")) root = itr->get();
        }

        if (root == NULL)
        {
            fprintf(stderr, "empty document\n");
            return ReadResult::FILE_NOT_HANDLED;
        }

        if (!match(root->name, "kml"))
        {
            fprintf(stderr, "document of the wrong type, root node != kml");
            return ReadResult::FILE_NOT_HANDLED;
        }
        osgDB::XmlNode* Document = NULL;
        for (osgDB::XmlNode::Children::iterator itr = root->children.begin();
            itr != root->children.end() && !Document;
            ++itr)
        {
            Document = itr->get();
            if (!match(Document->name, "Document"))
            {
                Document = NULL;
            }
        }
        osgDB::XmlNode* Placemark = NULL;
        for (osgDB::XmlNode::Children::iterator itr = Document->children.begin();
            itr != Document->children.end() && !Placemark;
            ++itr)
        {
            Placemark = itr->get();
            if (!match(Placemark->name, "Placemark"))
            {
                Placemark = NULL;
            }
        }
        osgDB::XmlNode* Model = NULL;
        for (osgDB::XmlNode::Children::iterator itr = Placemark->children.begin();
            itr != Placemark->children.end() && !Model;
            ++itr)
        {
            Model = itr->get();
            if (!match(Model->name, "Model"))
            {
                Model = NULL;
            }
        }
        osgDB::XmlNode* Location = NULL;
        for (osgDB::XmlNode::Children::iterator itr = Model->children.begin();
            itr != Model->children.end() && !Location;
            ++itr)
        {
            Location = itr->get();
            if (!match(Location->name, "Location"))
            {
                Location = NULL;
            }
        }
        osgDB::XmlNode* longitude = NULL;
        for (osgDB::XmlNode::Children::iterator itr = Location->children.begin();
            itr != Location->children.end() && !longitude;
            ++itr)
        {
            longitude = itr->get();
            if (!match(longitude->name, "longitude"))
            {
                longitude = NULL;
            }
        }
        osgDB::XmlNode* latitude = NULL;
        for (osgDB::XmlNode::Children::iterator itr = Location->children.begin();
            itr != Location->children.end() && !latitude;
            ++itr)
        {
            latitude = itr->get();
            if (!match(latitude->name, "latitude"))
            {
                latitude = NULL;
            }
        }
        osgDB::XmlNode* altitude = NULL;
        for (osgDB::XmlNode::Children::iterator itr = Location->children.begin();
            itr != Location->children.end() && !altitude;
            ++itr)
        {
            altitude = itr->get();
            if (!match(altitude->name, "altitude"))
            {
                altitude = NULL;
            }
        }
        osgDB::XmlNode* Link = NULL;
        for (osgDB::XmlNode::Children::iterator itr = Model->children.begin();
            itr != Model->children.end() && !Link;
            ++itr)
        {
            Link = itr->get();
            if (!match(Link->name, "Link"))
            {
                Link = NULL;
            }
        }
        osgDB::XmlNode* href = NULL;
        if (Link) {
            for (osgDB::XmlNode::Children::iterator itr = Link->children.begin();
                itr != Link->children.end() && !href;
                ++itr)
            {
                href = itr->get();
                if (!match(href->name, "href"))
                {
                    href = NULL;
                }
            }
        }
        double longitudeVal = osg::asciiToDouble(longitude->contents.c_str());
        double latitudeVal = osg::asciiToDouble(latitude->contents.c_str());
        double altitudeVal = osg::asciiToDouble(altitude->contents.c_str());


        osg::ref_ptr<osg::Node> node = osgDB::readRefNodeFile(href->contents.c_str(), options);

        if (!node)
        {
            // propagate the read failure upwards
            OSG_WARN << "Subfile \"" << href->contents.c_str() << "\" could not be loaded" << std::endl;
            return ReadResult::FILE_NOT_HANDLED;
        }
        //------------------------------------------------------------------------------------------
        OSG_INFO << "ReaderWriterKML( \"" << href->contents.c_str() << "\" )" << std::endl;

        OGRSpatialReference InSR;
        OGRErr osg_error = InSR.importFromEPSG(4326);
        OGRSpatialReference OutSR;
        osg_error = OutSR.importFromEPSG(28992);//Amersfoort / RD New -- Netherlands - Holland - Dutch

		//OGRPoint Point(-latitudeVal, -longitudeVal, altitudeVal);
		OGRPoint Point(longitudeVal, latitudeVal, altitudeVal);
		Point.assignSpatialReference(&InSR);
		osg_error = Point.transformTo(&OutSR);
        double x = Point.getX() - 157000.0;
        double y = Point.getY() - 576000.0;
        double z = Point.getZ() + 41.5;

        /*
        // strip the pseudo-loader extension
        std::string tmpName = osgDB::getNameLessExtension( fileName );

        if (tmpName.empty())
            return ReadResult::FILE_NOT_HANDLED;


        std::string subFileName, params;
        if (!getFilenameAndParams(tmpName, subFileName, params))
        {
            return ReadResult::FILE_NOT_HANDLED;
        }

        if( subFileName.empty())
        {
            OSG_WARN << "Missing subfilename for " EXTENSION_NAME " pseudo-loader" << std::endl;
            return ReadResult::FILE_NOT_HANDLED;
        }

        OSG_INFO << " params = \"" << params << "\"" << std::endl;
        OSG_INFO << " subFileName = \"" << subFileName << "\"" << std::endl;

        float rx, ry, rz;
        int count = sscanf( params.c_str(), "%f,%f,%f", &rx, &ry, &rz );
        if( count != 3 )
        {
            OSG_WARN << "Bad parameters for " EXTENSION_NAME " pseudo-loader: \"" << params << "\"" << std::endl;
            return ReadResult::FILE_NOT_HANDLED;
        }

        // recursively load the subfile.
        osg::ref_ptr<osg::Node> node = osgDB::readRefNodeFile( subFileName, options );
        if( !node )
        {
            // propagate the read failure upwards
            OSG_WARN << "Subfile \"" << subFileName << "\" could not be loaded" << std::endl;
            return ReadResult::FILE_NOT_HANDLED;
        }
        */
        osg::ref_ptr<osg::MatrixTransform> xform = new osg::MatrixTransform;
        xform->setDataVariance( osg::Object::STATIC );
        xform->setMatrix( osg::Matrix::translate(x, y, z));
        xform->addChild( node );
        return xform;
    }
};


// Add ourself to the Registry to instantiate the reader/writer.
REGISTER_OSGPLUGIN(kml, ReaderWriterKML)

/*EOF*/

