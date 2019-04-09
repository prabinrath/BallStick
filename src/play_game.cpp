#include<game.h>
#include <CSocketServer.h>
#include <fstream>
#define train 1

extern double getTimeDifference(TimePoint end, TimePoint start);

int main(int argc, char** argv)
{
	char msg[512];
	#if train
	ofstream file;
  	file.open ("datasets/dataset_new.txt");
  	#else
	cout<<"Waiting for connection...\n";
	CSocketServer srv(2300);
	char tar[512];
	strcpy(msg,"CONN_CHCK");
	srv.talkToclient(msg,strlen(msg),tar,sizeof(tar));
	printf("%s\n",tar);
	if(strcmp(tar,"ALL_SET")!=0)
	{
		cout<<"Connection Error!\n";
		exit(0);
	}
	#endif
	
	TimePoint sampletime = Clock::now();
	BalanceGame game(argc,argv);
	while(true)
	{
		game.draw();
		game.timer();
		if(game.RST)
		{
			game.reset_env(30,15);
		}
		if(game.QUIT)
		{
			break;
		}
		
		#if train
		if(getTimeDifference(Clock::now(), sampletime)>100 )
		{
			sprintf(msg,"%f,%f,%f,%f,%f\n",game.ballPos.length(),game.ballVel.length(),game.curang,game.motionang,game.getTAR());
			cout<<msg<<endl;
			if(game.ballPos.getX()>=-20 && game.ballPos.getX()<=20 && game.ballPos.getY()>=-20 && game.getTAR()!=0)
				file << msg;
			sampletime=Clock::now();
		}
		file.close();
		#else
		if(getTimeDifference(Clock::now(), sampletime)>50 )
		{
			sprintf(msg,"%f,%f,%f,%f,%f\n",game.ballPos.length(),game.ballVel.length(),game.curang,game.motionang,game.getTAR());
			srv.talkToclient(msg,strlen(msg),tar,sizeof(tar));
			sampletime=Clock::now();
		}
		#endif
		file.close();
	}
	return 0;
}




