
//Zizhao Zheng 825807834
//Maxine Mayor 825551461

#include "WSClock.h"

WSClock::WSClock(int numFrames, int ageThreshold)
        : numFrames_(numFrames), ageThreshold_(ageThreshold), currentFrame_(0), circularList_(numFrames) {
}

int WSClock::performPageReplacement(int newVpn) {
    int victimFrame = -1;

    while (true) {
        PageFrame& frame = circularList_[currentFrame_];

        if (frame.lastAccess < newVpn - ageThreshold_) {
            if (!frame.dirty) {
                victimFrame = currentFrame_;
                break;
            } else {
                frame.dirty = false;
            }
        }

        currentFrame_ = (currentFrame_ + 1) % numFrames_;

        if (currentFrame_ == 0) {
            break;
        }
    }

    circularList_[victimFrame].vpn = newVpn;
    return victimFrame;
}

void WSClock::updateAccessTime(int frameIndex, int accessTime) {
    circularList_[frameIndex].lastAccess = accessTime;
}
