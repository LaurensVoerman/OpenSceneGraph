#include <iostream>
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <osg/Notify>
#include <osg/Group>
#include <osgDB/ReadFile>

#include <osgDB/FileNameUtils>
#include <osgDB/Registry>
#include <osg/Vec4>

class FlipValue : public osg::ValueVisitor
{
public:
	FlipValue() {}

	virtual void apply(GLbyte& v)     {v = -v;}
	virtual void apply(GLshort& v)    {v = -v;}
	virtual void apply(GLint& v)      {v = -v;}
//	virtual void apply(GLushort&){}
//	virtual void apply(GLubyte&) {}
//	virtual void apply(GLuint&)  {}
	virtual void apply(GLfloat& v)    {v = -v;}
	virtual void apply(GLdouble& v)   {v = -v;}
	virtual void apply(osg::Vec2b& v) {v.set(-v[0],-v[1]);}
	virtual void apply(osg::Vec3b& v) {v.set(-v[0],-v[1],-v[2]);}
	virtual void apply(osg::Vec4b& v) {v.set(-v[0],-v[1],-v[2],-v[3]);}
	virtual void apply(osg::Vec2s& v) {v = -v;}
	virtual void apply(osg::Vec3s& v) {v = -v;}
	virtual void apply(osg::Vec4s& v) {v = -v;}
	virtual void apply(osg::Vec2i& v) {v.set(-v[0],-v[1]);}
	virtual void apply(osg::Vec3i& v) {v.set(-v[0],-v[1],-v[2]);}
	virtual void apply(osg::Vec4i& v) {v.set(-v[0],-v[1],-v[2],-v[3]);}
//	virtual void apply(osg::Vec2ub&)  {}
//	virtual void apply(osg::Vec3ub&)  {}
//	virtual void apply(osg::Vec4ub&)  {}
//	virtual void apply(osg::Vec2us&)  {}
//	virtual void apply(osg::Vec3us&)  {}
//	virtual void apply(osg::Vec4us&)  {}
//	virtual void apply(osg::Vec2ui&)  {}
//	virtual void apply(osg::Vec3ui&)  {}
//	virtual void apply(osg::Vec4ui&)  {}
	virtual void apply(osg::Vec2& v)  {v = -v;}
	virtual void apply(osg::Vec3& v)  {v = -v;}
	virtual void apply(osg::Vec4& v)  {v = -v;}
	virtual void apply(osg::Vec2d& v) {v = -v;}
	virtual void apply(osg::Vec3d& v) {v = -v;}
	virtual void apply(osg::Vec4d& v) {v = -v;}

//	virtual void apply(Matrixf&) {}
//	virtual void apply(Matrixd&) {}
protected:
	FlipValue& operator = (const FlipValue&) { return *this; }
};

template<class T>
class FindAllNodesOfTypeVisitor : public osg::NodeVisitor
{
public:
	FindAllNodesOfTypeVisitor(osg::NodeVisitor::TraversalMode tm) : osg::NodeVisitor(tm) {}
	void apply(osg::Node& node)
	{
		if (T* tn = dynamic_cast<T*>(&node)) result.push_back(tn);
		traverse(node);
	}
	std::vector<T*> result;
};

class NormalFlipReader : public osgDB::ReaderWriter
{
    public:
        NormalFlipReader()
        {
            supportsExtension("normalflip","Normals Flip loader");
        }

        virtual const char* className() { return "Normal Flip Pseudo Loader"; }

        virtual bool acceptsExtension(const std::string& extension) const
        {
            return osgDB::equalCaseInsensitive(extension,"normalflip");
        }

        virtual ReadResult readObject(const std::string& fileName, const Options* opt) const
        { return readNode(fileName,opt); }

        virtual ReadResult readNode(const std::string& fileName, const Options* options) const
        {
            std::string ext = osgDB::getFileExtension(fileName);
            if (!acceptsExtension(ext))
                return ReadResult::FILE_NOT_HANDLED;

            if (options)
            {
                std::istringstream iss(options->getOptionString());
                std::string opt;
                while (iss >> opt)
                {
                    if( opt == "help" || opt == "HELP" )
                    {
                        usage();
                    }
                }
            }

            std::string nodeName = osgDB::getNameLessExtension( fileName );
			if (nodeName.empty()) return ReadResult::FILE_NOT_FOUND;
			osg::ref_ptr<osg::Node> node;
			ReaderWriter::ReadResult rr = osgDB::Registry::instance()->readNode(nodeName, options);
			if (rr.validNode()) {
				node = rr.takeNode();
			} else {
				if (rr.error()) OSG_WARN << rr.message() << std::endl;
				if (rr.notEnoughMemory()) OSG_INFO << "Not enought memory to load file " << fileName << std::endl;
				return rr;
			}
			std::map<const osg::Array*, osg::Array*> normalArrays;
			osg::ref_ptr<osg::Node> flipNode = dynamic_cast<osg::Node *>(node->clone(osg::CopyOp::DEEP_COPY_NODES | osg::CopyOp::DEEP_COPY_DRAWABLES));
			osg::ref_ptr<FindAllNodesOfTypeVisitor<osg::Geode>> allGeodes = new FindAllNodesOfTypeVisitor<osg::Geode>(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
			flipNode->accept(*allGeodes);
			for (unsigned int i = 0; i < allGeodes->result.size(); ++i) {
				osg::Geode *geode = allGeodes->result[i];
				for (unsigned int l = 0; l < geode->getNumDrawables(); ++l) {
					osg::Geometry *geom = geode->getDrawable(l)->asGeometry();
					const osg::Array *normals = geom->getNormalArray();
					if (normals->getNumElements() != 0) {
						int refcount = normals->referenceCount();//any shared arrays?
						bool createFlippedNormals = true;
						if (refcount > 2) {//besides original drawable and it's copy in flipnode, more drawables refer to this array. 
							std::map<const osg::Array*, osg::Array*>::const_iterator it = normalArrays.find(normals);
							if (it != normalArrays.end()) {
								osg::Array* flipnormals = it->second;
								geom->setNormalArray(flipnormals);
								if (normals->referenceCount() == flipnormals->referenceCount()) {
									normalArrays.erase(normals); //all references are accounted for.
								}
								createFlippedNormals = false;
							}
						}
						if (createFlippedNormals) {
							osg::Array *flipnormals = dynamic_cast<osg::Array *>(normals->clone(osg::CopyOp::DEEP_COPY_ALL));
							FlipValue flipVisitor;
							unsigned int index, maxindex;
							maxindex = flipnormals->getNumElements();
							for (index = 0; index < maxindex; ++index) flipnormals->accept(index, flipVisitor);
							geom->setNormalArray(flipnormals);
							if (refcount > 2) {
								normalArrays[normals] = flipnormals;//store pointers in lookup table so we can re-use.
							}
						}
					}
				}
			}
			return flipNode.release();
            
        }

    private:
        void usage() const {
            OSG_INFO <<
                "Normals Flip Plugin usage:  <application> [-O options] <model.ext>.normalflip\n"<< std::endl;
        }
};

REGISTER_OSGPLUGIN(normals, NormalFlipReader)
