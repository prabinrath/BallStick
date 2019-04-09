#include<game.h>
#include <iostream>
#include <fstream>
extern double getTimeDifference(TimePoint end, TimePoint start);
int main(int argc, char** argv)
{
	TimePoint time1=Clock::now();
	BalanceGame game(argc,argv);
	ofstream file;
  	file.open ("datasets/dataset_new.txt", ios::app);
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
				
		if(getTimeDifference(Clock::now(), time1)>100 )
		{
			// file saving
			char buffer[200];
			sprintf(buffer,"%f,%f,%f,%f,%f\n",game.ballPos.length(),game.ballVel.length(),game.curang,game.motionang,game.getTAR());
			if(game.ballPos.getX()>=-20 && game.ballPos.getX()<=20 && game.ballPos.getY()>=-20 && game.getTAR()!=0)
				file << buffer;
			time1=Clock::now();
		}
		file.close();
	}
	return 0;
}




