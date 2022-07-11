#include "Camera.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "Constants.h"

namespace Graphics
{
	Camera::Camera(int w, int h) : fovy(60.0), lookAt(Eigen::Vector3d(0, 0.1, 0)), eye(Eigen::Vector3d(0, 1, 3)), up(Eigen::Vector3d(0, 1, 0)), m_width(w), m_height(h)
	{
	}

	void Camera::SetCamera(const Eigen::Vector3d &lookAt, const Eigen::Vector3d &eye, const Eigen::Vector3d &up)
	{
		this->lookAt = lookAt, this->eye = eye, this->up = up;
	}

	void Camera::Apply()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fovy, (GLfloat)m_width / (GLfloat)m_height, 0.01, 1000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eye.x(), eye.y(), eye.z(),
				  lookAt.x(), lookAt.y(), lookAt.z(),
				  up.x(), up.y(), up.z());
	}

	void Camera::Pan(int x, int y, int prev_x, int prev_y)
	{
		double delta = (double)prev_y - (double)y;
		delta = 1 - delta / 200.0;
		eye = lookAt - (lookAt - eye) * delta;
	}

	void Camera::Zoom(int x, int y, int prev_x, int prev_y)
	{
		double delta = (double)prev_y - (double)y;
		fovy += delta / 200.0;
	}

	void Camera::Rotate(int x, int y, int prev_x, int prev_y)
	{
		GLint w = m_width;
		GLint h = m_height;

		Eigen::Vector3d prevPoint = GetTrackballPoint(prev_x, prev_y, w, h);
		Eigen::Vector3d curPoint = GetTrackballPoint(x, y, w, h);
		Eigen::Vector3d rotVec = curPoint.cross(prevPoint);
		if (rotVec.norm() < 1E-6)
		{
			return;
		}

		rotVec = UnProject(rotVec);
		double cosT = curPoint.dot(prevPoint) / (curPoint.norm() * prevPoint.norm());
		double sinT = (curPoint.cross(prevPoint)).norm() / (curPoint.norm() * prevPoint.norm());

		double angle = -atan2(sinT, cosT);

		Eigen::Vector3d n = this->lookAt - this->eye;
		n = Rotateq(n, rotVec, angle);
		this->up = Rotateq(this->up, rotVec, angle);
		this->eye = this->lookAt - n;
	}

	void Camera::Translate(int x, int y, int prev_x, int prev_y)
	{
		Eigen::Vector3d delta((double)x - (double)prev_x, (double)y - (double)prev_y, 0);
		delta = UnProject(delta) / 200.0;
		lookAt += delta;
		eye += delta;
	}

	void Camera::SetEye(Eigen::Vector3d eye)
	{
		this->eye = eye;
	}

	void Camera::SetLookAt(Eigen::Vector3d lookAt)
	{
		this->lookAt = lookAt;
	}

	void Camera::SetFocusAt(Eigen::Vector3d lookAt)
	{
		Eigen::Vector3d diff = lookAt - this->lookAt;
		this->lookAt = lookAt;
		this->eye = this->eye + diff;
	}

	void Camera::SetFovy(double fovy)
	{
		this->fovy = fovy;
	}

	Eigen::Vector3d Camera::Rotateq(const Eigen::Vector3d &target, const Eigen::Vector3d &rotateVector, double angle)
	{
		Eigen::Vector3d rv = rotateVector.normalized();

		Eigen::Quaternion<double> rot(cos(angle / 2.0), sin(angle / 2.0) * rv.x(), sin(angle / 2.0) * rv.y(), sin(angle / 2.0) * rv.z());
		rot.normalize();
		Eigen::Quaternion<double> tar(0, target.x(), target.y(), target.z());
		tar = rot.inverse() * tar * rot;

		return Eigen::Vector3d(tar.x(), tar.y(), tar.z());
	}

	Eigen::Vector3d Camera::GetTrackballPoint(int mouseX, int mouseY, int w, int h)
	{
		double rad = sqrt((double)(w * w + h * h)) / 2.0;
		double dx = (double)(mouseX) - (double)w / 2.0;
		double dy = (double)(mouseY) - (double)h / 2.0;
		double dx2pdy2 = dx * dx + dy * dy;

		if (rad * rad - dx2pdy2 <= 0)
			return Eigen::Vector3d(dx, dy, 0);
		else
			return Eigen::Vector3d(dx, dy, sqrt(rad * rad - dx * dx - dy * dy));
	}

	Eigen::Vector3d Camera::UnProject(const Eigen::Vector3d &vec)
	{
		Eigen::Vector3d n = lookAt - eye;
		n.normalize();

		Eigen::Vector3d v = up.cross(n);
		v.normalize();

		Eigen::Vector3d u = n.cross(v);
		u.normalize();

		return vec.z() * n + vec.x() * v + vec.y() * u;
	}

	Eigen::Vector3d Camera::GetWorldPosition(int x, int y)
	{
		GLdouble wx, wy, wz;
		GLint w = m_width;
		GLint h = m_height;
		double aspect_ratio_inv = (double)h / (double)w;

		Eigen::Vector2d screen_pos((double)x / (double)w - 0.5, (1.0 - (double)y / (double)h - 0.5) * aspect_ratio_inv);
		Eigen::Vector2d mouse_position = 8 * screen_pos + Eigen::Vector2d(0, 0);

		GLdouble projection[16];
		GLdouble modelView[16];
		GLint viewPort[4];
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
		glGetIntegerv(GL_VIEWPORT, viewPort);

		GLfloat zCursor, winX, winY;
		winX = (float)x;
		winY = -(float)y;
		glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zCursor);
		gluUnProject(winX, winY, zCursor, modelView, projection, viewPort, &wx, &wy, &wz);
		Eigen::Vector3d world_position((double)wx, (double)wy, (double)wz);
		return world_position;
	}

	void Camera::GetProjectionMatrix(Eigen::Matrix4f &out)
	{
		GLint w = m_width;
		GLint h = m_height;

		out.setIdentity();
		float aspect = float(w) / float(h);
		float theta = fovy * 0.5 * DegreesToRadians;

		float far = 1000;
		float near = 0.01f;

		float range = far - near;
		float invtan = 1. / tan(theta);

		out(0, 0) = invtan / aspect;
		out(1, 1) = invtan;
		out(2, 2) = -(near + far) / range;
		out(3, 2) = -1;
		out(2, 3) = -2 * near * far / range;
		out(3, 3) = 0;
	}

	void Camera::GetViewMatrix(Eigen::Matrix4f &out)
	{
		// Eigen::Vector3d orientation = lookAt - eye;
		// Eigen::Vector3d defaultOri(0, 0, -1);

		// Eigen::Quaterniond rotationInv;
		// rotationInv.setFromTwoVectors(orientation, defaultOri);
		// rotationInv.normalize();

		// Eigen::Matrix4f rotateInv;
		// rotateInv.setIdentity();
		// rotateInv.block<3, 3>(0, 0) = rotationInv.toRotationMatrix().cast<float>();

		// Eigen::Matrix4f translateInv;
		// translateInv.setIdentity();
		// translateInv.block<3,1>(0, 3) = Eigen::Vector3f(-eye[0], -eye[1], -eye[2]);

		// out = rotateInv * translateInv;

		Eigen::Vector3f backward = (eye - lookAt).normalized().cast<float>();
		Eigen::Vector3f upf = up.cast<float>();
		Eigen::Vector3f right = upf.cross(backward);

		upf.normalize();
		right.normalize();

		out.setIdentity();
		out.block<1, 3>(0, 0) = right;
		out.block<1, 3>(1, 0) = upf;
		out.block<1, 3>(2, 0) = backward;

		auto negEye = Eigen::Vector3f(-eye[0], -eye[1], -eye[2]);
		out(0, 3) = right.dot(negEye);
		out(1, 3) = upf.dot(negEye);
		out(2, 3) = backward.dot(negEye);
	}
}