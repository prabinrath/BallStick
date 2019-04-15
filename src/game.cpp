#include<game.h>

float BalanceGame::TAR = 0;
int BalanceGame::x_=0;
int BalanceGame::y_=0;
bool BalanceGame::setpointer=false;                                           
bool BalanceGame::RST = false;                   
bool BalanceGame::QUIT = false;                 
TimePoint BalanceGame::entry_time=TimePoint();  // 0 ms
float BalanceGame::elapsed_time=0;
#if train
float BalanceGame::range_of_termination=5;
float BalanceGame::time_of_termination=3;
bool BalanceGame::lock = false; 
#else
float BalanceGame::range_of_termination=10;
float BalanceGame::time_of_termination=3;
bool BalanceGame::lock = true; 
#endif
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
	shape = new btBoxShape(btVector3(30,2.5,2.5));
	trans.setIdentity();
	trans.setOrigin(btVector3(0, 0, 0));
	motionState = new btDefaultMotionState(trans);
	mass = 0.1;
	shape->calculateLocalInertia(mass, localInertia);
	stick = new btRigidBody(mass, motionState, shape, localInertia);
	hinge = new btHingeConstraint(*stick, btVector3(0, 0, 0), btVector3(0, 0, 1), true);
	hinge->enableMotor(true);
	hinge->setMaxMotorImpulse(70.0f);
	dynamicsWorld->addConstraint(hinge);
	dynamicsWorld->addRigidBody(stick);
	
	//ball initialization
	shape = new btSphereShape(btScalar(5));
	trans.setIdentity();
	trans.setOrigin(btVector3(10, 20, 0));
	motionState = new btDefaultMotionState(trans);
	mass = 3.0;
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
	setpointer = false;
	score = 0;
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
		if(setpointer)
		{
			XWarpPointer(display, None, window, 0, 0, 0, 0, x_, y_);
			XFlush(display);
			setpointer = false;
		}
	}
}

void BalanceGame::isGameDone()
{
	if(ballPos.getX()>40 || ballPos.getX()<-40 || ballPos.getY()>200 || ballPos.getY()<-40)
	{
		entry_time=TimePoint();
		reset_env(20,20);
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
				score++;
				entry_time=TimePoint();
				cout<<"total time taken in seconds: "<<getTimeDifference(Clock::now(),game_start_time)/1000<<endl;
				reset_env(20,20);
			} 
		}
	}
	else
	{
		entry_time=TimePoint();
	}
}

void BalanceGame::drawBitmapText(char *string,float x,float y,float z) 
{  
	char *c;
	glRasterPos3f(x, y,z);
	glColor3f(0, 255, 0); 
	for (c=string; *c != '\0'; c++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
}

void BalanceGame::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPointSize(10.0f);
  	glBegin(GL_POINTS);
  	glColor3f(1,1,1);
  	glVertex3f(0,0,3);
  	glEnd();
  	char text[100];
	sprintf(text,"Current Score: %d",score);
	drawBitmapText(text,10,40,0);

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
	double default_angle=atan(2.5f/20.0f);
	double radius=sqrt(pow(2.5,2)+pow(20,2));
	srand(time(0));
	double t=angle_of_rotation-rand()%(2*angle_of_rotation);
	double changed_angle=(t)*SIMD_PI/180;
	double final_angle1=default_angle+changed_angle;
	double final_angle2=final_angle1+SIMD_PI-2*default_angle;
	double x1=radius*cos(final_angle1);
	double y1=radius*sin(final_angle1);
	double x2=radius*cos(final_angle2);
	double y2=radius*sin(final_angle2);
	double x=(x1+x2)/2;
	double y=(y1+y2)/2;
	x+=5*cos(SIMD_PI/2+changed_angle);
	y+=5*sin(SIMD_PI/2+changed_angle);
	double displace_from_center=disp-rand()%(2*disp);
	double x_dis;
	double y_dis;
	x_dis=displace_from_center*cos(changed_angle);
	y_dis=displace_from_center*sin(changed_angle);

	trans.setIdentity();
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
	#if train
	TAR = -t;
	y_ = (TAR+90)*767/180;
	setpointer = true;
	#endif
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
		if(lock)
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
			#if train
			if(!lock)
				lock = true;
			else
				lock = false;
			#endif
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

void BalanceGame::setTAR(float data)
{
	TAR = data;
}

float BalanceGame::getTAR()
{
	return TAR;
}

void BalanceGame::mouseMotion(int x,int y)
{
	x_=x;
	#if train
	TAR = 180*(float)y/767 - 90;
	#endif
}

BalanceGame::~BalanceGame()
{
	delete shape;
	delete motionState;
	delete collisionCfg;
	delete overlappingPairCache;
}
