#ifndef ENGINEWINDOWEVENT_H
#define ENGINEWINDOWEVENT_H

#include "../System.h"
#include "../libraries/SDL/include/SDL.h"
#include "EventDelegate.h"

#define WINDOWEVENT_SHOWN SDL_WINDOWEVENT_SHOWN
#define WINDOWEVENT_HIDDEN SDL_WINDOWEVENT_HIDDEN
#define WINDOWEVENT_EXPOSED SDL_WINDOWEVENT_EXPOSED
#define WINDOWEVENT_MOVED SDL_WINDOWEVENT_MOVED
#define WINDOWEVENT_RESIZED SDL_WINDOWEVENT_RESIZED
#define WINDOWEVENT_MINIMIZED SDL_WINDOWEVENT_MINIMIZED
#define WINDOWEVENT_MAXIMIZED SDL_WINDOWEVENT_MAXIMIZED
#define WINDOWEVENT_RESTORED SDL_WINDOWEVENT_RESTORED
#define WINDOWEVENT_MOUSE_ENTERED SDL_WINDOWEVENT_ENTER
#define WINDOWEVENT_MOUSE_LEAVED SDL_WINDOWEVENT_LEAVE
#define WINDOWEVENT_FOCUS_GAINED SDL_WINDOWEVENT_FOCUS_GAINED
#define WINDOWEVENT_FOCUS_LOST SDL_WINDOWEVENT_FOCUS_LOST
#define WINDOWEVENT_CLOSE SDL_WINDOWEVENT_CLOSE

/**
 * A class to distribute window events.
 */
class EngineWindowEvent {
	
public:
	EngineWindowEvent(SDL_WindowEvent event) {
		windowID = event.windowID;
		type = event.event;
		data = ivec2(event.data1, event.data2);
	}

	/**
	 * The ID of the window the event occurred in
	 */
	uint32_t windowID;

	/**
	 * The type of the window event. See {@link EngineWindowEvent.h} for more
	 */
	uint8_t type;

	/**
	 * The data specific to the event. Represents screen space coordinates
	 */
	ivec2 data;

};


#endif