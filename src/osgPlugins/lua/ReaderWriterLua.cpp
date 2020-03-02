/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2013 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgText/String>

#include "LuaScriptEngine.h"

class ReaderWriterLua : public osgDB::ReaderWriter
{
    public:

        ReaderWriterLua()
        {
            supportsExtension("lua","lua script");
        }

        virtual const char* className() const { return "Lua ScriptEngine plugin"; }

        lua::LuaScriptEngine* createScriptEngine(const osgDB::ReaderWriter::Options* options) const
        {
            osg::ref_ptr<lua::LuaScriptEngine> se = new lua::LuaScriptEngine();

            // add file paths
            if (options) se->addPaths(options);
            else se->addPaths(osgDB::Registry::instance()->getOptions());

            return se.release();
        }


        virtual ReadResult readObjectFromScript(std::istream& fin, const osgDB::ReaderWriter::Options* options =NULL) const
        {
            ReadResult result = readScript(fin, options);

            if (!result.validObject()) return result;
            osg::ref_ptr<osg::Script> script = dynamic_cast<osg::Script*>(result.getObject());
            if (!script) return ReadResult::ERROR_IN_READING_FILE;

            std::string entryPoint = "";
            osg::Parameters inputParameters;
            osg::Parameters outputParameters;

            osg::ref_ptr<lua::LuaScriptEngine> se = createScriptEngine(options);

            //osg::ref_ptr<Options> optString = options ? new osgDB::ReaderWriter::Options(options->getOptionString()) : new osgDB::ReaderWriter::Options();
            //inputParameters.push_back(optString);
            const std::string fn = options->getPluginStringData("filename");
            const std::string arg = options->getPluginStringData("arguments");
            std::vector<std::string> argv;
            argv.push_back(fn);
            size_t start = 0;
            bool quoted = arg[start] == '\"';
            size_t space = arg.find_first_of(quoted ? '\"' : ' ', 1);
            while (space != std::string::npos) {
                if (quoted) argv.push_back(arg.substr(start + 1, space - start - 1));
                else argv.push_back(arg.substr(start, space - start));
                start = space + 1;
                if (quoted && (arg[start] == ' ')) ++start;//skip space after closing quote
                quoted = arg[start] == '\"';
                space = arg.find_first_of(quoted ? '\"' : ' ', start+1);
                if (quoted && (space == arg.length() - 1)) break;
            }
            if (quoted) argv.push_back(arg.substr(start + 1, arg.length() - start - 2));
            else argv.push_back(arg.substr(start));
            //argv.push_back(arg);//todo: split
            int argc = argv.size();
            lua_State *L = se->getLuaState();
            lua_createtable(L, argc, 1);
            for (int i = 0; i < argc; ++i) {
                lua_pushstring(L, argv[i].c_str());
                lua_rawseti(L, -2, i);
            }
            lua_setglobal(L, "arg");

            if (!se->run(script.get(), entryPoint, inputParameters, outputParameters)) return 0;

            if (outputParameters.empty()) return 0;

            typedef std::vector< osg::ref_ptr<osg::Object> > Objects;
            Objects objects;

            for(osg::Parameters::iterator itr = outputParameters.begin();
                itr != outputParameters.end();
                ++itr)
            {
                osg::Object* object = dynamic_cast<osg::Object*>(itr->get());
                if (object) objects.push_back(object);
            }

            if (objects.empty()) return 0;

            if (objects.size()==1)
            {
                return objects[0].get();
            }

            osg::ref_ptr<osg::Group> group = new osg::Group;
            for(Objects::iterator itr = objects.begin();
                itr != objects.end();
                ++itr)
            {
                osg::Node* node = dynamic_cast<osg::Node*>(itr->get());
                if (node) group->addChild(node);
            }

            if (group->getNumChildren()>0) return group.get();
            else return 0;
        }

        virtual ReadResult readObject(std::istream& fin, const osgDB::ReaderWriter::Options* options =NULL) const
        {
            return readObjectFromScript(fin, options);
        }

        virtual ReadResult readObject(const std::string& file, const osgDB::ReaderWriter::Options* options =NULL) const
        {
            if (file=="ScriptEngine.lua")
            {
                return createScriptEngine(options);
            }

            std::string ext = osgDB::getLowerCaseFileExtension(file);
            if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

            std::string fileName = osgDB::findDataFile( file, options );
            if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

            osg::ref_ptr<Options> local_opt = options ? static_cast<Options*>(options->clone(osg::CopyOp::SHALLOW_COPY)) : new Options;
            local_opt->getDatabasePathList().push_front(osgDB::getFilePath(fileName));

            osgDB::ifstream istream(fileName.c_str(), std::ios::in);
            if(!istream) return ReadResult::FILE_NOT_HANDLED;

            return readObject(istream, local_opt.get());
        }

        virtual ReadResult readImage(std::istream& fin, const osgDB::ReaderWriter::Options* options =NULL) const
        {
            return readObjectFromScript(fin, options);
        }

        virtual ReadResult readImage(const std::string& file, const osgDB::ReaderWriter::Options* options =NULL) const
        {
            std::string ext = osgDB::getLowerCaseFileExtension(file);
            if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

            std::string fileName = osgDB::findDataFile( file, options );
            if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

            osg::ref_ptr<Options> local_opt = options ? static_cast<Options*>(options->clone(osg::CopyOp::SHALLOW_COPY)) : new Options;
            local_opt->getDatabasePathList().push_front(osgDB::getFilePath(fileName));

            osgDB::ifstream istream(fileName.c_str(), std::ios::in);
            if(!istream) return ReadResult::FILE_NOT_HANDLED;

            return readImage(istream, local_opt.get());
        }

        virtual ReadResult readNode(std::istream& fin, const osgDB::ReaderWriter::Options* options =NULL) const
        {
            return readObjectFromScript(fin, options);
        }

         virtual ReadResult readNode(const std::string& file, const osgDB::ReaderWriter::Options* options =NULL) const
        {
            std::string ext = osgDB::getLowerCaseFileExtension(file);
            if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

            std::string fileName = osgDB::findDataFile( file, options );
            if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

            osg::ref_ptr<Options> local_opt = options ? static_cast<Options*>(options->clone(osg::CopyOp::SHALLOW_COPY)) : new Options;
            local_opt->getDatabasePathList().push_front(osgDB::getFilePath(fileName));
            //from curl
            local_opt->setPluginStringData("STREAM_FILENAME", osgDB::getSimpleFileName(fileName));
            local_opt->setPluginStringData("filename", fileName);

            osgDB::ifstream istream(fileName.c_str(), std::ios::in);
            if(!istream) return ReadResult::FILE_NOT_HANDLED;

            return readNode(istream, local_opt.get());
        }

        virtual ReadResult readScript(std::istream& fin,const osgDB::ReaderWriter::Options* /*options*/ =NULL) const
        {
            osg::ref_ptr<osg::Script> script = new osg::Script;
            script->setLanguage("lua");

            std::string str;
            while(fin)
            {
                int c = fin.get();
                if (c>=0 && c<=255)
                {
                    str.push_back(c);
                }
            }
            script->setScript(str);

            return script.release();
        }

        virtual ReadResult readScript(const std::string& file, const osgDB::ReaderWriter::Options* options =NULL) const
        {
            std::string ext = osgDB::getLowerCaseFileExtension(file);
            if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

            std::string fileName = osgDB::findDataFile( file, options );
            if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

            osgDB::ifstream istream(fileName.c_str(), std::ios::in);
            if(!istream) return ReadResult::FILE_NOT_HANDLED;

            return readScript(istream, options);
        }

};

// now register with Registry to instantiate the above
// reader/writer.
REGISTER_OSGPLUGIN(lua, ReaderWriterLua)
