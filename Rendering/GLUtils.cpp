// This file is part of the SimMedTK project.
// Copyright (c) Center for Modeling, Simulation, and Imaging in Medicine,
//                        Rensselaer Polytechnic Institute
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//---------------------------------------------------------------------------
//
// Authors:
//
// Contact:
//---------------------------------------------------------------------------

#include "Rendering/GLUtils.h"
#include "Rendering/TextureManager.h"

// VTK glew library
#include <vtk_glew.h>

#ifndef __APPLE__
#  include <GL/gl.h> // for GLfloat, etc.
#  include <GL/glu.h> // for GLfloat, etc.
#else
#  include <OpenGL/gl.h> // for GLfloat, etc.
#endif

#ifndef _MSC_VER
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

///checks the openGL error. if there is an error then it returns
///the error text otherwise it returns nullptr
bool GLUtils::queryGLError(std::string& err)
{
    GLenum errCode;
    const GLubyte *errString;

    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);

        err = "OpenGL Error: " + std::string(reinterpret_cast<const char*>(errString)) + "\n";

        return true;
    }
    else
    {
        return false;
    }
}

///taken from glProgramming.com.  Checks the extension.
bool GLUtils::QueryExtension(const std::string& extName)
{
    auto it = std::find(
        openGLExtensions.begin(),openGLExtensions.end(),extName);

    return it != openGLExtensions.end();
}

///fade background draw
void GLUtils::fadeBackgroundDraw()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, 0.1, 1.1);
    glBegin(GL_POLYGON);
    glColor3f(0.4, 0.4, 0.8);
    glVertex3d(-1, -1, -1);
    glColor3f(0.4, 0.4, 0.8);
    glVertex3d(1, -1, -1);
    glColor3f(0.1, 0.1, 0.2);
    glVertex3d(1, 1, -1);
    glColor3f(0.1, 0.1, 0.2);
    glVertex3d(-1, 1, -1);
    glEnd();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}

void GLUtils::drawQuadOnScreen(Color p_color, float p_left,
                                 float p_bottom, float p_right,
                                 float p_top)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1.0, 0.0, 1, 0.1, 1.1);
    glBegin(GL_QUADS);
    glColor4fv(p_color.toGLColor<GLfloat>());
    glVertex3d(p_left, p_bottom, -1.0);
    glVertex3d(p_right, p_bottom, -1.0);
    glVertex3d(p_right, p_top , -1.0);
    glVertex3d(p_left, p_top , -1.0);
    glEnd();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}
void GLUtils::drawUnitQuadOnScreen()
{
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3d(-1, -1, -1);
    glTexCoord2f(1, 0);
    glVertex3d(1, -1, -1);
    glTexCoord2f(1, 1);
    glVertex3d(1, 1.0, -1);
    glTexCoord2f(0, 1);
    glVertex3d(-1, 1.0, -1);
    glEnd();
}
void GLUtils::queryProjectionMatrix(Matrix44f& p_matrix)
{
    glGetFloatv(GL_PROJECTION_MATRIX, p_matrix.data());
}
void GLUtils::queryModelViewMatrix(Matrix44f& p_matrix)
{
    glGetFloatv(GL_MODELVIEW_MATRIX, p_matrix.data());
}
