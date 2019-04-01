#include<game.h>

float BalanceGame::TAR = 0;
bool BalanceGame::flag = false;
bool BalanceGame::RST = false;
bool BalanceGame::QUIT = false;

double getTimeDifference(TimePoint end, TimePoint start)
{
    return ((double)chrono::duration_cast<chrono::milliseconds>(end-start).count()) * 0.001;
}

BalanceGame::BalanceGame(int argc, char** argv)
{	
	collisionCfg = new btDefaultCollisionConfiguration();
	overlappingPairCache = new btDbvtBroadphase();
	dynamicsWorld = new btDiscreteDynamicsWorld(new btCollisionDispatcher(collisionCfg),
	overlappingPairCache, new btSequentialImpulseConstraintSolver, collisionCfg);
	dynamicsWorld->setGravity(btVector3(0, -5, 0));
	
	//stick initialization
	shape = new btBoxShape(btVector3(20,2.5,2.5));
	trans.setIdentity();
	trans.setOrigin(btVector3(0, 0, 0));
	motionState = new btDefaultMotionState(trans);
	mass = 0.1;
	shape->calculateLocalInertia(mass, localInertia);
	stick = new btRigidBody(mass, motionState, shape, localInertia);
	hinge = new btHingeConstraint(*stick, btVector3(0, 0, 0), btVector3(0, 0, 1), true);
	hinge->enableMotor(true);
	hinge->setMaxMotorImpulse(10.0f);
	dynamicsWorld->addConstraint(hinge);
	dynamicsWorld->addRigidBody(stick);
	
	//ball initialization
	shape = new btSphereShape(btScalar(5));
	trans.setIdentity();
	trans.setOrigin(btVector3(10, 30, 0));
	motionState = new btDefaultMotionState(trans);
	mass = 1.0;
	shape->calculateLocalInertia(mass, localInertia);
	ball = new btRigidBody(mass, motionState, shape, localInertia);
	ball->setLinearFactor(btVector3(1,1,0));
	dynamicsWorld->addRigidBody(ball);
	
	//OpenGL initialization
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Balancing Ball");
	//glutPassiveMotionFunc(BalanceGame::mouseMotion);
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glMatrixMode(GL_PROJECTION);
	gluPerspective( 50.0, 1.0, 20.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 90.0, 0.0, 8.0, 0.0, 0.0, 1.0, 0.0);
	
	this->mouse_handle = new thread(&BalanceGame::handleMouse);
	//glutMainLoop();
}

void BalanceGame::change_type(btScalar m[])
{
	for(int i=0;i<16;i++)
	{
		mat[i]=m[i];
	}
}

void BalanceGame::handleMouse()
{
	Display *display;
	XEvent xevent;
	Window window;
	if( (display = XOpenDisplay(NULL)) == NULL )
        return;
    window = DefaultRootWindow(display);
    XAllowEvents(display, AsyncBoth, CurrentTime);
	XGrabPointer(display, window, 1, PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
	XGrabKeyboard(display, window, KeyPressMask, GrabModeAsync, GrabModeAsync, CurrentTime);
	while(1)
	{
		XNextEvent(display, &xevent);
		if(xevent.type == MotionNotify)
		{
			mouseMotion(xevent.xmotion.x_root, xevent.xmotion.y_root);
		}
		if(xevent.type == KeyPress)
		{
			keyboardFunc(xevent.xkey.keycode);
		}
	}
}

void BalanceGame::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	stick->getMotionState()->getWorldTransform(trans);
	trans.getOpenGLMatrix(matrix);
	change_type(matrix);
	glMultMatrixf(mat);
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

void BalanceGame::update_hinge_pos(float target, float kp, float dt)
{
	curang = hinge->getHingeAngle()*180/SIMD_PI;
	float diff = (target - curang);
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

void BalanceGame::reset_env()
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
	RST=false;
}

void BalanceGame::timer()
{
	dtime = tme;
	tme = glutGet(GLUT_ELAPSED_TIME) / 250.0;
	dtime = tme - dtime;

	if(dynamicsWorld)
	{
		update_hinge_pos(TAR,0.06, dtime);
		dynamicsWorld->stepSimulation(dtime, 10);
		ballPos = ball->getCenterOfMassPosition();
		ballVel = ball->getLinearVelocity();
		motionang = -atan(ballVel.getY()/ballVel.getX())*180/SIMD_PI;
	}

	glutPostRedisplay();
}

void BalanceGame::keyboardFunc(int keycode)
{
	switch(keycode)
	{
		case 65:
			flag = true;
			break;
		case 27:
			RST=true;
			break;
		case 9:
			QUIT=true;
		default:
			break;
	}
}

void BalanceGame::mouseMotion(int x,int y)
{
	if(flag)
		TAR = 180*(float)y/767 - 90;
}

BalanceGame::~BalanceGame()
{
	delete shape;
	delete motionState;
	delete collisionCfg;
	delete overlappingPairCache;
}
