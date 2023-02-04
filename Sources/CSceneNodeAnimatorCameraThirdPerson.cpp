#include "CSceneNodeAnimatorCameraThirdPerson.h"

#include"ICameraSceneNode.h"
#include"ICursorControl.h"

#define _USE_MATH_DEFINES
#include "math.h"

#define CAMERA_UP_OFFSET 15
#define ROTATION_DELTA 1.5F
#define CURSOR_CENTER irr::core::vector2df(0.5F, 0.5F)

namespace irr::scene {
	void CSceneNodeAnimatorCameraThirdPerson::updateTargetPositionData()
	{
		_targetPosition.X = _radius * sin(_teta) * cos(_fi);
		_targetPosition.Z = _radius * sin(_teta) * sin(_fi);
		_targetPosition.Y = _radius * cos(_teta) + CAMERA_UP_OFFSET;
	}

	void CSceneNodeAnimatorCameraThirdPerson::setFi(float offset)
	{
		_fi += offset;
	}

	float CSceneNodeAnimatorCameraThirdPerson::getFi() { return _fi; }

	void CSceneNodeAnimatorCameraThirdPerson::setTeta(float offset)
	{
		if (offset > 0) {
			if ((_teta + offset) < M_PI) {
				_teta += offset;
			}
		}
		else {
			if ((_teta + offset) >= 0) {
				_teta += offset;
			}
		}
	}

	float CSceneNodeAnimatorCameraThirdPerson::getTeta() { return _teta; }

	void CSceneNodeAnimatorCameraThirdPerson::setRadius(short offset)
	{
		if (offset > 0) {
			if (_radius > 30) {
				_radius += offset;
			}
		}
		else if (offset < 0) {
			if (_radius < 100) {
				_radius += offset;
			}
		}
	}

	float CSceneNodeAnimatorCameraThirdPerson::getRadius() { return _radius; }

	CSceneNodeAnimatorCameraThirdPerson::CSceneNodeAnimatorCameraThirdPerson(irr::gui::ICursorControl* cursorControl)
	{
		_fi = 0;
		_teta = 0;
		_radius = 50;

		// ---------------------------
		_cursorControl = cursorControl;
		_cursorControl->setVisible(false);
	}

	CSceneNodeAnimatorCameraThirdPerson::~CSceneNodeAnimatorCameraThirdPerson() {}

	bool CSceneNodeAnimatorCameraThirdPerson::isEventReceiverEnabled() const { return true; }

	bool CSceneNodeAnimatorCameraThirdPerson::OnEvent(const irr::SEvent& event)
	{
		if (event.EventType != irr::EET_MOUSE_INPUT_EVENT)
			return false;

		auto relativePosition = _cursorControl->getRelativePosition();

		switch (event.MouseInput.Event) {
		case irr::EMIE_MOUSE_MOVED:

			if (relativePosition != CURSOR_CENTER) {

				setFi((0.5F - relativePosition.X) * ROTATION_DELTA);
				setTeta((0.5F - relativePosition.Y) * ROTATION_DELTA);

				updateTargetPositionData();

				_cursorControl->setPosition(CURSOR_CENTER);
			}

			break;
		default:
			return false;
		}

		return true;
	}

	void CSceneNodeAnimatorCameraThirdPerson::animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs)
	{
		irr::scene::ICameraSceneNode* camera = static_cast<irr::scene::ICameraSceneNode*>(node);

		auto parent = camera->getParent();
		irr::core::vector3df parentPosition = parent->getPosition();
		parentPosition.Y += CAMERA_UP_OFFSET;

		camera->setTarget(parentPosition);
		camera->setPosition(_targetPosition);
	}

	irr::scene::ISceneNodeAnimator* CSceneNodeAnimatorCameraThirdPerson::createClone(
		irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager)
	{
		return new CSceneNodeAnimatorCameraThirdPerson(_cursorControl);
	}
}