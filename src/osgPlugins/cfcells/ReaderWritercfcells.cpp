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

#include "ReaderWritercfcells.h"
#include <osg/Notify>
#include <osg/Matrix>
#include <osg/MatrixTransform>

#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osg/Texture2D>

#include <stdio.h>
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#include <direct.h>
#define GETCWD _getcwd
#else
#include <unistd.h>
#define GETCWD getcwd
#endif

#define EXTENSION_NAME "cfcells"

class ReaderWritercfcells : public osgDB::ReaderWriter
{
public:
	ReaderWritercfcells()
    {
        supportsExtension(EXTENSION_NAME,"cfcells loader");
    }

    virtual const char* className() const { return "cfcells loader"; }

	virtual ReadResult readNode(const std::string& file, const Options* opt) const
	{
		std::string ext = osgDB::getLowerCaseFileExtension(file);

		if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

		std::string fileName = osgDB::findDataFile(file, opt);
		if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

		// code for setting up the database path so that internally referenced file are searched for on relative paths.
		osg::ref_ptr<Options> local_opt = opt ? static_cast<Options*>(opt->clone(osg::CopyOp::SHALLOW_COPY)) : new Options;
		local_opt->getDatabasePathList().push_front(osgDB::getFilePath(fileName));

		osgDB::ifstream fin(fileName.c_str(), std::ios::in | std::ios::binary);
		if (fin)
		{
			return readNode(fin, local_opt.get());
		}
		return 0L;

	}

	virtual ReadResult readNode(std::istream& fin, const Options* options) const
	{
		main_header mainHeader;
		//stucture is not packet tightly on win64 - read fields into struct
		//fin.read((char *)&mainHeader, sizeof(mainHeader));
		fin.read((char*)&mainHeader.fourcc, sizeof(mainHeader.fourcc));
		fin.read((char*)&mainHeader.file_format_version, sizeof(mainHeader.file_format_version));
		fin.read((char*)&mainHeader.flags, sizeof(mainHeader.flags));
		fin.read((char*)&mainHeader.footer_off, sizeof(mainHeader.footer_off));
		fin.read((char*)&mainHeader.cellCount, sizeof(mainHeader.cellCount));
		fin.read((char*)&mainHeader.footer_size, sizeof(mainHeader.footer_size));

		if (mainHeader.fourcc[0] != 'C' || mainHeader.fourcc[1] != 'F' || mainHeader.fourcc[0] != 'C' || mainHeader.fourcc[0] != 'C' ) {
			OSG_WARN << "readcfcells fourcc mismatch: expected CFCL, not " << mainHeader.fourcc[0] << mainHeader.fourcc[1] << mainHeader.fourcc[2] << mainHeader.fourcc[3] << std::endl;
			return ReadResult::ERROR_IN_READING_FILE;
		}
		cfcells_int64 sizeofMainHeader = sizeof(mainHeader.fourcc) + sizeof(mainHeader.file_format_version) + sizeof(mainHeader.flags) + sizeof(mainHeader.footer_off) + sizeof(mainHeader.cellCount) + sizeof(mainHeader.footer_size);
		cfcells_int64 sizeofCells = mainHeader.footer_off - sizeofMainHeader;
		char* cells = new char[sizeofCells];
		if (!cells)
		{
			OSG_WARN << "ReaderWritercfcells: new cells failed for " << sizeofCells << " bytes." << std::endl;
			return ReadResult::ERROR_IN_READING_FILE;
		}
		//read all cells as a large blob, need to read footer to give meaning to the content
		if (!fin.read((char*)cells, sizeofCells))
		{
			if (fin.eof()) OSG_WARN << "ReaderWritercfcells: couldn't read cells - eof" << std::endl;
			if (fin.fail()) OSG_WARN << "ReaderWritercfcells: couldn't read cells - fail" << std::endl;
			if (fin.bad()) OSG_WARN << "ReaderWritercfcells: couldn't read cells - bad" << std::endl;
			delete[] cells;
			OSG_WARN << "ReaderWritercfcells: couldn't read cells" << std::endl;
			return ReadResult::ERROR_IN_READING_FILE;
		}

		cfcells_int64 sizeofFooters = mainHeader.cellCount * mainHeader.footer_size;
		char* footers = new char[sizeofFooters];
		if (!footers)
		{
			OSG_WARN << "ReaderWritercfcells: new footers failed for " << sizeofFooters << " bytes." << std::endl;
			delete[] cells;
			return ReadResult::ERROR_IN_READING_FILE;
		}
		//read all footers as a large blob
		if (!fin.read((char*)footers, sizeofFooters))
		{
			delete[] footers;
			delete[] cells;
			OSG_WARN << "ReaderWritercfcells: couldn't read footers" << std::endl;
			return ReadResult::ERROR_IN_READING_FILE;
		}

		osg::ref_ptr<osg::Group> loadGroup = new osg::Group;
		//parse cells
		cell_footer *currentFooter = (cell_footer *)footers;
		for (cfcells_int64 cell = 0; cell < mainHeader.cellCount; ++cell) {
			cfcells_int64 cOff = currentFooter->file_pos - sizeofMainHeader;
			char *currentCell = cells + cOff;
			char *currentCellVertices = currentCell + (currentFooter->stationCount * currentFooter->stationSize);
			char *currentCellFaces = currentCellVertices + (currentFooter->vertexCount * currentFooter->vertexSize);
			char *nextCell = currentCellFaces + (currentFooter->faceCount * currentFooter->faceSize);
			if (nextCell - cells > sizeofCells) {
				OSG_WARN << "ReaderWritercfcells: couldn't read cell - data beyond end of data block" << std::endl;
				cOff = -1;
			}
			if ((cOff >= 0) && (currentFooter->type & VERTEX_HAS_POSITION)) {
#if 0
				for (cfcells_int64 station = 0; station < currentFooter->stationCount; ++station) {
					if (currentFooter->type & STATION_HAS_ID) {}
					if (currentFooter->type & STATION_HAS_POSITION) {}
					if (currentFooter->type & STATION_HAS_TYPE) {}
				}
#endif
				// set up the geode.
				osg::Geode* geode = new osg::Geode;
				osg::Geometry* geom = new osg::Geometry;
//				geom->setStateSet(dstate);


//				geom->addPrimitiveSet(new DrawArrays(PrimitiveSet::QUADS, 0, 4));

				osg::Vec3Array* coords = NULL;
				if (currentFooter->type & VERTEX_HAS_POSITION) {
					coords = new osg::Vec3Array(currentFooter->vertexCount);
				}
				osg::Vec4ubArray* colours = NULL;
				if (currentFooter->type & VERTEX_HAS_COLOR) {
					colours = new osg::Vec4ubArray(currentFooter->vertexCount);
				}
				osg::Vec3Array* normals = NULL;
				if (currentFooter->type & VERTEX_HAS_NORMAL) {
					normals = new osg::Vec3Array(currentFooter->vertexCount);
				}
				osg::UIntArray* priStationLow = NULL;
				osg::IntArray*  priStationHigh = NULL;
				if (currentFooter->type & VERTEX_HAS_PRIMARY_STATION) {
					priStationLow = new osg::UIntArray(currentFooter->vertexCount);
					priStationHigh = new osg::IntArray(currentFooter->vertexCount);
				}
				osg::UIntArray* vizFlagsLow = NULL;
				osg::UIntArray* vizFlagsMidLow = NULL;
				osg::UIntArray* vizFlagsMidHigh = NULL;
				osg::UIntArray*  vizFlagsHigh = NULL;
				if (currentFooter->type & VERTEX_HAS_VISIBILITY_FLAGS) {
					vizFlagsLow = new osg::UIntArray(currentFooter->vertexCount);
					vizFlagsMidLow = new osg::UIntArray(currentFooter->vertexCount);
					vizFlagsMidHigh = new osg::UIntArray(currentFooter->vertexCount);
					priStationHigh = new osg::IntArray(currentFooter->vertexCount);
				}
				osg::IntArray*  blobID = NULL;
				if (currentFooter->type & VERTEX_HAS_BLOB_ID) {
					blobID = new osg::IntArray(currentFooter->vertexCount);
				}
				osg::Vec2Array* tcoords = NULL;
				if (currentFooter->type & VERTEX_HAS_TEX_COORDS) {
					tcoords = new osg::Vec2Array(currentFooter->vertexCount);
				}
				osg::ShortArray*  kind = NULL;
				if (currentFooter->type & VERTEX_HAS_KIND) {
					kind = new osg::ShortArray(currentFooter->vertexCount);
				}
				osg::FloatArray* confidence = NULL;
				if (currentFooter->type & VERTEX_HAS_CONFIDENCE_VALUE) {
					confidence = new osg::FloatArray(currentFooter->vertexCount);
				}

				for (cfcells_int64 vertex = 0; vertex < currentFooter->vertexCount; ++vertex) {
					if (currentFooter->type & VERTEX_HAS_POSITION) {
						(*coords)[vertex].set(*(osg::Vec3f *)(currentCellVertices));
						currentCellVertices += 12;
					}
					if (currentFooter->type & VERTEX_HAS_COLOR) {
						(*colours)[vertex].set(currentCellVertices[0], currentCellVertices[1], currentCellVertices[2], currentCellVertices[3] );
						currentCellVertices += 4;
					}
					if (currentFooter->type & VERTEX_HAS_NORMAL) {
						float x = *(float *)currentCellVertices;
						float y = *(float *)(currentCellVertices + 4);
						float z = sqrtf(1 - x * x - y * y);
						(*normals)[vertex].set(x, y, z);
						currentCellVertices += 8;
					}
					if (currentFooter->type & VERTEX_HAS_PRIMARY_STATION) {
						(*priStationLow)[vertex] = *(cfcells_uint32 *)currentCellVertices;
						(*priStationHigh)[vertex] = *(cfcells_int32 *)(currentCellVertices + 4);
						currentCellVertices += 8;
					}
					if (currentFooter->type & VERTEX_HAS_VISIBILITY_FLAGS) {
						(*vizFlagsLow)[vertex] = *(cfcells_uint32 *)currentCellVertices;
						(*vizFlagsMidLow)[vertex] = *(cfcells_uint32 *)(currentCellVertices + 4);
						(*vizFlagsMidHigh)[vertex] = *(cfcells_uint32 *)(currentCellVertices + 8);
						(*vizFlagsHigh)[vertex] = *(cfcells_uint32 *)(currentCellVertices + 12);
						currentCellVertices += 16;
					}
					if (currentFooter->type & VERTEX_HAS_BLOB_ID) {
						(*blobID)[vertex] = *(cfcells_int32 *)currentCellVertices;
						currentCellVertices += 4;
					}
					if (currentFooter->type & VERTEX_HAS_TEX_COORDS) {
						(*tcoords)[vertex].set(*(float*)currentCellVertices, *(float*)(currentCellVertices + 4));
						currentCellVertices += 8;
					}
					if (currentFooter->type & VERTEX_HAS_KIND) {
						(*kind)[vertex] = *(cfcells_int16 *)currentCellVertices;
						currentCellVertices += 2;
					}
					if (currentFooter->type & VERTEX_HAS_CONFIDENCE_VALUE) {
						(*confidence)[vertex] = *(float *)currentCellVertices;
						currentCellVertices += 4;
					}

				}
				if (coords) geom->setVertexArray(coords);
				if (colours) geom->setColorArray(colours, osg::Array::BIND_PER_VERTEX);
				if (normals) geom->setNormalArray(normals, osg::Array::BIND_PER_VERTEX);
				if (priStationLow) {
					geom->setVertexAttribArray(8, priStationLow, osg::Array::BIND_PER_VERTEX);
					geom->setVertexAttribArray(9, priStationHigh, osg::Array::BIND_PER_VERTEX);
				}
				if (vizFlagsLow) {
					geom->setVertexAttribArray(10, vizFlagsLow, osg::Array::BIND_PER_VERTEX);
					geom->setVertexAttribArray(11, vizFlagsMidLow, osg::Array::BIND_PER_VERTEX);
					geom->setVertexAttribArray(12, vizFlagsMidHigh, osg::Array::BIND_PER_VERTEX);
					geom->setVertexAttribArray(13, vizFlagsHigh, osg::Array::BIND_PER_VERTEX);
				}
				if (blobID) {
					geom->setVertexAttribArray(14, blobID, osg::Array::BIND_PER_VERTEX);
				}
				if (tcoords) {
					geom->setTexCoordArray(0, tcoords);
				}
				if (kind) {
					geom->setVertexAttribArray(15, kind, osg::Array::BIND_PER_VERTEX);
				}
				if (confidence) {
					geom->setVertexAttribArray(16, confidence, osg::Array::BIND_PER_VERTEX);
				}
				//process triangles...
				std::vector<std::vector<cfcells_int32>*> meshes;
				std::vector<cfcells_uint32> meshTex;//lookup table: meshes[idx] has texID meshTex[idx]
				if (currentFooter->type & FACE_HAS_VERTEX_INDICES) {
					std::vector<cfcells_int32> *faces = new std::vector<cfcells_int32>;
					faces->reserve(3 * currentFooter->faceCount);
					meshes.push_back(faces);
					std::vector<cfcells_int32>* currentMesh = faces;
					std::vector<cfcells_uint32> texMesh;//lookup table: meshTex[texMesh[texID]] == texID
					cfcells_uint32 currentTexID = 0;

					for (cfcells_int64 face = 0; face < currentFooter->faceCount; ++face) {
						cfcells_uint32 v1, v2, v3;
//						if (currentFooter->type & FACE_HAS_VERTEX_INDICES) {
							v1 = *(cfcells_uint32 *)currentCellFaces;
							v2 = *(cfcells_uint32 *)(currentCellFaces + 4);
							v3 = *(cfcells_uint32 *)(currentCellFaces + 8);
							currentCellFaces += 12;
//						}
						if (currentFooter->type & FACE_HAS_TEXMAP_ID) {
							cfcells_uint32 texID = *(cfcells_uint32 *)currentCellFaces;
							if (face != 0) {
								if (texID != currentTexID) {
									// switch current mesh
									if (texID >= texMesh.size()) texMesh.resize(texID + 1, 0);
									if (meshTex[texMesh[texID]] != texID) { //new mesh
										texMesh[texID] = meshTex.size();
										std::vector<cfcells_int32> *newMesh = new std::vector<cfcells_int32>;
										newMesh->reserve(3 * currentFooter->faceCount);
										meshes.push_back(newMesh);
										meshTex.push_back(texID);
										currentMesh = newMesh;
									}
									currentTexID = texID;
								}
							} else {
								currentTexID = texID;
								meshTex.push_back(texID);
								texMesh.resize(texID + 1, 0);
								texMesh[texID] = 0;
							}
							(*currentMesh).push_back(v1);
							(*currentMesh).push_back(v2);
							(*currentMesh).push_back(v3);
							currentCellFaces += 4;
						}
					}
				} else {
					//what to do with only a texmap id?
					if (currentFooter->type & FACE_HAS_TEXMAP_ID) {
						cfcells_uint32 texID = *(cfcells_uint32 *)currentCellFaces;
						meshTex.push_back(texID);
						currentCellFaces += 4 * currentFooter->faceCount;//skip reading .. no idea what to do with it
					}
					geom->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, coords->size())); //render as points
				}
				
				osg::Matrix mat;
				 
				mat.setTrans(currentFooter->localOrigin[0] - 336825, currentFooter->localOrigin[1] - 5898525, currentFooter->localOrigin[2]);
				osg::MatrixTransform *matTrans = new osg::MatrixTransform(mat);

				for (unsigned int texIndex = 0; texIndex < meshTex.size(); ++texIndex) {// for every mesh
					if (texIndex > 0) {
						geode = new osg::Geode;
						geom = dynamic_cast<osg::Geometry *>(geom->clone(osg::CopyOp::SHALLOW_COPY));
					}
					if (meshes.size() > texIndex) {
						osg::DrawElementsUInt* face_indices = new osg::DrawElementsUInt(GL_TRIANGLES, meshes[texIndex]->size(), (GLuint *)&(meshes[texIndex]->front()));
						delete meshes[texIndex];
						if (texIndex > 0) geom->setPrimitiveSet(0, face_indices);
						else geom->addPrimitiveSet(face_indices);
					}
					std::ostringstream texName;
					texName << meshTex[texIndex] << ".crn" ;
					std::string relTexName = osgDB::concatPaths("textures", texName.str());
					std::string fullTexName = osgDB::findDataFile(relTexName, options);
					std::string texPath = osgDB::getFilePath(fullTexName);
					if (texPath == "textures") {
						char* buf = GETCWD(NULL, 0);
						if (buf) {
							std::string currDirName = osgDB::getSimpleFileName(buf);
							relTexName = osgDB::concatPaths("..", osgDB::concatPaths(currDirName, fullTexName));
							fullTexName = osgDB::concatPaths(buf,fullTexName);
							free(buf);
						}
					} else {
						std::string myPath = osgDB::getFilePath(texPath);
						std::string mySubdir = osgDB::getSimpleFileName(myPath);
						relTexName = osgDB::concatPaths("..", osgDB::concatPaths(mySubdir, relTexName));

					}

					//osg::ref_ptr<osg::Image> image = osgDB::readRefImageFile(fullTexName);
					osg::ref_ptr<osg::Image> image = osgDB::readRefImageFile(relTexName, options);
					osg::Texture2D* texture = new osg::Texture2D;
					texture->setImage(image.get());
					osg::StateSet *tex = new osg::StateSet();
					tex->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

					geode->setStateSet(tex);
					geode->addDrawable(geom);
					matTrans->addChild(geode);
				}
				loadGroup->addChild(matTrans);
			} else {
				if (cOff >= 0) OSG_WARN << "ReaderWritercfcells: couldn't read cell - data not in range" << std::endl;
				else OSG_WARN << "ReaderWritercfcells: cannot handle a vertex with no position" << std::endl;
			}
			//next!
			++currentFooter;
		}

		delete[] footers;
		delete[] cells;
		return loadGroup.release();
    }
};


// Add ourself to the Registry to instantiate the reader/writer.
REGISTER_OSGPLUGIN(cfcells, ReaderWritercfcells)

/*EOF*/

