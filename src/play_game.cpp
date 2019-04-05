
#include<game.h>
#include <iostream>
#include <fstream>
extern double getTimeDifference(TimePoint end, TimePoint start);
int main(int argc, char** argv)
{
	TimePoint time1=Clock::now();
	BalanceGame game(argc,argv);
	while(true)
	{
		game.draw();
		game.timer();
		if(game.RST)
		{
			game.reset_env(30,15);  // angle_of_rotation,disp
		}
		if(game.QUIT)
		{
			break;
		}
		// file saving
		ofstream file;
  		file.open ("src/dataset.txt", ios::app);
		char buffer[200];
		
		sprintf(buffer,"%f,%f,%f,%f,%f\n",game.ballPos.length(),game.ballVel.length(),game.curang,game.motionang,game.getTAR());
		//cout<<game.ballPos.length()<<" "<<game.ballVel.length()<<" "<<game.curang<<" "<<game.motionang<<" "<<game.getTAR()<<endl;
		//cout<<buffer<<endl;
		//float update_time=time(0);
		
		if(getTimeDifference(Clock::now(), time1)>100 )
		{
			if(game.ballPos.getX()>=-20 && game.ballPos.getX()<=20 && game.ballPos.getY()>=-20 && game.getTAR()!=0)
				file << buffer;
			time1=Clock::now();
		}
		file.close();
	}
	return 0;
}




