#include <GL/glut.h>
#include "btBulletDynamicsCommon.h"
#include <iostream>
#include <X11/Xlib.h>
#include <thread>
#include <chrono>
#include <cmath>
#include <unistd.h>
using namespace std;
#define THETA SIMD_PI / 4.f

typedef chrono::high_resolution_clock Clock;
typedef chrono::system_clock::time_point TimePoint;

class BalanceGame
{
	private:
		GLfloat vertices[8][3] = {{-20, -2.5, -2.5}, {-20, -2.5, 2.5}, {-20, 2.5, -2.5}, {-20, 2.5, 2.5},{20, -2.5, -2.5}, {20, -2.5, 2.5}, {20, 2.5,-2.5}, {20, 2.5, 2.5}};

		GLint faces[6][4] = {{1, 5, 7, 3}, {5, 4, 6, 7}, {4, 0, 2, 6},{3, 7, 6, 2}, {0, 1, 3, 2}, {0, 4, 5, 1}};
		float tme = 0.0,dtime;
		btScalar matrix[16];
		GLfloat mat[16];
		static float TAR;
		static bool flag;
		btTransform trans;
		btDiscreteDynamicsWorld *dynamicsWorld;
		btRigidBody *stick, *ball;
		btHingeConstraint* hinge;
		btQuaternion qtn;
		btCollisionShape *shape;
		btDefaultMotionState *motionState;
		btDefaultCollisionConfiguration *collisionCfg;
		btBroadphaseInterface *overlappingPairCache;
		btScalar mass;
		btVector3 localInertia;
		TimePoint win;
		
		void change_type(btScalar m[]);
		void update_hinge_pos(float target, float kp, float dt);
		static void keyboardFunc(int);
		static void mouseMotion(int x,int y);
		
				
	public:
		thread *mouse_handle;
		static bool RST,QUIT;
		static float range_of_termination;
		static float time_of_termination;
		static float entry_time,elapsed_time;
		btVector3 ballPos,ballVel;
		float curang,motionang;
		void reset_env(int,int);
		void draw();
		void timer();
		void isGameDone();
		static void handleMouse();
		BalanceGame(int argc, char** argv);
		~BalanceGame();	
};
