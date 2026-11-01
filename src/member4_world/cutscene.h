// =====================================================================
//  cutscene.h  -  the 10-20 second intro.
//  Implemented by Friend 3 in member4_world/cutscene.cpp.
// =====================================================================
#pragma once

void cutscene_start();            // reset its timer to 0
void cutscene_update(float dt);   // advance time, animate
void cutscene_draw();             // draw the current frame
bool cutscene_finished();         // true once it is over
