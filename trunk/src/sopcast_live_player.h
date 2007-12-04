
#ifndef SOPCAST_LIVE_PLAYER_H_
#define SOPCAST_LIVE_PLAYER_H_

#include "live_player.h"

class SopcastPlayer : public LivePlayer {
	public:
		SopcastPlayer(GMplayer& gmp, const std::string& stream_);
		~SopcastPlayer();
		void play();
		void stop();
	private:
		int sop_pid;
		std::string stream;
};

#endif
