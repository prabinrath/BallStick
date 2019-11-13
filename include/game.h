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
#define train 0


typedef chrono::high_resolution_clock Clock;
typedef chrono::system_clock::time_point TimePoint;


class BalanceGame
{
	private:
		GLfloat vertices[8][3] = {{-30, -2.5, -2.5}, {-30, -2.5, 2.5}, {-30, 2.5, -2.5}, {-30, 2.5, 2.5},{30, -2.5, -2.5}, {30, -2.5, 2.5}, {30, 2.5,-2.5}, {30, 2.5, 2.5}};
		GLint faces[6][4] = {{1, 5, 7, 3}, {5, 4, 6, 7}, {4, 0, 2, 6},{3, 7, 6, 2}, {0, 1, 3, 2}, {0, 4, 5, 1}};
		float tme = 0.0,dtime;
		btScalar matrix[16];
		GLfloat mat[16];
		static float range_of_termination;
		static float time_of_termination;
		static TimePoint entry_time;
		static float elapsed_time;
		static TimePoint game_start_time;
		static float TAR;
		static bool setpointer,lock;
		static int x_,y_;
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
		thread *mouse_handle;
		
		void change_type(btScalar m[]);
		void update_hinge_pos(float target, float kp, float dt);
		void isGameDone();
		static void handleEvents();
		static void keyboardFunc(int);
		static void mouseMotion(int x,int y);
		void drawBitmapText(char *string,float x,float y,float z);
		
	public:
		static bool RST,QUIT;
		static bool ISBALLDROPPED;
		btVector3 ballPos,ballVel;
		int score,indivisual_score;
		float fitness_value,total_gametime_in_seconds,penalty_per_drop,award_per_win,
		total_time_spent_near_center;
		float curang,motionang;
		void reset_env(int,int);
		void draw();
		void timer();
		void setTAR(float);
		float getTAR();
		float evaluateFitness1(float mean_distance_from_center);
		float evaluateFitness2(TimePoint GameStartTime,float avg_of_angles,float mean_distance_from_center);
		BalanceGame(int argc, char** argv);
		~BalanceGame();	
};
