#include<game.h>
#include <CSocketServer.h>
#include <fstream>

extern double getTimeDifference(TimePoint end, TimePoint start);

int main(int argc, char** argv)
{
	char msg[512];
	#if train
	ofstream file;
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
			game.reset_env(20,20);
		}
		if(game.QUIT)
		{
			#if !train
			srv.closeSocket();srv.closeSocket();
			#endif
			break;
		}
		
		#if train
		if(getTimeDifference(Clock::now(), sampletime)>50 )
		{
			file.open ("datasets/dataset_new.txt",ios::app);
			sprintf(msg,"%f,%f,%f,%f,%f,%f,%f\n",game.ballPos.getX(),game.ballPos.getY(),game.ballPos.length(),game.ballVel.length(),game.curang,game.motionang,game.getTAR());
			cout<<msg;
			if(game.ballPos.getX()>=-30 && game.ballPos.getX()<=30 && game.ballPos.getY()>=-20)
				file << msg;
			file.close();
			sampletime=Clock::now();
		}
		#else
		if(getTimeDifference(Clock::now(), sampletime)>50 )
		{
			//sprintf(msg,"%f,%f,%f,%f",game.ballPos.length(),game.ballVel.length(),game.curang,game.motionang);
			sprintf(msg,"%f,%f,%f,%f",game.ballPos.getX(),game.ballPos.getY(),game.ballPos.length(),game.ballVel.length());
			srv.talkToclient(msg,strlen(msg),tar,sizeof(tar));
			cout<<msg<<" | "<<atof(tar)<<endl;
			game.setTAR(atof(tar)*2.5);
			sampletime=Clock::now();
		}
		#endif
	}
	return 0;
}
