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
			game.reset_env(30,15);  // angle_of_rotation,disp
		}
		if(game.QUIT)
		{
			break;
		}
		cout<<game.ballPos.length()<<" "<<game.ballVel.length()<<" ";
		cout<<game.curang<<" "<<game.motionang<<endl;
	}
	return 0;
}
