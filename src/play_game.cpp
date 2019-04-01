#include<game.h>

int main(int argc, char** argv)
{
	BalanceGame game(argc,argv);
	while(true)
	{
		game.draw();
		game.timer();
		if(game.RST)
		{
			game.reset_env();
		}
		if(game.QUIT)
		{
			break;
		}
		//cout<<game.ballVel.getX()<<" "<<game.ballVel.getY()<<" "<<game.ballVel.getZ()<<endl;
		cout<<game.curang<<" "<<game.motionang<<endl;
	}
	return 0;
}
