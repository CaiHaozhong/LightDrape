#include "KNNSHelper.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <fstream>
#include "HoleInfo.h"
#include <string>
#include <sstream>
#include <tuple>
#include <io.h>
typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;
using namespace std;
typedef std::pair<std::string,std::string> PPair;
struct PLYHeader {
	bool    binary;
	bool    bigEndian;
	bool    isTriangleMesh;
	int     numVertices;
	bool    hasVertexNormals;
	bool    hasVertexColors;
	bool    hasVertexColorAlpha;
	// If true, colors are separated in ambient, diffuse and specular
	bool    hasVertexTexCoords;
	std::vector<PPair> vProps;
	int     numFaces;
	bool    hasFaceNormals;
	bool    hasFaceColors;
	bool    hasFaceColorAlpha;
	std::string valenceType;
	std::string  indexType;
	// If true, colors are separated in ambient, diffuse and specular
	std::vector<PPair> fProps;
};
bool parseHeader(string _filename, PLYHeader& _header) {
	std::ifstream ifs(_filename);
	if (!ifs.is_open() || !ifs.good() || ifs.eof()) {
		cout << "Error: Could not read header data of specified PLY-file! Aborting.";
		return false;
	}
	std::string line;
	std::istringstream sstr;
	std::string dString = "";
	std::string dStringPType = "";
	int         dInt;
	std::string lastElement = "";
	while(dString != "end_header" && !ifs.eof()) {
		// Get whole line
		sstr.clear();
		std::getline(ifs, line);
		sstr.str(line);
		sstr >> dString;
		if(dString == "ply") {
			continue;
		} else if(dString == "format") {
			// Determine file format (either ascii or binary)
			// format ascii/binary version
			sstr >> dString;
			if ( dString.find("ascii") != std::string::npos ) {
				_header.binary = false;
			} else {
				_header.binary = true;
				if ( dString.find("big") != std::string::npos ) {
					_header.bigEndian = true;
				} else {
					if ( dString.find("little") == std::string::npos ) {
						cout << "Binary PLY file without endian specification. Assuming little endian.";
					}
					_header.bigEndian = false;
				}
			}
			// Skip version
			continue;
		} else if(dString == "element") {
			// Get number of elements
			// element vertex/face/edge number
			sstr >> dString;
			sstr >> dInt;
			if(dString == "vertex") {
				lastElement = dString;
				_header.numVertices = dInt;
			} else if (dString == "face") {
				lastElement = dString;
				_header.numFaces = dInt;
			}
			// Skip processing of rest of line
			continue;
		} else if(dString == "property") {
			// Get property data type
			sstr >> dStringPType;
			// Get property name
			sstr >> dString;
			if(dStringPType == "list") {
				_header.valenceType = dString;
				// Get index type
				sstr >> dString;
				_header.indexType = dString;
			} else if(dString == "x" || dString == "y" || dString == "z") {
				_header.vProps.push_back(PPair("xyz",dStringPType));
			} else if(dString == "nx" || dString == "ny" || dString == "nz") {
				if(lastElement == "vertex") {
					_header.hasVertexNormals = true;
					_header.vProps.push_back(PPair("n_xyz",dStringPType));
				} else if(lastElement == "face") {
					_header.hasFaceNormals = true;
					_header.fProps.push_back(PPair("n_xyz",dStringPType));
				}
			} else if(dString == "red" || dString == "green" || dString == "blue") {
				if(lastElement == "vertex") {
					_header.hasVertexColors = true;
					_header.vProps.push_back(PPair("rgb",dStringPType));
				} else if(lastElement == "face") {
					_header.hasFaceColors = true;
					_header.fProps.push_back(PPair("rgb",dStringPType));
				}
			} else if(dString == "alpha" || dString == "a") {
				if(lastElement == "vertex") {
					_header.hasVertexColorAlpha = true;
					_header.vProps.push_back(PPair("rgb",dStringPType));
				} else if(lastElement == "face") {
					_header.hasFaceColorAlpha = true;
					_header.vProps.push_back(PPair("rgb",dStringPType));
				}
			} else if(dString == "u" || dString == "v" || dString == "texture_u" || dString == "texture_v") {
				if(lastElement == "vertex") {
					_header.hasVertexTexCoords = true;
					_header.vProps.push_back(PPair("uv",dStringPType));
				}
			} else if(dString == "ambient_red" || dString == "ambient_green" || dString == "ambient_blue") {
				if(lastElement == "vertex") {
					_header.hasVertexColors = true;
					_header.vProps.push_back(PPair("a_rgb",dStringPType));
				} else if(lastElement == "face") {
					_header.hasFaceColors = true;
					_header.fProps.push_back(PPair("a_rgb",dStringPType));
				}
			} else if(dString == "diffuse_red" || dString == "diffuse_green" || dString == "diffuse_blue") {
				if(lastElement == "vertex") {
					_header.hasVertexColors = true;
					_header.vProps.push_back(PPair("d_rgb",dStringPType));
				} else if(lastElement == "face") {
					_header.hasFaceColors = true;
					_header.fProps.push_back(PPair("d_rgb",dStringPType));
				}
			} else if(dString == "specular_red" || dString == "specular_green" || dString == "specular_blue") {
				if(lastElement == "vertex") {
					_header.hasVertexColors = true;
					_header.vProps.push_back(PPair("s_rgb",dStringPType));
				} else if(lastElement == "face") {
					_header.hasFaceColors = true;
					_header.fProps.push_back(PPair("s_rgb",dStringPType));
				}
			} else {
				if(lastElement == "vertex") {
					_header.vProps.push_back(PPair("unknown",dStringPType));
				} else if(lastElement == "face") {
					_header.fProps.push_back(PPair("unknown",dStringPType));
				}
			}
		} else {
			continue;
		}
	}
	// Now we're at 'end_header'
	// We skip the next numVertices lines in order to
	// get to the face definitions (unless there are any)
	// If the file is binary, we don't know the exact
	// number of bytes to skip in order to reach the
	// face definitions -> assume polymesh here
	if(_header.numFaces == 0 || _header.binary) {
		// We're done here
		return true;
	}
	for(int i = 0; i < _header.numVertices; ++i) {
		std::getline(ifs, line);
	}
	std::getline(ifs, line);
	if(ifs.eof()) {
		cout << "Could not read face valence. Something is wrong with your file!";
		ifs.close();
		return false;
	}
	sstr.clear();
	sstr.str(line);
	// Get number of vertices per face
	sstr >> dInt;
	_header.isTriangleMesh = (dInt == 3);
	ifs.close();
	return true;
}
size_t getTypeSize(std::string _type) {
	if(_type == "char" || _type == "uchar")
		return sizeof(char);
	else if(_type == "short" || _type == "ushort")
		return sizeof(short);
	else if(_type == "float" || _type == "float32")
		return sizeof(float);
	else if(_type == "int" || _type == "int32" || _type == "uint" || _type == "uint32")
		return sizeof(int);
	else if(_type == "double" || _type == "int64" || _type == "uint64")
		return sizeof(double);
	else
		std::cerr << "Unable to parse SizeType. I got " << _type << std::endl;
	// We don't support long type since its size differs depending on
	// the host architecture
	return 0;
}
template <class MeshT>
bool readMeshFileAscii(string _filename, MeshT* _mesh) {
	PLYHeader _header = {0,false,0,0,0,0,0,0,std::vector<PPair>(),
		0,0,0,0,"","",std::vector<PPair>()};
	parseHeader(_filename, _header);
	std::string line;
	std::istringstream sstr;
	std::string dString = "";
	int         dInt;
	uint        du, dv;
	double      dx, dy, dz;
	double      alpha;
	MyMesh::VertexHandle currentVertex;
	std::vector<MyMesh::VertexHandle> vertexIndices;
	// Parse file
	std::ifstream ifs(_filename);
	if (!ifs.is_open() || !ifs.good() || ifs.eof()) {
		cout << "Error: Could not read PLY data! Aborting.";
		return false;
	}
	// Skip header
	while(!ifs.eof()) {
		sstr.clear();
		std::getline(ifs, line);
		sstr.str(line);
		sstr >> dString;
		if(dString == "end_header") break;
	}
	bool vNormals   =  true;
	bool vColors    =  false;
	bool vTexCoords =  false;
	bool fNormals   =  true;
	bool fColors    =  false;
	// Request properties
	if(vNormals)
		_mesh->request_vertex_normals();
	if(vColors)
		_mesh->request_vertex_colors();
	if(vTexCoords)
		_mesh->request_vertex_texcoords2D();
	if(fNormals)
		_mesh->request_face_normals();
	if(fColors)
		_mesh->request_face_colors();
	unsigned int propIndex;
	bool next;
	// Reserve enough space for all vertices to avoid resizing and save time
	vertexIndices.reserve(_header.numVertices);
	// Parse vertices
	for(int i = 0; i < _header.numVertices; ++i) {
		// Get line
		sstr.clear();
		std::getline(ifs, line);
		sstr.str(line);
		propIndex = 0;
		next = false;
		while(!next) {
			if(propIndex >= _header.vProps.size())  {
				next = true;
				break;
			}
			if(_header.vProps[propIndex].first == "xyz") {
				// Parse xyz-coordinates
				sstr >> dx >> dy >> dz;
				// Add vertex
				currentVertex = _mesh->add_vertex(typename MeshT::Point(dx, dy, dz));
				vertexIndices.push_back(currentVertex);
				// Go over to next property
				propIndex += 3;
			} else if (_header.vProps[propIndex].first == "n_xyz" && vNormals) {
				// Parse vertex normal coordinates
				sstr >> dx >> dy >> dz;
				// Set normal
				if ( vNormals ) _mesh->set_normal(currentVertex, typename MeshT::Normal(dx, dy, dz));
				// Go over to next property
				propIndex +=3;
			} else if (_header.vProps[propIndex].first == "rgb" && vColors) {
				bool divide = false;
				if (getTypeSize(_header.vProps[propIndex].second) == 1 ||
					getTypeSize(_header.vProps[propIndex].second) == 4)
					divide = true;
				// Parse vertex colors
				sstr >> dx >> dy >> dz;
				if (divide) {
					dx /= 255.0;
					dy /= 255.0;
					dz /= 255.0;
				}
				if(_header.hasVertexColorAlpha) {
					sstr >> alpha;
					if (divide)
						alpha /= 255.0;
					propIndex++;
				} else {
					alpha = 1.0;
				}
				// Set color
				if ( vColors ) _mesh->set_color(currentVertex, typename MeshT::Color(dx, dy, dz/*,alpha*/));
				// Go over to next property
				propIndex += 3;
			} else if (_header.vProps[propIndex].first == "a_rgb" && vColors) {
				sstr >> dx >> dy >> dz;
				// Go over to next property
				propIndex += 3;
			} else if (_header.vProps[propIndex].first == "d_rgb" && vColors) {
				bool divide = false;
				if (getTypeSize(_header.vProps[propIndex].second) == 1 ||
					getTypeSize(_header.vProps[propIndex].second) == 4)
					divide = true;
				// Parse vertex colors
				sstr >> dx >> dy >> dz;
				if (divide) {
					dx /= 255.0;
					dy /= 255.0;
					dz /= 255.0;
				}
				// Set color
				if ( vColors ) _mesh->set_color(currentVertex, typename MeshT::Color(dx, dy, dz/*, 1.0*/));
				// Go over to next property
				propIndex += 3;
			} else if (_header.vProps[propIndex].first == "s_rgb" && vColors) {
				sstr >> dx >> dy >> dz;
				// Go over to next property
				propIndex += 3;
			} else if (_header.vProps[propIndex].first == "uv" && vTexCoords) {
				// Parse uv-texture coordinates
				sstr >> du >> dv;
				// Set texture coordinate
				if ( vTexCoords )_mesh->set_texcoord2D(currentVertex, typename MeshT::TexCoord2D(du, dv));
				// Go over to next property
				propIndex += 2;
			} else {
				// Nothing else to parse in this line
				next = true;
				break;
			}
		}
	}
	std::vector<typename MeshT::VertexHandle> vlist;
	int index;
	typename MeshT::FaceHandle currentFace;
	// Parse faces
	for(int i = 0; i < _header.numFaces; ++i) {
		// Get line
		sstr.clear();
		std::getline(ifs, line);
		sstr.str(line);
		// Get number of vertices per face
		sstr >> dInt;
		vlist.clear();
		for(int c = 0; c < dInt; ++c) {
			sstr >> index;
			if ( (unsigned int)index < vertexIndices.size() )
				vlist.push_back(vertexIndices[index]);
			else {
				cout << "Illegal index in index List for face %1 vertex %2/%3. Vertices available: %4 ; Index: %5  ";
				return false;
			}
		}
		// Add face
		currentFace = _mesh->add_face(vlist);
		propIndex = 0;
		next = false;
		// Search for further props in same line
		while(!next || _header.fProps.size() != 0) {
			if(propIndex >= _header.fProps.size())  {
				next = true;
				break;
			}
			if (_header.fProps[propIndex].first == "n_xyz" && fNormals) {
				// Parse face normal coordinates
				sstr >> dx >> dy >> dz;
				// Set normal
				if ( fNormals ) _mesh->set_normal(currentFace, typename MeshT::Normal(dx, dy, dz));
				// Go over to next property
				propIndex +=3;
			} else if (_header.fProps[propIndex].first == "rgb" && fColors) {
				bool divide = false;
				if (getTypeSize(_header.fProps[propIndex].second) == 1 ||
					getTypeSize(_header.fProps[propIndex].second) == 4)
					divide = true;
				// Parse face colors
				sstr >> dx >> dy >> dz;
				if (divide) {
					dx /= 255.0;
					dy /= 255.0;
					dz /= 255.0;
				}
				if(_header.hasFaceColorAlpha) {
					sstr >> alpha;
					if (divide)
						alpha /= 255.0;
					propIndex++;
				} else {
					alpha = 1.0;
				}
				// Set color
				if (fColors) _mesh->set_color(currentFace, typename MeshT::Color(dx, dy, dz/*,alpha*/));
				// Go over to next property
				propIndex += 3;
			} else if (_header.fProps[propIndex].first == "a_rgb" && fColors) {
				sstr >> dx >> dy >> dz;
				// Go over to next property
				propIndex += 3;
			} else if (_header.fProps[propIndex].first == "d_rgb" && fColors) {
				bool divide = false;
				if (getTypeSize(_header.fProps[propIndex].second) == 1 ||
					getTypeSize(_header.fProps[propIndex].second) == 4)
					divide = true;
				// Parse face colors
				sstr >> dx >> dy >> dz;
				if (divide) {
					dx /= 255.0;
					dy /= 255.0;
					dz /= 255.0;
				}
				// Set color
				if (fColors) _mesh->set_color(currentFace, typename MeshT::Color(dx, dy, dz/*,1.0*/));
				// Go over to next property
				propIndex += 3;
			} else if (_header.fProps[propIndex].first == "s_rgb" && fColors) {
				sstr >> dx >> dy >> dz;
				// Go over to next property
				propIndex += 3;
			} else {
				// Nothing else to parse in this line
				next = true;
				break;
			}
		}
	}
	// Update normals if we do not have them or did not read them
	if ( !_header.hasFaceNormals || !vNormals)
		_mesh->update_normals();
	ifs.close();
	return true;
}
void transformMesh(MyMesh& mesh){
	for(MyMesh::VertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); it++){
		MyMesh::Point& v = mesh.point(*it);
		MyMesh::Point tmp = MyMesh::Point(v);
		v.values_[0] = -tmp.values_[2];
		v.values_[1] = tmp.values_[1];
		v.values_[2] = tmp.values_[0];
	}
}
void getFiles( string path, string exd, vector<string>& files )
{
	//文件句柄
	long   hFile   =   0;
	//文件信息
	struct _finddatai64_t fileinfo;
	string pathName, exdName;

	if (0 != strcmp(exd.c_str(), ""))
	{
		exdName = "\\*." + exd;
	}
	else
	{
		exdName = "\\*";
	}

	if((hFile = _findfirsti64(pathName.assign(path).append(exdName).c_str(),&fileinfo)) !=  -1)
	{
		do
		{
			//如果是文件夹中仍有文件夹,迭代之
			//如果不是,加入列表
			if((fileinfo.attrib &  _A_SUBDIR))
			{
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
					getFiles( pathName.assign(path).append("\\").append(fileinfo.name), exd, files );
			}
			else
			{
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
					files.push_back(fileinfo.name);
			}
		}while(_findnexti64(hFile, &fileinfo)  == 0);
		_findclose(hFile);
	}
}
// int mainforPLY2OBJAndHoleFill(){
// 	MyMesh mesh;	
// 	string inPath = string("E:\\CG\\DrapeRepository\\Resource\\SCAPE\\scapecomp\\");
// 	string outPath = string("E:\\CG\\DrapeRepository\\Resource\\SCAPE\\scapewatertightobj\\");
// 	for (int i = 0; i < 72; i++){		
// 		if(i == 51) continue;
// 		char filename[20];
// 		sprintf(filename, "mesh0%02d", i);
// 		cout << "processing " << string(filename) << endl;
// 		MyMesh mesh;
// 		string infile = inPath + string(filename) + ".ply";		
// 		readMeshFileAscii(infile, &mesh);
// 		cout << "v: " << mesh.n_vertices() << "\t" << "e: " << mesh.n_edges() << "\t" << "f: " << mesh.n_faces() << endl;
// 		cout << "import suc!" << "\n";
// 		HoleInfo holeInfo(&mesh);
// 		holeInfo.getHoles();
// 		vector<HoleInfo::Hole>* holes = holeInfo.holes();
// 		cout << "holes cout: " << holes->size() << "\n";
// 		holeInfo.fillAllHoles();
// 		cout << "transform mesh... " << endl;
// 		transformMesh(mesh);
// 		cout << "end transform mesh... " << endl;
// 		string s = outPath + string(filename) + ".obj";
// 		bool suc = OpenMesh::IO::write_mesh(mesh, s.c_str());
// 		if(!suc){
// 			cout << "save fail!" << "\n";
// 		}
// 		else
// 			cout << "save suc!" << "\n";
// 		cout << "end processing " << string(filename) << endl;
// 	}	
// 	getchar();
// 	return 0;
// }
int main(){	
	MyMesh mesh;	
	string inPath = string("E:\\Project\\LightDrape\\data\\scapecomp_watertight\\");
	string outPath = string("E:\\Project\\LightDrape\\data\\scapecomp_watertight_transform\\");
	vector<string> files;
	getFiles(inPath,"",files);
	for (int i = 0; i < files.size(); i++){										
		bool suc = OpenMesh::IO::read_mesh(mesh, (inPath+files[i]).c_str());
		if(!suc){
			cout << "import err!" << endl;
			getchar();
			return 0;
		}
		cout << string(files[i]) + " import suc" << endl;
		transformMesh(mesh);
		suc = OpenMesh::IO::write_mesh(mesh, (outPath+files[i]).c_str());
		if(!suc){
			cout << "save fail!" << "\n";
		}
		else
			cout << "save suc!" << "\n";
	}
	getchar();
	return 0;
}