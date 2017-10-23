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

/* file:    src/osgPlugins/rot/ReaderWritercfcells.h
 * author:    Laurens Voerman 2015-04-15
 * copyright:    (C) 2015 Laurens Voerman
 * license:    OpenSceneGraph Public License (OSGPL)
*/
#if defined(_MSC_VER)

typedef __int8  cfcells_int8;
typedef __int16 cfcells_int16;
typedef __int32 cfcells_int32;
typedef __int64 cfcells_int64;

typedef unsigned __int8  cfcells_uint8;
typedef unsigned __int16 cfcells_uint16;
typedef unsigned __int32 cfcells_uint32;
typedef unsigned __int64 cfcells_uint64;

#else

typedef char        cfcells_int8;
typedef short       cfcells_int16;
typedef int         cfcells_int32;
typedef long long   cfcells_int64;
typedef unsigned char        cfcells_uint8;
typedef unsigned short       cfcells_uint16;
typedef unsigned int         cfcells_uint32;
typedef unsigned long long   cfcells_uint64;
#endif

#pragma pack(push, 1)

struct main_header {
	char fourcc[4];//fourCC, always CFCL
    cfcells_int32 file_format_version;//cf-cells file format version
	cfcells_uint32 flags; // (reserved for future use)
	cfcells_int64 footer_off; //offset of cell footers (from start of file in bytes)
	cfcells_int64 cellCount; //number of cells
	cfcells_int32 footer_size; //size of one cell footer (in bytes)
};
struct cell_footer {
	cfcells_int64 file_pos;	// relative(from start of file) adress of cell refered to
	cfcells_uint32 type;    // type of cell(see bitflags table)
	cfcells_int64	stationCount;  // number of stations in cell
	cfcells_int64	vertexCount;   // number of vertices in cell
	cfcells_int64	faceCount;     // number of faces in cell
	cfcells_int32	stationSize;   // size of a station in bytes
	cfcells_int32	vertexSize;    // size of a vertex in bytes
	cfcells_int32	faceSize;      // size of a face in bytes
	cfcells_int64	primaryStation;// primary station of this cell
	       double	localOrigin[3];  // local origin of this cell
	       float	lbbox[6];  // bounding - box of this cell relative to local origin
};

enum typeflags {
	VERTEX_HAS_POSITION           = 0x00000001,  //vertex has position
	VERTEX_HAS_COLOR			  = 0x00000002,	//vertex has color
	VERTEX_HAS_NORMAL			  = 0x00000004,	//vertex has normal
	VERTEX_HAS_PRIMARY_STATION	  = 0x00000008,	//vertex has primary station
	VERTEX_HAS_VISIBILITY_FLAGS	  = 0x00000010,	//vertex has visibility - flags
	VERTEX_HAS_BLOB_ID			  = 0x00000020,	//vertex has blob - id
	VERTEX_HAS_TEX_COORDS		  = 0x00000040,	//vertex has tex - coords
	VERTEX_HAS_KIND				  = 0x00000080,	//vertex has kind (v995+)
	VERTEX_HAS_CONFIDENCE_VALUE	  = 0x00000100,	//vertex has confidence - value (v995+)
	FACE_HAS_VERTEX_INDICES		  = 0x00010000,	//face has vertex - indices
	FACE_HAS_TEXMAP_ID			  = 0x00020000,	//face has texmap - id
	STATION_HAS_ID				  = 0x01000000,	//station has id
	STATION_HAS_POSITION		  = 0x02000000,	//station has position
	STATION_HAS_TYPE			  = 0x04000000	//station has type
};

struct station_record {
	cfcells_int64 id;   //id of station
	cfcells_int32 type;	//type of station
	double pos[3];		//positions per station(not relative to local origin of cell)
};

struct vertex_record {
	float pos[3];     // positions per vertex(relative to local origin of cell)
	cfcells_int32 color; // color per vertex
	float normal[2]; //normals per vertex
	cfcells_int64 primaryStation; // primary station of vertex
	cfcells_int64 visibilityOfStationLow; // 
	cfcells_int64 visibilityOfStationHigh; // uint128_t - visibility of stations per vertex *
	cfcells_int32 blobId; //blob id per vertex(in actual intended use as of version 995, previously unused)
	float tc[2]; //texture coordinates per vertex
	cfcells_uint16 kind; // (v995+)	the(user - defined) ‘kind’ of vertex(per vertex)
	float confidence;    // (v995+)	a ‘confidence’ value associated with the vertex
};

struct face_record {
	cfcells_int32 face[3]; // per face(triangle) a triplet of indices to vertices
	cfcells_int32 textureIndex; //per face an index to the relevant texture-map
};

#pragma pack(pop)