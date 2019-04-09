#include<game.h>

float BalanceGame::TAR = 0;                       
bool BalanceGame::flag = false;                      
bool BalanceGame::RST = false;                   
bool BalanceGame::QUIT = false;                 
TimePoint BalanceGame::entry_time=TimePoint();  // 0 ms
float BalanceGame::elapsed_time=0;	
float BalanceGame::range_of_termination=5;
float BalanceGame::time_of_termination=10;
TimePoint BalanceGame::game_start_time=Clock::now(); // current time

double getTimeDifference(TimePoint end, TimePoint start)
{
    return ((double)chrono::duration_cast<chrono::milliseconds>(end-start).count()) ;
}

BalanceGame::BalanceGame(int argc, char** argv)
{	
	collisionCfg = new btDefaultCollisionConfiguration();
	overlappingPairCache = new btDbvtBroadphase();
	dynamicsWorld = new btDiscreteDynamicsWorld(new btCollisionDispatcher(collisionCfg),
	overlappingPairCache, new btSequentialImpulseConstraintSolver, collisionCfg);
	dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
	
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
	hinge->setMaxMotorImpulse(50.0f);
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
	
	this->mouse_handle = new thread(&BalanceGame::handleEvents);
	//glutMainLoop();
}

void BalanceGame::change_type(btScalar m[])
{
	for(int i=0;i<16;i++)
	{
		mat[i]=m[i];
	}
}

void BalanceGame::handleEvents()
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
void BalanceGame::isGameDone()
{
	if(ballPos.length()>30)
	{
		entry_time=TimePoint();
		reset_env(30,15);
		return;
	}
	if(ballPos.length() < sqrt(pow(7.5,2)+pow(range_of_termination,2)))
	{
		if(entry_time==TimePoint())
			entry_time=Clock::now();
		else
		{
			elapsed_time=getTimeDifference(Clock::now(),entry_time)/1000;
			if(elapsed_time>time_of_termination)
			{
				cout<<"Balanced Successful"<<endl;
				entry_time=TimePoint();
				cout<<"total time taken in seconds: "<<getTimeDifference(Clock::now(),game_start_time)/1000<<endl;
				reset_env(30,15);
			} 
		}
	}
	else
	{
		entry_time=TimePoint();
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

void BalanceGame::reset_env(int angle_of_rotation,int disp)
{
	trans.setIdentity();
	double default_angle=atan(2.5f/20.0f);
	double radius=sqrt(pow(2.5,2)+pow(20,2));
	srand(time(0));
	double t=angle_of_rotation-rand()%(2*angle_of_rotation);
	double changed_angle=(t)*SIMD_PI/180;
	//cout<<default_angle*180/SIMD_PI<<"  "<<t<<endl;
	double final_angle1=default_angle+changed_angle;
	double final_angle2=final_angle1+SIMD_PI-2*default_angle;
	//cout<<"final_angle1 : "<<final_angle1*180/SIMD_PI<<" "<<"final_angle2 :"<<final_angle2*180/SIMD_PI<<endl;
	double x1=radius*cos(final_angle1);
	double y1=radius*sin(final_angle1);
	double x2=radius*cos(final_angle2);
	double y2=radius*sin(final_angle2);
	
	double x=(x1+x2)/2;
	double y=(y1+y2)/2;
	//int disp=10;
	x+=5*cos(SIMD_PI/2+changed_angle);
	y+=5*sin(SIMD_PI/2+changed_angle);
	
	double displace_from_center=disp-rand()%(2*disp);
	double x_dis;
	double y_dis;
	x_dis=displace_from_center*cos(changed_angle);
	y_dis=displace_from_center*sin(changed_angle);

	if(y_dis>=0)
		trans.setOrigin(btVector3(x+x_dis,y+y_dis+1,0));
	else
		trans.setOrigin(btVector3(x-x_dis,y-y_dis+1,0));
	ball->setCenterOfMassTransform(trans);
	ball->setAngularVelocity(btVector3(0,0,0));
	ball->setLinearVelocity(btVector3(0,0,0));
	trans.setIdentity();
	btQuaternion qtn;
	
	
	trans.setIdentity();
	qtn.setEuler(0.0, 0.0, changed_angle);
	trans.setRotation(qtn);
	trans.setOrigin(btVector3(0, 0, 0));

	stick->setCenterOfMassTransform(trans);
	TAR=-changed_angle*180/SIMD_PI;
	stick->setLinearVelocity(btVector3(0,0,0));
	stick->setAngularVelocity(btVector3(0,0,0));

	RST=false;
	game_start_time=Clock::now();
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
		if(ballVel.getY()<=0)
			motionang = -atan(ballVel.getY()/ballVel.getX())*180/SIMD_PI;
		else
			motionang = atan(ballVel.getY()/ballVel.getX())*180/SIMD_PI;
	}

	glutPostRedisplay();
	isGameDone();
	
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
float BalanceGame::getTAR()
{
	return TAR;
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
