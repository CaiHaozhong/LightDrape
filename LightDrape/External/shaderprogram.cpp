#include "shaderprogram.h"
#include <fstream>
using std::ifstream;
using std::ios;

#include <sstream>
using std::ostringstream;

#include <sys/stat.h>

ShaderProgram::ShaderProgram() : handle(0), linked(false) { }

bool ShaderProgram::compileShaderFromFile( const char * fileName,
                                         GLSLShader::GLSLShaderType type )
{
    if( ! fileExists(fileName) )
    {
        logString = "File not found.";
        return false;
    }

    if( handle <= 0 ) {
        handle = glCreateProgram();
        if( handle == 0) {
            logString = "Unable to create shader program.";
            return false;
        }
    }

    ifstream inFile( fileName, ios::in );
    if( !inFile ) {
        return false;
    }

    ostringstream code;
    while( inFile.good() ) {
        int c = inFile.get();
        if( ! inFile.eof() )
            code << (char) c;
    }
    inFile.close();
	//std::cout << code.str() << std::endl;
    return compileShaderFromString(code.str(), type);
}

bool ShaderProgram::compileShaderFromString( const std::string & source, GLSLShader::GLSLShaderType type )
{
    if( handle <= 0 ) {
        handle = glCreateProgram();
        if( handle == 0) {
            logString = "Unable to create shader program.";
            return false;
        }
    }

    GLuint shaderHandle = 0;

    switch( type ) {
    case GLSLShader::VERTEX:
        shaderHandle = glCreateShader(GL_VERTEX_SHADER);
        break;
    case GLSLShader::FRAGMENT:
        shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    case GLSLShader::GEOMETRY:
        shaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
        break;
    case GLSLShader::TESS_CONTROL:
        shaderHandle = glCreateShader(GL_TESS_CONTROL_SHADER);
        break;
    case GLSLShader::TESS_EVALUATION:
        shaderHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);
        break;
    default:
        return false;
    }

    const char * c_code = source.c_str();
    glShaderSource( shaderHandle, 1, &c_code, NULL );

    // Compile the shader
    glCompileShader(shaderHandle );

    // Check for errors
    int result;
    glGetShaderiv( shaderHandle, GL_COMPILE_STATUS, &result );
    if( GL_FALSE == result ) {
        // Compile failed, store log and return false
        int length = 0;
        logString = "";
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length );
        if( length > 0 ) {
            char * c_log = new char[length];
            int written = 0;
            glGetShaderInfoLog(shaderHandle, length, &written, c_log);
            logString = c_log;
            delete [] c_log;
        }

        return false;
    } else {
        // Compile succeeded, attach shader and return true
        glAttachShader(handle, shaderHandle);
        return true;
    }
}

bool ShaderProgram::link()
{
    if( linked ) return true;
    if( handle <= 0 ) return false;

    glLinkProgram(handle);

    int status = 0;
    glGetProgramiv( handle, GL_LINK_STATUS, &status);
    if( GL_FALSE == status ) {
        // Store log and return false
        int length = 0;
        logString = "";

        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length );

        if( length > 0 ) {
            char * c_log = new char[length];
            int written = 0;
            glGetProgramInfoLog(handle, length, &written, c_log);
            logString = c_log;
            delete [] c_log;
        }

        return false;
    } else {
        linked = true;
        return linked;
    }
}

void ShaderProgram::use()
{
    if( handle <= 0 || (! linked) ) return;
    glUseProgram( handle );
}

std::string ShaderProgram::log()
{
    return logString;
}

int ShaderProgram::getHandle()
{
    return handle;
}

bool ShaderProgram::isLinked()
{
    return linked;
}

void ShaderProgram::bindAttribLocation( GLuint location, const char * name)
{
    glBindAttribLocation(handle, location, name);
}

void ShaderProgram::bindFragDataLocation( GLuint location, const char * name )
{
    glBindFragDataLocation(handle, location, name);
}

void ShaderProgram::setUniform( const char *name, double x, double y, double z)
{
    int loc = getUniformLocation(name);
    if( loc >= 0 ) {
        glUniform3d(loc,x,y,z);
    } else {
        printf("Uniform: %s not found.\n",name);
    }
}

void ShaderProgram::setUniform( const char *name, const Vec3d & v)
{
    this->setUniform(name,v.values_[0],v.values_[1],v.values_[2]);
}

void ShaderProgram::setUniform( const char *name, const OpenMesh::Vec4d & v)
{
    int loc = getUniformLocation(name);
    if( loc >= 0 ) {
        glUniform4d(loc,v.values_[0],v.values_[1],v.values_[2],v.values_[3]);
    } else {
        printf("Uniform: %s not found.\n",name);
    }
}

void ShaderProgram::setUniform( const char *name, const OpenMesh::Vec2d & v)
{
    int loc = getUniformLocation(name);
    if( loc >= 0 ) {
        glUniform2d(loc,v.values_[0],v.values_[1]);
    } else {
        printf("Uniform: %s not found.\n",name);
    }
}

void ShaderProgram::setUniform( const char *name, const double*  m)
{
    int loc = getUniformLocation(name);
    if( loc >= 0 )
    {
        glUniformMatrix4dv(loc, 1, GL_FALSE, m);
    } else {
        printf("Uniform: %s not found.\n",name);
    }
}

// void ShaderProgram::setUniform( const char *name, const mat3 & m)
// {
//     int loc = getUniformLocation(name);
//     if( loc >= 0 )
//     {
//         glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
//     } else {
//         printf("Uniform: %s not found.\n",name);
//     }
// }

void ShaderProgram::setUniform( const char *name, double val )
{
    int loc = getUniformLocation(name);
    if( loc >= 0 )
    {
        glUniform1d(loc, val);
    } else {
        printf("Uniform: %s not found.\n",name);
    }
}

void ShaderProgram::setUniform( const char *name, int val )
{
    int loc = getUniformLocation(name);
    if( loc >= 0 )
    {
        glUniform1i(loc, val);
    } else {
        printf("Uniform: %s not found.\n",name);
    }
}

void ShaderProgram::setUniform( const char *name, bool val )
{
    int loc = getUniformLocation(name);
    if( loc >= 0 )
    {
        glUniform1i(loc, val);
    } else {
        printf("Uniform: %s not found.\n",name);
    }
}

void ShaderProgram::printActiveUniforms() {

    GLint nUniforms, size, location, maxLen;
    GLchar * name;
    GLsizei written;
    GLenum type;

    glGetProgramiv( handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
    glGetProgramiv( handle, GL_ACTIVE_UNIFORMS, &nUniforms);

    name = (GLchar *) malloc( maxLen );

    printf(" Location | Name\n");
    printf("------------------------------------------------\n");
    for( int i = 0; i < nUniforms; ++i ) {
        glGetActiveUniform( handle, i, maxLen, &written, &size, &type, name );
        location = glGetUniformLocation(handle, name);
        printf(" %-8d | %s\n",location, name);
    }

    free(name);
}

void ShaderProgram::printActiveAttribs() {

    GLint written, size, location, maxLength, nAttribs;
    GLenum type;
    GLchar * name;

    glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
    glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &nAttribs);

    name = (GLchar *) malloc( maxLength );

    printf(" Index | Name\n");
    printf("------------------------------------------------\n");
    for( int i = 0; i < nAttribs; i++ ) {
        glGetActiveAttrib( handle, i, maxLength, &written, &size, &type, name );
        location = glGetAttribLocation(handle, name);
        printf(" %-5d | %s\n",location, name);
    }

    free(name);
}

bool ShaderProgram::validate()
{
    if( ! isLinked() ) return false;

    GLint status;
    glValidateProgram( handle );
    glGetProgramiv( handle, GL_VALIDATE_STATUS, &status );

    if( GL_FALSE == status ) {
        // Store log and return false
        int length = 0;
        logString = "";

        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length );

        if( length > 0 ) {
            char * c_log = new char[length];
            int written = 0;
            glGetProgramInfoLog(handle, length, &written, c_log);
            logString = c_log;
            delete [] c_log;
        }

        return false;
    } else {
       return true;
    }
}

int ShaderProgram::getUniformLocation(const char * name )
{
    return glGetUniformLocation(handle, name);
}

bool ShaderProgram::fileExists( const std::string & fileName )
{
    struct stat info;
    int ret = -1;

    ret = stat(fileName.c_str(), &info);
    return 0 == ret;
}

void ShaderProgram::deleteProgram()
{
    glDeleteProgram(handle);

}
