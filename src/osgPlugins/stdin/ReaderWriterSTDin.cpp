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

/* file:      src/osgPlugins/stdin/ReaderWriterSTDin.cpp
 * author:    Laurens Voerman 2017-03-02
 * copyright:    (C) 2017 Laurens Voerman
 * license:    OpenSceneGraph Public License (OSGPL)
*/

#include <osg/Notify>
#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include <stdio.h>
#if WIN32
#include <io.h>
#include <fcntl.h>  
#endif
#define EXTENSION_NAME "stdin"

///////////////////////////////////////////////////////////////////////////

/**
 * An OSG reader plugin for the ".stdin" pseudo-loader, which reads from
 * stdin instead of from an actual file.
 *
 * Usage: <dummyfilename>.ext.stdin
 * where:
 *    <dummyfilename> = an model filename.
 *    .ext =the file type used for the input data
 *
 * example: type cow.osg | osgviewer .osg.stdin
 */

class ReaderWriterSTDIN : public osgDB::ReaderWriter
{
public:
    ReaderWriterSTDIN()
    {
        supportsExtension(EXTENSION_NAME,"stdin pseudo loader");
    }

    virtual const char* className() const { return "stdin pseudo-loader"; }

    osgDB::ReaderWriter *getReaderForFilename(const std::string& fileName) const
    {
        std::string ext = osgDB::getLowerCaseFileExtension(fileName);
        if( !acceptsExtension(ext) )
            return NULL;

//        __debugbreak();
        OSG_INFO << "ReaderWriterSTDIN( \"" << fileName << "\" )" << std::endl;

        // strip the pseudo-loader extension
        std::string tmpName = osgDB::getNameLessExtension( fileName );

        std::string subFileExt = osgDB::getFileExtension(tmpName);

        if(subFileExt.empty())
        {
            OSG_WARN << "Missing data extention for " EXTENSION_NAME " pseudo-loader" << std::endl;
            return NULL;
        }
        osgDB::ReaderWriter *reader = osgDB::Registry::instance()->getReaderWriterForExtension(subFileExt);
        if (reader == NULL)
        {
            OSG_NOTICE << "Error: No ReaderWriter for file " << fileName << std::endl;
        }
        return reader;
    }
    const osgDB::ReaderWriter::Options* getOptionsForFilename(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
    {
        std::string ext = osgDB::getFileExtension(osgDB::getNameLessExtension(fileName));
        bool extensionIsAscii = (ext == std::string("osgt"));
        bool extensionIsXML = (ext == std::string("osgx"));
        bool extensionIsBIN = (ext == std::string("osgb"));
        if (extensionIsAscii || extensionIsXML || extensionIsBIN) {
            osg::ref_ptr<osgDB::ReaderWriter::Options> local_opt = options ?
                static_cast<osgDB::ReaderWriter::Options*>(options->clone(osg::CopyOp::SHALLOW_COPY)) :
                new osgDB::ReaderWriter::Options;
            if (extensionIsAscii) local_opt->setPluginStringData("fileType", "Ascii");
            else if (extensionIsXML) local_opt->setPluginStringData("fileType", "XML");
            else  if (extensionIsBIN) local_opt->setPluginStringData("fileType", "Binary");
            return local_opt.release();
          }
        return options;
    }
    std::istream& getBinIstream(std::istream& stream) const
    {
#if WIN32
        if (_setmode(_fileno(stdin), _O_BINARY) == -1)
            OSG_NOTICE << "ERROR: cin to binary:" << strerror(errno);
#endif
        return stream;
    }

    virtual ReadResult readObject(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? ReadResult::FILE_NOT_HANDLED : reader->readObject(getBinIstream(std::cin), local_opt.get());
    }
    virtual ReadResult readImage(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? ReadResult::FILE_NOT_HANDLED : reader->readImage(getBinIstream(std::cin), local_opt.get());
    }
    virtual ReadResult readHeightField(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? ReadResult::FILE_NOT_HANDLED : reader->readHeightField(getBinIstream(std::cin), local_opt.get());
    }
    virtual ReadResult readNode(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? ReadResult::FILE_NOT_HANDLED : reader->readNode(getBinIstream(std::cin), local_opt.get());
    }
    virtual ReadResult readShader(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? ReadResult::FILE_NOT_HANDLED : reader->readShader(getBinIstream(std::cin), local_opt.get());
    }
    virtual ReadResult readScript(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? ReadResult::FILE_NOT_HANDLED : reader->readScript(std::cin, local_opt.get());
    }

    std::ostream& getBinOstream(std::ostream& stream) const
    {
#if WIN32
        if (_setmode(_fileno(stdout), _O_BINARY) == -1)
            OSG_NOTICE << "ERROR: cout to binary:" << strerror(errno);
#endif
        return stream;
    }
    virtual WriteResult writeObject(const osg::Object& obj, const std::string& fileName, const osgDB::ReaderWriter::Options* options)  const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? WriteResult::FILE_NOT_HANDLED : reader->writeObject(obj, getBinOstream(std::cout), local_opt.get());
    }
    virtual WriteResult writeImage(const osg::Image& obj, const std::string& fileName, const osgDB::ReaderWriter::Options* options)  const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? WriteResult::FILE_NOT_HANDLED : reader->writeImage(obj, getBinOstream(std::cout), local_opt.get());
    }
    virtual WriteResult writeHeightField(const osg::HeightField& obj, const std::string& fileName, const osgDB::ReaderWriter::Options* options)  const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? WriteResult::FILE_NOT_HANDLED : reader->writeHeightField(obj, getBinOstream(std::cout), local_opt.get());
    }
    virtual WriteResult writeNode(const osg::Node& obj, const std::string& fileName, const osgDB::ReaderWriter::Options* options)  const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? WriteResult::FILE_NOT_HANDLED : reader->writeNode(obj, getBinOstream(std::cout), local_opt.get());
    }
    virtual WriteResult writeShader(const osg::Shader& obj, const std::string& fileName, const osgDB::ReaderWriter::Options* options)  const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? WriteResult::FILE_NOT_HANDLED : reader->writeShader(obj, getBinOstream(std::cout), local_opt.get());
    }
    virtual WriteResult writeScript(const osg::Script& obj, const std::string& fileName, const osgDB::ReaderWriter::Options* options)  const
    {
        osgDB::ReaderWriter *reader = getReaderForFilename(fileName);
        osg::ref_ptr<const osgDB::ReaderWriter::Options> local_opt = getOptionsForFilename(fileName, options);
        return(reader == NULL) ? WriteResult::FILE_NOT_HANDLED : reader->writeScript(obj, getBinOstream(std::cout), local_opt.get());
    }

};


// Add ourself to the Registry to instantiate the reader/writer.
REGISTER_OSGPLUGIN(EXTENSION_NAME, ReaderWriterSTDIN)

/*EOF*/

