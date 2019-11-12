/*
 * The definition file for the default application object.
 *
 * Part of the Cyclone physics system.
 *
 * Copyright (c) Icosagon 2003. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */
#include <cstring>
#include <iostream>
#include "ogl_headers.h"
#include "app.h"
#include "timing.h"

#include <GLKit/GLKMathUtils.h>
#include <GLKit/GLKVector3.h>
#include <GLKit/GLKMatrix4.h>
#include <GLKit/GLKMatrixStack.h>

void Application::initGraphics()
{
    glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    setView();
}

void Application::setView()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)width/(double)height, 1.0, 500.0);
//    glMultMatrix( GLKMatrix4MakePerspective(60.0, (double)width/(double)height, 1.0, 500.0).m ); // << .m is the GLfloat* you are accessing
    glMatrixMode(GL_MODELVIEW);
}

void Application::display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_LINES);
    glVertex2i(1, 1);
    glVertex2i(639, 319);
    glEnd();
}

const char* Application::getTitle()
{
    return "MayaBrain Demo";
}

void Application::deinit()
{
}

void Application::update() {
    // Find the duration of the last frame in seconds
    float duration = (float) TimingData::get().lastFrameDuration * 0.001f;
    if (duration <= 0.0f) return;
    else if (duration > 0.05f) duration = 0.05f;

    // Update the objects
    updateObjects(duration);

    // Perform the contact generation
    generateContacts();

    // Resolve detected contacts
    resolver.resolveContacts(
            cData.contactArray,
            cData.contactCount,
            duration
    );

    glutPostRedisplay();
}


void Application::key(unsigned char key)
{
    switch(key)
    {
        case 'R': case 'r':
            // Reset the simulation
            reset();
            return;

        case 'C': case 'c':
            // Toggle rendering of contacts
            renderDebugInfo = !renderDebugInfo;
            return;

        case 'P': case 'p':
            // Toggle running the simulation
            pauseSimulation = !pauseSimulation;
            return;

        case ' ':
            // Advance one frame
            autoPauseSimulation = true;
            pauseSimulation = false;
    }

}


void Application::resize(int width, int height)
{
    // Avoid the divide by zero.
    if (height <= 0) height = 1;

    // Set the internal variables and update the view
    Application::width = width;
    Application::height = height;
    glViewport(0, 0, width, height);
    setView();
}

void Application::mouse(int button, int state, int x, int y)
{
    // Set the position
    last_x = x;
    last_y = y;
}

void Application::mouseDrag(int x, int y)
{
    // Remember the position
    last_x = x;
    last_y = y;
}

// The following methods aren't intended to be overloaded
void Application::renderText(float x, float y, const char *text, void *font)
{
    glDisable(GL_DEPTH_TEST);

    // Temporarily set up the view in orthographic projection.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)width, 0.0, (double)height, -1.0, 1.0);

    // Move to modelview mode.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Ensure we have a font
    if (font == NULL) {
        font = GLUT_BITMAP_HELVETICA_10;
    }

    // Loop through characters displaying them.
    size_t len = strlen(text);

    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(x, y);
    for (const char *letter = text; letter < text+len; letter++) {

        // If we meet a newline, then move down by the line-height
        // TODO: Make the line-height a function of the font
        if (*letter == '\n') {
            y -= 12.0f;
            glRasterPos2f(x, y);
        }
        glutBitmapCharacter(font, *letter);
    }

    // Pop the matrices to return to how we were before.
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}

Application::Application()
: resolver(maxContacts*8),
    renderDebugInfo(false),
    pauseSimulation(true),
    autoPauseSimulation(false)
{
    cData.contactArray = contacts;
}

void Application::drawDebug()
{
    if (!renderDebugInfo) return;

    // Recalculate the contacts, so they are current (in case we're
    // paused, for example).
    generateContacts();

    // Render the contacts, if required
    glBegin(GL_LINES);
    for (unsigned i = 0; i < cData.contactCount; i++)
    {
        // Interbody contacts are in green, floor contacts are red.
        if (contacts[i].body[1]) {
            glColor3f(0,1,0);
        } else {
            glColor3f(1,0,0);
        }

        cyclone::Vector3 vec = contacts[i].contactPoint;
        glVertex3f(vec.x, vec.y, vec.z);

        vec += contacts[i].contactNormal;
        glVertex3f(vec.x, vec.y, vec.z);
    }

    glEnd();
}
