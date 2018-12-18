#ifndef SYSTEMCONTROLLERAXISEVENT_H
#define SYSTEMCONTROLLERAXISEVENT_H

#include "../System.h"
#include "../libraries/SDL/include/SDL.h"
#include "EventDelegate.h"

#define CONTROLLERAXIS_LEFTX SDL_CONTROLLER_AXIS_LEFTX
#define CONTROLLERAXIS_LEFTY SDL_CONTROLLER_AXIS_LEFTY
#define CONTROLLERAXIS_RIGHTX SDL_CONTROLLER_AXIS_RIGHTX
#define CONTROLLERAXIS_RIGHTY SDL_CONTROLLER_AXIS_RIGHTY
#define CONTROLLERAXIS_LEFTTRIGGER SDL_CONTROLLER_AXIS_TRIGGERLEFT
#define CONTROLLERAXIS_RIGHTTRIGGER SDL_CONTROLLER_AXIS_TRIGGERRIGHT

/**
 * A class to distribute controller axis events.
 */
class EngineControllerAxisEvent {

public:
	EngineControllerAxisEvent(SDL_ControllerAxisEvent event) {

		axis = event.axis;
		value = event.value;
		device = event.which;

	}

	/**
	 * The axis which was moved on the controller. See {@link EngineControllerAxisEvent.h} for more.
	 */
	uint8_t axis;

	/**
	 * The value of the axis. Ranges from -32767-32767.
	 */
	int16_t value;

	/**
	 * The device ID of the game controller.
	 */
	int32_t device;

};


#endif