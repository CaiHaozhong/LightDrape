#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H
#include <gl/glew.h>
#include <gl/freeglut.h>
#include "Vec3d.h"
#include <string>
#include "Common.h"

namespace GLSLShader {
    enum GLSLShaderType {
        VERTEX, FRAGMENT, GEOMETRY,
        TESS_CONTROL, TESS_EVALUATION
    };
};

class ShaderProgram
{
private:
    int  handle;
    bool linked;
    std::string logString;

    int  getUniformLocation(const char * name );
    bool fileExists( const std::string & fileName );

public:
    ShaderProgram();

    bool   compileShaderFromFile( const char * fileName, GLSLShader::GLSLShaderType type );
    bool   compileShaderFromString( const std::string & source, GLSLShader::GLSLShaderType type );
    bool   link();
    bool   validate();
    void   use();

    std::string log();

    int    getHandle();
    bool   isLinked();
    void deleteProgram();

    void   bindAttribLocation( GLuint location, const char * name);
    void   bindFragDataLocation( GLuint location, const char * name );

    void   setUniform( const char *name, double x, double y, double z);
    void   setUniform( const char *name, const OpenMesh::Vec2d & v);
    void   setUniform( const char *name, const Vec3d & v);
    void   setUniform( const char *name, const OpenMesh::Vec4d & v);
    void   setUniform( const char *name, const double* matrix_4x4);
    //void   setUniform( const char *name, const mat3 & m);
    void   setUniform( const char *name, double val );
    void   setUniform( const char *name, int val );
    void   setUniform( const char *name, bool val );

    void   printActiveUniforms();
    void   printActiveAttribs();
};
S_PTR(ShaderProgram);

#endif // SHADER_H
