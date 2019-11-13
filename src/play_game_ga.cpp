#include<game.h>
#include <CSocketServer.h>
#include <fstream>


float sum_of_angles_in_seconds=0;
int number_of_angles=0;
float number_of_distances_from_center=0;
int	sum_of_distances_from_center=0;
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
	TimePoint GameStartTime=sampletime;
	
	BalanceGame game(argc,argv);
	while(true)
	{
		
		game.draw();
		game.timer();
		
		if(game.indivisual_score >= 20)
		{
			float avg_of_angles=sum_of_angles_in_seconds/number_of_angles;
			float mean_distance_from_center=sum_of_distances_from_center/number_of_distances_from_center;
			float fitness=game.evaluateFitness2(GameStartTime,avg_of_angles,mean_distance_from_center);
			printf("score is %f \n",fitness);
			sprintf(msg,"%f,%f",fitness,(float)(game.indivisual_score));
			srv.talkToclient(msg,strlen(msg),tar,sizeof(tar));
			
			number_of_angles=0;
			sum_of_angles_in_seconds=0;
			number_of_distances_from_center=0;
			sum_of_distances_from_center=0;
			game.reset_env(20,20);
			
			sampletime=Clock::now();
			GameStartTime=sampletime;
			
		}
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
		if(game.ISBALLDROPPED)
		{
			
			
			float avg_of_angles=sum_of_angles_in_seconds/number_of_angles;
			float mean_distance_from_center=sum_of_distances_from_center/number_of_distances_from_center;
			float fitness=game.evaluateFitness2(GameStartTime,avg_of_angles,mean_distance_from_center);
			printf("score is %f \n",fitness);
			sprintf(msg,"%f,%f",fitness,(float)(game.indivisual_score));
			srv.talkToclient(msg,strlen(msg),tar,sizeof(tar));
			
			number_of_angles=0;
			sum_of_angles_in_seconds=0;
			number_of_distances_from_center=0;
			sum_of_distances_from_center=0;
			
			
			game.reset_env(20,20);

			sampletime=Clock::now();
			GameStartTime=sampletime;
		}
		#if train
		if(getTimeDifference(Clock::now(), sampletime)>50 )
		{
			file.open ("datasets/dataset_new.txt",ios::app);
			sprintf(msg,"%f,%f,%f,%f,%f,%f,%f\n",game.ballPos.getX(),game.ballPos.getY(),game.ballPos.length(),game.ballVel.length(),game.curang,game.motionang,game.getTAR());
			//cout<<msg;
			if(game.ballPos.getX()>=-30 && game.ballPos.getX()<=30 && game.ballPos.getY()>=-20)
				file << msg;
			file.close();
			sampletime=Clock::now();
			
			
		}	
		#else
		if(getTimeDifference(Clock::now(), sampletime)>50 )
		{	
			
			sprintf(msg,"%f,%f,%f,%f",game.ballPos.getX(),game.ballPos.getY(),game.ballPos.length(),game.ballVel.length());
			srv.talkToclient(msg,strlen(msg),tar,sizeof(tar));
			//cout<<msg<<" | "<<atof(tar)<<endl;
			game.setTAR(atof(tar));
			sampletime=Clock::now();
			
			//updating distance parameters of the game
			number_of_distances_from_center++;
			sum_of_distances_from_center+=game.ballPos.length();
			number_of_angles++;
			sum_of_angles_in_seconds+=abs(game.curang);
			
		}	
		#endif
	}
	return 0;
}
