#include <GL/glut.h>
#include "btBulletDynamicsCommon.h"
#include <iostream>
using namespace std;

#define THETA SIMD_PI / 4.f

static float tme = 0.0,dtime;
static btScalar matrix[16];
static GLfloat mat[16];
static float TAR=0;
bool flag = false;
static btTransform trans;
static btDiscreteDynamicsWorld *dynamicsWorld;
static btRigidBody *stick, *ball;
static btHingeConstraint* hinge;
btQuaternion qtn;

GLfloat vertices[8][3] = {{-20, -2.5, -2.5}, {-20, -2.5, 2.5}, {-20, 2.5, -2.5}, {-20, 2.5, 2.5},{20, -2.5, -2.5}, {20, -2.5, 2.5}, {20, 2.5,-2.5}, {20, 2.5, 2.5}};

GLint faces[6][4] = {{1, 5, 7, 3}, {5, 4, 6, 7}, {4, 0, 2, 6},{3, 7, 6, 2}, {0, 1, 3, 2}, {0, 4, 5, 1}};

void change_type(btScalar m[16])
{
	for(int i=0;i<16;i++)
	{
		mat[i]=m[i];
	}
}

static void draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	stick->getMotionState()->getWorldTransform(trans);
	trans.getOpenGLMatrix(matrix);
	change_type(matrix);
	glMultMatrixf(mat);
	//glutSolidCube(20);
	glBegin(GL_QUADS);
	glColor3f(2,0.5,5);
  	for (int i = 0; i < 6; i++) 
  	{
  	    for (int j = 0; j < 4; j++) 
  	    {
      		glVertex3fv((GLfloat*)&vertices[faces[i][j]]);
    	}
  	}
  	glEnd();
  	//cout<<hinge->getHingeAngle()<<endl;
	glPopMatrix();

	glColor3f(1.0, 0.0, 0.0);
	glPushMatrix();	
	ball->getMotionState()->getWorldTransform(trans);
	trans.getOpenGLMatrix(matrix);
	change_type(matrix);
	glMultMatrixf(mat);
	glutSolidSphere(5,200,200);
	glPopMatrix();

	glutSwapBuffers();
}

void update_hinge_pos(float target, float kp, float dt)
{
	float curang = hinge->getHingeAngle()*180/SIMD_PI;
	float diff = (target - curang);
	//cout<<target<<" "<<curang<<" "<<diff<<endl;
	stick->activate();
	if(diff<-1)
	{
		hinge->setMotorTargetVelocity(-kp*-diff);
	}
	else if(diff>1)
	{
		hinge->setMotorTargetVelocity(kp*diff);
	}
	else
	{
		hinge->setMotorTargetVelocity(0);
	}
}

void reset_env()
{
	trans.setIdentity();
	trans.setOrigin(btVector3(0,7.5, 0));
	ball->setCenterOfMassTransform(trans);
	ball->setAngularVelocity(btVector3(0,0,0));
	ball->setLinearVelocity(btVector3(0,0,0));
	trans.setIdentity();
	trans.setOrigin(btVector3(0,0, 0));
	stick->setCenterOfMassTransform(trans);
	stick->setLinearVelocity(btVector3(0,0,0));
	stick->setAngularVelocity(btVector3(0,0,0));
}

static void timer()
{
	dtime = tme;
	tme = glutGet(GLUT_ELAPSED_TIME) / 250.0;
	dtime = tme - dtime;

	if(dynamicsWorld)
	{
		update_hinge_pos(TAR,0.06, dtime);
		dynamicsWorld->stepSimulation(dtime, 10);
	}

	glutPostRedisplay();
}

void keyboardFunc(unsigned char Key, int x, int y)
{
	switch(Key)
	{
		case ' ':
			flag = true;
			break;
		case 'r':
			reset_env();
			break;
		case 27:
			exit(1);
		default:
			break;
	}
}

void mouseMotion(int x,int y)
{
	if(flag)
		TAR = 3*(float)y/10 - 90;
}

int main(int argc, char** argv)
{
	btCollisionShape *shape;
	btDefaultMotionState *motionState;
	btScalar mass;
	btVector3 localInertia;
	
	btDefaultCollisionConfiguration *collisionCfg = new btDefaultCollisionConfiguration();
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	dynamicsWorld = new btDiscreteDynamicsWorld(new btCollisionDispatcher(collisionCfg),
	overlappingPairCache, new btSequentialImpulseConstraintSolver, collisionCfg);
	dynamicsWorld->setGravity(btVector3(0, -5, 0));
	
	//stick initialization
	shape = new btBoxShape(btVector3(20,2.5,2.5));
	trans.setIdentity();
	//qtn.setEuler(0.0, 0.0, 0.25);
	//trans.setRotation(qtn);
	trans.setOrigin(btVector3(0, 0, 0));
	motionState = new btDefaultMotionState(trans);
	mass = 0.1;
	shape->calculateLocalInertia(mass, localInertia);
	stick = new btRigidBody(mass, motionState, shape, localInertia);
	//stick->setFriction(btSqrt(10));
	//stick->setCollisionFlags(btCollisionObject::CollisionObjectTypes::CO_FEATHERSTONE_LINK);
	//stick->setCollisionFlags( stick->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	//stick->setActivationState(DISABLE_DEACTIVATION);
	//stick->setLinearFactor(btVector3(0,0,0));
	hinge = new btHingeConstraint(*stick, btVector3(0, 0, 0), btVector3(0, 0, 1), true);
	hinge->enableMotor(true);
	hinge->setMaxMotorImpulse(10.0f);
	//hinge->setLimit(-THETA, THETA);
	dynamicsWorld->addConstraint(hinge);
	dynamicsWorld->addRigidBody(stick);
	
	//ball initialization
	shape = new btSphereShape(btScalar(5));
	trans.setIdentity();
	//qtn.setEuler(0.8, 0.1, 0.1);
	//trans.setRotation(qtn);
	trans.setOrigin(btVector3(10, 30, 0));
	motionState = new btDefaultMotionState(trans);
	mass = 1.0;
	shape->calculateLocalInertia(mass, localInertia);
	ball = new btRigidBody(mass, motionState, shape, localInertia);
	//ball->setFriction(btSqrt(10));
	ball->setLinearFactor(btVector3(1,1,0));
	dynamicsWorld->addRigidBody(ball);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Balancing Ball");
	glutDisplayFunc(draw);
	glutIdleFunc(timer);
	glutKeyboardFunc(keyboardFunc);
	glutPassiveMotionFunc(mouseMotion);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glMatrixMode(GL_PROJECTION);
	gluPerspective( 50.0, 1.0, 20.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 90.0, 0.0, 8.0, 0.0, 0.0, 1.0, 0.0);

	glutMainLoop();

	delete shape;
	delete motionState;
	delete collisionCfg;
	//delete axisSweep;
	delete overlappingPairCache;
}
